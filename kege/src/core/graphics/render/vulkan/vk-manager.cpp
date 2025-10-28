//
//  vk-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/10/25.
//

#include "vk-manager.hpp"
#include "vk-instance.hpp"

namespace kege::vk{

    VkDevice Manager::createDevice( vk::PhysicalDevice* physical_device, QueueFamilyIndices& queue_family_indices )
    {
        if (_device != VK_NULL_HANDLE)
        {
            KEGE_LOG_WARN << "Logical device already created."<<Log::nl;
            return VK_NULL_HANDLE;
        }

        _surface = physical_device->getSurface();
        _instance = physical_device->getInstance();
        _physical_device_memory_properties = physical_device->getMemoryProperties();

        // 2. Prepare Unique Queue Create Infos
        std::vector< VkDeviceQueueCreateInfo > queue_create_infos;
        std::set< uint32_t > unique_queue_families; // Use set to avoid duplicates

        // Add required queue families
        unique_queue_families.insert( queue_family_indices.graphics_family.value() );
        if ( queue_family_indices.compute_family.has_value() )
        {
            unique_queue_families.insert( queue_family_indices.compute_family.value() );
        }
        if ( queue_family_indices.transfer_family.has_value() )
        {
            unique_queue_families.insert( queue_family_indices.transfer_family.value() );
        }
        // Add present family if it's different and needed (usually needed for swapchain)
        // Presentation check often requires a surface, handle carefully.
        if ( queue_family_indices.present_family.has_value() )
        {
             unique_queue_families.insert( queue_family_indices.present_family.value() );
        }

        // Create info for each unique family index
        float queue_priority = 1.0f; // Default priority for all queues
        for ( uint32_t queue_family_index : unique_queue_families )
        {
            VkDeviceQueueCreateInfo queue_create_info = {};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family_index;
            queue_create_info.queueCount = 1; // Create only one queue per family for now
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back( queue_create_info );
        }

        // 3. Prepare Device Create Info
        VkDeviceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        // Link Queue Infos
        create_info.queueCreateInfoCount = static_cast< uint32_t >( queue_create_infos.size() );
        create_info.pQueueCreateInfos = queue_create_infos.data();

        VkPhysicalDeviceFeatures device_features{};
        device_features.fillModeNonSolid    = VK_TRUE; // Enable the fillModeNonSolid feature
        device_features.tessellationShader  = VK_TRUE; // Enable the tessellationShader feature
        device_features.imageCubeArray      = VK_TRUE;
        device_features.multiDrawIndirect   = VK_TRUE;
        //device_features.geometryShader      = VK_TRUE; // Enable the geometryShader feature
        // Link Features (Crucial!)
        // The caller prepared the required_features struct (VkPhysicalDeviceFeatures2)
        // with the pNext chain pointing to specific feature structs (Vulkan11, 12, 13, etc.)
        // We pass this entire structure via pNext. pEnabledFeatures *must* be null.
        VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features = {};
        dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
        dynamic_rendering_features.dynamicRendering = VK_TRUE;

        VkPhysicalDeviceFeatures2 device_features2 = {};
        device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        device_features2.features = {}; // default or custom features
        device_features2.pNext = &dynamic_rendering_features;

        VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT listRestart{};
        listRestart.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT;
        listRestart.primitiveTopologyListRestart = VK_TRUE;
        dynamic_rendering_features.pNext = &listRestart;

        vkGetPhysicalDeviceFeatures2( physical_device->getHandle(), &device_features2 );

        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pNext = &device_features2; // ← CHAIN STARTS HERE
        create_info.pEnabledFeatures = nullptr; // MUST BE NULL if using features2


        // Enable Required Extensions
        const std::vector<const char*>& required_extensions = _instance->getRequiredDeviceExtensions();
        create_info.enabledExtensionCount = static_cast< uint32_t >( required_extensions.size() );
        create_info.ppEnabledExtensionNames = required_extensions.data();
        KEGE_LOG_INFO << "Enabling Device Extensions:\n";
        for( const char* ext_name : required_extensions ) {
            KEGE_LOG_INFO <<"  - " << ext_name <<"\n";
        }
        KEGE_LOG_INFO << Log::nl;

        // Enable Validation Layers (Device layers are deprecated - use instance layers)
        if ( _instance->isValidationEnabled() )
        {
            // While device layers are deprecated, some older examples/drivers might check this.
            // It's generally safe to pass the same layers as the instance IF they were instance-level layers.
            // However, the modern approach relies solely on instance layers.
            // Best practice: Keep this 0 unless you have a specific reason related to older drivers.
            create_info.enabledLayerCount = (int)_instance->getValidationLayers().size(); // static_cast<uint32_t>(REQUIRED_VALIDATION_LAYERS.size());
            create_info.ppEnabledLayerNames = _instance->getValidationLayers().data(); // REQUIRED_VALIDATION_LAYERS.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
            create_info.ppEnabledLayerNames = nullptr;
        }

        // 4. Create the Logical Device
        VkResult result = vkCreateDevice( physical_device->getHandle(), &create_info, nullptr, &_device );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << "vkCreateDevice failed! Error code: " << result<<Log::nl;
            _device = VK_NULL_HANDLE; // Ensure null on failure
            return VK_NULL_HANDLE;
        }

