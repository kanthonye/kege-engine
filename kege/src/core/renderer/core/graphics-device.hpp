//
//  graphics-device.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/24/25.
//

#ifndef graphics_device_hpp
#define graphics_device_hpp

#include "graphics-core.hpp"
#include "swapchain.hpp"
#include "graphics-physical-device.hpp"
#include "shader-pipeline-manager.hpp"
#include "shader-resource-manager.hpp"

namespace kege{
    
    /**
     * @brief Structure containing parameters for graphics device initialization.
     *
     * Used to configure the creation and behavior of the graphics device.
     * Provides hints and requirements for the implementation to select
     * appropriate backend API and hardware capabilities.
     */
    struct DeviceInitializationInfo
    {
        /**
         * @brief Native platform-specific window handle
         *
         * Examples:
         * - Windows: HWND
         * - X11: xcb_window_t
         * - macOS: CAMetalLayer*
         *
         * This requires platform-specific code to properly set up.
         */
        GraphicsWindow* window = nullptr;

        /**
         * @brief Preferred graphics API to initialize.
         *
         * Defaults to Vulkan if available. Can be set to:
         * - GraphicsAPI::Vulkan
         * - GraphicsAPI::Direct3D12
         * - GraphicsAPI::Metal
         * - GraphicsAPI::Auto (let implementation choose)
         *
         * @note The implementation may fall back to a different API if the
         * requested one isn't available.
         */
        GraphicsAPI preferred_API = GraphicsAPI::Vulkan;

        /**
         * @brief Enable API validation layers and debug checks.
         *
         * When true:
         * - Enables validation layers (Vulkan)
         * - Turns on debug device creation (D3D12)
         * - Enables Metal validation (Metal)
         *
         * @note Should typically be enabled in debug builds and disabled
         * in release builds for performance.
         */
        bool enable_debug_validation = false;
        bool enable_debug_performance = false;
        bool enable_debug_general = false;

        /**
         * @brief Minimum required dedicated video memory (in MB).
         *
         * Used as a hint for adapter selection. Set to 0 for no minimum.
         */
        uint32_t min_dedicated_video_memory = 0;

        /**
         * @brief Prefer discrete GPU over integrated graphics.
         *
         * When true, the implementation will try to select:
         * - Discrete NVIDIA/AMD GPUs (PC)
         * - Apple Silicon GPU with dedicated memory (Mac)
         */
        bool prefer_discrete_gpu = true;

        /**
         * @brief Enable support for ray tracing features.
         *
         * When true, the implementation will try to select:
         * - Hardware with ray tracing capabilities
         * - Enable appropriate extensions/feature levels
         */
        bool enable_raytracing = false;

        /**
         * @brief Enable cross-API shader compatibility mode.
         *
         * When true:
         * - Uses SPIR-V for Vulkan
         * - Uses DXIL for Direct3D12
         * - Attempts to use common feature subsets
         */
        bool compatibility_mode = false;

        /**
         * @brief Additional backend-specific initialization flags.
         *
         * Bitmask that can contain:
         * - API-specific feature flags
         * - Debugging options
         * - Special initialization modes
         */
        uint64_t backend_flags = 0;


        std::string name, engine;


        // --- New Requirements ---
        bool require_shader_float64 = false;
        bool require_geometry_shader = false;
        bool require_tessellation_shader = false;
        // Add more required features as needed (e.g., wideLines, largePoints)

        // --- New Preferences (influence scoring) ---
        bool prefer_shader_float64 = false; // Score higher if available, even if not required
        bool prefer_geometry_shader = false;
        bool prefer_tessellation_shader = false;
        bool prefer_max_vram = true;        // Give more weight to VRAM score
        bool prefer_high_compute_limits = true; // Score based on compute limits
        bool prefer_higher_api_version = true;  // Score based on Vulkan API version
    };


   // --- The Device Interface ---

