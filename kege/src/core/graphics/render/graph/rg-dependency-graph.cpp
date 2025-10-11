//
//  render-graph-dependency-graph.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 9/30/25.
//

#include "render-stage.hpp"
#include "rg-dependency-graph.hpp"

namespace kege {

    void RgDependencyGraph::buildReadAfterWriteDependencies
    (
        const std::vector<RenderStage*>& passes,
        std::unordered_map<RgResrcHandle, ResourceState>& resource_states
    )
    {
        for (int32_t pass_idx = 0; pass_idx < passes.size(); ++pass_idx)
        {
            const RenderStage* pass = passes[ pass_idx ];

            for ( const auto& read : pass->getReads() )
            {
                auto it = resource_states.find( read.handle );
                if ( it != resource_states.end() )
                {
                    if ( it->second.last_writer != -1 )
                    {
                        // RAW dependency: this read depends on the last writer
                        RenderPassDependency dep = {};
                        dep.producer_pass = it->second.last_writer;
                        dep.consumer_pass = pass_idx;
                        dep.resource = read.handle;
                        dep.producer_access = it->second.last_access;
                        dep.consumer_access = read.access;
                        _dependencies.push_back(dep);

                        _adjacency_list[ it->second.last_writer ].push_back( pass_idx );
                    }
                }
            }
        }
    }

    void RgDependencyGraph::buildWriteAfterReadDependencies
    (
        const std::vector<RenderStage*>& passes,
        std::unordered_map<RgResrcHandle, ResourceState>& resource_states
    )
    {
        for (int32_t pass_idx = 0; pass_idx < passes.size(); ++pass_idx)
        {
            RenderStage* pass = passes[ pass_idx ];

            for (const auto& write : pass->getWrites())
            {
                auto it = resource_states.find(write.handle);
                if (it != resource_states.end())
                {
                    // WAR dependency: this write depends on all previous readers
                    for ( int32_t reader_idx : it->second.last_readers )
                    {
                        if (reader_idx != pass_idx)
                        {
                            RenderPassDependency dep;
                            dep.producer_pass = reader_idx;
                            dep.consumer_pass = pass_idx;
                            dep.resource = write.handle;
                            dep.producer_access = it->second.last_access;
                            dep.consumer_access = write.access;
                            _dependencies.push_back(dep);

                            _adjacency_list[reader_idx].push_back(pass_idx);
                        }
                    }
                }
            }
        }
    }

    void RgDependencyGraph::buildWriteAfterWriteDependencies
    (
        const std::vector<RenderStage*>& passes,
        std::unordered_map<RgResrcHandle, ResourceState>& resource_states
    )
    {
        for (int32_t pass_idx = 0; pass_idx < passes.size(); ++pass_idx)
        {
            RenderStage* pass = passes[pass_idx];

            for (const auto& write : pass->getWrites())
            {
                auto it = resource_states.find(write.handle);
                if ( it != resource_states.end() )
                {
                    if ( it->second.last_writer != -1 && it->second.last_writer != pass_idx )
                    {
                        // WAW dependency: this write depends on last writer
                        RenderPassDependency dep;
                        dep.producer_pass = it->second.last_writer;
                        dep.consumer_pass = pass_idx;
                        dep.resource = write.handle;
                        dep.producer_access = it->second.last_access;
                        dep.consumer_access = write.access;
                        _dependencies.push_back(dep);

                        _adjacency_list[it->second.last_writer].push_back(pass_idx);
                    }
                }
            }
        }
    }

    bool RgDependencyGraph::hasCycleDFS( int32_t pass_index, std::vector<bool>& visited, std::vector<bool>& recursion_stack )const
    {
        if (!visited[pass_index])
        {
            visited[pass_index] = true;
            recursion_stack[pass_index] = true;

            for (int32_t neighbor : _adjacency_list[pass_index])
            {
                if (!visited[neighbor] && hasCycleDFS(neighbor, visited, recursion_stack))
                {
                    return true;
                }
                else if (recursion_stack[neighbor])
                {
                    return true;
                }
            }
        }
        recursion_stack[pass_index] = false;
        return false;
    }

