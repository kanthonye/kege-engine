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

#include "../../core/resource-recycler.hpp"
#include "vulkan-resources.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-command-buffer.hpp"
#include "vulkan-descriptor-manager.hpp"

namespace kege::vk{

    class Swapchain;

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
         * @brief Get the graphics API implemented by this device
         * @return Always returns GraphicsAPI::Vulkan
         */
        kege::GraphicsAPI getCurrentAPI() const override { return kege::GraphicsAPI::Vulkan; }

        /**
         * @brief Get the enabled device features
         * @return Reference to the device features structure
         */
        const kege::DeviceFeatures& getFeatures() const override { return _features; }

        /**
         * @brief Get the physical device limits
         * @return Reference to the device limits structure
         */
        const kege::DeviceLimits& getLimits() const override { return _limits; }

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

        /**
         * @brief Get a command queue for the specified queue type
         *
         * Returns a handle to a command queue of the requested type.
         * The implementation may share physical queues when appropriate.
         *
         * @param type The type of queue to retrieve (graphics, compute, transfer)
         * @return Handle to the requested command queue
         */
        //CommandQueueHandle getCommandQueue(QueueType type) override;

        bool submitCommands
        (
            const std::vector< kege::CommandBuffer* >& command_buffers,
            kege::FenceHandle* signal_fence,
            kege::SemaphoreHandle* signal_semaphore,
            kege::SemaphoreHandle* wait_semaphore
        )
        override;



        void endTransferQueueCommandBuffer( VkCommandBuffer command_buffer );
        VkCommandBuffer beginTransferQueueCommandBuffer();
        
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
        kege::ImageHandle createImage( const kege::ImageDesc& desc ) override;

        /**
         * @brief Create a buffer resource
         *
         * Creates a VkBuffer with associated memory allocation based on the
         * provided buffer description.
         *
         * @param desc Description of the buffer to create
         * @return Handle to the newly created buffer
         */
        kege::BufferHandle createBuffer( const kege::BufferDesc& desc ) override;
        VkResult createBuffer( VkBufferUsageFlags usage, VkMemoryPropertyFlags property, VkDeviceSize size, const void* data, vk::Buffer* buffer );
        void setBufferData( VkDeviceSize size, const void* data, vk::Buffer* buffer );

        void updateBuffer( const BufferHandle& handle, uint64_t offset, uint64_t size, const void* data ) override;

        /**
         * @brief Create a sampler object
         *
         * Creates a VkSampler based on the provided sampler description.
         *
         * @param desc Description of the sampler to create
         * @return Handle to the newly created sampler
         */
        kege::SamplerHandle createSampler( const kege::SamplerDesc& desc ) override;

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
         * @brief Get the internal Vulkan pipeline layout object
         * @param handle Handle to the pipeline layout
         * @return Pointer to the internal PipelineLayout object
         */
        const vk::PipelineLayout* getPipelineLayout( kege::PipelineLayoutHandle handle ) const;

        /**
         * @brief Create a graphics pipeline
         *
         * Creates a VkPipeline for graphics rendering based on the provided description.
         * Uses pipeline caching for improved performance when creating similar pipelines.
         *
         * @param desc Description of the graphics pipeline to create
         * @return Handle to the newly created graphics pipeline
         */
        kege::PipelineHandle createGraphicsPipeline( const kege::GraphicsPipelineDesc& desc ) override;

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

        /**
         * @brief Create a descriptor set layout
         *
         * Creates a VkDescriptorSetLayout based on the provided binding descriptions.
         *
         * @param bindings Description of the descriptor set layout to create
         * @return Handle to the newly created descriptor set layout
         */
        kege::DescriptorSetLayoutHandle createDescriptorSetLayout( const std::vector< kege::DescriptorSetLayoutBinding >& bindings ) override;

        /**
         * @brief Get the internal Vulkan descriptor-set-layout object
         * @param handle Handle to the descriptor-set-layout
         * @return Pointer to the internal DescriptorSetLayout object
         */
        const vk::DescriptorSetLayout* getDescriptorSetLayout( kege::DescriptorSetLayoutHandle handle ) const;

        /**
         * @brief Create a descriptor set
         *
         * Creates a VkDescriptorSetLayout based on the provided binding descriptions.
         *
         * @param bindings Description of the descriptor set to create
         * @return Handle to the newly created descriptor set
         */
        kege::DescriptorSetHandle allocateDescriptorSet( const std::vector< kege::DescriptorSetLayoutBinding >& bindings ) override;