   /**
    * @brief Abstract interface representing a graphics device and its capabilities.
    *
    * Provides cross-API abstraction for GPU resource management, command execution,
    * and synchronization. Implementations should support at least one graphics API
    * (Vulkan, Direct3D, Metal, etc.).
    */
    class GraphicsDevice : public RefCounter
    {
    public:

        virtual bool submit( const std::vector< kege::SubmitInfo >& submit_infos, kege::Swapchain* swapchain ) = 0;
        virtual bool submit( const kege::SubmitInfo& submit_info ) = 0;
        virtual bool present( kege::Swapchain* swapchain ) = 0;
        virtual int  getFrameIndex()const = 0;
        virtual bool beginSubmit() = 0;
        virtual void endSubmit() = 0;

        //-------------------------------------------------------------------------
        // CommandBuffer Life Cycle
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a command buffer for recording commands.
         * @param type The type of queue the command buffer will be submitted to.
         * @return Pointer to the created command buffer, or nullptr on failure.
         */
        virtual CommandBuffer* createCommandBuffer( QueueType type ) = 0;

        /**
         * @brief Destroys a command buffer.
         * @param cmb Pointer to the command buffer to destroy.
         * @warning Ensure the command buffer is no longer in use.
         */
        virtual void destroyCommandBuffer( kege::CommandBuffer* cmb ) = 0;

        //-------------------------------------------------------------------------
        // Shader Pipeline Life Cycle
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a shader module from source or bytecode.
         * @param desc Shader description including code and entry point.
         * @return Handle to the created shader, or invalid handle on failure.
         */
        virtual kege::ShaderHandle createShader( const kege::ShaderDesc& desc ) = 0;

        /**
         * @brief Destroys a shader module.
         * @param handle Handle to the shader to destroy.
         * @warning Ensure no pipelines are using this shader.
         */
        virtual void destroyShader( kege::ShaderHandle handle ) = 0;

        //-------------------------------------------------------------------------
        // Shader Pipeline Life Cycle
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a pipeline layout defining resource bindings.
         * @param desc Description of push constants and descriptor set layouts.
         * @return Handle to the created pipeline layout.
         */
        virtual kege::PipelineLayoutHandle createPipelineLayout( const kege::PipelineLayoutDesc& desc ) = 0;

        /**
         * @brief Creates a graphics rendering pipeline.
         * @param desc Complete graphics pipeline state description.
         * @return Handle to the created pipelines.
         */
        virtual std::vector< PipelineHandle > createGraphicsPipeline( const CreateShaderPipelineInfo& desc ) = 0;

        /**
         * @brief Creates a compute pipeline.
         * @param desc Compute pipeline description including shader and layout.
         * @return Handle to the created compute pipeline.
         */
        virtual kege::PipelineHandle createComputePipeline( const kege::ComputePipelineDesc& desc ) = 0;

        /**
         * @brief Destroys a pipeline layout.
         * @param handle Handle to the pipeline layout to destroy.
         * @warning Ensure no pipelines are using this layout.
         */
        virtual void destroyPipelineLayout( kege::PipelineLayoutHandle handle ) = 0;

        /**
         * @brief Destroys a graphics pipeline.
         * @param handle Handle to the pipeline to destroy.
         * @warning Ensure the pipeline is not in use by any command buffers.
         */
        virtual void destroyGraphicsPipeline( kege::PipelineHandle handle ) = 0;

        /**
         * @brief Destroys a compute pipeline.
         * @param handle Handle to the pipeline to destroy.
         * @warning Ensure the pipeline is not in use by any command buffers.
         */
        virtual void destroyComputePipeline( kege::PipelineHandle handle ) = 0;

        //-------------------------------------------------------------------------
        // Image Life Cycle
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a texture resource.
         * @param desc Texture description including dimensions, format, and usage.
         * @return Handle to the created texture, or invalid handle on failure.
         */
        virtual kege::ImageHandle createImage( const kege::ImageDesc& desc ) = 0;