        return _device;
    }

    /**
     * @brief Create a semaphore synchronization object
     *
     * Creates a VkSemaphore that can be used to synchronize GPU operations.
     *
     * @return Handle to the newly created semaphore
     */

    VkSemaphore Manager::createSemaphore()
    {
        VkResult result;
        VkSemaphore semaphore;
        VkSemaphoreCreateInfo crate_info{};
        crate_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        result = vkCreateSemaphore( _device, &crate_info, nullptr, &semaphore );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vk::vkResultToString( result ) << kege::Log::nl;
            return VK_NULL_HANDLE;
        }
        return semaphore;
    }

    void Manager::destroySemaphore( VkSemaphore& semaphore )
    {
        if ( semaphore != VK_NULL_HANDLE )
        {
            vkDestroySemaphore( _device, semaphore, nullptr );
            semaphore = VK_NULL_HANDLE;
        }
    }
    /**
     * @brief Create a fence synchronization object
     *
     * Creates a VkFence that can be used to synchronize CPU and GPU operations.
     *
     * @param initially_signaled Whether the fence should be created in the signaled state
     * @return Handle to the newly created fence
     */

    VkFence Manager::createFence( bool initially_signaled )
    {
        VkFence fence;
        VkResult result;
        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = (initially_signaled) ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        result = vkCreateFence( _device, &fence_info, nullptr, &fence );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vk::vkResultToString( result ) << kege::Log::nl;
            return VK_NULL_HANDLE;
        }
        return fence;
    }

    void Manager::destroyFence( VkFence& fence )
    {
        if ( fence != VK_NULL_HANDLE )
        {
            vkDestroyFence( _device, fence, nullptr );
            fence = VK_NULL_HANDLE;
        }
    }


    void Manager::copyBufferToImage
    (
        VkCommandBuffer command,
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height,
        VkImageAspectFlags aspect_flags,
        VkImageLayout old_layout,
        VkImageLayout new_layout
    )
    {
        // Transition image layout to TRANSFER_DST_OPTIMAL
        VkImageMemoryBarrier barrier{};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = old_layout;
        barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = aspect_flags;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.srcAccessMask                   = 0; // since old layout is undefined
        barrier.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier
        (
            command,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        // Prepare buffer-to-image region
        VkBufferImageCopy region{};
        region.bufferOffset                     = 0;
        region.bufferRowLength                  = 0; // tightly packed
        region.bufferImageHeight                = 0; // tightly packed
        region.imageSubresource.aspectMask      = aspect_flags;
        region.imageSubresource.mipLevel        = 0;
        region.imageSubresource.baseArrayLayer  = 0;
        region.imageSubresource.layerCount      = 1;
        region.imageOffset                      = {0, 0, 0};
        region.imageExtent                      = {width, height, 1};

        // Copy buffer to image
        vkCmdCopyBufferToImage
        (
            command,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        // Transition image to SHADER_READ_ONLY_OPTIMAL
        barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout     = new_layout;//;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier
        (
            command,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
    }


    void Manager::debugSetObjectName( uint64_t object_handle, VkObjectType object_type, const char* name)
    {
        VkDebugUtilsObjectNameInfoEXT name_info = {};
        name_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        name_info.objectType = object_type;
        name_info.objectHandle = object_handle; // Handle to the Vulkan object
        name_info.pObjectName = name;         // Name you want to assign

        // Call the function via the device dispatch table
        if (vkSetDebugUtilsObjectName == nullptr)
        {
            vkSetDebugUtilsObjectName = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr( _device, "vkSetDebugUtilsObjectNameEXT");
        }
        if (vkSetDebugUtilsObjectName != nullptr)
        {
            vkSetDebugUtilsObjectName( _device, &name_info );
        }
    }

    VkResult Manager::createBuffer
    (
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags memory_properties,
        VkDeviceSize size,
        const void* data,
        VkBuffer* buffer,
        VkDeviceMemory* memory
    )
    {
        VkBufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.usage = usage;
        create_info.size = size;

        VkResult result = vkCreateBuffer( _device, &create_info, nullptr, buffer );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return result;
        }

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements( _device, *buffer, &memory_requirements );
        result = allocateDeviceMemory( memory_requirements, memory_properties, memory );
        if( result != VK_SUCCESS )
        {
            kege::Log::error << "Could not allocate memory for a buffer in createBuffer()"<<Log::nl;
            return result;
        }

        vkBindBufferMemory( _device, *buffer, *memory, 0 );

        if ( data != nullptr && size != 0 && buffer )
        {
            void* mapped_ptr = nullptr;
            vkMapMemory( _device, *memory, 0, size, 0, &mapped_ptr );
            memcpy( mapped_ptr, data, size );
            vkUnmapMemory( _device, *memory );
        }
        return result;
    }

    void Manager::destroyBuffer( VkBuffer& buffer )
    {
        if ( buffer != VK_NULL_HANDLE )
        {
            waitIdle();
            vkDestroyBuffer( _device, buffer, nullptr );
            buffer = VK_NULL_HANDLE;
        }
    }

    void Manager::destroyMemory( VkDeviceMemory& memory )
    {
        if ( memory != VK_NULL_HANDLE )
        {
            waitIdle();
            vkFreeMemory( _device, memory, nullptr );
            memory = VK_NULL_HANDLE;
        }
    }

    VkImage Manager::createImage
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
    )
    {
        VkImageCreateInfo image_info = {};
        image_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.flags         = flags;
        image_info.imageType     = type; // Use conversion helper
        image_info.extent.width  = width;
        image_info.extent.height = height;
        image_info.extent.depth  = depth;
        image_info.mipLevels     = mip_levels;
        image_info.arrayLayers   = array_layers;
        image_info.format        = format;
        image_info.tiling        = VK_IMAGE_TILING_OPTIMAL; // Almost always optimal
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage         = usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; // Use conversion helper
        image_info.samples       = sample_count;
        image_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE; // Assuming not sharing between queue families for now

        VkImage image;
        VkResult result = vkCreateImage( _device, &image_info, nullptr, &image );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result ) <<Log::nl;
            return VK_NULL_HANDLE;
        };
        return image;
    }
    void  Manager::destroyImage( VkImage& image )
    {
        if ( image != VK_NULL_HANDLE )
        {
            vkDestroyImage( _device, image, nullptr );
            image = VK_NULL_HANDLE;
        }
    }

    VkImageView Manager::createImageView
    (
        VkImage image,
        VkImageViewType type,
        VkFormat format,
        int array_layers,
        int mip_levels,
        VkImageAspectFlags aspect
    )
    {

        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image;
        view_info.viewType = type;
        view_info.format = format;
        view_info.components = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        };

        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = mip_levels;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = array_layers;
        view_info.subresourceRange.aspectMask = aspect;

        VkImageView image_view;
        VkResult result = vkCreateImageView( _device, &view_info, nullptr, &image_view );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result ) <<Log::nl;
            return VK_NULL_HANDLE;
        }

        return image_view;
    }

    void Manager::destroyImageView( VkImageView& image_view )
    {
        if ( image_view != VK_NULL_HANDLE )
        {
            vkDestroyImageView( _device, image_view, nullptr );
            image_view = VK_NULL_HANDLE;
        }
    }

    void Manager::destroySampler( VkSampler& sampler )
    {
        if ( sampler != VK_NULL_HANDLE )
        {
            vkDestroySampler( _device, sampler, nullptr );
            sampler = VK_NULL_HANDLE;
        }
    }

    VkResult Manager::createPipelineLayout
    (
        const VkPipelineLayoutCreateInfo* info,
        const VkAllocationCallbacks* allocator,
        VkPipelineLayout* layout
    )
    {
        return vkCreatePipelineLayout( _device, info, allocator, layout );
    }

    void Manager::destroyPipelineLayout( VkPipelineLayout& layout, const VkAllocationCallbacks *allocator )
    {
        if ( layout != VK_NULL_HANDLE )
        {
            waitIdle();
            vkDestroyPipelineLayout( _device, layout, allocator );
            layout = VK_NULL_HANDLE;
        }
    }

    VkResult Manager::createDescriptorPool( const VkDescriptorPoolCreateInfo* info, const VkAllocationCallbacks *allocator, VkDescriptorPool *pool )
    {
        return vkCreateDescriptorPool( _device, info, allocator, pool );
    }

    void Manager::destroyDescriptorPool( VkDescriptorPool pool, const VkAllocationCallbacks* allocator )
    {
        vkDestroyDescriptorPool( _device, pool, allocator );
    }

    void Manager::updateDescriptorSets
    (
        uint32_t write_count,
        const VkWriteDescriptorSet* writes,
        uint32_t descriptor_count,
        const VkCopyDescriptorSet* descriptor_copies
    )
    {
        vkUpdateDescriptorSets( _device, write_count, writes, descriptor_count, descriptor_copies );
    }

    VkResult Manager::allocateDescriptorSets( const VkDescriptorSetAllocateInfo* info, VkDescriptorSet* descriptor_sets )
    {
        return vkAllocateDescriptorSets( _device, info, descriptor_sets );
    }

    void Manager::freeDescriptorSets( VkDescriptorPool pool, uint32_t count, const VkDescriptorSet* sets )
    {
        vkFreeDescriptorSets( _device, pool, count, sets );
    }

    VkResult Manager::createDescriptorSetLayout
    (
        const VkDescriptorSetLayoutCreateInfo* info,
        const VkAllocationCallbacks* allocator,
        VkDescriptorSetLayout* layout
    )
    {
        return vkCreateDescriptorSetLayout( _device, info, allocator, layout );
    }

    void Manager::destroyUniformSetLayout( VkDescriptorSetLayout layout, const VkAllocationCallbacks* allocator )
    {
        waitIdle();
        vkDestroyDescriptorSetLayout( _device, layout, allocator );
    }

    VkResult Manager::createSwapchain( const VkSwapchainCreateInfoKHR *info, const VkAllocationCallbacks* allocator, VkSwapchainKHR* swapchain )
    {
        return vkCreateSwapchainKHR( _device, info, allocator, swapchain );
    }
    void Manager::destroySwapchain( VkSwapchainKHR swapchain, const VkAllocationCallbacks* allocator )
    {
        waitIdle();
        vkDestroySwapchainKHR( _device, swapchain, allocator );
    }

    VkResult Manager::allocateDeviceMemory
    (
        VkMemoryRequirements memory_requirements,
        VkMemoryPropertyFlags memory_properties,
        VkDeviceMemory* memory
    )
    {
        uint32_t memory_type_index = 0;
        for( ; memory_type_index < _physical_device_memory_properties.memoryTypeCount; ++memory_type_index )
        {
            VkMemoryPropertyFlags type = _physical_device_memory_properties.memoryTypes[ memory_type_index ].propertyFlags;
            if
            (
                (memory_requirements.memoryTypeBits & (1 << memory_type_index)) &&
                ((type & memory_properties) == memory_properties)
            )
            {
                break;
            }
        }

        VkMemoryAllocateInfo memory_allocate_info =
        {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            nullptr,
            memory_requirements.size,
            memory_type_index
        };

        return vkAllocateMemory( _device, &memory_allocate_info, nullptr, memory );
    }


    void Manager::waitIdle()const
    {
        if ( _device == VK_NULL_HANDLE || _device == VK_NULL_HANDLE) return;
        vkDeviceWaitIdle(_device);
    }

    Manager::Manager()
    :   _device( nullptr )
    ,   _instance( nullptr )
    ,   vkSetDebugUtilsObjectName( nullptr )
    {}

}
