//
//  render-graph-compiler.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 9/30/25.
//

#include "render-stage.hpp"
#include "render-graph.hpp"
#include "render-graph-compiler.hpp"

namespace kege {

    void RenderGraphCompiler::buildDependencyGraph( DependencyGraph& dependency_graph )
    {
        // --- Step 1: Resource Writers ---
        std::unordered_map< kege::RgResrcHandle, std::vector< int > > writes;
        for ( int pass_id = 0; pass_id < _passes->size(); ++pass_id )
        {
            const RenderStage& pass = _passes->at( pass_id );
            for ( const auto& write : pass.getWrites() )
            {
                writes[ write.handle ].push_back( pass_id );
            }
        }

        // --- Step 2: Build Dependency Graph ---
        for ( int reader = 0; reader < _passes->size(); ++reader )
        {
            const RenderStage& curr_pass = _passes->at( reader );
            for ( const auto& read : curr_pass.getReads() )
            {
                auto itr = writes.find( read.handle );
                if ( itr == writes.end() )
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
    }

    bool RenderGraphCompiler::doKahnsTopologicalSort( const DependencyGraph& dependency_graph, std::vector< int >& sorted_pass_indices )
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
        std::vector< int > in_degree(  _passes->size(), 0 );
        for (const auto& [ writer, readers ] : dependency_graph)
        {
            for (int reader : readers)
            {
                in_degree[reader]++; // reader depends on writer
            }
            //in_degree[ writer ] = static_cast< int >( readers.size() );
        }

        // Step 4: collect the passes with in_degree == 0 to seed the algorithm (the ones with no prerequisites).
        std::queue< int > ready_queue;
        for (int i = 0; i <  _passes->size(); ++i)
        {
            if (in_degree[i] == 0)
            {
                ready_queue.push(i);
            }
        }
        _ready_queue = ready_queue;

        // --- Step 5: Kahn’s Topological Sort ---
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
        if ( sorted_pass_indices.size() != _passes->size() )
        {
            kege::Log::error << "RenderGraph Error: Cycle detected in pass dependencies!" << kege::Log::nl;
            return false;
        }
        return true;
    }

    void RenderGraphCompiler::generateSubmitInfo( DependencyGraph& dependency_graph )
    {
        for ( int pass_id = 0; pass_id < _passes->size(); ++pass_id )
        {
            RenderStage& pass = _passes->at( pass_id );
            if ( !pass._submit_info.empty() ) continue;

            pass._submit_info.resize( MAX_FRAMES_IN_FLIGHT );
            for ( int i=0; i<pass._submit_info.size(); ++i )
            {
                pass._submit_info[ i ].command_buffer = _graph->_graphics->createCommandBuffer( pass._defn.type );
            }
        }
        
        for (const auto& [ writer, readers ] : dependency_graph)
        {
            _passes->at( writer ).destroySemaphores();
            for ( int reader : readers )
            {
                _passes->at( reader ).destroySemaphores();
            }
        }

        // all readers (consumer) depends on the write (producer), thus readers wait on the writers.
        for (const auto& [ writer, readers ] : dependency_graph)
        {
            RenderStage& producer = _passes->at( writer );
            for ( int i=0; i<producer._submit_info.size(); ++i )
            {
                producer._submit_info[i].render_complete_semaphore = getGraphics()->createSemaphore();
                for ( int reader : readers )
                {
                    // the semaphore to wait on, before beginning execution
                    _passes->at( reader )._submit_info[i].wait_semaphores.push_back
                    ( producer._submit_info[i].render_complete_semaphore );
                    //producer._defn.writes
                    _passes->at( reader )._submit_info[i].wait_stages.push_back
                    ( kege::PipelineStageFlag::ColorOutput );
                }
            }
        }
    }

    void RenderGraphCompiler::updateExecutionOrder( const std::vector< int >& sorted_pass_indices )
    {
        _execution_order->clear();
        _execution_order->reserve( sorted_pass_indices.size() );
        for (int pass_index : sorted_pass_indices)
        {
            _execution_order->push_back( &_passes->at( pass_index ) );
        }
    }

    ImageLayout RenderGraphCompiler::inferLayout( AccessFlags access )
    {
        if ( hasFlag( access, AccessFlags::ColorWrite ) )
            return ImageLayout::Color;

        if ( hasFlag( access, AccessFlags::ColorRead ) )
            return ImageLayout::ShaderRead; // some engines use same layout for read/write

        if ( hasFlag( access, AccessFlags::DepthStencilWrite ) )
            return ImageLayout::DepthStencil;

        if ( hasFlag( access, AccessFlags::DepthStencilRead ) )
            return ImageLayout::DepthStencilRead;

        if ( hasFlag( access, AccessFlags::ShaderRead ) )
            return ImageLayout::ShaderRead;

        if ( hasFlag( access, AccessFlags::ShaderWrite ) )
            return ImageLayout::General;

        if ( hasFlag( access, AccessFlags::TransferRead ) )
            return ImageLayout::TransferSrc;

        if ( hasFlag( access, AccessFlags::TransferWrite ) )
            return ImageLayout::TransferDst;

        if ( hasFlag( access, AccessFlags::HostRead ) )
            return ImageLayout::HostRead;

        if ( hasFlag( access, AccessFlags::HostWrite ) )
            return ImageLayout::HostWrite;

        return ImageLayout::Undefined;
    }

    void emitBarrier
    (
        BarrierDescriptions* barriers, RgResrcHandle handle,
        PipelineStageFlag src_stage, AccessFlags src_access, ImageLayout old_layout,
        PipelineStageFlag dst_stage, AccessFlags dst_access, ImageLayout new_layout
    )
    {
        // Decide whether we actually need a barrier:
        bool layout_changed = (old_layout != new_layout);
        bool access_changed = (src_access != dst_access);
        bool stage_changed  = (src_stage  != dst_stage );

        if (layout_changed || access_changed || stage_changed)
        {
            RgResrcBarrierInfo barrier{};
            barrier.src_stage_mask = src_stage;
            barrier.dst_stage_mask = dst_stage;
            barrier.src_access_mask = src_access;
            barrier.dst_access_mask = dst_access;
            barrier.old_layout = old_layout;
            barrier.new_layout = new_layout;
            barrier.resource_handle = handle;

            barriers->push_back( barrier );
        }
    };
//
//    void RenderGraphCompiler::transitionToInitialImageLayout( const std::vector< int >& sorted_pass_indices )
//    {
//        std::map< RgResrcHandle, ImageLayout > image_layout_last_state;
//        for (int pass_index : sorted_pass_indices)
//        {
//            for (RgReadResrcDesc& read : _passes->at( pass_index )._defn.reads)
//            {
//                image_layout_last_state[ read.handle ] = read.usage.layout;
//            }
//            for (RgWriteResrcDesc& write : _passes->at( pass_index )._defn.writes)
//            {
//                image_layout_last_state[ write.handle ] = write.usage.layout;
//            }
//        }
//    }

    void RenderGraphCompiler::generateBarriers(const std::vector<int>& sorted_pass_indices)
    {
        /**
         * Internal struct that records the *last known usage* of a resource.
         * This allows the compiler to detect when a resource transitions
         * from one render stage to another, and to generate a synchronization
         * barrier accordingly.
         *
         * Each tracked resource is identified by its handle, and we store:
         *  - The resource name (for debugging/logging)
         *  - The last usage info (pipeline stage, access, layout)
         *  - The stage where that last usage occurred
         *  - The handle itself (as the map key)
         */
        struct LastUsage
        {
            std::string name;
            RgResrcUsage usage{};     // last known usage (stage, access, layout)
            RenderStage* stage = nullptr;
            RgResrcHandle handle{};
        };

        // Maps each resource handle to its last known usage state
        std::unordered_map<RgResrcHandle, LastUsage> last_usage;

        //---------------------------------------------------------------------
        // 1️⃣ MAIN FORWARD PASS: traverse all render stages in dependency order
        //     - Detects transitions between consecutive passes.
        //     - For every resource read/write, generate barriers relative
        //       to its previous use.
        //---------------------------------------------------------------------
        for (int pass_id : sorted_pass_indices)
        {
            RenderStage* stage = &_passes->at(pass_id);
            stage->_barriers.clear();

            // --- HANDLE READS (this pass *consumes* resources) ---
            for (auto& read : stage->_defn.reads)
            {
                // Only image/buffer types are relevant for GPU barriers
                if (read.type != RgResrcType::Image && read.type != RgResrcType::Buffer)
                    continue;

                auto& last = last_usage[read.handle];

                // If the resource was used before, and by a *different stage*,
                // create a barrier that transitions FROM the last usage TO this read.
                if (last.stage && last.stage != stage && read.type == RgResrcType::Image )
                {
                    // Only emit if there’s an actual difference between usages.
                    if (last.usage.layout != read.usage.layout ||
                        last.usage.stage  != read.usage.stage  ||
                        last.usage.access != read.usage.access)
                    {
                        RgResrcBarrierInfo barrier{};
                        barrier.name = read.name;
                        barrier.src_stage_mask = last.usage.stage;
                        barrier.dst_stage_mask = read.usage.stage;
                        barrier.src_access_mask = last.usage.access;
                        barrier.dst_access_mask = read.usage.access;
                        barrier.old_layout = last.usage.layout;
                        barrier.new_layout = read.usage.layout;
                        barrier.resource_handle = read.handle;

                        // Store this barrier on the *current stage*, since it’s the consumer.
                        stage->_barriers.push_back(barrier);
                    }
                }

                // Update the last usage state for this resource
                last.handle = read.handle;
                last.usage  = read.usage;
                last.stage  = stage;
                last.name   = read.name;
            }

            // --- HANDLE WRITES (this pass *produces* or *modifies* resources) ---
            for (auto& write : stage->_defn.writes)
            {
                if (write.type != RgResrcType::Image && write.type != RgResrcType::Buffer)
                    continue;

                auto& last = last_usage[write.handle];

                // If it was used before in another stage,
                // generate a barrier to transition into this write state.
                if (last.stage && last.stage != stage && write.type == RgResrcType::Image )
                {
                    // Only emit if there’s an actual difference between usages.
                    if (last.usage.layout != write.usage.layout ||
                        last.usage.stage  != write.usage.stage  ||
                        last.usage.access != write.usage.access)
                    {
                        RgResrcBarrierInfo barrier{};
                        barrier.name = write.name;
                        barrier.src_stage_mask = last.usage.stage;
                        barrier.dst_stage_mask = write.usage.stage;
                        barrier.src_access_mask = last.usage.access;
                        barrier.dst_access_mask = write.usage.access;
                        barrier.old_layout = last.usage.layout;
                        barrier.new_layout = write.usage.layout;
                        barrier.resource_handle = write.handle;

                        stage->_barriers.push_back(barrier);
                    }
                }

                // Record the current write as the latest usage state.
                last.handle = write.handle;
                last.usage  = write.usage;
                last.stage  = stage;
                last.name   = write.name;
            }
        }

        //---------------------------------------------------------------------
        // 2️⃣ FINALIZATION PASS (Barrier Cycle Closure)
        //
        // At this point, last_usage[] holds the *final* state of each resource
        // after all passes have been processed.
        //
        // To “close the loop” (important for cyclic resources like swapchain
        // images reused every frame), we generate barriers that connect
        // the *last known layout* back to the *first stage’s expected layout*.
        //
        // This ensures that at the start of a new frame, each resource
        // begins in the correct layout as expected by its first user.
        //---------------------------------------------------------------------
        std::set<RgResrcHandle> first_used;
        //std::map< RgResrcHandle, ImageLayout > image_layout_initial_states;

        for (int pass_id : sorted_pass_indices)
        {
            RenderStage* stage = &_passes->at(pass_id);

            // --- HANDLE FIRST-TIME READS ---
            for (auto& read : stage->_defn.reads)
            {
                // Only process if this is the first time we’ve seen this resource.
                if (first_used.find(read.handle) == first_used.end() && read.handle.type == RgResrcType::Image)
                {
                    first_used.insert(read.handle);
                    auto& last = last_usage[read.handle];

                    // Only emit if there’s an actual difference between usages.
                    if (last.usage.layout != read.usage.layout ||
                        last.usage.stage  != read.usage.stage  ||
                        last.usage.access != read.usage.access)
                    {
                        RgResrcBarrierInfo barrier{};
                        barrier.name            = read.name;
                        barrier.src_stage_mask  = last.usage.stage;
                        barrier.dst_stage_mask  = read.usage.stage;
                        barrier.src_access_mask = last.usage.access;
                        barrier.dst_access_mask = read.usage.access;
                        barrier.old_layout      = last.usage.layout;
                        barrier.new_layout      = read.usage.layout;
                        barrier.resource_handle = read.handle;

                        stage->_barriers.push_back(barrier);
                        //image_layout_initial_states[ read.handle ] = last.usage.layout;
                    }
                }
            }

            // --- HANDLE FIRST-TIME WRITES ---
            for (auto& write : stage->_defn.writes)
            {
                if (first_used.find(write.handle) == first_used.end() && write.handle.type == RgResrcType::Image)
                {
                    first_used.insert(write.handle);
                    auto& last = last_usage[write.handle];

                    // Emit only if it represents a real layout or access transition.
                    if (last.usage.layout != write.usage.layout ||
                        last.usage.stage  != write.usage.stage  ||
                        last.usage.access != write.usage.access)
                    {
                        RgResrcBarrierInfo barrier{};
                        barrier.name            = write.name;
                        barrier.src_stage_mask  = last.usage.stage;
                        barrier.dst_stage_mask  = write.usage.stage;
                        barrier.src_access_mask = last.usage.access;
                        barrier.dst_access_mask = write.usage.access;
                        barrier.old_layout      = last.usage.layout;
                        barrier.new_layout      = write.usage.layout;
                        barrier.resource_handle = write.handle;

                        stage->_barriers.push_back(barrier);
                        //image_layout_initial_states[ write.handle ] = last.usage.layout;
                    }
                }
            }
        }

        //---------------------------------------------------------------------
        // 3️⃣ TRANSITION IMAGES TO THEIR INITIAL LAYOUTS
        //
        // At this stage, all per-pass barriers have been generated and the
        // render graph’s dependency chain is fully established.
        //
        // Before the first frame begins execution, each image must be
        // transitioned out of VK_IMAGE_LAYOUT_UNDEFINED into the layout
        // expected by its first usage (the “initial layout” recorded during
        // graph compilation).
        //
        // This step ensures that all images start in the correct state
        // when their first pass begins, especially those created during
        // setup (e.g., scene color, depth, swapchain attachments).
        //---------------------------------------------------------------------
        kege::CommandBuffer* command = _graph->_graphics->createCommandBuffer( QueueType::Graphics );
        command->beginCommands( CommandBufferUsage::OneTimeSubmit );
        for (auto& m : last_usage )
        {
            if (m.first.type != RgResrcType::Image) continue;
            ImageDefn* defn = _graph->_asset_manager.get< ImageDefn >( m.first.index );
            for ( int j=0; j<defn->physical_handle.size(); ++j)
            {                ///<
                command->transitionImageLayout
                (
                    defn->physical_handle[j],
                    defn->layout,
                    m.second.usage.layout
                );
            }
            defn->layout = m.second.usage.layout;
        }
        command->endCommands();
        _graph->_graphics->submit({ .command_buffer = command });
        _graph->_graphics->destroyCommandBuffer( command );
    }

    bool RenderGraphCompiler::resolveResosurceLinks()
    {
        for ( RenderStage& pass : *_passes )
        {
            for ( auto& write : pass.getWrites() )
            {
                if ( !write.handle )
                {
                    if ( write.type == RgResrcType::Image )
                    {
                        ImageDefn* def = _graph->_asset_manager.fetch< ImageDefn >( write.name );
                        if ( def == nullptr )
                        {
                            KEGE_LOG_ERROR <<"undefinded image resource - " << write.name <<Log::nl;
                            return false;
                        }
                        write.handle = def->handle;
                    }
                    else if ( write.type == RgResrcType::Buffer )
                    {
                        BufferDefn* def = _graph->_asset_manager.fetch< BufferDefn >( write.name );
                        if ( def == nullptr )
                        {
                            KEGE_LOG_ERROR <<"undefinded buffer resource - " << write.name <<Log::nl;
                            return false;
                        }
                        write.handle = def->handle;
                    }
                }
            }

            for ( auto& read : pass.getReads() )
            {
                if ( !read.handle )
                {
                    switch ( read.type )
                    {
                        case RgResrcType::Image:
                        {
                            ImageDefn* def = _graph->_asset_manager.fetch< ImageDefn >( read.name );
                            if ( def == nullptr )
                            {
                                kege::Log::error <<"undefinded image resource - " << read.name <<Log::nl;
                                return false;
                            }
                            read.handle = def->handle;
                            break;
                        }

                        case RgResrcType::Buffer:
                        {
                            BufferDefn* def = _graph->_asset_manager.fetch< BufferDefn >( read.name );
                            if ( def == nullptr )
                            {
                                kege::Log::error <<"undefinded buffer resource - " << read.name <<Log::nl;
                                return false;
                            }
                            read.handle = def->handle;
                            break;
                        }

                        case RgResrcType::Sampler:
                        {
                            SamplerDefn* def = _graph->_asset_manager.fetch< SamplerDefn >( read.name );
                            if ( def == nullptr )
                            {
                                kege::Log::error <<"undefinded sampler resource - " << read.name <<Log::nl;
                                return false;
                            }
                            read.handle = def->handle;
                            break;
                        }

                        case RgResrcType::ShaderResource:
                        {
                            RgShaderResrcDefn* def = _graph->_asset_manager.fetch< RgShaderResrcDefn >( read.name );
                            if ( def == nullptr )
                            {
                                kege::Log::error <<"undefinded shader resource - " << read.name <<Log::nl;
                                return false;
                            }
                            read.handle = def->handle;
                            pass._shader_resources.push_back( read.handle );
                            break;
                        }

                        default:
                        {
                            kege::Log::error <<"unsupported read resource -> " << read.name <<Log::nl;
                            break;
                        }
                    }
                }
            }
        }
        return true;
    }

    bool RenderGraphCompiler::resolvePhysicalResosurces( const std::vector< int >& sorted_pass_indices )
    {
        for ( int pass_id : sorted_pass_indices )
        {
            RenderStage& pass = _passes->at( pass_id );
            for ( auto& write : pass.getWrites() )
            {
                if ( write.type == RgResrcType::Image )
                {
                    ImageDefn* def = _graph->_asset_manager.get< ImageDefn >( write.handle );
                    if ( def->physical_handle.empty() )
                    {
                        _graph->createImage( *def );
                    }
                }
                else if ( write.type == RgResrcType::Buffer )
                {
                    BufferDefn* def = _graph->_asset_manager.get< BufferDefn >( write.handle );
                    if ( def->physical_handle.empty() )
                    {
                        _graph->createBuffer( *def );
                    }
                }
            }
            for ( auto& read : pass.getReads() )
            {
                switch ( read.type )
                {
                    case RgResrcType::Image:
                    {
                        ImageDefn* def = _graph->_asset_manager.get< ImageDefn >( read.handle );
                        if ( def->physical_handle.empty() )
                        {
                            _graph->createImage( *def );
                        }
                        break;
                    }

                    case RgResrcType::Buffer:
                    {
                        BufferDefn* def = _graph->_asset_manager.get< BufferDefn >( read.handle );
                        if ( def->physical_handle.empty() )
                        {
                            _graph->createBuffer( *def );
                        }
                        break;
                    }

                    case RgResrcType::Sampler:
                    {
                        SamplerDefn* def = _graph->_asset_manager.get< SamplerDefn >( read.handle );
                        if ( !def->physical_handle )
                        {
                            _graph->createSampler( *def );
                        }
                        break;
                    }

                    case RgResrcType::ShaderResource:
                    {
                        RgShaderResrcDefn* def = _graph->_asset_manager.get< RgShaderResrcDefn >( read.handle );
                        if ( def->physical_handles.empty() )
                        {
                            _graph->createShaderResource( def );
                        }
                        break;
                    }

                    default:
                    {
                        kege::Log::error <<"unsupported read resource -> " << read.name <<Log::nl;
                        break;
                    }
                }
            }
        }
        return true;
    }

    bool RenderGraphCompiler::compile( RenderGraph* graph )
    {
        _graph = graph;
        _passes = &graph->_passes;
        _execution_order = &graph->_execution_order;

        if( !resolveResosurceLinks() )
        {
            return false;
        }

        DependencyGraph dependency_graph;
        buildDependencyGraph( dependency_graph );

        std::vector< int > sorted_pass_indices;
        if( !doKahnsTopologicalSort( dependency_graph, sorted_pass_indices ) )
        {
            return false;
        }

        generateSubmitInfo( dependency_graph );

        // Final Plan Generation
        updateExecutionOrder( sorted_pass_indices );

        resolvePhysicalResosurces( sorted_pass_indices );

        // --- Barrier Calculation ---
        generateBarriers( sorted_pass_indices );

        return true;
    }

    Graphics* RenderGraphCompiler::getGraphics()
    {
        return _graph->_graphics;
    }
}
