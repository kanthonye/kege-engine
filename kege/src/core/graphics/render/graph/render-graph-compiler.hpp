//
//  render-graph-compiler.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 9/30/25.
//

#ifndef render_graph_compiler_hpp
#define render_graph_compiler_hpp

#include <set>
#include "render-pass-desc.hpp"

namespace kege{
    
    class RenderGraphCompiler
    {
    private:

        struct ResourceState
        {
            PipelineStageFlag stage = PipelineStageFlag::None;
            AccessFlags access = AccessFlags::None;
            ImageLayout layout = ImageLayout::Undefined;
            bool first_use_found = false;
        };
        
        typedef std::unordered_map< kege::PassId, std::unordered_set< kege::PassId > > DependencyGraph;

    public:

        kege::RenderGraphExecutionPlan compile
        (
            const kege::RenderGraphFrame& frame,
            const std::vector< kege::RgHandle >& root
        );

        kege::RenderPassExecutionSequence compile
        (
            const kege::ActiveRenderPassDescs& active_passes,
            const std::vector< kege::RgHandle >& root
        );

    private:

        DependencyGraph buildDependencyGraph( kege::RenderPassNodes& nodes );

        std::vector< int > doKahnsTopologicalSort
        (
            const kege::RenderPassNodes& nodes,
            const DependencyGraph& dependency_graph
        );

        void cullUnusedPasses
        (
            const std::vector< kege::RgHandle >& root,
            kege::RenderPassNodes& nodes
        );

        void generateBarriers
        (
            kege::RenderPassNodes& nodes
        );

        void processUsage
        (
            std::unordered_map<RgHandle, ResourceState> current_states,
            const AccessFlags& access,
            const ImageLayout& layout,
            const PipelineStageFlag& stage,
            const RgHandle& resource,
            bool is_write,
            RenderPassNode& node
        );

        void generateSubmitInfo
        (
            DependencyGraph& dependency_graph,
            kege::RenderPassExecutionSequence& execution_sequence
        );

    private:

        std::unordered_map< kege::RgHandle, std::vector< kege::PassId > > _writes;
    };

}
// namespace kege
#endif /* render_graph_compiler_hpp */
