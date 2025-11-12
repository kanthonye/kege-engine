//
//  device
//  ecs
//
//  Created by Kenneth Esdaile on 4/24/25.
//

#ifndef kege_graphics_device_hpp
#define kege_graphics_device_hpp

#include "image.hpp"
#include "buffer.hpp"
#include "fence.hpp"
#include "semaphore.hpp"
#include "swapchain.hpp"
#include "physical-device.hpp"
#include "../pipeline/shader-pipeline.hpp"
#include "../../../utils/array.hpp"


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
        AppWindow* window = nullptr;

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

        //-------------------------------------------------------------------------
        // Command Submission & Presentation
        //-------------------------------------------------------------------------

        /**
         * @brief Submit command buffers and synchronization primitives to the GPU.
         * @param submit_infos Vector of SubmitInfo structures describing command buffers and sync.
         * @param image_available Semaphore signaled when an image is available for rendering.
         * @param render_complete Semaphore signaled when rendering is complete.
         * @return True if submission succeeded, false otherwise.
         */
        virtual bool submit
        (
            const std::vector< kege::SubmitInfo >& submit_infos,
            const ref::Semaphore& image_available,
            const ref::Semaphore& render_complete
        )= 0;

        /**
         * @brief Submit a single command buffer and synchronization primitives to the GPU.
         * @param submit_info SubmitInfo structure describing command buffer and sync.
         * @return True if submission succeeded, false otherwise.
         */
        virtual bool submit( const kege::SubmitInfo& submit_info ) = 0;
        
        /**
         * @brief Present the rendered image to the swapchain.
         * @param swapchain Pointer to the swapchain to present to.
         * @param wait_sem Semaphore to wait on before presentation.
         * @return True if presentation succeeded, false otherwise.
         */
        virtual bool present( kege::Swapchain* swapchain, const ref::Semaphore& wait_sem ) = 0;

        /**
         * @brief Get the current frame index for frame-in-flight management.
         * @return Current frame index.
         */
        virtual int  getFrameIndex()const = 0;

        /**
         * @brief Begin recording submissions for the current frame.
         * @return True if successful, false otherwise.
         */
        virtual bool beginSubmit() = 0;

        /**
         * @brief End recording submissions for the current frame.
         */
        virtual void endSubmit() = 0;

        /**
         * @brief Creates a command buffer for recording commands.
         * @param type The type of queue the command buffer will be submitted to.
         * @return Pointer to the created command buffer, or nullptr on failure.
         */
        virtual ref::CommandBuffer createCommandBuffer( QueueType type ) = 0;

        /**
         * @brief Creates a shader binding set layout based on a description.
         * @param bindings Description of binding points for resources.
         * @return Handle to the created shader binding set layout.
         */
        virtual ref::SetLayout createSetLayout( const LayoutBindings& bindings ) = 0;

        /**
         * @brief Creates a shader layout based on a description.
         * @param description Description of the shader layout.
         * @return Handle to the created shader layout.
         */
        virtual ref::ShaderLayout createShaderLayout( const ShaderLayoutDesc& description ) = 0;

        /**
         * @brief Creates a shader module from source or bytecode.
         * @param desc Shader description including code and entry point.
         * @return Handle to the created shader, or invalid handle on failure.
         */
        virtual ref::Shader createShader( const kege::ShaderDesc& desc ) = 0;

        /**
         * @brief Creates a compute pipeline.
         * @param desc Compute pipeline description including shader and layout.
         * @return Handle to the created compute pipeline.
         */
        virtual ref::ShaderPipeline createShaderPipeline( const kege::PipelineCreateInfo& desc ) = 0;

        /**
         * @brief Creates a texture resource.
         * @param desc Texture description including dimensions, format, and usage.
         * @return Handle to the created texture, or invalid handle on failure.
         */
        virtual ref::Image createImage( const kege::Image::Desc& desc ) = 0;

        /**
         * @brief Creates a texture sampler.
         * @param desc Sampler description including filtering and addressing modes.
         * @return Handle to the created sampler, or invalid handle on failure.
         */
        virtual ref::Sampler createSampler( const kege::SamplerDesc& desc ) = 0;

        /**
         * @brief Creates a buffer resource (vertex, index, uniform, etc.).
         * @param desc Buffer description including size and usage flags.
         * @return Handle to the created buffer, or invalid handle on failure.
         */
        virtual ref::Buffer createBuffer( const kege::BufferDesc& desc ) = 0;

        /**
         * @brief Creates a semaphore for GPU-GPU synchronization.
         * @return Handle to the created semaphore.
         */
        virtual ref::Semaphore createSemaphore() = 0;

        /**
         * @brief Creates a fence for CPU-GPU synchronization.
         * @param initially_signaled Whether the fence starts in signaled state.
         * @return Handle to the created fence.
         */
        virtual ref::Fence createFence( bool initially_signaled = false ) = 0;

        /**
         * @brief Creates a swapchain for presentation.
         * @param desc Swapchain description including format and size.
         * @return Handle to the created swapchain.
         */
        virtual ref::Swapchain createSwapchain( const kege::SwapchainDesc& desc ) = 0;

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

        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         */
        virtual ~GraphicsDevice() = default;

    protected:

        virtual vk::Device* vk(){ return nullptr; }

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
namespace kege::ref{
    typedef kege::Ref< kege::GraphicsDevice > GraphicsDevice;
}
#endif /* kege_graphics_device_hpp */
