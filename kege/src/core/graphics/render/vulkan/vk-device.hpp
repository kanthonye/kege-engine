/**
 * @file Device.h
 * @brief A Vulkan implementation of the GraphicsDevice interface
 *
 * This class encapsulates a Vulkan logical device and provides a complete
 * abstraction over all Vulkan resources and operations. It manages the creation,
 * usage, and destruction of all Vulkan resources through handle-based references.
 */

#ifndef vulkan_device_hpp
#define vulkan_device_hpp

#include "vk-fence.hpp"
#include "vk-semaphore.hpp"
#include "vk-image.hpp"
#include "vk-buffer.hpp"
#include "vk-manager.hpp"
#include "vk-physical-device.hpp"
#include "vk-command-buffer.hpp"
#include "vk-swapchain.hpp"
#include "vk-queue-manager.hpp"
#include "vk-descriptor-manager.hpp"
#include "vk-pipeline-layout-manager.hpp"

namespace kege::vk{

    /**
     * @file Device.h
     * @brief A Vulkan implementation of the GraphicsDevice interface
     *
     * This class encapsulates a Vulkan logical device and provides a complete
     * abstraction over all Vulkan resources and operations. It manages the creation,
     * usage, and destruction of all Vulkan resources through handle-based references.
     */
    class Device final : public kege::GraphicsDevice {
    public:

        bool submit
        (
            const std::vector< kege::SubmitInfo >& submit_infos,
            const ref::Semaphore& image_available,
            const ref::Semaphore& render_complete
        )
        override;
        
        bool submit( const kege::SubmitInfo& submit_info )override;
        bool present( kege::Swapchain* swapchain, const ref::Semaphore& wait_sem )override;
        int  getFrameIndex()const override;
        bool beginSubmit()override;
        void endSubmit()override;

        ref::ShaderLayout createShaderLayout( const kege::ShaderLayoutDesc& desc );
        void destroyShaderLayout( vk::ShaderLayout* layout );

        
        /**
         * @brief Create a pipeline layout
         *
         * Creates a VkPipelineLayout based on the provided descriptor set layouts
         * and push constant ranges.
         *
         * @param desc Description of the pipeline layout to create
         * @return Handle to the newly created pipeline layout
         */
        kege::PipelineLayoutHandle createPipelineLayout( const kege::PipelineLayoutDesc& desc ) override;

        /**
         * @brief Create a shader module
         *
         * Creates a VkShaderModule from the provided shader code.
         *
         * @param desc Description of the shader to create, including compiled shader code
         * @return Handle to the newly created shader
         */
        kege::ShaderHandle createShader( const kege::ShaderDesc& desc ) override;

        /**
         * @brief Get the internal Vulkan pipeline layout object
         * @param pipeline_layout_id Handle to the pipeline layout
         * @return Pointer to the internal PipelineLayout object
         */
        const vk::PipelineLayout* getPipelineLayout( int32_t pipeline_layout_id ) const;

        /**
         * @brief Create a graphics pipeline
         *
         * Creates a VkPipeline for graphics rendering based on the provided description.
         * Uses pipeline caching for improved performance when creating similar pipelines.
         *
         * @param desc Description of the graphics pipeline to create
         * @return Handle to the newly created graphics pipeline
         */
        std::vector< PipelineHandle > createGraphicsPipeline( const CreateShaderPipelineInfo& desc )override;

        /**
         * @brief Create a compute pipeline
         *
         * Creates a VkPipeline for compute operations based on the provided description.
         * Uses pipeline caching for improved performance when creating similar pipelines.
         *
         * @param desc Description of the compute pipeline to create
         * @return Handle to the newly created compute pipeline
         */
        kege::PipelineHandle createComputePipeline( const kege::ComputePipelineDesc& desc ) override;



        //-------------------------------------------------------------------------
        // Command & Queue Management
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a command buffer for recording commands.
         * @param type The type of queue the command buffer will be submitted to.
         * @return Pointer to the created command buffer, or nullptr on failure.
         */
        kege::CommandBuffer* createCommandBuffer( kege::QueueType type )override;