        /**
         * @brief Destroys a texture resource.
         * @param handle Handle to the texture to destroy.
         * @warning Ensure the texture is no longer in use by the GPU.
         */
        virtual void destroyImage( kege::ImageHandle handle ) = 0;

        //-------------------------------------------------------------------------
        // Sampler Life Cycle
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a texture sampler.
         * @param desc Sampler description including filtering and addressing modes.
         * @return Handle to the created sampler, or invalid handle on failure.
         */
        virtual kege::SamplerHandle createSampler( const kege::SamplerDesc& desc ) = 0;

        /**
         * @brief Destroys a sampler resource.
         * @param handle Handle to the sampler to destroy.
         */
        virtual void destroySampler( kege::SamplerHandle handle ) = 0;

        //-------------------------------------------------------------------------
        // Buffer Life Cycle
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a buffer resource (vertex, index, uniform, etc.).
         * @param desc Buffer description including size and usage flags.
         * @return Handle to the created buffer, or invalid handle on failure.
         */
        virtual kege::BufferHandle createBuffer( const kege::BufferDesc& desc ) = 0;
        virtual void updateBuffer( const BufferHandle& handle, uint64_t offset, uint64_t size, const void* data ) = 0;
        virtual bool resizeBuffer( const BufferHandle& handle, uint64_t size ) = 0;

        /**
         * @brief Destroys a buffer resource.
         * @param handle Handle to the buffer to destroy.
         * @warning Ensure the buffer is no longer in use by the GPU.
         */
        virtual void destroyBuffer( kege::BufferHandle handle ) = 0;
        virtual void destroyBufferView( kege::BufferViewHandle handle ) = 0;

        /**
         * @brief Maps buffer memory for CPU access.
         * @param handle Handle to the buffer to map.
         * @param offset Byte offset into the buffer.
         * @param size Number of bytes to map (or -1 for entire buffer).
         * @return Pointer to mapped memory, or nullptr on failure.
         * @note Must call unmapBuffer when done.
        */
        virtual void* mapBuffer( kege::BufferHandle handle, size_t offset = 0, size_t size = -1) = 0;

        /**
         * @brief Unmaps previously mapped buffer memory.
         * @param handle Handle to the buffer to unmap.
         */
        virtual void unmapBuffer( kege::BufferHandle handle ) = 0;

        virtual size_t bufferSize( const BufferHandle& handle ) = 0;

        //-------------------------------------------------------------------------
        // UniformSetLayout Life Cycle
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a descriptor set layout.
         * @param descriptors Description of binding points for resources.
         * @return Handle to the created descriptor set layout.
         */
        virtual UniformSetLayout createUniformSetLayout( const UniformDescriptors& descriptors ) = 0;

        /**
         * @brief Retrieves or creates a descriptor set layout based on bindings.
         * @param descriptors Description of binding points for resources.
         * @return Handle to the descriptor set layout.
         */
        virtual UniformSetLayout getUniformSetLayout( const UniformDescriptors& descriptors ) = 0;

        /**
         * @brief Destroys a descriptor set layout.
         * @param layout Handle to the layout to destroy.
         * @warning Ensure no descriptor sets or pipelines are using this layout.
         */
        virtual void destroyUniformSetLayout( const UniformSetLayout& layout ) = 0;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
        // Shader Resource Set Lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

        virtual int  makeSet( const UniformDescriptorSet& descriptors, const UniformResourceSet& resources ) = 0;
        virtual bool updateSet( int handle, const UniformResourceSet& resources ) = 0;
        virtual int  allocateSet( const UniformDescriptorSet& descriptors ) = 0;
        virtual void freeSet( int set ) = 0;

        //-------------------------------------------------------------------------
        // Descriptor Set Lifecycle
        //-------------------------------------------------------------------------

        /**
         * @brief Update multiple descriptor sets with new resource bindings.
         * @param handles Vector of descriptor set handles to update.
         * @param resource_sets Vector of resource sets containing the new bindings.
         * @return True if all updates succeeded, false otherwise.
         */
        virtual bool updateUniformSets( const std::vector< int >& handles, const UniformSets& resource_sets ) = 0;