        /**
         * @brief Get the internal Vulkan descriptor-set object
         * @param handle Handle to the descriptor-set
         * @return Pointer to the internal DescriptorSet object
         */
        const DescriptorSet* getDescriptorSet( kege::DescriptorSetHandle handle ) const;

        /**
         * @brief Allocate a descriptor set
         *
         * Allocates a VkDescriptorSet from the main descriptor pool based on the
         * provided descriptor set layout.
         *
         * @param layoutHandle Handle to the descriptor set layout to use
         * @return Handle to the newly allocated descriptor set
         */
        kege::DescriptorSetHandle allocateDescriptorSet(DescriptorSetLayoutHandle layoutHandle) override;

        //-------------------------------------------------------------------------
        // Resource Destruction Methods
        //-------------------------------------------------------------------------

        /**
         * @brief Destroy a texture resource
         *
         * Releases the VkImage, VkImageView, and associated memory.
         *
         * @param handle Handle to the texture to destroy
         */
        void destroyImage( kege::ImageHandle handle) override;

        /**
         * @brief Destroy a buffer resource
         *
         * Releases the VkBuffer and associated memory.
         *
         * @param handle Handle to the buffer to destroy
         */
        void destroyBuffer(kege::BufferHandle handle) override;

        /**
         * @brief Destroy a sampler object
         *
         * Releases the VkSampler.
         *
         * @param handle Handle to the sampler to destroy
         */
        void destroySampler(kege::SamplerHandle handle) override;

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

        /**
         * @brief Destroy a descriptor set layout
         *
         * Releases the VkDescriptorSetLayout.
         *
         * @param handle Handle to the descriptor set layout to destroy
         */
        void destroyDescriptorSetLayout(kege::DescriptorSetLayoutHandle handle) override;

        /**
         * @brief Free a descriptor set
         *
         * Returns the VkDescriptorSet to the descriptor pool.
         *
         * @param handle Handle to the descriptor set to free
         */
        void freeDescriptorSet(kege::DescriptorSetHandle handle) override;

        //-------------------------------------------------------------------------
        // Synchronization Primitives
        //-------------------------------------------------------------------------

        /**
         * @brief Create a fence synchronization object
         *
         * Creates a VkFence that can be used to synchronize CPU and GPU operations.
         *
         * @param initially_signaled Whether the fence should be created in the signaled state
         * @return Handle to the newly created fence
         */
        kege::FenceHandle createFence(bool initially_signaled = false) override;

        /**
         * @brief Create a semaphore synchronization object
         *
         * Creates a VkSemaphore that can be used to synchronize GPU operations.
         *
         * @return Handle to the newly created semaphore
         */
        kege::SemaphoreHandle createSemaphore() override;

        /**
         * @brief Destroy a fence
         *
         * Releases the VkFence.
         *
         * @param handle Handle to the fence to destroy
         */
        void destroyFence(kege::FenceHandle handle) override;

        /**
         * @brief Destroy a semaphore
         *
         * Releases the VkSemaphore.
         *
         * @param handle Handle to the semaphore to destroy
         */
        void destroySemaphore(kege::SemaphoreHandle handle) override;

        /**
         * @brief Wait for a fence to become signaled
         *
         * Blocks the calling thread until the fence is signaled or the timeout expires.
         *
         * @param fences Handle to the fences to wait on
         * @param wait_all Boolean value to indicate if the device should wait for all
         * @param timeout_nanoseconds Maximum time to wait in nanoseconds (UINT64_MAX for indefinite)
         * @return true if the fence was signaled, false if the timeout expired
         */
        bool waitForFence( uint32_t count, kege::FenceHandle* fences, uint32_t wait_all, uint64_t timeout_nanoseconds ) override;

        /**
         * @brief Reset a fence to the unsignaled state
         *
         * @param fences Handle to the fences to reset
         */
        void resetFence( uint32_t count, kege::FenceHandle* fences ) override;

        /**
         * @brief Get the current status of a fence
         *
         * @param handle Handle to the fence to check
         * @return true if the fence is signaled, false otherwise
         */
        FenceStatus getFenceStatus(kege::FenceHandle handle) override;

