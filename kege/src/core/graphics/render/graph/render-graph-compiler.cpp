//
//  render-graph-compiler.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 9/30/25.
//

#include "render-graph.hpp"
#include "render-graph-compiler.hpp"

namespace kege {

    kege::RenderGraphExecutionPlan RenderGraphCompiler::compile( const kege::RenderGraphFrame& frame, const std::vector< RgHandle >& roots )
    {
        kege::RenderGraphExecutionPlan execution_plan( frame.render_pass_frame.size() );
        for (int sequence = 0; sequence < frame.render_pass_frame.size(); ++sequence)
        {
            const kege::ActiveRenderPassDescs& active_passes = frame.render_pass_frame[ sequence ];
            execution_plan[ sequence ].execution_sequence = compile( active_passes, roots );
            execution_plan[ sequence ].render_view = frame.render_view_frame->at( sequence );
        }
        return execution_plan;
    }

    kege::RenderPassExecutionSequence RenderGraphCompiler::compile
    (
        const kege::ActiveRenderPassDescs& active_passes,
        const std::vector< RgHandle >& roots
    )
    {
        RenderPassNodes nodes( active_passes.size() );
        for (int i = 0; i < active_passes.size(); ++i)
        {
            nodes[i].desc = active_passes[i];
            nodes[i].culled = false;
            nodes[i].id = i;
        }

        _writes.clear();
        DependencyGraph dependencies = buildDependencyGraph( nodes );
        cullUnusedPasses( roots, nodes );
        std::vector< int > sorted = doKahnsTopologicalSort( nodes, dependencies );
        if( sorted.empty() )
        {
            return {};
        }

        kege::RenderPassExecutionSequence execution_sequence;
        execution_sequence.nodes.reserve( sorted.size() );
        for(auto& pass_id : sorted)
        {
            execution_sequence.nodes.push_back( nodes[ pass_id ] );
        }

        // --- Compute Barrier And Resource Lifetime ---
        generateBarriers( execution_sequence.nodes );
        generateSubmitInfo(dependencies, execution_sequence);
        return execution_sequence;
    }

    RenderGraphCompiler::DependencyGraph RenderGraphCompiler::buildDependencyGraph( kege::RenderPassNodes& nodes )
    {
        DependencyGraph dependency_graph;
        // --- Step 1: Resource Writers ---
        for ( int pass_id = 0; pass_id < nodes.size(); ++pass_id )
        {
            const RenderPassNode& node = nodes.at( pass_id );
            for ( const auto& write : node.desc->writes )
            {
                _writes[ write.resource ].push_back( pass_id );
            }
        }

        // --- Step 2: Build Dependency Graph ---
        for ( int reader = 0; reader < nodes.size(); ++reader )
        {
            const RenderPassNode& node = nodes.at( reader );
            for ( const auto& read : node.desc->reads )
            {
                auto itr = _writes.find( read.resource );
                if ( itr == _writes.end() )
                {
                    continue;
                }
                for ( const auto& writer : itr->second )
                {
                    if ( reader != writer )
                    {
                        // The `reader` pass depends on the writer pass
                        dependency_graph[ writer ].insert( reader );
                    }
                }
            }
        }
        return dependency_graph;
    }

