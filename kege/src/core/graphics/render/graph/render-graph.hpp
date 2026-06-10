//
//  render-graph.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#ifndef render_graph_hpp
#define render_graph_hpp

#include "render-graph-compiler.hpp"
#include "render-graph-executor.hpp"
#include "render-graph-loader.hpp"

#include "../../components/camera/frustum.hpp"
#include "../../../resource/asset-manager.hpp"
#include "../../../utils/frames.hpp"

namespace kege
{
    class RenderGraph;
}

namespace kege::ref
{
    typedef kege::Ref< kege::RenderGraph > RenderGraph;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::RenderGraph > RenderGraph;
}

namespace kege{
    
    /**
     * @brief A render graph implementation for managing frame rendering operations.
     *
     * The RenderGraph class provides a high-level interface for defining and executing
     * rendering operations in a structured, dependency-aware manner. It manages resources,
     * synchronization, and pass dependencies automatically.
     *
     * @note Inherits from kege::RefCounter for reference-counted lifetime management.
     */
    class RenderGraph : public kege::RefCounter
    {
    public:

        /**
         * @brief Compiles the render graph for execution.
         * @return True if compilation succeeded, false otherwise.
         */
        kege::RenderGraphFrame build( const kege::RenderViewFrame& views );

        /**
         * @brief Compiles the render graph for execution.
         * @return True if compilation succeeded, false otherwise.
         */
        kege::RenderGraphExecutionPlan compile
        (
            const kege::RenderGraphFrame& frame,
            const std::vector< RgHandle >& roots
        );

        /**
         * @brief Executes the compiled render graph.
         */
        std::vector< kege::SubmitInfo > execute
        (
            kege::Renderer* renderer,
            const kege::RenderGraphExecutionPlan& execution_plan
        );


        const kege::RgRenderPassDesc* findPass( const std::string& name )
        {
            return _description.findPass( name );
        }

        const RgRenderPassDesc* getRenderPassDesc( kege::RenderPassType type )
        {
            return _description.getPassByType( type );
        }

        kege::RenderGraphDescriptor* getRenderGraphDescriptor();

        /**
         * @brief Adds a graphics pass to the render graph.
         * @param desc Parameters defining the graphics pass.
         */
        void addPass( const RgRenderPassDesc& desc );

        bool load( const std::string& filename );
        
        void integrateResources();

        kege::Renderer* getRenderer();
        void begin();
        void end();

        /**
         * @brief Clears all resources and passes from the graph.
         */
        void clear();

        /**
         * @brief Constructs a render graph.
         * @param renderer Associated graphics context.
         */
        RenderGraph(kege::Renderer* renderer);

        /**
         * @brief Destructor.
         */
        ~RenderGraph();

    private:

        kege::RenderGraphDescriptor _description;
        kege::RenderGraphCompiler _compiler;
        kege::RenderGraphExecutor _executor;

        kege::Renderer* _renderer;

        uint32_t _frame_index;
    };

}

namespace kege::ref{
    typedef kege::Ref< kege::RenderGraph > RenderGraph;
}
#endif /* render_graph_hpp */
