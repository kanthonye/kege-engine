//
//  graphics.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//

#ifndef graphics_hpp
#define graphics_hpp


#include "../devices/vulkan/vulkan-instance.hpp"
#include "../devices/vulkan/vulkan-device.hpp"
#include "../../graphics/core/glfw-window.hpp"
#include "../../graphics/core/shader-resource-manager.hpp"
#include "../../graphics/core/shader-pipeline-manager.hpp"

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

        void submitCommands( std::vector< CommandBuffer* > command_buffers );

        /**
         * @brief Creates a texture resource.
         * @param desc Texture description including dimensions, format, and usage.
         * @return Handle to the created texture, or invalid handle on failure.
         */
        ImageHandle createImage(const ImageDesc& desc);

        /**
         * @brief Destroys a texture resource.
         * @param handle Handle to the texture to destroy.
         * @warning Ensure the texture is no longer in use by the GPU.
         */
        void destroyImage(ImageHandle handle);

        /**
         * @brief Creates a buffer resource (vertex, index, uniform, etc.).
         * @param desc Buffer description including size and usage flags.
         * @return Handle to the created buffer, or invalid handle on failure.
         */
        BufferHandle createBuffer(const BufferDesc& desc);
        void updateBuffer( const BufferHandle& handle, uint64_t offset, uint64_t size, const void* data );
        void unmapBuffer( const BufferHandle& handle );
        void* mapBuffer( const BufferHandle& handle, size_t offset = 0, size_t size = 0 );

        /**
         * @brief Destroys a buffer resource.
         * @param handle Handle to the buffer to destroy.
         * @warning Ensure the buffer is no longer in use by the GPU.
         */
        void destroyBuffer(BufferHandle handle);

        /**
         * @brief Creates a texture sampler.
         * @param desc Sampler description including filtering and addressing modes.
         * @return Handle to the created sampler, or invalid handle on failure.
         */
        SamplerHandle createSampler(const SamplerDesc& desc);

        /**
         * @brief Destroys a sampler resource.
         * @param handle Handle to the sampler to destroy.
         */
        void destroySampler(SamplerHandle handle);

        void insertPipeline(const std::string& name, const PipelineHandle& handle);

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

        /**
         * @brief Creates a graphics rendering pipeline.
         * @param desc Complete graphics pipeline state description.
         * @return Handle to the created pipeline.
         */
        PipelineHandle createGraphicsPipeline(const GraphicsPipelineDesc& desc);

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

        /**
         * @brief Creates a descriptor set layout.
         * @param bindings Description of binding points for resources.
         * @return Handle to the created descriptor set layout.
         */
        DescriptorSetLayoutHandle createDescriptorSetLayout( const std::vector<DescriptorSetLayoutBinding>& bindings );

        /**
         * @brief Destroys a descriptor set layout.
         * @param handle Handle to the layout to destroy.
         * @warning Ensure no descriptor sets or pipelines are using this layout.
         */
        void destroyDescriptorSetLayout(DescriptorSetLayoutHandle handle);

        /**
         * @brief Creates a descriptor set.
         * @param bindings Description of binding points for resources.
         * @return Handle to the created descriptor set.
         */
        DescriptorSetHandle allocateDescriptorSet( const std::vector<DescriptorSetLayoutBinding>& bindings );

        DescriptorSetHandle allocateDescriptorSet( const DescriptorSetLayoutHandle& layout );
        DescriptorSetHandle allocateDescriptorSet( const DescriptorSetAllocateInfo& info );


        /**
         * @brief Frees a descriptor set.
         * @param handle Handle to the descriptor set to free.
         * @note The descriptor set handle becomes invalid after this call.
         */
        void freeDescriptorSet(DescriptorSetHandle handle);

        /**
         * @brief Allocates a descriptor set from a layout.
         * @param layout Layout describing the descriptor set structure.
         * @return Handle to the allocated descriptor set.
         * @note Consider using a descriptor pool for better allocation management.
         */
        DescriptorSetHandle allocateDescriptorSet(DescriptorSetLayoutHandle layout);

        bool updateDescriptorSets( const std::vector< kege::WriteDescriptorSet >& writes );

        /**
         * @brief Creates a fence for CPU-GPU synchronization.
         * @param initiallySignaled Whether the fence starts in signaled state.
         * @return Handle to the created fence.
         */
        FenceHandle createFence(bool initiallySignaled = false);

        /**
         * @brief Destroys a fence.
         * @param handle Handle to the fence to destroy.
         * @warning Ensure the fence is no longer in use.
         */
        void destroyFence(FenceHandle handle);

        /**
         * @brief Creates a semaphore for GPU-GPU synchronization.
         * @return Handle to the created semaphore.
         */
        SemaphoreHandle createSemaphore();

        /**
         * @brief Destroys a semaphore.
         * @param handle Handle to the semaphore to destroy.
         * @warning Ensure the semaphore is no longer in use.
         */
        void destroySemaphore(SemaphoreHandle handle);

        ImageHandle getSwapchainColorImage( uint32_t image_index );
        ImageHandle getSwapchainDepthImage( uint32_t image_index );
        std::vector< ImageHandle > getSwapchainColorImages();
        std::vector< ImageHandle > getSwapchainDepthImages();
        uint32_t getSwapchainImageCount();
        uint32_t getSwapchainImageIndex();
        Extent2D getSwapchainExtent();
        Format getSwapchainColorFormat();
        Format getSwapchainDepthFormat();

        uint32_t getCurrFrameIndex()const;


        kege::ShaderPipelineManager* getShaderPipelineManager();
        kege::ShaderResourceManager* getShaderResourceManager();

        kege::GraphicsWindow* getWindow();

        int32_t windowHeight()const;
        int32_t windowWidth()const;
        void pollWindowEvents();
        bool windowIsOpen()const;

        int  beginFrame();
        bool endFrame();
        
        bool initalize
        (
            kege::Ref< kege::GraphicsWindow > window,
            const kege::DeviceInitializationInfo& device_init_info,
            const kege::SwapchainDesc& swapchain_create_info
        );

        bool initalize();

        void shutdown();


        //Graphics( GraphicsAPIInfo info );
        Graphics();

        ~Graphics();

    private:

        kege::SemaphoreHandle _image_available_semaphores[ kege::MAX_FRAMES_IN_FLIGHT ];

        /**
         * An array of semaphore handles used for synchronizing command buffer execution across frames.
         * Each frame in flight has its own set of signal semaphores that may be waited upon by subsequent
         * command buffer submissions. These ensure proper GPU execution ordering and frame pacing.
         *
         * The array size is fixed to MAX_FRAMES_IN_FLIGHT to match the swapchain's concurrency model.
         * The semaphores are typically signaled when a command buffer completes execution.
         */
        std::vector< kege::SemaphoreHandle > _cmb_semaphores[ kege::MAX_FRAMES_IN_FLIGHT ];

        /**
         * An array of fence handles used for CPU-GPU synchronization of command buffer execution.
         * Each frame in flight has its own set of fences that ensure the CPU waits for GPU work
         * completion before reusing or modifying associated resources.
         *
         * The array size matches MAX_FRAMES_IN_FLIGHT to maintain synchronization across the
         * swapchain's frame pipeline. These fences are typically signaled when the GPU finishes
         * executing the associated command buffers, allowing the CPU to safely proceed.
         */
        std::vector< kege::FenceHandle > _cmb_fences[ kege::MAX_FRAMES_IN_FLIGHT ];

        SemaphoreHandle _wait_semaphore;

        uint32_t _initial_submits_per_frame;
        uint32_t _cmb_submit_count;

        kege::Ref< kege::GraphicsInstance > _instance;
        kege::Ref< kege::GraphicsWindow > _window;
        kege::GraphicsDevice* _device;

        kege::SwapchainDesc _swapchain_create_info;
        kege::Swapchain _swapchain;
        uint32_t _current_frame;
        uint32_t _image_index;

        kege::ShaderResourceManager _shader_resource_manager;
        kege::ShaderPipelineManager _shader_pipeline_manager;

        //GraphicsAPIInfo _info;
    };
    
}
#endif /* graphics_hpp */
