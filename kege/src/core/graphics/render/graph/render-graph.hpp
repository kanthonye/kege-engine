//
//  render-graph.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#ifndef render_graph_hpp
#define render_graph_hpp

#include "render-stage.hpp"
#include "asset-manager.hpp"
#include "render-executor.hpp"
#include "render-graph-compiler.hpp"
#include "render-graph-loader.hpp"

#include "../../../utils/frames.hpp"

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

        void addInitialImageTransition( const RgImageLayoutTransition& transition );

        kege::RgResrcHandle importShaderResource( std::string name, const ref::ShaderSet& handles );
        const kege::ShaderSet* fetchShaderResource( const std::string& name )const;
        const kege::ShaderSet* getShaderResource( const RgResrcHandle& handle )const;
        kege::RgResrcHandle defnShaderResource( const RgShaderResrcDefn& defn );
        void removeShaderResource( const RgResrcHandle& handle );
        void removeShaderResource( const std::string& name );
        RgResrcHandle getRgResrcShaderResrc( const std::string& name );

        kege::RgResrcHandle importBuffer( std::string name, const std::vector<ref::Buffer>& handles );
        const kege::BufferDefn* getBufferDefn( const std::string& name );
        kege::RgResrcHandle defnBuffer( const kege::BufferDefn& defn );
        ref::Buffer getBuffer( const RgResrcHandle& handle )const;
        ref::Buffer fetchBuffer( const std::string& name )const;
        void removeBuffer( const RgResrcHandle& handle );
        void removeBuffer( const std::string& name );
        RgResrcHandle getRgResrcBuffer( const std::string& name );


        kege::RgResrcHandle defnImage( const kege::ImageDefn& defn );
        const ImageDefn* getImageDefn( const kege::RgResrcHandle& handle );
        const ImageDefn* getImageDefn( const std::string& name );

        RgResrcHandle importImage( std::string name, const std::vector<ref::Image>& handles );
        const ref::Image getImage( const kege::RgResrcHandle& handle )const;
        const ref::Image fetchImage( const std::string& name )const;
        void removeImage( const kege::RgResrcHandle& handle );
        void removeImage( const std::string& name );
        RgResrcHandle getRgResrcImage( const std::string& name );


        RgResrcHandle importSampler( std::string name, const ref::Sampler& handle );
        RgResrcHandle defnSampler( const kege::SamplerDefn& defn );
        const ref::Sampler getSampler( const RgResrcHandle& handle )const;
        const ref::Sampler fetchSampler( const std::string& name )const;
        void removeSampler( const kege::RgResrcHandle& handle );
        void removeSampler( const std::string& name );
        RgResrcHandle getRgResrcSampler( const std::string& name );

        /**
         * @brief Adds a graphics pass to the render graph.
         * @param definition Parameters defining the graphics pass.
         */
        void addPass(const RenderPassDefn& definition);

        /**
         * @brief Get the render executor for this render graph.
         *
         *  The render executor handle the drawing of render objects.
         *  Render objects are submitted to the render executor for rendering
         */
        ref::RenderExecutor getRenderExecutor();
        
        /**
         * @brief Executes the compiled render graph.
         */
        void execute(const ref::Semaphore& image_available_sem, const ref::Semaphore& render_complete_sem);

        bool load( const std::string& filename );

        /**
         * @brief Compiles the render graph for execution.
         * @return True if compilation succeeded, false otherwise.
         */
        bool compile();

        /**
         * @brief Clears all resources and passes from the graph.
         */
        void clear();

        /**
         * @brief Gets the associated graphics context.
         * @return Pointer to the graphics context.
         */
        kege::Graphics* getGraphics();

        /**
         * @brief Constructs a render graph.
         * @param graphics Associated graphics context.
         */
        RenderGraph(kege::Graphics* graphics, kege::AssetManager* asset_manager);

        /**
         * @brief Destructor.
         */
        ~RenderGraph();

    private:

        /**
         * @brief Create the necessary transition required for each pass.
         * @param sorted_passes The RenderPasses that needs their physical resources initialized.
         */
        //void analyzeTransitions( std::vector<RenderStage*>& sorted_passes );

        void createShaderResource( RgShaderResrcDefn* def );

        /**
         * @brief Create the buffer physical resource handle for the given BufferDefn.
         * @param defn The buffer defnition containing the necessary detail for the buffer creation.
         */
        void createBuffer(BufferDefn& defn);

        /**
         * @brief Create the buffer physical resource handle for the given BufferDefn.
         * @param defn The image defnition containing the necessary detail for the image creation.
         */
        void createImage(ImageDefn& defn);

        void createSampler( SamplerDefn& defn );

    private:

        std::vector< RgImageLayoutTransition > _initial_image_transitions;

        /**
         * compiled render passes read for executions
         */
        std::vector< RenderStage* > _execution_order;

        /**
         * all the available render passes
         */
        std::vector< RenderStage > _passes;

        kege::AssetManager* _asset_manager;
        kege::Graphics* _graphics;

        ref::RenderExecutor _executor;

        friend RenderGraphCompiler;
        friend RenderExecutor;
        friend RenderStage;
    };

}

namespace kege::ref{
    typedef kege::Ref< kege::RenderGraph > RenderGraph;
}
#endif /* render_graph_hpp */