        /**
         * @brief Update a single descriptor set with new resource bindings.
         * @param handle Handle of the descriptor set to update.
         * @param resource_set Resource set containing the new bindings.
         * @return True if the update succeeded, false otherwise.
         */
        virtual bool updateUniformSet( int handle, const UniformSet& resource_set ) = 0;

        /**
         * @brief Allocate multiple descriptor sets from layouts.
         * @param description Descriptions of the descriptor set layouts to allocate from.
         * @return Vector of handles to the newly allocated descriptor sets.
         */
        virtual std::vector< int > allocateUniformSets( const UniformSetsDesc& description ) = 0;

        /**
         * @brief Allocate a single descriptor set from a layout.
         * @param description Description of the descriptor set layout to allocate from.
         * @return Handle to the newly allocated descriptor set.
         */
        virtual int allocateUniformSet( const UniformSetDesc& description ) = 0;

        //-------------------------------------------------------------------------
        // CommandBuffer Life Cycle
        //-------------------------------------------------------------------------

        // --- Synchronization ---

        /**
         * @brief Creates a semaphore for GPU-GPU synchronization.
         * @return Handle to the created semaphore.
         */
        virtual kege::Ref< kege::Semaphore > createSemaphore() = 0;

        /**
         * @brief Destroys a semaphore.
         * @param semaphore Handle to the semaphore to destroy.
         * @warning Ensure the semaphore is no longer in use.
         */
        virtual void destroySemaphore( kege::Semaphore* semaphore ) = 0;

        /**
         * @brief Creates a fence for CPU-GPU synchronization.
         * @param initially_signaled Whether the fence starts in signaled state.
         * @return Handle to the created fence.
         */
        virtual kege::Ref< kege::Fence > createFence( bool initially_signaled = false ) = 0;

        /**
         * @brief Destroys a fence.
         * @param fence Handle to the fence to destroy.
         * @warning Ensure the fence is no longer in use.
         */
        virtual void destroyFence( kege::Fence* fence ) = 0;

        // --- Swapchain ---

        /**
         * @brief Creates a swapchain for presentation.
         * @param desc Swapchain description including format and size.
         * @return Handle to the created swapchain.
         */
        virtual kege::Swapchain* createSwapchain( const kege::SwapchainDesc& desc ) = 0;

        /**
         * @brief Destroys a swapchain.
         * @param swapchain Pointer to the swapchain to destroy.
         * @warning Ensure no images from this swapchain are in use.
         */
        virtual void destroySwapchain( kege::Swapchain* swapchain ) = 0;

        /**
         * @brief Waits for the device to complete all outstanding operations.
         * @note This is a heavyweight operation - use sparingly.
         */
        virtual void waitIdle() = 0;

        /**
         * @brief Get the graphics API implemented by this device
         * @return Always returns GraphicsAPI::Vulkan
         */
        kege::GraphicsAPI getCurrentAPI() const { return _api; }

        /**
         * @brief Get the enabled device features
         * @return Reference to the device features structure
         */
        const kege::DeviceFeatures& getFeatures() const { return _features; }

        /**
         * @brief Get the physical device limits
         * @return Reference to the device limits structure
         */
        const kege::DeviceLimits& getLimits() const { return _limits; }

        /**
         * @brief Shuts down the graphics device and releases all resources.
         * @warning Must be called before destruction to ensure proper cleanup.
         */
        virtual void shutdown() = 0;

        uint32_t id()const{ return _id; }

    protected:

        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         */
        virtual ~GraphicsDevice() = default;

        GraphicsDevice(): _id(0) {}

    protected:

        /** @brief Enabled device features */
        DeviceFeatures _features = {};

        /** @brief Physical device limits */
        DeviceLimits _limits = {};

        kege::GraphicsAPI _api;
        
        uint32_t _id;
   };

}
#endif /* graphics_device_hpp */