    std::vector< int > RenderGraphCompiler::doKahnsTopologicalSort
    (
        const kege::RenderPassNodes& nodes,
        const DependencyGraph& dependency_graph
    )
    {
        /**
         * --- Step 3: Compute In-Degrees ---
         *
         * What indegree means here:
         * - Each render pass is a node in your dependency graph.
         * - An edge exists if Pass A must run before Pass B (because B reads something A writes).

         * So:
         * - In-degree of a node = number of other passes it directly depends on.
         * - In other words, how many edges are pointing into that node.
         */
        std::vector< int > in_degree( nodes.size(), 0 );
        for (const auto& [ writer, readers ] : dependency_graph)
        {
            for (int reader : readers)
            {
                in_degree[reader]++; // reader depends on writer
            }
        }

        // Step 4: collect the passes with in_degree == 0 to seed the algorithm (the ones with no prerequisites).
        std::queue< int > ready_queue;
        for (int i = 0; i < nodes.size(); ++i)
        {
            if (in_degree[i] == 0)
            {
                ready_queue.push(i);
            }
        }
        //_ready_queue = ready_queue;

        // --- Step 5: Kahn’s Topological Sort ---
        std::vector< int > sorted_pass_indices;
        while (!ready_queue.empty())
        {
            int curr_pass_id = ready_queue.front();
            ready_queue.pop();

            sorted_pass_indices.push_back( curr_pass_id );

            auto itr = dependency_graph.find( curr_pass_id );
            if ( itr != dependency_graph.end() )
            {
                // Get all passes that depend on this one
                for (int dependent :itr->second)
                {
                    if (--in_degree[ dependent ] == 0)
                    {
                        ready_queue.push( dependent );
                    }
                }
            }
        }

        // --- Step 5b: Cycle Detection ---
        if ( sorted_pass_indices.size() != nodes.size() )
        {
            kege::Log::error << "RenderGraph Error: Cycle detected in pass dependencies!" << kege::Log::nl;
            return {};
        }
        return sorted_pass_indices;
    }

    void RenderGraphCompiler::cullUnusedPasses
    (
        const std::vector< RgHandle >& root,
        kege::RenderPassNodes& nodes
    )
    {
        // You start from roots: get passes that WRITE root resources
        std::queue< PassId > q;
        for (const RgHandle& res : root)
        {
            auto m = _writes.find( res );
            if (m != _writes.end())
            {
                for (auto& pass_id : m->second)
                {
                    q.push( pass_id );
                    nodes[ pass_id ].culled = true;
                }
            }
        }

        while (!q.empty())
        {
            PassId id = q.front(); q.pop();

            for (auto& read : nodes[id].desc->reads)
            {
                auto it = _writes.find( read.resource );
                if (it != _writes.end())
                {
                    for (PassId writer : it->second)
                    {
                        if ( nodes[writer].culled )
                        {
                            nodes[writer].culled = true;
                            q.push(writer);
                        }
                    }
                }
            }
        }
    }

    void RenderGraphCompiler::generateBarriers(RenderPassNodes& sorted_passes)
    {
        std::unordered_map< RgHandle, ResourceState> current_states;
        std::unordered_map< RgHandle, uint32_t> first_use_pass_idx;
        std::unordered_map< RgHandle, uint32_t> last_use_pass_idx;

        // 1. Find first/last use for each resource
        for (uint32_t i = 0; i < sorted_passes.size(); ++i)
        {
            const RenderPassNode& node = sorted_passes[i];
            if ( node.culled ) continue;

            for (const auto& r : node.desc->reads)
            {
                if (!first_use_pass_idx.count( r.resource ))
                {
                    first_use_pass_idx[ r.resource ] = i;
                }
                last_use_pass_idx[ r.resource ] = i;
            }

            for (const auto& w : node.desc->writes)
            {
                if (!first_use_pass_idx.count( w.resource ))
                {
                    first_use_pass_idx[ w.resource ] = i;
                }
                last_use_pass_idx[ w.resource ] = i;
            }
        }

        // 2. Walk passes in order and insert barriers when state changes
        for (uint32_t i = 0; i < sorted_passes.size(); ++i)
        {
            RenderPassNode& node = sorted_passes[ i ];
            if ( node.culled ) continue;

            // Process writes first so write-after-read hazards get correct barrier
            for (const auto& w : node.desc->writes)
            {
                processUsage( current_states, w.access, w.layout, w.stage, w.resource, true, node );
            }
            for (const auto& r : node.desc->reads)
            {
                processUsage( current_states, r.access, r.layout, r.stage, r.resource, false, node );
            }
        }

        // 3. Store lifetimes for transient allocation later
//        for (auto& [res_id, first_idx] : first_use_pass_idx)
//        {
//            RgVirResrc virres{};
//            virres.resource = res_id;
//            // name/type would be looked up from the original resource list
//            transient_resources.push_back( virres );
//        }
    }

