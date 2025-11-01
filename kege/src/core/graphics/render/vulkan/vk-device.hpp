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
#include "vk-shader-pipeline.hpp"
#include "vk-shader-set.hpp"
#include "vk-set-layout.hpp"
#include "vk-set-allocator.hpp"
#include "vk-shader-layout.hpp"

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

        /**
         * @brief Submit command buffers and synchronization primitives to the GPU.
         * @param submit_infos Vector of SubmitInfo structures describing command buffers and sync.
         * @param image_available Semaphore signaled when an image is available for rendering.
         * @param render_complete Semaphore signaled when rendering is complete.
         * @return True if submission succeeded, false otherwise.
         */
        bool submit
        (
            const std::vector< kege::SubmitInfo >& submit_infos,
            const ref::Semaphore& image_available,
            const ref::Semaphore& render_complete
        )
        override;
        
        /**
         * @brief Submit a single command buffer and synchronization primitives to the GPU.
         * @param submit_info SubmitInfo structure describing command buffer and sync.
         * @return True if submission succeeded, false otherwise.
         */
        bool submit( const kege::SubmitInfo& submit_info )override;

        /**
         * @brief Present the rendered image to the swapchain.
         * @param swapchain Pointer to the swapchain to present to.
         * @param wait_sem Semaphore to wait on before presentation.
         * @return True if presentation succeeded, false otherwise.
         */
        bool present( kege::Swapchain* swapchain, const ref::Semaphore& wait_sem )override;
        int  getFrameIndex()const override;
        bool beginSubmit()override;
        void endSubmit()override;

        //-------------------------------------------------------------------------
        // SetAllocator Management
        //-------------------------------------------------------------------------

        /**
         * @brief Retrieves a descriptor allocator for the specified descriptor types.
         * @param descriptor_types Vector of VkDescriptorType to allocate.
         * @return Pointer to the SetAllocator managing these types.
         */
        vk::SetAllocator* getDescriptorAllocator( const std::vector< VkDescriptorType >& descriptor_types );

        //-------------------------------------------------------------------------
        // SetLayout Lifecycle
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a shader binding set layout based on a description.
         * @param bindings Description of binding points for resources.
         * @return Handle to the created shader binding set layout.
         */
        ref::SetLayout createSetLayout( const SetBindings& bindings ) override;

        /**
         * @brief Destroys a shader binding set layout.
         * @param layout Is the object to destroy.
         * @warning Ensure no descriptor sets or pipelines aren't using this layout.
         */
        void destroySetLayout( vk::SetLayout* layout );

        //-------------------------------------------------------------------------
        // ShaderLayout Lifecycle
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a shader layout based on a description.
         * @param description Description of the shader layout.
         * @return Handle to the created shader layout.
         */
        ref::ShaderLayout createShaderLayout( const ShaderLayoutDesc& description ) override;

        /**
         * @brief Destroys a shader layout.
         * @param layout Is the object to destroy.
         * @warning Ensure no pipelines are using this layout.
         */
        void destroyShaderLayout( vk::ShaderLayout* layout );

        //-------------------------------------------------------------------------
        // ShaderPipeline Lifecycle
        //-------------------------------------------------------------------------

        /**
         * @brief Create a shader pipeline
         *
         * Creates a VkPipeline based on the provided description.
         *
         * @param desc Description of the shader pipeline to create
         * @return Handle to the newly created shader pipeline
         */
        ref::ShaderPipeline createShaderPipeline( const PipelineCreateInfo& desc ) override;

        /**
         * @brief Destroy a shader pipeline
         *
         * Releases the VkPipeline associated with the shader pipeline.
         *
         * @param pipeline Handle to the shader pipeline to destroy
         */
        void destroyShaderPipeline( vk::ShaderPipeline* pipeline );

        //-------------------------------------------------------------------------
        // Shader Lifecycle
        //-------------------------------------------------------------------------

        /**
         * @fn createShader: Construct a shader object
         * @param desc The description use to create the shader object
         * @return The shader object
         */
        ref::Shader createShader( const kege::ShaderDesc& desc ) override;

        /**
         * @fn destroyShader: Deconstruct a shader object
         * @param shader The shader object to de-construct
         */
        void destroyShader( vk::Shader* shader );

        //-------------------------------------------------------------------------
        // Command & Queue Management
        //-------------------------------------------------------------------------

        /**
         * @brief Creates a command buffer for recording commands.
         * @param type The type of queue the command buffer will be submitted to.
         * @return Pointer to the created command buffer, or nullptr on failure.
         */
        ref::CommandBuffer createCommandBuffer( kege::QueueType type )override;

        /**
         * @brief Destroy a command buffer
         *
         * Returns the VkCommandBuffer to its command pool.
         *
         * @param command_buffer The command buffer to destroy
         */
        void destroyCommandBuffer( vk::CommandBuffer* command_buffer );

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
        void destroySemaphore( vk::Semaphore* semaphore );

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
        void destroyFence( vk::Fence* fence );

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
        ref::Swapchain createSwapchain( const kege::SwapchainDesc& desc ) override;

        /**
         * @brief Destroy a swapchain
         *
         * Releases the VkSwapchainKHR and associated resources.
         *
         * @param swapchain Pointer to the swapchain to destroy
         */
        void destroySwapchain( vk::Swapchain* swapchain );

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


        vk::Instance* instance();
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

        std::unordered_map
        <
            kege::SetBindings,
            kege::Ref< vk::SetLayout >,
            kege::SetBindingsHash,
            kege::SetBindingsEqual
        >
        _set_layout_library;

        std::unordered_map
        <
            std::vector< VkDescriptorSetLayout >,
            kege::Ref< vk::ShaderLayout >,
            vk::DescriptorSetLayoutVectorHash
        >
        _shader_layout_lookup;

        std::unordered_map
        <
            std::vector< VkDescriptorType >,
            Ref< SetAllocator >,
            vk::DescriptorTypeVectorHash
        >
        _descriptor_allocators;

        vk::Manager _manager;
        vk::QueueManager* _queue_manager;
        QueueFamilyIndices _queue_family_indices;

        int32_t _frame_index;

        vk::List< vk::ShaderLayout > _shader_layouts;
        vk::List< vk::SetLayout > _set_layouts;
        vk::List< vk::Image > _images;
        vk::List< vk::Sampler > _samplers;
        vk::List< vk::Buffer > _buffers;
        vk::List< vk::Fence > _fences;
        vk::List< vk::Semaphore > _semaphores;
        vk::List< vk::Swapchain > _swapchains;
        vk::List< vk::CommandBuffer > _command_buffers;
        vk::List< vk::ShaderPipeline > _pipelines;
        vk::List< vk::Shader > _shaders;

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
        friend vk::ShaderPipeline;
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
