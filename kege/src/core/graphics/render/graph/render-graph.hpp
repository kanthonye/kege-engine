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
#include "render-graph-compiler.hpp"

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

        kege::RgResrcHandle importShaderResource( std::string name, const ShaderResrcs& handles );
        const kege::ShaderResrc* fetchShaderResource( const std::string& name )const;
        const kege::ShaderResrc* getShaderResource( const RgResrcHandle& handle )const;
        kege::RgResrcHandle defnShaderResource( const RgShaderResrcDefn& defn );
        void removeShaderResource( const RgResrcHandle& handle );
        void removeShaderResource( const std::string& name );
        RgResrcHandle getRgResrcShaderResrc( const std::string& name );

        kege::RgResrcHandle importBuffer( std::string name, const std::vector<kege::BufferHandle>& handles );
        const kege::BufferDefn* getBufferDefn( const std::string& name );
        kege::RgResrcHandle defnBuffer( const kege::BufferDefn& defn );
        const kege::BufferHandle* getBuffer( const RgResrcHandle& handle )const;
        const kege::BufferHandle* fetchBuffer( const std::string& name )const;
        void removeBuffer( const RgResrcHandle& handle );
        void removeBuffer( const std::string& name );
        RgResrcHandle getRgResrcBuffer( const std::string& name );


        kege::RgResrcHandle defnImage( const kege::ImageDefn& defn );
        const ImageDefn* getImageDefn( const kege::RgResrcHandle& handle );
        const ImageDefn* getImageDefn( const std::string& name );

        RgResrcHandle importImage( std::string name, const std::vector<kege::ImageHandle>& handles );
        const kege::ImageHandle* getImage( const kege::RgResrcHandle& handle )const;
        const kege::ImageHandle* fetchImage( const std::string& name )const;
        void removeImage( const kege::RgResrcHandle& handle );
        void removeImage( const std::string& name );
        RgResrcHandle getRgResrcImage( const std::string& name );


        RgResrcHandle importSampler( std::string name, const kege::SamplerHandle& handle );
        RgResrcHandle defnSampler( const kege::SamplerDefn& defn );
        const kege::SamplerHandle* getSampler( const RgResrcHandle& handle )const;
        const kege::SamplerHandle* fetchSampler( const std::string& name )const;
        void removeSampler( const kege::RgResrcHandle& handle );
        void removeSampler( const std::string& name );
        RgResrcHandle getRgResrcSampler( const std::string& name );



        /**
         * @brief Adds a graphics pass to the render graph.
         * @param definition Parameters defining the graphics pass.
         */
        void addPass(const RenderPassDefn& definition);

        /**
         * @brief Executes the compiled render graph.
         */
        void execute( RenderManager& render_manager );

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
        RenderGraph(kege::Graphics* graphics);

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

        AssetManager _asset_manager;

        std::vector< RgImageLayoutTransition > _initial_image_transitions;

        /**
         * compiled render passes read for executions
         */
        std::vector< RenderStage* > _execution_order;

        /**
         * all the available render passes
         */
        std::vector< RenderStage > _passes;

        kege::Graphics* _graphics;

        friend RenderGraphCompiler;
        friend RenderStage;
    };

}

#endif /* render_graph_hpp */
