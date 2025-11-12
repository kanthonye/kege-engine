//
//  graphics.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//

#ifndef graphics_hpp
#define graphics_hpp


#include "glfw-window.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "../pipeline/shader-layout.hpp"
#include "command-encoder.hpp"
#include "command-buffer.hpp"
#include "instance.hpp"
#include "device.hpp"
#include "swapchain.hpp"


namespace kege{

    struct GraphicsAPIInfo
    {
        kege::GraphicsAPI api;

        int  width;
        int  height;
        const char* title;
        bool fullscreen;
        bool vsync;

        bool enable_raytracing;

        bool require_shader_float64 = false;
        bool require_geometry_shader = false;
        bool require_tessellation_shader = false;

        kege::Format color_format = kege::Format::bgra_u8_norm_srbg;
        kege::Format depth_format = kege::Format::depth_32;
        int frames_in_flight = 2;
    };


    class Graphics : public kege::RefCounter
    {
    public:

        static std::vector< uint32_t > compileGlslToSpv
        (
            const char* shader_name,
            ShaderStageFlag shader_stage,
            const std::vector< char >& source
        );
        
        bool submit
        (
            const std::vector< kege::SubmitInfo >& submit_infos,
            const ref::Semaphore& image_available,
            const ref::Semaphore& render_complete
        );

        bool submit( const kege::SubmitInfo& submit_info );

        bool present( kege::Swapchain* swapchain, const ref::Semaphore& wait_sem );

        /**
         * @brief Creates a command buffer for recording commands.
         * @param type The type of queue the command buffer will be submitted to.
         * @return Pointer to the created command buffer, or nullptr on failure.
         */
        ref::CommandBuffer createCommandBuffer( QueueType type );

        /**
         * @brief Creates a shader binding set layout based on a description.
         * @param config Description of binding points for resources.
         * @return Handle to the created shader binding set layout.
         */
        ref::SetLayout createSetLayout( const LayoutBindings& config );

        /**
         * @brief Creates a shader binding set layout based on a description.
         * @param desc Description of binding points for resources.
         * @return Handle to the created shader binding set layout.
         */
        ref::ShaderLayout createShaderLayout( const ShaderLayoutDesc& desc );

        /**
         * @brief Creates a shader pipeline.
         * @param create_info Compute pipeline description including shader and layout.
         * @return Handle to the created compute pipeline.
         */
        ref::ShaderPipeline createShaderPipeline( const kege::PipelineCreateInfo& create_info );

        /**
         * @brief Creates a shader module from source or bytecode.
         * @param desc Shader description including code and entry point.
         * @return Handle to the created shader, or invalid handle on failure.
         */
        ref::Shader createShader(const ShaderDesc& desc);

        /**
         * @brief Creates a texture resource.
         * @param desc Texture description including dimensions, format, and usage.
         * @return Handle to the created texture, or invalid handle on failure.
         */
        ref::Image createImage(const Image::Desc& desc);

        /**
         * @brief Creates a texture sampler.
         * @param desc Sampler description including filtering and addressing modes.
         * @return Handle to the created sampler, or invalid handle on failure.
         */
        ref::Sampler createSampler(const SamplerDesc& desc);

        /**
         * @brief Creates a buffer resource (vertex, index, uniform, etc.).
         * @param desc Buffer description including size and usage flags.
         * @return Handle to the created buffer, or invalid handle on failure.
         */
        ref::Buffer createBuffer( const BufferDesc& desc );

        /**
         * @brief Creates a fence for CPU-GPU synchronization.
         * @param initially_signaled Whether the fence starts in signaled state.
         * @return Handle to the created fence.
         */
        ref::Fence createFence( bool initially_signaled = false );

        /**
         * @brief Creates a semaphore for GPU-GPU synchronization.
         * @return Pointer to the created semaphore.
         */
        ref::Semaphore createSemaphore();

        kege::Swapchain* getSwapchain();
        kege::AppWindow* getWindow();
        int32_t getFrameIndex()const;

        bool beginFrame();
        void endFrame();

        bool initalize
        (
            const kege::DeviceInitializationInfo& device_init_info,
            const kege::SwapchainDesc& swapchain_create_info
        );

        void shutdown();

        Graphics();
        ~Graphics();

    private:

        kege::SwapchainDesc _swapchain_create_info;

        ref::GraphicsInstance _instance;
        ref::AppWindow _window;

        ref::GraphicsDevice _device;
        ref::Swapchain _swapchain;

        // Map of descriptor set layout bindings to their corresponding layout handles.
        std::unordered_map< size_t, UniformSetLayout > _uniform_set_layouts;

        //kege::ShaderPipelineManager _shader_pipeline_manager;

        //GraphicsAPIInfo _info;
    };
    
}
namespace kege::ref{
    typedef kege::Ref< kege::Graphics > Graphics;
}
#endif /* graphics_hpp */