        /**
         * @brief Destroy a command buffer
         *
         * Returns the VkCommandBuffer to its command pool.
         *
         * @param command_buffer The command buffer to destroy
         */
        void destroyCommandBuffer( kege::CommandBuffer* command_buffer ) override;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
        // Shader Resource Set Lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

        int  makeSet( const UniformDescriptorSet& descriptors, const UniformResourceSet& resources )override;
        bool updateSet( int handle, const UniformResourceSet& resources )override;
        int  allocateSet( const UniformDescriptorSet& descriptors )override;
        void freeSet( int set )override;


        /**
         * @brief Get the internal Vulkan descriptor-set object
         * @param descriptor_id Handle to the descriptor-set
         * @return Pointer to the internal DescriptorSet object
         */
        const DescriptorSet* getSet( int32_t descriptor_id ) const;

        //-------------------------------------------------------------------------
        // Descriptor Set Layout Lifecycle
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a descriptor set layout.
         * @param descriptors Description of binding points for resources.
         * @return Handle to the created descriptor set layout.
         */
        UniformSetLayout createUniformSetLayout( const UniformDescriptors& descriptors )override;

        /**
         * @brief Retrieves or creates a descriptor set layout based on bindings.
         * @param descriptors Description of binding points for resources.
         * @return Handle to the descriptor set layout.
         */
        UniformSetLayout getUniformSetLayout( const UniformDescriptors& descriptors )override;

        /**
         * @brief Destroys a descriptor set layout.
         * @param layout Handle to the layout to destroy.
         * @warning Ensure no descriptor sets or pipelines are using this layout.
         */
        void destroyUniformSetLayout( const UniformSetLayout& layout )override;

        //-------------------------------------------------------------------------
        // Buffer Management
        //-------------------------------------------------------------------------

        void copyBuffer(Buffer::Source& dst, Buffer::Source& src, uint64_t size );

        /**
         * @brief Create a buffer resource
         *
         * Creates a VkBuffer with associated memory allocation based on the
         * provided buffer description.
         *
         * @param desc Description of the buffer to create
         * @return Handle to the newly created buffer
         */
        ref::Buffer createBuffer( const kege::BufferDesc& desc ) override;
        void destroyBuffer( vk::Buffer* buffer );

        //-------------------------------------------------------------------------
        // BufferView
        //-------------------------------------------------------------------------

         //TODO: void createBufferView( const kege::BufferViewDesc& desc) override;
         //TODO: void destroyBufferView( vk::BufferView* view ) override;

        //-------------------------------------------------------------------------
        // Resource Creation Methods
        //-------------------------------------------------------------------------

        /**
         * @brief Create a texture resource
         *
         * Creates a VkImage with associated VkImageView and memory allocation
         * based on the provided texture description.
         *
         * @param desc Description of the texture to create
         * @return Handle to the newly created texture
         */
        ref::Image createImage( const kege::Image::Desc& desc ) override;


        /**
         * @brief Destroy a texture resource
         *
         * Releases the VkImage, VkImageView, and associated memory.
         *
         * @param image Handle to the texture to destroy
         */
        void destroyImage( vk::Image* image );

        /**
         * @brief Create a sampler object
         *
         * Creates a VkSampler based on the provided sampler description.
         *
         * @param desc Description of the sampler to create
         * @return Handle to the newly created sampler
         */
        ref::Sampler createSampler( const kege::SamplerDesc& desc ) override;

        /**
         * @brief Destroy a sampler object
         *
         * Releases the VkSampler.
         *
         * @param sampler Handle to the sampler to destroy
         */
        void destroySampler( vk::Sampler* sampler );

        void copyBufferToImage
        (
            VkBuffer buffer,
            VkImage image,
            uint32_t width,
            uint32_t height,
            VkImageAspectFlags aspect_flags,
            VkImageLayout old_layout,
            VkImageLayout new_layout
        );

