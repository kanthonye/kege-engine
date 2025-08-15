//
//  render-graph.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#ifndef render_graph_hpp
#define render_graph_hpp

#include "../../graphics/graph/render-pass-context.hpp"

namespace kege{

    struct RenderPassSetupContext
    {
        std::function< void( RenderGraph* ) > add_resources;
        std::function< void( RenderGraph* ) > add_passes;
    };

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
         * @brief Defines a new shader resource in the render graph.
         * @param name Unique name for the resource.
         * @param frames_in_flight Number of frames the resource should persist.
         * @param bindings Descriptor set layout bindings for the resource.
         * @return Handle to the created resource.
         */
        kege::RgResrcHandle defineShaderResource
        (
            const std::string& name,
            int frames_in_flight,
            const std::vector<kege::DescriptorSetLayoutBinding>& bindings
        );

        /**
         * @brief Updates an existing shader resource's bindings.
         * @param handle Handle to the resource to update.
         * @param bindings New bindings for the resource.
         * @return True if update was successful, false otherwise.
         */
        bool updateShaderResource
        (
            kege::RgResrcHandle handle,
            const std::vector<RgShaderResource>& bindings
        );

        /// @name Physical Resource Access
        /// @{
        /**
         * @brief Gets the physical descriptor set for a resource.
         * @param handle Resource handle.
         * @return The descriptor set handle.
         */
        kege::DescriptorSetHandle getPhysicalDescriptorSet
        (
            const RgResrcHandle& handle
        );

        /**
         * @brief Gets the physical descriptor set by resource name.
         * @param name Resource name.
         * @return The descriptor set handle.
         */
        kege::DescriptorSetHandle getPhysicalDescriptorSet
        (
            const std::string& name
        );

        /**
         * @brief Gets the physical buffer for a resource.
         * @param handle Resource handle.
         * @return The buffer handle.
         */
        kege::BufferHandle getPhysicalBuffer
        (
            const RgResrcHandle& handle
        );

        /**
         * @brief Gets the physical image for a resource.
         * @param handle Resource handle.
         * @return The image handle.
         */
        kege::ImageHandle getPhysicalImage
        (
            const RgResrcHandle& handle
        );
        /// @}

        /// @name Frames In Flight Resource Access
        /// @{
        ///
        /**
         * @brief Gets all physical buffers flight-in-frame.
         */
        const std::vector< kege::BufferHandle >& getPhysicalBuffers( const RgResrcHandle& handle )const;

        /**
         * @brief Gets all physical images flight-in-frame.
         */
        const std::vector< kege::ImageHandle >& getPhysicalImages( const RgResrcHandle& handle )const;

        /**
         * @brief Gets all physical buffers flight-in-frame by name.
         */
        const std::vector< kege::BufferHandle >* getPhysicalBuffers( const std::string& name );

        /**
         * @brief Gets all physical images flight-in-frame by name.
         */
        const std::vector< kege::ImageHandle >* getPhysicalImages( const std::string& name );
        /// @}

        /**
         * @brief Gets all physical sampler handle.
         */
        const kege::SamplerHandle& getPhysicalSampler( const RgResrcHandle& handle )const;

        /**
         * @brief Gets all physical sampler by name.
         */
        const kege::SamplerHandle* getPhysicalSampler( const std::string& name );

        /// @name Resource Import
        /// @{
        /**
         * @brief Imports an existing buffer into the render graph.
         * @param name Unique name for the resource.
         * @param handles Buffer handles to import.
         * @return Handle to the imported resource.
         */
        RgResrcHandle importBuffer
        (
            std::string name,
            const std::vector<kege::BufferHandle>& handles
        );

        /**
         * @brief Imports an existing image into the render graph.
         * @param name Unique name for the resource.
         * @param handles Image handles to import.
         * @return Handle to the imported resource.
         */
        RgResrcHandle importImage
        (
            std::string name,
            const std::vector<kege::ImageHandle>& handles
        );

        /**
         * @brief Imports an existing sampler into the render graph.
         * @param name Unique name for the resource.
         * @param handle Sampler handle to import.
         * @return Handle to the imported resource.
         */
        RgResrcHandle importSampler
        (
            std::string name,
            const kege::SamplerHandle& handle
        );
        /// @}

        /// @name Resource Definition
        /// @{
        /**
         * @brief Defines a new sampler resource.
         * @param name Unique name for the sampler.
         * @param desc Sampler description.
         * @return Handle to the created sampler.
         */
        RgResrcHandle defineSampler
        (
            std::string name,
            const kege::SamplerDesc& desc
        );

        /**
         * @brief Defines a new buffer resource.
         * @param defn Buffer definition parameters.
         * @return Handle to the created buffer.
         */
        RgResrcHandle defineBuffer( const BufferDefn& defn );

        /**
         * @brief Defines a new image resource.
         * @param defn Image definition parameters.
         * @return Handle to the created image.
         */
        RgResrcHandle defineImage( const ImageDefn& defn );
        /// @}

        /// @name Resource Lookup
        /// @{
        RgResrcHandle getSamplerRgResrc(const std::string& name);
        RgResrcHandle getBufferRgResrc(const std::string& name);
        RgResrcHandle getImageRgResrc(const std::string& name);
        /// @}

        /// @name Pass Management
        /// @{
        /**
         * @brief Adds a graphics pass to the render graph.
         * @param definition Parameters defining the graphics pass.
         */
        void addGraphicsPass(const RenderPassDefn& definition);

        /**
         * @brief Adds a compute pass to the render graph.
         * @param definition Parameters defining the compute pass.
         */
        void addComputePass(const RenderPassDefn& definition);

        /**
         * @brief Adds a render pass using setup context.
         * @param context Setup context for the pass.
         */
        void add(RenderPassSetupContext context);
        /// @}

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
        void execute();

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

        /// @name Internal Implementation
        /// @{
        /**
         * @brief Assign the required resources to each context of the given RenderPasses.
         * @param sorted_passes The RenderPasses that needs their physical resources initialized.
         */
        void setupRenderPassContext( std::vector<RenderPass*>& sorted_passes );

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

        /**
         * @brief Update all shader resources.
         */
        bool updateShaderResources();

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

        std::vector< RenderPassSetupContext > _render_pass_setup_context;

        /**
         * Shader resources
         */
        std::unordered_map< std::string, RgResrcHandle > _shader_resrc_map;
        std::vector< RgShaderResourceDefn > _shader_resrc_definitions;

        /**
         * Buffer resources
         */
        std::unordered_map< std::string, RgResrcHandle > _buffer_resource_map;
        std::vector< BufferDefn > _buffer_definitions;

        /**
         * Image resources
         */
        std::unordered_map< std::string, RgResrcHandle > _image_resource_map;
        std::vector< ImageDefn > _image_definitions;

        /**
         * Sampler resources
         */
        std::unordered_map< std::string, RgResrcHandle > _sampler_resource_map;
        std::vector< SamplerDefn > _sampler_definitions;

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