    void RgDependencyGraph::generateBarriersForPass
    (
        RenderStage* pass,
        std::unordered_map<RgResrcHandle, ResourceState>& resource_states,
        uint32_t pass_index
    )
    {
        BarrierDescription& barriers = pass->_barriers;

        // Process reads first
        for (const auto& read : pass->getReads())
        {
            auto it = resource_states.find( read.handle );
            if ( it != resource_states.end() )
            {
                bool needs_barrier = false;
                RgResrcBarrierInfo barrier{};

                if (read.handle.type == RgResrcType::Image)
                {
                    if ( it->second.last_layout != read.layout )
                    {
                        needs_barrier = true;
                        barrier.old_layout = it->second.last_layout;
                        barrier.new_layout = read.layout;
                        barrier.src_stage_mask = it->second.last_stage;
                        barrier.dst_stage_mask = read.stage;
                        barrier.src_access_mask = it->second.last_access;
                        barrier.dst_access_mask = read.access;
                    }
                }
                else
                {
                    if ((it->second.last_access & read.access) != read.access || it->second.last_stage != read.stage)
                    {
                        needs_barrier = true;
                        barrier.src_stage_mask = it->second.last_stage;
                        barrier.dst_stage_mask = read.stage;
                        barrier.src_access_mask = it->second.last_access;
                        barrier.dst_access_mask = read.access;
                    }
                }

                if (needs_barrier)
                {
                    barriers.resource_barriers.push_back(barrier);
                }
            }

            // Update state for this read
            resource_states[read.handle].last_access = read.access;
            resource_states[read.handle].last_stage = read.stage;
            if (read.handle.type == RgResrcType::Image)
            {
                resource_states[read.handle].last_layout = read.layout;
            }
            resource_states[read.handle].last_readers.push_back(pass_index);
        }

        // Process writes
        for (const auto& write : pass->getWrites())
        {
            auto it = resource_states.find(write.handle);
            if (it != resource_states.end()) {
                bool needs_barrier = false;
                RgResrcBarrierInfo barrier{};

                if (write.handle.type == RgResrcType::Image)
                {
                    if (it->second.last_layout != write.layout)
                    {
                        needs_barrier = true;
                        barrier.old_layout = it->second.last_layout;
                        barrier.new_layout = write.layout;
                        barrier.src_stage_mask = it->second.last_stage;
                        barrier.dst_stage_mask = write.stage;
                        barrier.src_access_mask = it->second.last_access;
                        barrier.dst_access_mask = write.access;
                    }
                }
                else
                {
                    if ((it->second.last_access & write.access) != write.access || it->second.last_stage != write.stage)
                    {
                        needs_barrier = true;
                        barrier.src_stage_mask = it->second.last_stage;
                        barrier.dst_stage_mask = write.stage;
                        barrier.src_access_mask = it->second.last_access;
                        barrier.dst_access_mask = write.access;
                    }
                }

                if (needs_barrier)
                {
                    barriers.resource_barriers.push_back(barrier);
                }
            }

            // Update state for this write
            resource_states[write.handle].last_access = write.access;
            resource_states[write.handle].last_stage = write.stage;
            resource_states[write.handle].last_writer = pass_index;
            resource_states[write.handle].last_readers.clear();
            if (write.handle.type == RgResrcType::Image)
            {
                resource_states[write.handle].last_layout = write.layout;
            }
        }
    }

    // Updated analyzeDependencies using the helper functions:
    bool RgDependencyGraph::analyzeDependencies( std::vector< RenderStage* >& passes )
    {
        _dependencies.clear();
        _adjacency_list.resize(passes.size());

        std::unordered_map<RgResrcHandle, ResourceState> resource_states;

        // First pass: track resource usage
        for (int32_t pass_idx = 0; pass_idx < passes.size(); ++pass_idx)
        {
            RenderStage* pass = passes[pass_idx];

            // Update resource states for this pass
            for (const auto& read : pass->getReads())
            {
                resource_states[read.handle].last_readers.push_back(pass_idx);
            }
            for (const auto& write : pass->getWrites())
            {
                resource_states[write.handle].last_writer = pass_idx;
                resource_states[write.handle].last_readers.clear();
            }
        }

        // Build dependencies using helper functions
        buildReadAfterWriteDependencies( passes, resource_states );
        buildWriteAfterReadDependencies( passes, resource_states );
        buildWriteAfterWriteDependencies( passes, resource_states );
        return true;
    }

    // Updated topologicalSort that creates the execution plan:
    bool RgDependencyGraph::topologicalSort( std::vector<RenderStage*>& sorted_passes, const std::vector<RenderStage*>& all_passes )
    {
        sorted_passes.clear();

        // Calculate in-degrees
        std::vector<int32_t> in_degree(_adjacency_list.size(), 0);
        for (const auto& neighbors : _adjacency_list)
        {
            for (int32_t neighbor : neighbors)
            {
                in_degree[neighbor]++;
            }
        }

        // Kahn's algorithm
        std::queue<int32_t> queue;
        for (int32_t i = 0; i < in_degree.size(); ++i)
        {
            if (in_degree[i] == 0) {
                queue.push(i);
            }
        }

        while (!queue.empty())
        {
            int32_t current = queue.front();
            queue.pop();
            sorted_passes.push_back(all_passes[current]);

            for (int32_t neighbor : _adjacency_list[current])
            {
                in_degree[neighbor]--;
                if (in_degree[neighbor] == 0)
                {
                    queue.push(neighbor);
                }
            }
        }

        // Check for cycles
        if (sorted_passes.size() != all_passes.size())
        {
            kege::Log::error <<("RenderGraph: Cycle detected in dependency graph!") <<kege::Log::nl;
            return false;
        }

        return true;
    }

    // Updated generateBarriers using the helper function:
    void RgDependencyGraph::generateBarriers( std::vector<RenderStage*>& passes )
    {
        std::unordered_map< RgResrcHandle, ResourceState > current_states;
        for (int32_t pass_idx = 0; pass_idx < passes.size(); ++pass_idx)
        {
            generateBarriersForPass( passes[pass_idx], current_states, pass_idx );
        }
    }

} // namespace kege
