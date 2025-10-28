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
#include "shader-layout.hpp"
#include "shader-resource-manager.hpp"
#include "shader-pipeline-manager.hpp"
#include "command-encoder.hpp"
#include "command-buffer.hpp"


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

        bool submit
        (
            const std::vector< kege::SubmitInfo >& submit_infos,
            const ref::Semaphore& image_available,
            const ref::Semaphore& render_complete
        );

        bool submit( const kege::SubmitInfo& submit_info );

        bool present( kege::Swapchain* swapchain, const ref::Semaphore& wait_sem );
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // CommandBuffer lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        /**
         * @brief Creates a command buffer for recording commands.
         * @param type The type of queue the command buffer will be submitted to.
         * @return Pointer to the created command buffer, or nullptr on failure.
         */
        CommandBuffer* createCommandBuffer( QueueType type );

        /**
         * @brief Destroys a command buffer.
         * @param command_buffer Pointer to the command buffer to destroy.
         * @warning Ensure the command buffer is no longer in use.
         */
        void destroyCommandBuffer(CommandBuffer* command_buffer);

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // Image lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        /**
         * @brief Creates a texture resource.
         * @param desc Texture description including dimensions, format, and usage.
         * @return Handle to the created texture, or invalid handle on failure.
         */
        ref::Image createImage(const Image::Desc& desc);

        /**
         * @brief Destroys a texture resource.
         * @param handle Handle to the texture to destroy.
         * @warning Ensure the texture is no longer in use by the GPU.
         */
        void destroyImage(ref::Image handle);

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // Sampler lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        /**
         * @brief Creates a texture sampler.
         * @param desc Sampler description including filtering and addressing modes.
         * @return Handle to the created sampler, or invalid handle on failure.
         */
        ref::Sampler createSampler(const SamplerDesc& desc);

        /**
         * @brief Destroys a sampler resource.
         * @param handle Handle to the sampler to destroy.
         */
        void destroySampler(ref::Sampler handle); //TODO: deprecated

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // Buffer lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        /**
         * @brief Creates a buffer resource (vertex, index, uniform, etc.).
         * @param desc Buffer description including size and usage flags.
         * @return Handle to the created buffer, or invalid handle on failure.
         */
        ref::Buffer createBuffer( const BufferDesc& desc );

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // Shader lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        /**
         * @brief Creates a shader module from source or bytecode.
         * @param desc Shader description including code and entry point.
         * @return Handle to the created shader, or invalid handle on failure.
         */
        ShaderHandle createShader(const ShaderDesc& desc);

        /**
         * @brief Destroys a shader module.
         * @param handle Handle to the shader to destroy.
         * @warning Ensure no pipelines are using this shader.
         */
        void destroyShader(ShaderHandle handle);

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // Pipeline lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        /**
         * @brief Creates a pipeline layout defining resource bindings.
         * @param desc Description of push constants and descriptor set layouts.
         * @return Handle to the created pipeline layout.
         */
        PipelineLayoutHandle createPipelineLayout(const PipelineLayoutDesc& desc);

        /**
         * @brief Destroys a pipeline layout.
         * @param handle Handle to the pipeline layout to destroy.
         * @warning Ensure no pipelines are using this layout.
         */
        void destroyPipelineLayout(PipelineLayoutHandle handle);

        std::vector< PipelineHandle > createGraphicsPipeline( const CreateShaderPipelineInfo& desc );

        /**
         * @brief Destroys a graphics pipeline.
         * @param handle Handle to the pipeline to destroy.
         * @warning Ensure the pipeline is not in use by any command buffers.
         */
        void destroyGraphicsPipeline(PipelineHandle handle);

        /**
         * @brief Creates a compute pipeline.
         * @param desc Compute pipeline description including shader and layout.
         * @return Handle to the created compute pipeline.
         */
        PipelineHandle createComputePipeline(const ComputePipelineDesc& desc);

        /**
         * @brief Destroys a compute pipeline.
         * @param handle Handle to the pipeline to destroy.
         * @warning Ensure the pipeline is not in use by any command buffers.
         */
        void destroyComputePipeline(PipelineHandle handle);

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // UniformSetLayout lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        /**
         * @brief Creates a descriptor set layout.
         * @param descriptors Description of binding points for resources.
         * @return Handle to the created descriptor set layout.
         */
        UniformSetLayout createUniformSetLayout( const UniformDescriptors& descriptors );

        /**
         * @brief Retrieves or creates a descriptor set layout based on bindings.
         * @param descriptors Description of binding points for resources.
         * @return Handle to the descriptor set layout.
         */
        UniformSetLayout getUniformSetLayout( const UniformDescriptors& descriptors );

        /**
         * @brief Destroys a descriptor set layout.
         * @param layout Handle to the layout to destroy.
         * @warning Ensure no descriptor sets or pipelines are using this layout.
         */
        void destroyUniformSetLayout( const UniformSetLayout& layout );

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
        // Shader Resource Set Lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

        int  makeSet( const UniformDescriptorSet& descriptors, const UniformResourceSet& resources );
        bool updateSet( int handle, const UniformResourceSet& resources );
        int  allocateSet( const UniformDescriptorSet& descriptors );
        void freeSet( int set );

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
        // kege::Fence Objects Lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

        /**
         * @brief Creates a fence for CPU-GPU synchronization.
         * @param initially_signaled Whether the fence starts in signaled state.
         * @return Handle to the created fence.
         */
        ref::Fence createFence( bool initially_signaled = false );

        /**
         * @brief Destroys a fence.
         * @param fence Handle to the fence to destroy.
         * @warning Ensure the fence is no longer in use.
         */
        void destroyFence( kege::Fence* fence );

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // Semaphore lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        /**
         * @brief Creates a semaphore for GPU-GPU synchronization.
         * @return Pointer to the created semaphore.
         */
        ref::Semaphore createSemaphore();

        /**
         * @brief Destroys a semaphore.
         * @param semaphore Handle to the semaphore to destroy.
         * @warning Ensure the semaphore is no longer in use.
         */
        void destroySemaphore( kege::Semaphore* semaphore );

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
        // Swapchain Access
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //



        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
        //
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

        kege::ShaderPipelineManager* getShaderPipelineManager();
        kege::AppWindow* getWindow();
        kege::Swapchain* getSwapchain();
        int32_t getFrameIndex()const;
        int32_t windowHeight()const;
        int32_t windowWidth()const;
        void pollWindowEvents();
        bool windowIsOpen()const;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
        //
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

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

        kege::Ref< kege::GraphicsInstance > _instance;
        kege::Ref< kege::AppWindow > _window;
        kege::GraphicsDevice* _device;

        kege::SwapchainDesc _swapchain_create_info;
        kege::Swapchain* _swapchain;

        // Map of descriptor set layout bindings to their corresponding layout handles.
        std::unordered_map< size_t, UniformSetLayout > _uniform_set_layouts;

        kege::ShaderPipelineManager _shader_pipeline_manager;

        //GraphicsAPIInfo _info;
    };
    
}
namespace kege::ref{
    typedef kege::Ref< kege::Graphics > Graphics;
}
#endif /* graphics_hpp */