    void RenderGraphCompiler::processUsage
    (
        std::unordered_map<RgHandle, ResourceState> current_states,
        const AccessFlags& access,
        const ImageLayout& layout,
        const PipelineStageFlag& stage,
        const RgHandle& resource,
        bool is_write,
        RenderPassNode& node
    )
    {
        auto& state = current_states[ resource ];

        bool needs_barrier = false;
        if (!state.first_use_found)
        {
            // First use of this resource. Transition from UNDEFINED/EXTERNAL.
            needs_barrier = true;
            state.first_use_found = true;
        }
        else
        {
            // Check if stage, access, or layout changed
            if (state.stage != stage ||
                state.access != access ||
                state.layout != layout)
            {
                needs_barrier = true;
            }
        }

        if (needs_barrier)
        {
            RgResrcBarrier barrier{};
            barrier.resource = resource;
            barrier.src_stage = state.stage;
            barrier.dst_stage = stage;
            barrier.src_access = state.access;
            barrier.dst_access = access;
            barrier.old_layout = state.layout;
            barrier.new_layout = layout;

            // If this is first use, src_stage/access should be TOP/BOTTOM/NONE
            if (barrier.src_stage == PipelineStageFlag::None)
            {
                barrier.src_stage = PipelineStageFlag::TopOfPipe;
                barrier.src_access = AccessFlags::None;
                barrier.old_layout = ImageLayout::Undefined;
            }

            node.barriers.push_back(barrier);
        }

        // Update current state
        state.stage = stage;
        state.access = access;
        state.layout = layout;
    };

    void RenderGraphCompiler::generateSubmitInfo( DependencyGraph& dependencies, kege::RenderPassExecutionSequence& execution_sequence )
    {
        uint32_t current_submit_id = 0;
        SubmitDesc* current_submit = nullptr;

        auto start_new_submit = [&](QueueType queue)
        {
            execution_sequence.submits.emplace_back();
            current_submit = &execution_sequence.submits.back();
            current_submit->submit_id = current_submit_id++;
            current_submit->queue_type = queue;
        };

        QueueType last_queue = QueueType::Graphics;
        start_new_submit(last_queue);

        for (uint32_t i = 0; i < execution_sequence.nodes.size(); ++i)
        {
            RenderPassNode& pass = execution_sequence.nodes[i];
            pass.submit_index = current_submit->submit_id;

            // Decide if we need to break the submit
            //QueueType pass_queue = pass.desc->pass == RenderPassType::Compute
            //                    ? QueueType::Compute : QueueType::Graphics;

            bool need_new_submit = false;
            if (pass.desc->type != last_queue)
            {
                need_new_submit = true; // queue family change
            }
            // TODO: check pass.desc->force_submit or async compute rules

            if (need_new_submit && !current_submit->pass_ids.empty())
            {
                start_new_submit( pass.desc->type );
                last_queue = pass.desc->type;
            }

            current_submit->pass_ids.push_back( pass.id );

            // For each dependency of this pass, if the dependency is in a previous
            // submit, we need to wait on that submit.
            const std::unordered_set< PassId >& readers = dependencies[pass.id];
            for (PassId dep_id : readers)
            {
                uint32_t dep_submit = execution_sequence.nodes[dep_id].submit_index;
                if (dep_submit != current_submit->submit_id)
                {
                    // Add dep_submit to wait list if not already there
                    auto it = std::find
                    (
                        current_submit->wait_on_pass_ids.begin(),
                        current_submit->wait_on_pass_ids.end(),
                        dep_id
                    );

                    if ( it == current_submit->wait_on_pass_ids.end() )
                    {
                        current_submit->wait_on_pass_ids.push_back(dep_id);
                    }
                }
            }
        }
    }

}