        //-------------------------------------------------------------------------
        // Swapchain Management
        //-------------------------------------------------------------------------

        bool acquireNextSwapchainImage( const kege::Swapchain& swapchain, SemaphoreHandle signalSemaphore, uint32_t* out_image_index )override;
        bool presentSwapchainImage( const kege::Swapchain& swapchain, SemaphoreHandle waitSemaphore, uint32_t image_index )override;
        bool needsRecreation( const kege::Swapchain& swapchain )override;

        ImageHandle getSwapchainColorImage( const kege::Swapchain& swapchain, uint32_t image_index )override;
        ImageHandle getSwapchainDepthImage( const kege::Swapchain& swapchain, uint32_t image_index )override;
        std::vector< ImageHandle > getSwapchainColorImages( const kege::Swapchain& swapchain )override;
        std::vector< ImageHandle > getSwapchainDepthImages( const kege::Swapchain& swapchain )override;
        uint32_t getSwapchainImageCount( const kege::Swapchain& swapchain )override;
        uint32_t getSwapchainImageIndex( const kege::Swapchain& swapchain )override;
        Extent2D getSwapchainExtent( const kege::Swapchain& swapchain )override;
        Format getSwapchainColorFormat( const kege::Swapchain& swapchain )override;
        Format getSwapchainDepthFormat( const kege::Swapchain& swapchain )override;

        /**
         * @brief Create a swapchain for presentation
         *
         * Creates a VkSwapchainKHR based on the provided description.
         *
         * @param desc Description of the swapchain to create
         * @return Handle to the newly created swapchain
         */
        kege::Swapchain createSwapchain( const kege::SwapchainDesc& desc ) override;

        /**
         * @brief Destroy a swapchain
         *
         * Releases the VkSwapchainKHR and associated resources.
         *
         * @param swapchain Pointer to the swapchain to destroy
         */
        void destroySwapchain( kege::Swapchain swapchain ) override;

        //-------------------------------------------------------------------------
        // Buffer Management
        //-------------------------------------------------------------------------

        /**
         * @brief Map a buffer for CPU access
         *
         * Maps the buffer memory into CPU address space for direct access.
         *
         * @param handle Handle to the buffer to map
         * @param offset Offset within the buffer to start mapping from
         * @param size Size of the region to map (VK_WHOLE_SIZE for entire buffer)
         * @return Pointer to the mapped memory region
         */
        void* mapBuffer(BufferHandle handle, size_t offset = 0, size_t size = VK_WHOLE_SIZE) override;

        /**
         * @brief Unmap a previously mapped buffer
         *
         * Releases the CPU mapping of the buffer memory.
         *
         * @param handle Handle to the buffer to unmap
         */
        void unmapBuffer(BufferHandle handle) override;

        /**
         * @brief Flush a range of mapped memory
         *
         * Ensures that host writes to a mapped memory range are visible to the device.
         * Only needed for non-coherent memory types.
         *
         * @param handle Handle to the buffer
         * @param offset Offset within the buffer
         * @param size Size of the region to flush
         */
        void flushMappedBufferRange(BufferHandle handle, size_t offset, size_t size);

        /**
         * @brief Invalidate a range of mapped memory
         *
         * Ensures that device writes to a mapped memory range are visible to the host.
         * Only needed for non-coherent memory types.
         *
         * @param handle Handle to the buffer
         * @param offset Offset within the buffer
         * @param size Size of the region to invalidate
         */
        void invalidateMappedBufferRange(BufferHandle handle, size_t offset, size_t size);

        //-------------------------------------------------------------------------
        // Descriptor Set Updates
        //-------------------------------------------------------------------------

        /**
         * @brief Update descriptor sets with resource bindings
         *
         * Updates one or more descriptor sets with buffer, texture, or sampler bindings.
         *
         * @param writes Vector of descriptor write operations to perform
         * @return true if allocated successfully, false otherwise
         */
        bool updateDescriptorSets( const std::vector< WriteDescriptorSet >& writes ) override;

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
         * @brief Get the internal Vulkan buffer object
         * @param handle Handle to the buffer
         * @return Pointer to the internal Buffer object
         */
        const Buffer* getBuffer(BufferHandle handle) const;
        Buffer* getBuffer(BufferHandle handle);

        /**
         * @brief Get the internal Vulkan texture object
         * @param handle Handle to the texture
         * @return Pointer to the internal Image object
         */
        const Image* getImage(ImageHandle handle) const;

