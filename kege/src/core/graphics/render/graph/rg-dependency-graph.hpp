//
//  render-graph-dependency-graph.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 9/30/25.
//

#ifndef render_graph_dependency_graph_hpp
#define render_graph_dependency_graph_hpp

#include "render-pass-context.hpp"

namespace kege {

    struct RenderPassDependency
    {
        int32_t producer_pass;       // Index of pass that produces the resource
        int32_t consumer_pass;       // Index of pass that consumes the resource
        RgResrcHandle resource;      // The resource being passed between passes
        AccessFlags producer_access; // How producer uses the resource
        AccessFlags consumer_access; // How consumer uses the resource
    };

    class RgDependencyGraph
    {
    public:

        // Topological sort for execution order
        bool topologicalSort( std::vector<RenderStage*>& sorted_passes, const std::vector<RenderStage*>& all_passes );

        // Build dependency graph from render passes
        bool analyzeDependencies( std::vector< RenderStage* >& passes );

        // Generate barriers between passes
        void generateBarriers(std::vector<RenderStage*>& passes);

        // Debugging and visualization
        void dumpGraphviz(const std::string& filename) const;

        const std::vector< RenderPassDependency >& getDependencies() const { return _dependencies; }

    private:

        struct ResourceState
        {
            int32_t last_writer = -1;           // Last pass that wrote to this resource
            std::vector<int32_t> last_readers;  // Passes that read since last write
            AccessFlags last_access = AccessFlags::None;
            PipelineStageFlag last_stage = PipelineStageFlag::None;
            ImageLayout last_layout = ImageLayout::Undefined;
        };

    private:

        void buildReadAfterWriteDependencies
        (
            const std::vector<RenderStage*>& passes,
            std::unordered_map<RgResrcHandle, ResourceState>& resource_states
        );

        void buildWriteAfterReadDependencies
        (
            const std::vector<RenderStage*>& passes,
            std::unordered_map<RgResrcHandle, ResourceState>& resource_states
        );

        void buildWriteAfterWriteDependencies
        (
            const std::vector<RenderStage*>& passes,
            std::unordered_map<RgResrcHandle, ResourceState>& resource_states
        );

        bool hasCycleDFS
        (
            int32_t pass_index,
            std::vector<bool>& visited,
            std::vector<bool>& recursion_stack
        )
        const;

        void generateBarriersForPass
        (
            RenderStage* pass,
            std::unordered_map<RgResrcHandle,
            ResourceState>& resource_states,
            uint32_t pass_index
        );

    private:

        std::vector< RenderPassDependency > _dependencies;
        std::vector< std::vector<int32_t> > _adjacency_list; // pass_index -> dependent passes
        std::unordered_map< RgResrcHandle, ResourceState > _initial_resource_states;
    };

} // namespace kege

#endif /* render_graph_dependency_graph_hpp */