        bool copyBufferToImage
        (
            uint64_t buffer_size,
            const void* buffer,
            VkImage image,
            uint32_t width,
            uint32_t height,
            VkImageAspectFlags aspect_flags,
            VkImageLayout old_layout,
            VkImageLayout new_layout
        );

        //-------------------------------------------------------------------------
        // Resource Destruction Methods
        //-------------------------------------------------------------------------

        /**
         * @brief Destroy a shader module
         *
         * Releases the VkShaderModule.
         *
         * @param handle Handle to the shader to destroy
         */
        void destroyShader(kege::ShaderHandle handle) override;

        /**
         * @brief Destroy a pipeline layout
         *
         * Releases the VkPipelineLayout.
         *
         * @param handle Handle to the pipeline layout to destroy
         */
        void destroyPipelineLayout(kege::PipelineLayoutHandle handle) override;

        /**
         * @brief Destroy a graphics pipeline
         *
         * Releases the VkPipeline associated with a graphics pipeline.
         *
         * @param handle Handle to the graphics pipeline to destroy
         */
        void destroyGraphicsPipeline(kege::PipelineHandle handle) override;

        /**
         * @brief Destroy a compute pipeline
         *
         * Releases the VkPipeline associated with a compute pipeline.
         *
         * @param handle Handle to the compute pipeline to destroy
         */
        void destroyComputePipeline(kege::PipelineHandle handle) override;

        //-------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------

        const vk::DescriptorSetLayout* getDescriptorSetLayout(int32_t layout );

        //-------------------------------------------------------------------------
        // Descriptor Set Lifecycle
        //-------------------------------------------------------------------------

        /**
         * @brief Update multiple descriptor sets with new resource bindings.
         * @param handles Vector of descriptor set handles to update.
         * @param resource_sets Vector of resource sets containing the new bindings.
         * @return True if all updates succeeded, false otherwise.
         */
        bool updateUniformSets( const std::vector< int >& handles, const UniformSets& resource_sets )override;

        /**
         * @brief Update a single descriptor set with new resource bindings.
         * @param handle Handle of the descriptor set to update.
         * @param resource_set Resource set containing the new bindings.
         * @return True if the update succeeded, false otherwise.
         */
        bool updateUniformSet( int handle, const UniformSet& resource_set )override;

        /**
         * @brief Allocate multiple descriptor sets from layouts.
         * @param description Descriptions of the descriptor set layouts to allocate from.
         * @return Vector of handles to the newly allocated descriptor sets.
         */
        std::vector< int > allocateUniformSets( const UniformSetsDesc& description )override;

        /**
         * @brief Allocate a single descriptor set from a layout.
         * @param description Description of the descriptor set layout to allocate from.
         * @return Handle to the newly allocated descriptor set.
         */
        int allocateUniformSet( const UniformSetDesc& description )override;

        //-------------------------------------------------------------------------
        // Synchronization Primitives
        //-------------------------------------------------------------------------

        /**
         * @brief Create a semaphore synchronization object
         *
         * Creates a VkSemaphore that can be used to synchronize GPU operations.
         *
         * @return Handle to the newly created semaphore
         */
        ref::Semaphore createSemaphore() override;

        /**
         * @brief Destroy a semaphore
         *
         * Releases the VkSemaphore.
         *
         * @param semaphore Handle to the semaphore to destroy
         */
        void destroySemaphore( kege::Semaphore* semaphore ) override;

        /**
         * @brief Create a fence synchronization object
         *
         * Creates a VkFence that can be used to synchronize CPU and GPU operations.
         *
         * @param initially_signaled Whether the fence should be created in the signaled state
         * @return Handle to the newly created fence
         */
        ref::Fence createFence( bool initially_signaled = false ) override;

        /**
         * @brief Destroy a fence
         *
         * Releases the VkFence.
         *
         * @param fence Handle to the fence to destroy
         */
        void destroyFence( kege::Fence* fence ) override;

