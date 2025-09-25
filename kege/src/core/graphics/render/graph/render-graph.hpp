//
//  render-graph.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#ifndef render_graph_hpp
#define render_graph_hpp

#include "render-pass.hpp"
#include "asset-manager.hpp"

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



        RgResrcHandle importImage( std::string name, const std::vector<kege::ImageHandle>& handles );
        const ImageDefn* getImageDefn( const std::string& name );
        kege::RgResrcHandle defnImage( const kege::ImageDefn& defn );
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





        /// @name Pass Management
        /// @{
        /**
         * @brief Adds a graphics pass to the render graph.
         * @param definition Parameters defining the graphics pass.
         */
        void addPass(const RenderPassDefn& definition);

        /**
         * @brief Gets the associated graphics context.
         * @return Pointer to the graphics context.
         */
        kege::Graphics* getGraphics();

        /// @name Graph Execution
        /// @{
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
        /// @}

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
        
        bool resolveResosurceLinks( RenderPass* pass );

        /// @name Internal Implementation
        /// @{
        /**
         * @brief Assign the required resources to each context of the given RenderPasses.
         * @param sorted_passes The RenderPasses that needs their physical resources initialized.
         */
        //void setupRenderPassContext( std::vector<RenderPass*>& sorted_passes );

        /**
         * @brief Create the physical resources for each render graph resource that is not yet initialized.
         * @param sorted_passes The RenderPasses that needs their physical resources initialized.
         */
        bool resolveResosurceLinks( std::vector<RenderPass*>& sorted_passes );

        /**
         * @brief Create the necessary transition required for each pass.
         * @param sorted_passes The RenderPasses that needs their physical resources initialized.
         */
        void analyzeTransitions( std::vector<RenderPass*>& sorted_passes );

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

        void processUsage
        (
            const std::string& name,
            RgResrcType type,
            AccessFlags access,
            PipelineStageFlag stage,
            RgResrcHandle handle, bool is_write, BarrierDescription& pre_barriers,
            std::unordered_map<kege::ImageHandle, RgResrcUsage>& current_image_states,
            std::unordered_map<kege::BufferHandle, RgResrcUsage>& current_buffer_states
        );

        void updateStateAfterPass
        (
            const std::string& name,
            RgResrcType type,
            AccessFlags access,
            PipelineStageFlag stage,
            RgResrcHandle handle, bool is_write,
            std::unordered_map<kege::ImageHandle, RgResrcUsage>& current_image_states,
            std::unordered_map<kege::BufferHandle, RgResrcUsage>& current_buffer_states
        );
        /// @}

    private:

        AssetManager _asset_manager;

        /**
         * Shader resources
         */
//        ResourceManagerT< RgShaderResrcDefn >
//        std::unordered_map< std::string, RgResrcHandle > _shader_resrc_map;
//        std::vector< RgShaderResrcDefn > _shader_resrc_definitions;

        /**
         * Buffer resources
         */
//        std::unordered_map< std::string, RgResrcHandle > _buffer_resource_map;
//        std::vector< BufferDefn > _buffer_definitions;

        /**
         * Image resources
         */
//        std::unordered_map< std::string, RgResrcHandle > _image_resource_map;
//        std::vector< ImageDefn > _image_definitions;

        /**
         * Sampler resources
         */
//        std::unordered_map< std::string, RgResrcHandle > _sampler_resource_map;
//        std::vector< SamplerDefn > _sampler_definitions;

        /**
         * compiled render passes read for executions
         */
        std::vector< RenderPass* > _compiled_pass_execution_plan;

        /**
         * all the available render passes
         */
        std::vector< RenderPass > _render_passes;

        kege::Graphics* _graphics;

        friend RenderPass;
    };

}

#endif /* render_graph_hpp */
