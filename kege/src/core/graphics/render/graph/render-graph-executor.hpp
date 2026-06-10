//
//  render-graph-executor.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/23/26.
//

#ifndef render_graph_executor_hpp
#define render_graph_executor_hpp

#include "render-pass-desc.hpp"
#include "render-functions.hpp"
#include "render-graph-resource-manager.hpp"

namespace kege {

    class RenderGraphExecutor
    {
    public:

        struct Result
        {
            std::vector< kege::CommandBuffer* > submitted_cmds;
            std::vector< kege::Semaphore* > signal_semaphores;
        };

        std::vector< kege::SubmitInfo > execute
        (
            kege::Renderer* renderer,
            const kege::RenderGraphExecutionPlan& execution_plan
        );

    private:

        void executeRenderPass
        (
            const RenderPassNode& pass,
            kege::RenderPassContext& context
        );
        
        void applyBarriers( const RenderPassNode& pass, kege::RenderPassContext& context );
        void beginRendering( const RenderPassNode& pass, kege::RenderPassContext& context );
        void endRendering( kege::RenderPassContext& context );

        kege::GraphicsDevice* _graphics;
    };
    
}
#endif /* render_graph_executor_hpp */