        //-------------------------------------------------------------------------
        // Swapchain Management
        //-------------------------------------------------------------------------

        /**
         * @brief Create a swapchain for presentation
         *
         * Creates a VkSwapchainKHR based on the provided description.
         *
         * @param desc Description of the swapchain to create
         * @return Handle to the newly created swapchain
         */
        kege::Swapchain* createSwapchain( const kege::SwapchainDesc& desc ) override;

        /**
         * @brief Destroy a swapchain
         *
         * Releases the VkSwapchainKHR and associated resources.
         *
         * @param swapchain Pointer to the swapchain to destroy
         */
        void destroySwapchain( kege::Swapchain* swapchain ) override;

        //-------------------------------------------------------------------------
        // Utility Methods
        //-------------------------------------------------------------------------

        /**
         * @brief Wait for all operations on the device to complete
         *
         * Calls vkDeviceWaitIdle to ensure all pending operations are finished.
         */
        void waitIdle() override;

        //-------------------------------------------------------------------------
        // Vulkan-Specific Accessor Methods
        //-------------------------------------------------------------------------

        /**
         * @brief Get the Vulkan logical device handle
         * @return The VkDevice handle
         */
        VkDevice getVkDevice() const { return _device; }

        /**
         * @brief Get the VMA allocator
         * @return The VmaAllocator handle
         */
        //VmaAllocator getVmaAllocator() const { return _allocator; }

        /**
         * @brief Get the Vulkan physical device handle
         * @return The VkPhysicalDevice handle
         */
        const PhysicalDevice* getVkPhysicalDevice() const { return _physical_device; }
        PhysicalDevice* getVkPhysicalDevice() { return _physical_device; }

        /**
         * @brief Get the Vulkan instance handle
         * @return The VkInstance handle
         */
        Instance* getVkInstance() const { return _instance; }

        //-------------------------------------------------------------------------
        // Internal Resource Access Methods
        // These provide access to the internal Vulkan objects from handles
        //-------------------------------------------------------------------------

        /**
         * @brief Get the internal Vulkan shader object
         * @param handle Handle to the shader
         * @return Pointer to the internal Shader object
         */
        const Shader* getShader(ShaderHandle handle) const;

        /**
         * @brief Get the internal Vulkan graphics pipeline object
         * @param handle Handle to the graphics pipeline
         * @return Pointer to the internal GraphicsPipeline object
         */
        const GraphicsPipeline* getGraphicsPipeline(PipelineHandle handle) const;

        /**
         * @brief Get the internal Vulkan compute pipeline object
         * @param handle Handle to the compute pipeline
         * @return Pointer to the internal ComputePipeline object
         */
        const ComputePipeline* getComputePipeline(PipelineHandle handle) const;

        //VkSurfaceKHR surface();
        VkDevice handle();

        //-------------------------------------------------------------------------
        // Initialization & Device Information
        //-------------------------------------------------------------------------

        /**
         * @brief Initialize the Vulkan device
         *
         * Sets up the Vulkan logical device, and all required
         * resources based on the provided initialization information.
         *
         * @param physical_device The physical device to create the logical device from.
         * @return true on successful initialization, false otherwise
         */
        bool initialize( vk::PhysicalDevice* physical_device );

        /**
         * @brief Shut down the device and release all resources
         *
         * Performs a complete cleanup of all Vulkan resources and destroys
         * the logical device, debug messenger, and Vulkan instance.
         */
        void shutdown() override;

        // Disable copy/move semantics to prevent multiple instances managing the same Vulkan resources
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;
        Device(const Device&) = delete;
        Device(Device&&) = delete;


        vk::Manager& core();

        /**
         * @brief Destructor
         *
         * Ensures proper cleanup of all Vulkan resources if not already done by shutdown().
         */
        ~Device() override;

        /**
         * @brief Default constructor
         *
         * Creates an uninitialized Device instance. Call initialize() to
         * set up the Vulkan instance, physical device, and logical device.
         */
        Device();