        /**
         * @brief Get the internal Vulkan sampler object
         * @param handle Handle to the sampler
         * @return Pointer to the internal Sampler object
         */
        const Sampler* getSampler(SamplerHandle handle) const;

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

        /**
         * @brief Get the internal Vulkan fence object
         * @param handle Handle to the fence
         * @return Pointer to the internal Fence object
         */
        const Fence* getFence(FenceHandle handle) const;

        /**
         * @brief Get the internal Vulkan semaphore object
         * @param handle Handle to the semaphore
         * @return Pointer to the internal Semaphore object
         */
        const Semaphore* getSemaphore(SemaphoreHandle handle) const;

        void copyBufferToTexture( VkCommandBuffer command, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkImageAspectFlags aspect_flags, VkImageLayout old_layout, VkImageLayout new_layout );

        void debugSetObjectName( uint64_t object_handle, VkObjectType object_type, const char* name );

        inline VkResult createImageView
        (
            const VkImageViewCreateInfo* info,
            const VkAllocationCallbacks* allocator,
            VkImageView* view
        )
        {
            return vkCreateImageView( _device, info, allocator, view );
        }
        
        inline VkResult createPipelineLayout
        (
            const VkPipelineLayoutCreateInfo* info,
            const VkAllocationCallbacks* allocator,
            VkPipelineLayout* layout
        )
        {
            return vkCreatePipelineLayout( _device, info, allocator, layout );
        }

        inline void destroyPipelineLayout( VkPipelineLayout layout, const VkAllocationCallbacks *allocator )
        {
            waitIdle();
            vkDestroyPipelineLayout( _device, layout, allocator );
        }

        inline VkResult createDescriptorPool( const VkDescriptorPoolCreateInfo* info, const VkAllocationCallbacks *allocator, VkDescriptorPool *pool )
        {
            return vkCreateDescriptorPool( _device, info, allocator, pool );
        }

        inline void destroyDescriptorPool( VkDescriptorPool pool, const VkAllocationCallbacks* allocator )
        {
            vkDestroyDescriptorPool( _device, pool, allocator );
        }

        void updateDescriptorSets
        (
            uint32_t write_count,
            const VkWriteDescriptorSet* writes,
            uint32_t descriptor_count,
            const VkCopyDescriptorSet* descriptor_copies
        )
        {
            vkUpdateDescriptorSets( _device, write_count, writes, descriptor_count, descriptor_copies );
        }

        inline VkResult allocateDescriptorSets( const VkDescriptorSetAllocateInfo* info, VkDescriptorSet* descriptor_sets )
        {
            return vkAllocateDescriptorSets( _device, info, descriptor_sets );
        }
        
        inline void freeDescriptorSets( VkDescriptorPool pool, uint32_t count, const VkDescriptorSet* sets )
        {
            vkFreeDescriptorSets( _device, pool, count, sets );
        }

        VkResult createDescriptorSetLayout
        (
            const VkDescriptorSetLayoutCreateInfo* info,
            const VkAllocationCallbacks* allocator,
            VkDescriptorSetLayout* layout
        )
        {
            return vkCreateDescriptorSetLayout( _device, info, allocator, layout );
        }
        
        inline void destroyDescriptorSetLayout( VkDescriptorSetLayout layout, const VkAllocationCallbacks* allocator )
        {
            waitIdle();
            vkDestroyDescriptorSetLayout( _device, layout, allocator );
        }

        inline VkResult allocateCommandBuffers( const VkCommandBufferAllocateInfo* info, VkCommandBuffer* command_buffers )
        {
            return vkAllocateCommandBuffers( _device, info, command_buffers );
        }

        VkResult createSwapchain( const VkSwapchainCreateInfoKHR *info, const VkAllocationCallbacks* allocator, VkSwapchainKHR* swapchain )
        {
            return vkCreateSwapchainKHR( _device, info, allocator, swapchain );
        }
        void destroySwapchain( VkSwapchainKHR swapchain, const VkAllocationCallbacks* allocator )
        {
            waitIdle();
            vkDestroySwapchainKHR( _device, swapchain, allocator );
        }

        void destroyImageView( VkImageView image_view, const VkAllocationCallbacks* allocator)
        {
            vkDestroyImageView( _device, image_view, allocator );
        }

