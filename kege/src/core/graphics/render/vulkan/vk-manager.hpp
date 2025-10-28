//
//  vk-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/10/25.
//

#ifndef vk_manager_hpp
#define vk_manager_hpp

#include "vk-utils.hpp"

namespace kege::vk{

    class PhysicalDevice;
    
    class Manager : public RefCounter {
    public:

        VkDevice createDevice( vk::PhysicalDevice* physical_device, QueueFamilyIndices& queue_family_indices );

        /**
         * @brief Create a semaphore synchronization object
         *
         * Creates a VkSemaphore that can be used to synchronize GPU operations.
         *
         * @return Handle to the newly created semaphore
         */
        VkSemaphore createSemaphore();

        /**
         * @brief Destroy a semaphore
         *
         * Releases the VkSemaphore.
         *
         * @param semaphore Handle to the semaphore to destroy
         */
        void destroySemaphore( VkSemaphore& semaphore );

        /**
         * @brief Create a fence synchronization object
         *
         * Creates a VkFence that can be used to synchronize CPU and GPU operations.
         *
         * @param initially_signaled Whether the fence should be created in the signaled state
         * @return Handle to the newly created fence
         */
        VkFence createFence( bool initially_signaled = false );

        /**
         * @brief Destroy a fence
         *
         * Releases the VkFence.
         *
         * @param fence fence to destroy
         */
        void destroyFence( VkFence& fence );



        VkResult createBuffer
        (
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags memory_properties,
            VkDeviceSize size,
            const void* data,
            VkBuffer* buffer,
            VkDeviceMemory* memory
        );

        void destroyBuffer( VkBuffer& buffer );
        void destroyMemory( VkDeviceMemory& memory );

        VkImage createImage
        (
            VkImageType type,
            VkFormat format,
            int width,
            int height,
            int depth,
            int array_layers,
            int mip_levels,
            VkImageUsageFlags usage,
            VkSampleCountFlagBits sample_count,
            VkImageCreateFlags flags
        );
        
        void destroyImage( VkImage& image_view );

        VkImageView createImageView
        (
            VkImage image,
            VkImageViewType type,
            VkFormat format,
            int array_layers,
            int mip_levels,
            VkImageAspectFlags aspect
        );
        
        void destroyImageView( VkImageView& image_view );
        void destroySampler( VkSampler& sampler );

        void copyBufferToImage
        (
            VkCommandBuffer command,
            VkBuffer buffer,
            VkImage image,
            uint32_t width,
            uint32_t height,
            VkImageAspectFlags aspect_flags,
            VkImageLayout old_layout,
            VkImageLayout new_layout
        );

        void debugSetObjectName( uint64_t object_handle, VkObjectType object_type, const char* name );



        VkResult createPipelineLayout
        (
            const VkPipelineLayoutCreateInfo* info,
            const VkAllocationCallbacks* allocator,
            VkPipelineLayout* layout
        );

        void destroyPipelineLayout( VkPipelineLayout& layout, const VkAllocationCallbacks *allocator );

        VkResult createDescriptorPool( const VkDescriptorPoolCreateInfo* info, const VkAllocationCallbacks *allocator, VkDescriptorPool *pool );

        void destroyDescriptorPool( VkDescriptorPool pool, const VkAllocationCallbacks* allocator );

        void updateDescriptorSets
        (
            uint32_t write_count,
            const VkWriteDescriptorSet* writes,
            uint32_t descriptor_count,
            const VkCopyDescriptorSet* descriptor_copies
        );

        VkResult allocateDescriptorSets( const VkDescriptorSetAllocateInfo* info, VkDescriptorSet* descriptor_sets );

        void freeDescriptorSets( VkDescriptorPool pool, uint32_t count, const VkDescriptorSet* sets );

        VkResult createDescriptorSetLayout
        (
            const VkDescriptorSetLayoutCreateInfo* info,
            const VkAllocationCallbacks* allocator,
            VkDescriptorSetLayout* layout
        );

        void destroyUniformSetLayout( VkDescriptorSetLayout layout, const VkAllocationCallbacks* allocator );

        VkResult createSwapchain( const VkSwapchainCreateInfoKHR *info, const VkAllocationCallbacks* allocator, VkSwapchainKHR* swapchain );

        void destroySwapchain( VkSwapchainKHR swapchain, const VkAllocationCallbacks* allocator );

        VkResult allocateDeviceMemory
        (
            VkMemoryRequirements memory_requirements,
            VkMemoryPropertyFlags memory_properties,
            VkDeviceMemory* memory
        );

        void waitIdle()const;
        Manager();

        PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectName;


        VkPhysicalDeviceMemoryProperties _physical_device_memory_properties;
        VkSurfaceKHR _surface;
        VkDevice _device;

        vk::Instance* _instance;
    };
}
#endif /* vk_manager_hpp */