    private:

        //-------------------------------------------------------------------------
        // Internal Initialization Methods
        //-------------------------------------------------------------------------

        /**
         * @brief Set up the debug messenger for validation layers
         *
         * Creates a debug messenger for receiving validation layer messages.
         * Only used when validation layers are enabled.
         *
         * @return true on success, false otherwise
         */
        bool setupDebugMessenger();

        /**
         * @brief Create the VMA allocator
         *
         * Sets up the Vulkan Memory Allocator for efficient memory management.
         *
         * @return true on success, false otherwise
         */
        bool createAllocator();

        /**
         * @brief Create command pools
         *
         * Creates command pools for each queue family.
         *
         * @return true on success, false otherwise
         */
        bool createCommandPools();

        /**
         * @brief Create the pipeline cache
         *
         * Creates a VkPipelineCache for efficient pipeline creation.
         *
         * @return true on success, false otherwise
         */
        bool createPipelineCache();

    private:

        vk::DescriptorManager _shader_set_manager;
        vk::Manager _manager;

        //-------------------------------------------------------------------------
        // Member Variables
        //-------------------------------------------------------------------------

        vk::QueueManager* _queue_manager;
        QueueFamilyIndices _queue_family_indices;

        int32_t _frame_index;

        /** @brief Storage for image objects */
        vk::List< vk::Image > _images;

        /** @brief Storage for sampler objects */
        vk::List< vk::Sampler > _samplers;

        vk::List< vk::Buffer > _buffers;

        vk::List< vk::Fence > _fences;
        vk::List< vk::Semaphore > _semaphores;
        vk::List< vk::Swapchain > _swapchains;
        vk::List< vk::ShaderLayout > _shader_layouts;
        vk::List< vk::CommandBuffer > _command_buffers;

        PipelineLayoutManager _pipeline_layout_manager;



        /** @brief Storage for shader objects */
        ResourceRecycler< Shader > _shaders;

        /** @brief Storage for graphics pipeline objects */
        ResourceRecycler< GraphicsPipeline > _graphics_pipelines;

        /** @brief Storage for compute pipeline objects */
        ResourceRecycler< ComputePipeline > _compute_pipelines;

        PhysicalDevice* _physical_device = nullptr;

        /** @brief Vulkan instance handle */
        Instance* _instance = nullptr;

        /** @brief Debug messenger for validation layers */
        //VkDebugUtilsMessengerEXT _debug_messenger = VK_NULL_HANDLE;

        VkSurfaceKHR _surface = VK_NULL_HANDLE;

        /** @brief Logical device handle */
        VkDevice _device = VK_NULL_HANDLE;

        /** @brief Flag indicating if validation layers are enabled */
        bool _validation_enabled = false;

        /** @brief Copy of initialization info for reference */
        kege::DeviceInitializationInfo _init_info;

        /** @brief List of required device extensions */
        std::vector<const char*> _required_device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            // Additional platform-specific extensions
            #ifdef __APPLE__
            VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
            #endif
            // Add other conditional extensions as needed
        };

        /** @brief List of validation layers to use when enabled */
        const std::vector<const char*> _validation_layers = {
            "VK_LAYER_KHRONOS_validation"
        };

        /** @brief Handle to debug callback function for validation layers */
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
        (
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

        PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectName;

        //VkPhysicalDeviceMemoryProperties _physical_device_memory_properties;
        uint32_t _api_version;

        enum{ MAX_FRAMES_IN_FLIGHT = 2};
        friend vk::PipelineLayoutManager;
        friend vk::CommandBuffer;
        friend vk::QueueManager;
        friend vk::FrameRenderer;
        friend vk::Swapchain;
        friend vk::Instance;
        friend vk::Device;
        friend vk::Buffer;

        friend List< vk::Device >;
        vk::Device* prev;
        vk::Device* next;
    };

}
#endif // !vulkan_device_hpp