        void destroySurface( VkSurfaceKHR surface, const VkAllocationCallbacks *allocator );


        VkResult allocateDeviceMemory
        (
            VkMemoryRequirements memory_requirements,
            VkMemoryPropertyFlags memory_properties,
            VkDeviceMemory* memory
        );

        VkSurfaceKHR surface();
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
         * @param surface The surface assocaited with the  physical device
         * @return true on successful initialization, false otherwise
         */
        bool initialize( vk::PhysicalDevice* physical_device, VkSurfaceKHR surface );

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

        //-------------------------------------------------------------------------
        // Internal Cleanup Methods
        //-------------------------------------------------------------------------

        /**
         * @brief Clean up all swapchains
         *
         * Destroys all swapchain objects and their associated resources.
         */
        void cleanupSwapchains();

        /**
         * @brief Clean up all resource objects
         *
         * Destroys all textures, buffers, and other resources.
         */
        void cleanupResources();

        /**
         * @brief Clean up all synchronization primitives
         *
         * Destroys all fences and semaphores.
         */
        void cleanupSyncPrimitives();

        /**
         * @brief Clean up all pipelines and layouts
         *
         * Destroys all graphics and compute pipelines, and pipeline layouts.
         */
        void cleanupPipelines();

        /**
         * @brief Clean up all shaders
         *
         * Destroys all shader modules.
         */
        void cleanupShaders();

        /**
         * @brief Clean up command pools
         *
         * Destroys all command pools.
         */
        void cleanupCommandPools();

    private:

        //-------------------------------------------------------------------------
        // Member Variables
        //-------------------------------------------------------------------------

        /** @brief Storage for buffer objects */
        ResourceRecycler< vk::CommandBuffer* > _command_buffers;

        /** @brief Storage for buffer objects */
        ResourceRecycler< vk::Buffer > _buffers;

        /** @brief Storage for texture objects */
        ResourceRecycler< vk::Image > _textures;

        /** @brief Storage for sampler objects */
        ResourceRecycler< vk::Sampler > _samplers;

        /** @brief Storage for shader objects */
        ResourceRecycler< Shader > _shaders;

        /** @brief Storage for graphics pipeline objects */
        ResourceRecycler< GraphicsPipeline > _graphics_pipelines;

        /** @brief Storage for compute pipeline objects */
        ResourceRecycler< ComputePipeline > _compute_pipelines;

        /** @brief Storage for swapchain objects */
        ResourceRecycler< Swapchain > _swapchains;

        /** @brief Storage for fence objects */
        ResourceRecycler< Fence > _fences;

        /** @brief Storage for semaphore objects */
        ResourceRecycler< Semaphore > _semaphores;

        /** @brief Main descriptor pool for allocating descriptor sets */
        //DescriptorPool _main_descriptor_pool;

        /** @brief Command pools for each queue type */
        std::unordered_map< QueueType, VkCommandPool > _command_pools;

        QueueFamilyIndices _queue_family_indices;

        PhysicalDevice* _physical_device = nullptr;

        /** @brief Vulkan instance handle */
        Instance* _instance = nullptr;

        /** @brief Debug messenger for validation layers */
        VkDebugUtilsMessengerEXT _debug_messenger = VK_NULL_HANDLE;

        VkSurfaceKHR _surface = VK_NULL_HANDLE;

        /** @brief Logical device handle */
        VkDevice _device = VK_NULL_HANDLE;

        /** @brief Memory allocator handle */
        //VmaAllocator _allocator = VK_NULL_HANDLE;

        /** @brief Graphics queue information */
        QueueInfo _graphics_queue;

        /** @brief Compute queue information (might be same as graphics) */
        QueueInfo _compute_queue;

        /** @brief Transfer queue information (might be same as graphics or compute) */
        QueueInfo _transfer_queue;

        /** @brief Present queue information (often same as graphics) */
        QueueInfo _present_queue;

        /** @brief Enabled device features */
        DeviceFeatures _features = {};

        /** @brief Physical device limits */
        DeviceLimits _limits = {};

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

        VkPhysicalDeviceMemoryProperties _physical_device_memory_properties;
        uint32_t _api_version;

        DescriptorManager _descriptor_manager;

        enum{ MAX_FRAMES_IN_FLIGHT = 2};
        friend vk::CommandBuffer;
        friend Swapchain;
        friend Instance;
    };

}
#endif
