//
//  vulkan-device.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/24/25.
//

#include "vulkan-device.hpp"
#include "vulkan-instance.hpp"
#include "vulkan-swapchain.hpp"
#include "vulkan-command-buffer.hpp"

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wdocumentation"
//#pragma clang diagnostic ignored "-Wnullability-completeness"
//// Define VMA implementation in exactly one cpp file
//#define VMA_IMPLEMENTATION
//#include <vk_mem_alloc.h>  // Include Vulkan Memory Allocator (VMA)
//#pragma clang diagnostic pop

namespace kege::vk{

    Device::~Device()
    {
        if ( _device != VK_NULL_HANDLE )
        {
            shutdown(); // Ensure shutdown is called if not explicitly done
        }
    }

    Device::Device()
    :   vkSetDebugUtilsObjectName( nullptr )
    {
        //TODO:  Initialize maps, etc. if needed (often done implicitly)
    }

    VkResult createCommandBufferPool( VkDevice device, uint32_t queue_family_index, VkCommandPoolCreateFlags flags, VkCommandPool* command_pool )
    {
        VkCommandPoolCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        create_info.queueFamilyIndex = queue_family_index;
        create_info.flags = flags;
        return vkCreateCommandPool( device, &create_info, nullptr, command_pool );
    }

    void getDeviceQueue( VkDevice device, uint32_t queue_family_index, QueueInfo& queue )
    {
        queue.family_index = queue_family_index;
        vkGetDeviceQueue( device, queue_family_index, 0, &queue.queue );
    }

    bool Device::initialize( vk::PhysicalDevice* physical_device, VkSurfaceKHR surface )
    {
        if (_device != VK_NULL_HANDLE)
        {
            KEGE_LOG_WARN << "Logical device already created."<<Log::nl;
            return true;
        }

        _surface = surface;
        _instance = physical_device->getInstance();
        _physical_device = physical_device;

        if ( _physical_device->getHandle() == VK_NULL_HANDLE )
        {
            KEGE_LOG_ERROR << "Invalid PhysicalDevice passed to createLogicalDevice."<<Log::nl;
            return false;
        }

        // 1. Find Queue Families on the selected Physical Device
        // Note: Presentation support might require a VkSurfaceKHR. If your device
        // creation depends on presentation, the surface needs to be created earlier
        // and passed into findQueueFamilies. For simplicity here, we assume findQueueFamilies
        // can determine graphics/compute/transfer without a surface for now.
        // A more robust design might find queues after surface creation if needed.
        _queue_family_indices = findQueueFamilies( _physical_device->getHandle(), _surface );
        if ( !_queue_family_indices.graphics_family.has_value() ) // Assuming graphics is always required
        {
            KEGE_LOG_ERROR << "Failed to find a suitable graphics queue family on the selected device."<<Log::nl;
             return false;
        }
         // Optionally check for compute/transfer if strictly required by the engine
         // if (!indices.compute_family.has_value()) { ... }

        // 2. Prepare Unique Queue Create Infos
        std::vector< VkDeviceQueueCreateInfo > queue_create_infos;
        std::set< uint32_t > unique_queue_families; // Use set to avoid duplicates

        // Add required queue families
        unique_queue_families.insert( _queue_family_indices.graphics_family.value() );
        if ( _queue_family_indices.compute_family.has_value() )
        {
            unique_queue_families.insert( _queue_family_indices.compute_family.value() );
        }
        if ( _queue_family_indices.transfer_family.has_value() )
        {
            unique_queue_families.insert( _queue_family_indices.transfer_family.value() );
        }
        // Add present family if it's different and needed (usually needed for swapchain)
        // Presentation check often requires a surface, handle carefully.
        if ( _queue_family_indices.present_family.has_value() )
        {
             unique_queue_families.insert( _queue_family_indices.present_family.value() );
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

        vkGetPhysicalDeviceFeatures2( _physical_device->getHandle(), &device_features2 );

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
        VkResult result = vkCreateDevice( _physical_device->getHandle(), &create_info, nullptr, &_device );
        if ( result != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "vkCreateDevice failed! Error code: " << result<<Log::nl;
            _device = VK_NULL_HANDLE; // Ensure null on failure
            return false;
        }

        KEGE_LOG_INFO <<"- " << "Vulkan Logical Device created." <<"\n";

        // 5. Get Queue Handles
        // Use the stored _queue_family_indices
        vkGetDeviceQueue( _device, _queue_family_indices.graphics_family.value(), 0, &_graphics_queue.queue );
        _graphics_queue.family_index = _queue_family_indices.graphics_family.value();

        if ( _queue_family_indices.compute_family.has_value() )
        {
            vkGetDeviceQueue( _device, _queue_family_indices.compute_family.value(), 0, &_compute_queue.queue );
            _compute_queue.family_index = _queue_family_indices.compute_family.value();
        }
        else
        {
            // Compute might reuse graphics queue if no dedicated compute family found
            _compute_queue = _graphics_queue; // Or handle as unsupported if separate needed
            KEGE_LOG_INFO <<"- " << "Using graphics queue for compute operations.\n";
        }

        if ( _queue_family_indices.transfer_family.has_value() )
        {
            vkGetDeviceQueue( _device, _queue_family_indices.transfer_family.value(), 0, &_transfer_queue.queue );
            _transfer_queue.family_index = _queue_family_indices.transfer_family.value();
        }
        else
        {
            // Transfer might reuse graphics or compute queue
            _transfer_queue = _graphics_queue; // Or compute if available? Often graphics is fine.
            KEGE_LOG_INFO <<"- " << "Using graphics queue for transfer operations.\n";
        }

        if ( _queue_family_indices.present_family.has_value() )
        {
            vkGetDeviceQueue( _device, _queue_family_indices.present_family.value(), 0, &_present_queue.queue );
            _present_queue.family_index = _queue_family_indices.present_family.value();
        }
        else
        {
            // This is usually an error if you need devices, as findQueueFamilies should have failed.
            KEGE_LOG_ERROR << "Present queue family index not found after device creation (should not happen)."<<Log::nl;
            // Maybe default to graphics? But presentation might fail later.
            _present_queue = _graphics_queue;
        }

        /** ---------- Get Devuce Queues ---------- */

        getDeviceQueue( _device, _queue_family_indices.graphics_family.has_value(), _graphics_queue );
        getDeviceQueue( _device, _queue_family_indices.compute_family.has_value(),  _compute_queue  );
        getDeviceQueue( _device, _queue_family_indices.transfer_family.has_value(), _transfer_queue );
        getDeviceQueue( _device, _queue_family_indices.present_family.has_value(),  _present_queue  );

        /** ---------- Create Command Buffer Pools ---------- */

        result = createCommandBufferPool
        (
            _device,
            _graphics_queue.family_index,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            &_command_pools[ QueueType::Graphics ]
        );

        if ( result != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "unable to create CommandBufferPool. reason ->" << result<<Log::nl;
            return false;
        }

        result = createCommandBufferPool
        (
            _device,
            _compute_queue.family_index,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            &_command_pools[ QueueType::Compute ]
        );

        if ( result != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "unable to create CommandBufferPool. reason ->" << result<<Log::nl;
            return false;
        }

        result = createCommandBufferPool
        (
            _device,
            _transfer_queue.family_index,
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            &_command_pools[ QueueType::Transfer ]
        );

        if ( result != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "unable to create CommandBufferPool. reason ->" << result<<Log::nl;
            return false;
        }
        
        /*
        result = createCommandBufferPool
        (
            _device,
            _present_queue.family_index,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            &_command_pools[ QueueType::Present ]
        );

        if ( result != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "unable to create CommandBufferPool. reason ->" << result;
            return false;
        }
         */

        _descriptor_manager.initialize( _instance, this );
        return true;
    }

    void Device::shutdown()
    {
        if ( _device == VK_NULL_HANDLE ) return;

        waitIdle(); // Ensure GPU is idle before destroying anything

        // Destroy user-created resources first
        _descriptor_manager.shutdown();
        cleanupResources(); // Textures, Buffers, Samplers
        cleanupPipelines();
        cleanupShaders();
        cleanupCommandPools();
        cleanupSwapchains(); // Must be destroyed before surface/device
        cleanupSyncPrimitives(); // Fences, Semaphores

        // Destroy Surface
        if ( _surface != VK_NULL_HANDLE )
        {
            _instance->destroySurface( _surface, nullptr );
            _surface = VK_NULL_HANDLE;
        }

        // Destroy Device
        if (_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(_device, nullptr);
            _device = VK_NULL_HANDLE;
        }

        KEGE_LOG_INFO << "Device Shutdown Complete."<<Log::nl;
    }

    bool Device::acquireNextSwapchainImage( const kege::Swapchain& swapchain, SemaphoreHandle signal_semaphore, uint32_t* out_image_index )
    {
        Swapchain* sc = _swapchains.get( swapchain.id );
        if ( sc == nullptr ) return false;

        VkResult result = vkAcquireNextImageKHR
        (
            _device,
            sc->_swapchain,
            UINT64_MAX, // Timeout (no timeout)
            _semaphores.get( signal_semaphore.id )->semaphore, // Semaphore to signal
            VK_NULL_HANDLE, // Fence (not using fence here)
            &sc->_curr_image_index
        );
        *out_image_index = sc->_curr_image_index;

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            KEGE_LOG_WARN << "Swapchain out of date during acquire. Needs recreation."<<Log::nl;
            sc->_needs_recreation = true;
            return false; // Cannot render to this image index
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            KEGE_LOG_WARN << "Swapchain suboptimal during acquire. Needs recreation soon."<<Log::nl;
            sc->_needs_recreation = true;
            // Still okay to render to this image for now
            return true;
        }
        else if (result == VK_ERROR_DEVICE_LOST)
        {
            KEGE_LOG_ERROR << "VK_ERROR_DEVICE_LOST in acquireNextSwapchainImage()."<<Log::nl;
            // This could be a more serious error
            return false;
        }
        else if (result != VK_SUCCESS)
        {
            KEGE_LOG_ERROR << "Failed to acquire swap chain image! Error: "<<Log::nl;
            return false;
        }

        // Success
        sc->_needs_recreation = false; // Reset flag on successful acquire (if not suboptimal)
        return true;
    }

    bool Device::presentSwapchainImage( const kege::Swapchain& swapchain, kege::SemaphoreHandle wait_semaphore, uint32_t image_index )
    {
        if ( swapchain.id < 0 )
        {
            KEGE_LOG_ERROR << "Invalid swapchain id in presentSwapchainImage."<<Log::nl;
            return false; // QueueHandle needs proper lookup
        }

        if ( wait_semaphore.id < 0 )
        {
            KEGE_LOG_ERROR << "Invalid wait_semaphore id in presentSwapchainImage."<<Log::nl;
            return false; // QueueHandle needs proper lookup
        }

        Semaphore* sem = _semaphores.get( wait_semaphore.id );
        if ( sem == nullptr )
        {
            KEGE_LOG_ERROR << "Invalid semaphore in presentSwapchainImage."<<Log::nl;
            return false;
        }

        Swapchain* sc = _swapchains.get( swapchain.id );
        if ( sc == nullptr )
        {
            KEGE_LOG_ERROR << "Invalid swapchain in presentSwapchainImage."<<Log::nl;
            return false;
        }

        /** --- Prepare Present Info --- */
        VkSemaphore wait_semaphores[] = { sem->semaphore };
        VkSwapchainKHR swapchains[] = { sc->_swapchain };

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = wait_semaphores;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr; // Optional

        /** --- Submit Present Request --- */
        VkResult result = vkQueuePresentKHR( _present_queue.queue, &present_info );

        /** --- Handle Results --- */
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            KEGE_LOG_WARN << "Out of date image during present. Needs recreation. in presentSwapchainImage"<<Log::nl;
            sc->_needs_recreation = true;
            return false;
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            KEGE_LOG_WARN << "Suboptimal image during present. Needs recreation soon. in presentSwapchainImage"<<Log::nl;
            sc->_needs_recreation = true;
            return false;
        }
        else if (result != VK_SUCCESS)
        {
            /** This could be a more serious error (e.g., device lost) */
            KEGE_LOG_ERROR << "Failed to queue present! in presentSwapchainImage "<<Log::nl;
            /** Consider handling VK_ERROR_DEVICE_LOST */
            return false;
        }
        /** On success, no specific action needed here, but render loop needs to check needsRecreation flag. */
        return true;
    }

    bool Device::needsRecreation( const kege::Swapchain& swapchain )
    {
        vk::Swapchain* sc = _swapchains.get( swapchain.id );
        if ( sc == nullptr )
        {
            KEGE_LOG_ERROR << "Invalid Swapchain handle in Device::needsRecreation()."<<Log::nl;
            return false; // Cannot render to this image index
        }
        return sc->_needs_recreation;
    }

    kege::ImageHandle Device::getSwapchainColorImage( const kege::Swapchain& swapchain, uint32_t index_image )
    {
        return _swapchains.get( swapchain.id )->_color_images[ index_image ];
    }

    kege::ImageHandle Device::getSwapchainDepthImage( const kege::Swapchain& swapchain, uint32_t index_image )
    {
        return _swapchains.get( swapchain.id )->_depth_images[ index_image ];
    }

    std::vector< kege::ImageHandle > Device::getSwapchainColorImages( const kege::Swapchain& swapchain )
    {
        return _swapchains.get( swapchain.id )->_color_images;
    }

    std::vector< kege::ImageHandle > Device::getSwapchainDepthImages( const kege::Swapchain& swapchain )
    {
        return _swapchains.get( swapchain.id )->_depth_images;
    }

    uint32_t Device::getSwapchainImageCount( const kege::Swapchain& swapchain )
    {
        return (uint32_t)_swapchains.get( swapchain.id )->_color_images.size();
    }

    uint32_t Device::getSwapchainImageIndex( const kege::Swapchain& swapchain )
    {
        return _swapchains.get( swapchain.id )->_curr_image_index;
    }

    kege::Extent2D Device::getSwapchainExtent( const kege::Swapchain& swapchain )
    {
        return _swapchains.get( swapchain.id )->_extent;
    }

    kege::Format Device::getSwapchainColorFormat( const kege::Swapchain& swapchain )
    {
        return _swapchains.get( swapchain.id )->_color_format;
    }

    kege::Format Device::getSwapchainDepthFormat( const kege::Swapchain& swapchain )
    {
        return _swapchains.get( swapchain.id )->_depth_format;
    }

    kege::Swapchain Device::createSwapchain( const kege::SwapchainDesc& desc )
    {
        int32_t id = _swapchains.gen();
        vk::Swapchain* swapchain = _swapchains.get( id );
        if ( swapchain->create( this, desc ) != VK_SUCCESS )
        {
            // error
            return { -1 };
        }
        return { id };
    }

    void Device::destroySwapchain( kege::Swapchain swapchain )
    {
        if ( _swapchains.get( swapchain.id ) != nullptr )
        {
            _swapchains.get( swapchain.id )->destroy();
            _swapchains.free( swapchain.id );
        }
    }

    kege::CommandBuffer* Device::createCommandBuffer( kege::QueueType type )
    {
        vk::CommandBuffer* command_buffer = new vk::CommandBuffer;
        command_buffer->_id = _command_buffers.insert( command_buffer );
        command_buffer->_command_pool = _command_pools[ type ];
        command_buffer->_device = this;
        command_buffer->_is_recording = false;
        command_buffer->_queue_type = type;

        VkCommandBufferAllocateInfo info = {};
        info.commandPool = _command_pools[ type ];
        info.commandBufferCount = 1;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        if ( allocateCommandBuffers( &info, &command_buffer->_handle ) != VK_SUCCESS )
        {
            // error
            return nullptr;
        }
        return command_buffer;
    }

    void Device::destroyCommandBuffer( kege::CommandBuffer* cmb )
    {
        if( cmb )
        {;
            if ( _command_buffers.get( cmb->id() ) != nullptr )
            {
                vk::CommandBuffer* recorder = *_command_buffers.get( cmb->id() );
                if ( recorder->_handle != VK_NULL_HANDLE )
                {
                    vkFreeCommandBuffers( _device, recorder->_command_pool, 1, &recorder->_handle );
                    recorder->_handle = VK_NULL_HANDLE;
                    _command_buffers.free( recorder->id() );
                }
            }
        }
    }

    bool Device::submitCommands
    (
        const std::vector< kege::CommandBuffer* >& command_buffers,
        FenceHandle* signal_fence,
        SemaphoreHandle* signal_semaphore,
        SemaphoreHandle* wait_semaphore
    )
    {
        std::vector< VkCommandBuffer > vk_command_buffers;
        vk_command_buffers.reserve( command_buffers.size() );
        for (const kege::CommandBuffer* cmb : command_buffers )
        {
            vk::CommandBuffer* combuf = *_command_buffers.get( cmb->id() );
            if ( combuf == nullptr )
            {
                KEGE_LOG_ERROR << "submission to compute queue failed in submitCommands()"<<Log::nl;
                return false;
            }
            vk_command_buffers.push_back( combuf->_handle );
        }

        VkFence fence = VK_NULL_HANDLE;
        if ( signal_fence != nullptr )
        {
            if ( _fences.get( signal_fence->id ) == nullptr )
            {
                KEGE_LOG_ERROR << "Fence with ID "
                << signal_fence->id << " not found during submitCommands()."<<Log::nl;
                return false;
            }
            fence = _fences.get( signal_fence->id )->fence;
        }

        std::vector< VkSemaphore > signal_semaphores;
        if ( signal_semaphore != nullptr )
        {
            if ( _semaphores.get( signal_semaphore->id ) == nullptr )
            {
                KEGE_LOG_ERROR << "Signal Semaphore with ID "
                << signal_semaphore->id << " not found during submitCommands()."<<Log::nl;
                return false;
            }
            signal_semaphores.push_back( _semaphores.get( signal_semaphore->id )->semaphore );
        }

        std::vector< VkSemaphore > wait_semaphores;
        if ( wait_semaphore != nullptr )
        {
            if ( _semaphores.get( wait_semaphore->id ) == nullptr )
            {
                KEGE_LOG_ERROR << "Wait Semaphore with ID "
                << wait_semaphore->id << " not found during submitCommands()."<<Log::nl;
                return false;
            }
            wait_semaphores.push_back( _semaphores.get( wait_semaphore->id )->semaphore );
        }

        VkSubmitInfo submit_info{};
        submit_info.sType                   = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pCommandBuffers         = vk_command_buffers.data();
        submit_info.commandBufferCount      = static_cast<uint32_t>( vk_command_buffers.size() );

        submit_info.pSignalSemaphores      = signal_semaphores.data();
        submit_info.signalSemaphoreCount   = static_cast< uint32_t >( signal_semaphores.size() );

        submit_info.pWaitSemaphores        = wait_semaphores.data();
        submit_info.waitSemaphoreCount     = static_cast< uint32_t >( wait_semaphores.size() );

        switch ( command_buffers[0]->getQueueType() )
        {
            case QueueType::Graphics:
            {
                VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
                submit_info.pWaitDstStageMask      = wait_stages;

                if( vkQueueSubmit( _graphics_queue.queue, 1, &submit_info, fence ) != VK_SUCCESS )
                {
                    KEGE_LOG_ERROR << "submission to graphics queue failed in submitCommands()"<<Log::nl;
                    return false;
                }
                break;
            }

            case QueueType::Compute:
            {
                VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
                submit_info.pWaitDstStageMask      = wait_stages;

                if( vkQueueSubmit( _compute_queue.queue, 1, &submit_info, fence ) != VK_SUCCESS )
                {
                    KEGE_LOG_ERROR << "submission to compute queue failed in submitCommands()"<<Log::nl;
                    return false;
                }
                break;
            }

            case QueueType::Transfer:
            {
                if( vkQueueSubmit( _transfer_queue.queue, 1, &submit_info, fence ) != VK_SUCCESS )
                {
                    KEGE_LOG_ERROR << "submission to transfer queue failed in submitCommands()"<<Log::nl;
                    return false;
                }
                break;
            }

            default: break;
        }
        return true;
    }

    void Device::endTransferQueueCommandBuffer( VkCommandBuffer command_buffer )
    {
        vkEndCommandBuffer( command_buffer );

        // submit the command buffer to the graphics queue for execution
        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        if( vkQueueSubmit( _transfer_queue.queue, 1, &submit_info, VK_NULL_HANDLE ) != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "submission to compute queue failed in submitCommands()"<<Log::nl;
            return;
        }
        vkQueueWaitIdle( _transfer_queue.queue );
        vkFreeCommandBuffers( _device, _command_pools[ QueueType::Transfer ], 1, &command_buffer );
    }

    VkCommandBuffer Device::beginTransferQueueCommandBuffer()
    {
        VkCommandBuffer command_buffer;

        // allocate a single command buffer from the command pool
        VkCommandBufferAllocateInfo info = {};
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;
        info.commandPool = _command_pools[ QueueType::Transfer ];
        allocateCommandBuffers( &info, &command_buffer );

        // begin recording commands into the command buffer
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer( command_buffer, &begin_info );
        return command_buffer;
    }

    kege::ImageHandle Device::createImage( const kege::ImageDesc& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {-1};

        kege::ImageHandle handle = { _textures.gen() };

        vk::Image* texr = _textures.get( handle.id );
        texr->current_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        texr->desc = desc;

        /** ---- Create Image Handle ---- */

        VkImageCreateInfo image_info = {};
        image_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType     = convertTextureType(desc.type); // Use conversion helper
        image_info.extent.width  = desc.width;
        image_info.extent.height = desc.height;
        image_info.extent.depth  = (desc.type == ImageType::Type3D) ? desc.depth : 1;
        image_info.mipLevels     = desc.mip_levels;
        image_info.arrayLayers   = (desc.type != ImageType::Type3D) ? desc.depth : 1;
        image_info.format        = convertFormat( desc.format );
        image_info.tiling        = VK_IMAGE_TILING_OPTIMAL; // Almost always optimal
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage         = convertTextureUsage(desc.usage); // Use conversion helper
        image_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE; // Assuming not sharing between queue families for now
        image_info.samples       = convertSampleCount(desc.sample_count);
        if (desc.type == ImageType::TypeCube || desc.type == ImageType::TypeCubeArray)
        {
            image_info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }

        texr->format = image_info.format;
        
        if ( vkCreateImage( _device, &image_info, nullptr, &texr->image ) != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "Failed to create image in createImage."<<Log::nl;
            destroyImage( handle );
            return {-1};
        };

        VkImageAspectFlags image_aspect_flag = getImageAspectFlags( desc.format );

        VkImageSubresourceRange subresource_range;
        subresource_range.aspectMask     = image_aspect_flag;
        subresource_range.levelCount     = desc.mip_levels;
        subresource_range.layerCount     = desc.depth;
        subresource_range.baseMipLevel   = 0;
        subresource_range.baseArrayLayer = 0;

        /** ---- Create Image Memory ---- */

        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements( _device, texr->image, &memory_requirements );
        allocateDeviceMemory( memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texr->memory );
        if( vkBindImageMemory( _device, texr->image, texr->memory, 0 ) != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "Failed to create image memory in createImage."<<Log::nl;
            destroyImage( handle );
            return {-1};
        }

        /** ---- Create Image View ---- */

        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = texr->image;
        view_info.viewType = convertTextureViewType( desc.type );
        view_info.format = image_info.format; // Use the same format
        view_info.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        view_info.subresourceRange.aspectMask = isDepthStencilFormat(desc.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        if ( isDepthStencilFormat( desc.format ) && !isDepthOnlyFormat( desc.format ) && !isStencilOnlyFormat( desc.format ) )
        {
            view_info.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        else if (isStencilOnlyFormat(desc.format))
        {
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = desc.mip_levels;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = image_info.arrayLayers;
        if (vkCreateImageView( _device, &view_info, nullptr, &texr->view ) != VK_SUCCESS)
        {
            KEGE_LOG_ERROR << "Failed to create image view in createImage."<<Log::nl;
            destroyImage( handle );
            return {-1};
        }

        // Set Debug Name (requires VK_EXT_debug_utils)
        if ( _instance->isValidationEnabled() && !desc.debug_name.empty() )
        {
            debugSetObjectName( (uint64_t)texr->image, VK_OBJECT_TYPE_IMAGE, desc.debug_name.c_str() );
        }


        
        if ( desc.data != nullptr )
        {
            /**
             * create a seperate shared buffer between the CPU & GPU that contains the data.
             */
            Buffer source = {};
            size_t size = desc.width * desc.height * desc.depth * sizeOfFormat( desc.format );
            VkResult result = createBuffer
            (
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                size, desc.data, &source
            );
            if ( result != VK_SUCCESS )
            {
                return {};
            }
            setBufferData( size, desc.data, &source );

            VkCommandBuffer command_buffer = beginTransferQueueCommandBuffer();
            copyBufferToTexture
            (
                command_buffer,
                source.buffer,
                texr->image,
                desc.width, desc.height,
                image_aspect_flag,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );
            endTransferQueueCommandBuffer( command_buffer );
        }

        return handle;
    }

    void Device::destroyImage( ImageHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id < 0 ) return;

        if ( _textures.get( handle.id ) != nullptr )
        {
            waitIdle();
            Image* texture = _textures.get( handle.id );
            vkDestroyImageView( _device, texture->view, nullptr );
            if ( texture->memory != VK_NULL_HANDLE )
            {
                vkFreeMemory( _device, texture->memory, nullptr );
                vkDestroyImage( _device, texture->image, nullptr );
            }
            if ( texture->view != VK_NULL_HANDLE )
            {
                vkDestroyImageView( _device, texture->view, nullptr );
            }
            _textures.free( handle.id );
        }
        else
        {
            KEGE_LOG_ERROR << "Trying to destroy invalid ImageHandle: " << handle.id<<Log::nl;
        }
    }

    VkResult Device::createBuffer( VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkDeviceSize size, const void* data, vk::Buffer* buffer )
    {
        VkBufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.usage = usage;
        create_info.size = size;
        
        VkResult result = vkCreateBuffer( _device, &create_info, nullptr, &buffer->buffer );
        if ( result != VK_SUCCESS )
        {
            logVkError( result, "vulkan-device.hpp", "createBuffer" );
            return result;
        }

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements( _device, buffer->buffer, &memory_requirements );
        result = allocateDeviceMemory( memory_requirements, memory_properties, &buffer->memory );
        if( result != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "Could not allocate memory for a buffer in createBuffer()"<<Log::nl;
            return result;
        }
        
        vkBindBufferMemory( _device, buffer->buffer, buffer->memory, 0 );
        return result;
    }

    void Device::setBufferData( VkDeviceSize size, const void* data, vk::Buffer* buffer )
    {
        if ( data != nullptr && size != 0 )
        {
            vkMapMemory( _device, buffer->memory, 0, size, 0, &buffer->mapped_ptr );
            memcpy( buffer->mapped_ptr, data, size );
            vkUnmapMemory( _device, buffer->memory );
            buffer->mapped_ptr = nullptr;
        }
    }

    kege::BufferHandle Device::createBuffer(const kege::BufferDesc& desc)
    {
        int32_t id = _buffers.gen();
        Buffer* buffer = _buffers.get( id );
        buffer->desc = desc;

        VkMemoryPropertyFlags memory_properties = convertMemoryPropertyFlags( desc.memory_usage );
        if ( createBuffer( convertBufferUsageFlags( desc.usage ), memory_properties, desc.size, desc.data, buffer ) != VK_SUCCESS )
        {
            _buffers.free( id );
            return {};
        }

        /**
         * For debug purposes, assign a name to buffer handle if a name id present.
         */
        if ( _instance->isValidationEnabled() && desc.debug_name )
        {
            debugSetObjectName( (uint64_t)buffer->buffer, VK_OBJECT_TYPE_BUFFER, desc.debug_name );
        }

        /**
         * For buffer that are shared between the CPU & GPU. The data can be copied directly
         * to the buffer.
         */
        if ( desc.memory_usage == MemoryUsage::CpuToGpu )
        {
            setBufferData( desc.size, desc.data, buffer );
        }

        /**
         * For buffer that only exist on the GPU. You can not copy data to them directly.
         * Thus you must create a seperate shared buffer between the CPU & GPU that contains
         * the data, then use a command buffer to transfer the data to the designated buffer.
         */
        else if ( desc.memory_usage == MemoryUsage::GpuOnly )
        {
            /**
             * create a seperate shared buffer between the CPU & GPU that contains the data.
             */
            Buffer source = {};
            VkResult result = createBuffer
            (
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                desc.size, desc.data, &source
            );
            if ( result != VK_SUCCESS )
            {
                _buffers.free( id );
                return {};
            }
            setBufferData( desc.size, desc.data, &source );

            /**
             * transfer the data to the designated buffer.
             */
            VkBufferCopy copy_region{};
            copy_region.srcOffset = 0; // Optional
            copy_region.dstOffset = 0; // Optional
            copy_region.size = desc.size;
            VkCommandBuffer command_buffer = beginTransferQueueCommandBuffer();
            vkCmdCopyBuffer( command_buffer, source.buffer, buffer->buffer, 1, &copy_region );
            endTransferQueueCommandBuffer( command_buffer );
        }
        return { id };
    }

    void Device::updateBuffer( const BufferHandle& handle, uint64_t offset, uint64_t size, const void* data )
    {
        vk::Buffer* buffer = _buffers.get( handle.id );
        if ( buffer->desc.memory_usage == MemoryUsage::CpuToGpu )
        {
            vkMapMemory( _device, buffer->memory, offset, size, 0, &buffer->mapped_ptr );
            memcpy( buffer->mapped_ptr, data, size );
            vkUnmapMemory( _device, buffer->memory );
            buffer->mapped_ptr = nullptr;
        }
        else
        {
            KEGE_LOG_ERROR << "can not map buffer, MemoryUsage must be set to CpuToGpu.";
        }
    }

    void Device::destroyBuffer( kege::BufferHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        if ( _buffers.get( handle.id ) != nullptr )
        {
            waitIdle();
            vkFreeMemory( _device, _buffers.get( handle.id )->memory, nullptr );
            vkDestroyBuffer( _device, _buffers.get( handle.id )->buffer, nullptr );
            _buffers.get( handle.id )->buffer = VK_NULL_HANDLE;
            _buffers.get( handle.id )->memory = VK_NULL_HANDLE;
            _buffers.free( handle.id );
        }
    }

    kege::SamplerHandle Device::createSampler(const kege::SamplerDesc& desc)
    {
        if ( _device == VK_NULL_HANDLE ) return {-1};

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = convertFilter( desc.mag_filter );
        samplerInfo.minFilter = convertFilter( desc.min_filter );
        samplerInfo.mipmapMode = convertMipmapMode(desc.mipmap_mode);
        samplerInfo.addressModeU = convertAddressMode( desc.address_mode_u );
        samplerInfo.addressModeV = convertAddressMode( desc.address_mode_v );
        samplerInfo.addressModeW = convertAddressMode( desc.address_mode_w );
        samplerInfo.mipLodBias = desc.mip_lod_bias;
        samplerInfo.anisotropyEnable = desc.anisotropy_enable ? VK_TRUE : VK_FALSE;

        samplerInfo.maxAnisotropy = ( desc.anisotropy_enable )
        ? std::min(desc.max_anisotropy, _limits.max_sampler_anisotropy)
        : 1.0f; // Clamp to device limits

        samplerInfo.compareEnable = desc.compare_enable ? VK_TRUE : VK_FALSE;
        samplerInfo.compareOp = convertCompareOp(desc.compare_op);
        samplerInfo.minLod = desc.min_lod;
        samplerInfo.maxLod = desc.max_lod;
        samplerInfo.borderColor = convertBorderColor(desc.border_color);
        samplerInfo.unnormalizedCoordinates = VK_FALSE; // Default

        Sampler sampler;
        sampler.desc = desc;
        if (vkCreateSampler(_device, &samplerInfo, nullptr, &sampler.sampler) != VK_SUCCESS)
        {
            KEGE_LOG_ERROR << "Failed to create sampler!";
            return {-1};
        }

        kege::SamplerHandle handle = { _samplers.gen() };
        *_samplers.get( handle.id ) = sampler;

        // Set Debug Name (requires VK_EXT_debug_utils)
        if ( _instance->isValidationEnabled() && desc.debug_name )
        {
            debugSetObjectName( (uint64_t)sampler.sampler, VK_OBJECT_TYPE_SAMPLER, desc.debug_name );
        }

        return handle;
    }

    void Device::destroySampler(kege::SamplerHandle handle)
    {
         if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

         if ( _samplers.get( handle.id ) != nullptr )
         {
             waitIdle();
             vkDestroySampler( _device, _samplers.get( handle.id )->sampler, nullptr );
             _samplers.free( handle.id );
         }
    }

    kege::ShaderHandle Device::createShader( const kege::ShaderDesc& desc )
    {
         if ( _device == VK_NULL_HANDLE || desc.byte_code.empty() ) return {-1};

        kege::ShaderHandle handle = { _shaders.gen() };
        vk::Shader* shader = _shaders.get( handle.id );

        shader->desc = desc; // Store original desc if needed later

        VkShaderModuleCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = desc.byte_code.size() * sizeof( desc.byte_code[0] );
        create_info.pCode = desc.byte_code.data();
        VkResult result;
        if ((result = vkCreateShaderModule( _device, &create_info, nullptr, &shader->shader_module ) ) != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "Failed to create shader module!" <<Log::nl;
            return {-1};
        }

        // Set Debug Name (requires VK_EXT_debug_utils)
        if ( _instance->isValidationEnabled() && !desc.debug_name.empty() )
        {
            debugSetObjectName( (uint64_t)shader->shader_module, VK_OBJECT_TYPE_SHADER_MODULE, shader->desc.debug_name.data() );
        }

        return handle;
    }

    void Device::destroyShader( kege::ShaderHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        if ( _shaders.get( handle.id ) != nullptr )
        {
            waitIdle();
            vkDestroyShaderModule(_device, _shaders.get( handle.id )->shader_module, nullptr);
            _shaders.free( handle.id );
        }
    }

    kege::PipelineLayoutHandle Device::createPipelineLayout( const kege::PipelineLayoutDesc& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {-1};
        return _descriptor_manager.createPipelineLayout( desc );
    }

    const vk::PipelineLayout* Device::getPipelineLayout( kege::PipelineLayoutHandle handle ) const
    {
        return _descriptor_manager.getPipelineLayout( handle );
    }

    void Device::destroyPipelineLayout( kege::PipelineLayoutHandle handle )
    {
        return _descriptor_manager.destroyPipelineLayout( handle );
    }

    kege::PipelineHandle Device::createGraphicsPipeline( const kege::GraphicsPipelineDesc& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {-1};

        VkPipelineLayout layout = _descriptor_manager.getVkPipelineLayout( desc.pipeline_layout );
        if ( layout == VK_NULL_HANDLE )
        {
            KEGE_LOG_ERROR << "Invalid pipeline-layout in createGraphicsPipeline!"<<Log::nl;
            return {-1};
        }

        // --- Translate Desc to Vulkan Structures ---
        VkGraphicsPipelineCreateInfo graphics_create = {};
        graphics_create.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        // 1. Shader Stages
        std::vector< VkPipelineShaderStageCreateInfo > shader_stages;
        for (int i=0; i<desc.shader_stages.size(); ++i)
        {
            Shader* shader = _shaders.get( desc.shader_stages[i].id );
            //
            shader_stages.push_back
            ({
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
                convertShaderStage( shader->desc.stage ),
                shader->shader_module,
                shader->desc.entry_point.data()
            });
        }
        // Add other stages (Geometry, Tessellation) if present in desc...
        graphics_create.stageCount = static_cast< uint32_t >( shader_stages.size() );
        graphics_create.pStages = shader_stages.data();

        // 2. Vertex Input State
        std::vector< VkVertexInputBindingDescription > vertex_bindings;
        std::vector< VkVertexInputAttributeDescription > vertex_attributes;
        for (int i=0; i<desc.vertex_input_state.attributes.size(); ++i)
        {
            vertex_attributes.push_back
            ({
                .location = desc.vertex_input_state.attributes[i].location,
                .offset = desc.vertex_input_state.attributes[i].offset,
                .format = convertFormat( desc.vertex_input_state.attributes[i].format ),
                .binding = desc.vertex_input_state.attributes[i].binding
            });
        }
        for (int i=0; i<desc.vertex_input_state.bindings.size(); ++i)
        {
            vertex_bindings.push_back
            ({
                .stride = desc.vertex_input_state.bindings[i].stride,
                .inputRate = convertVertexInputRate( desc.vertex_input_state.bindings[i].input_rate ),
                .binding = desc.vertex_input_state.bindings[i].binding
            });
        }
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_bindings.size());
        vertexInputInfo.pVertexBindingDescriptions = vertex_bindings.empty() ? nullptr : vertex_bindings.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attributes.size());
        vertexInputInfo.pVertexAttributeDescriptions = vertex_attributes.empty() ? nullptr : vertex_attributes.data();
        graphics_create.pVertexInputState = &vertexInputInfo;

        // 3. Input Assembly
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        input_assembly_info.primitiveRestartEnable = desc.input_assembly_state.primitive_restart_enable;
        input_assembly_info.topology = convertPrimitiveTopology( desc.input_assembly_state.topology );
        graphics_create.pInputAssemblyState = &input_assembly_info;

        // 4. Viewport State (can be dynamic)
        // Assuming non-dynamic for now
        VkPipelineViewportStateCreateInfo viewport_state = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = nullptr; // Set via dynamic state or provide dummy
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = nullptr; // Set via dynamic state or provide dummy
        graphics_create.pViewportState = &viewport_state;

        // 5. Rasterization State
        VkPipelineRasterizationStateCreateInfo rasterization_info = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterization_info.lineWidth = desc.rasterization_state.line_width;
        rasterization_info.rasterizerDiscardEnable = desc.rasterization_state.rasterizer_discard_enable;
        rasterization_info.depthClampEnable = desc.rasterization_state.depth_clamp_enable;
        rasterization_info.depthBiasEnable = desc.rasterization_state.depth_bias_enable;
        rasterization_info.depthBiasClamp = desc.rasterization_state.depth_bias_clamp;
        rasterization_info.cullMode = VK_CULL_MODE_NONE;///convertCullMode( desc.rasterization_state.cull_mode );
        rasterization_info.frontFace = convertFrontFace( desc.rasterization_state.front_face );
        rasterization_info.depthBiasConstantFactor = desc.rasterization_state.depth_bias_constant_factor;
        rasterization_info.depthBiasSlopeFactor = desc.rasterization_state.depth_bias_slope_factor;
        rasterization_info.polygonMode = convertPolygonMode( desc.rasterization_state.polygon_mode );
        graphics_create.pRasterizationState = &rasterization_info;

        // 6. Multisample State
        VkPipelineMultisampleStateCreateInfo multisample_info = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisample_info.sampleShadingEnable = desc.multisample_state.sample_shading_enable;
        multisample_info.rasterizationSamples = convertSampleCount( desc.multisample_state.rasterization_samples );// VK_SAMPLE_COUNT_1_BIT;
        multisample_info.minSampleShading = desc.multisample_state.min_sample_shading;
        multisample_info.pSampleMask = 0;
        multisample_info.alphaToCoverageEnable = desc.multisample_state.alpha_to_coverage_enable;
        multisample_info.alphaToOneEnable = desc.multisample_state.alpha_to_one_enable;
        multisample_info.flags = 0;
        graphics_create.pMultisampleState = &multisample_info;

        // 7. Depth Stencil State
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depth_stencil_info.depthTestEnable       = desc.depth_stencil_state.depth_test_enable;
        depth_stencil_info.depthWriteEnable      = desc.depth_stencil_state.depth_write_enable;
        depth_stencil_info.depthBoundsTestEnable = desc.depth_stencil_state.depth_bounds_test_enable;
        depth_stencil_info.depthCompareOp       = convertCompareOp( desc.depth_stencil_state.depth_compare_op );
        depth_stencil_info.minDepthBounds       = desc.depth_stencil_state.min_depth_bounds;
        depth_stencil_info.maxDepthBounds       = desc.depth_stencil_state.max_depth_bounds;
        depth_stencil_info.stencilTestEnable    = desc.depth_stencil_state.stencil_test_enable;
        depth_stencil_info.front.compareMask    = desc.depth_stencil_state.front_op.compare_mask;
        depth_stencil_info.front.compareOp      = convertCompareOp( desc.depth_stencil_state.front_op.compare_op );
        depth_stencil_info.front.depthFailOp    = convertStencilOp( desc.depth_stencil_state.front_op.depth_fail_op );
        depth_stencil_info.front.failOp         = convertStencilOp( desc.depth_stencil_state.front_op.fail_op );
        depth_stencil_info.front.passOp         = convertStencilOp( desc.depth_stencil_state.front_op.pass_op );
        depth_stencil_info.front.reference      = desc.depth_stencil_state.front_op.reference;
        depth_stencil_info.front.writeMask      = desc.depth_stencil_state.front_op.write_mask;
        depth_stencil_info.back.depthFailOp     = convertStencilOp( desc.depth_stencil_state.back_op.depth_fail_op );
        depth_stencil_info.back.compareOp       = convertCompareOp( desc.depth_stencil_state.back_op.compare_op );
        depth_stencil_info.back.failOp          = convertStencilOp( desc.depth_stencil_state.back_op.fail_op );
        depth_stencil_info.back.passOp          = convertStencilOp( desc.depth_stencil_state.back_op.pass_op );
        depth_stencil_info.back.reference       = desc.depth_stencil_state.back_op.reference;
        depth_stencil_info.back.writeMask       = desc.depth_stencil_state.back_op.write_mask;
        graphics_create.pDepthStencilState      = &depth_stencil_info;

        // 8. Color Blend State
        std::vector< VkPipelineColorBlendAttachmentState > color_blend_attachment_states;
        for ( int i=0; i<desc.color_blend_state.attachments.size(); i++ )
        {
            VkPipelineColorBlendAttachmentState state = {};
            state.blendEnable         = desc.color_blend_state.attachments[i].blend_enable;
            state.alphaBlendOp        = convertBlendOp( desc.color_blend_state.attachments[i].alpha_blend_op );
            state.dstAlphaBlendFactor = convertBlendFactor( desc.color_blend_state.attachments[i].dst_alpha_blend_factor );
            state.dstColorBlendFactor = convertBlendFactor( desc.color_blend_state.attachments[i].dst_color_blend_factor );
            state.srcAlphaBlendFactor = convertBlendFactor( desc.color_blend_state.attachments[i].src_alpha_blend_factor );
            state.srcColorBlendFactor = convertBlendFactor( desc.color_blend_state.attachments[i].src_color_blend_factor );
            state.colorWriteMask      = convertColorComponentMask( desc.color_blend_state.attachments[i].color_write_mask );
            color_blend_attachment_states.push_back( state );
        }
        VkPipelineColorBlendStateCreateInfo color_blend_info = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        color_blend_info.attachmentCount = static_cast< int >( color_blend_attachment_states.size() );
        color_blend_info.pAttachments = color_blend_attachment_states.data();
        color_blend_info.logicOpEnable = desc.color_blend_state.logic_op_enable;
        graphics_create.pColorBlendState = &color_blend_info;

        // 9. Dynamic State (Optional but common for viewport/scissor)
        std::vector< VkDynamicState > dynamic_states = std::vector< VkDynamicState >
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            //VK_DYNAMIC_STATE_LINE_WIDTH
        };
        VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
        dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state_create_info.dynamicStateCount = static_cast< uint32_t >( dynamic_states.size() );
        dynamic_state_create_info.pDynamicStates = dynamic_states.data();
        graphics_create.pDynamicState = &dynamic_state_create_info;

        // 10. Pipeline Layout
        graphics_create.layout = layout;

        // 11. Render Pass / Rendering Info (Crucial!)
        // We'll use Dynamic Rendering (VK_KHR_dynamic_rendering) as it's simpler for a render graph
        // Check if dynamic rendering is supported first!
        std::vector< VkFormat > color_attachmen_formats;
        for(Format fmt : desc.color_attachment_formats )
        {
            VkFormat vk_fmt = convertFormat(fmt);
            if (vk_fmt == VK_FORMAT_UNDEFINED)
            {
                KEGE_LOG_ERROR << "Invalid format passed for color attachment!"<<Log::nl;
                return {-1};
            }
            color_attachmen_formats.push_back( vk_fmt );
        }
        VkPipelineRenderingCreateInfoKHR rendering_info{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR};
        rendering_info.stencilAttachmentFormat = convertFormat(desc.stencil_attachment_format);
        rendering_info.depthAttachmentFormat = convertFormat(desc.depth_attachment_format);
        rendering_info.colorAttachmentCount = static_cast< uint32_t >( color_attachmen_formats.size() );
        rendering_info.pColorAttachmentFormats = ( !color_attachmen_formats.empty() ) ? color_attachmen_formats.data() : nullptr;
        // Chain this to pipelineInfo.pNext
        graphics_create.pNext = &rendering_info;
        graphics_create.renderPass = VK_NULL_HANDLE; // Must be NULL for dynamic rendering

        // 12. Base Pipeline (for derivatives, ignore for now)
        graphics_create.basePipelineHandle = VK_NULL_HANDLE;
        graphics_create.basePipelineIndex = -1;

        // --- Create Pipeline ---
        VkPipeline handle;
        VkResult result = vkCreateGraphicsPipelines( _device, VK_NULL_HANDLE, 1, &graphics_create, nullptr, &handle );
        if ( result != VK_SUCCESS )
        {
            KEGE_LOG_ERROR <<"createGraphicsPipeline" <<Log::nl;
            return {-1};
        }

        // Add to runtime map and cache
        int id = _graphics_pipelines.gen();
        {
            vk::GraphicsPipeline* pipeline = _graphics_pipelines.get( id );
            pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
            pipeline->pipeline = handle;
            pipeline->desc = desc;
        }

        if ( _instance->isValidationEnabled() && !desc.debug_name.empty() )
        {
            debugSetObjectName( (uint64_t)handle, VK_OBJECT_TYPE_IMAGE, desc.debug_name.c_str() );
        }
        return {id};
    }

    void Device::destroyGraphicsPipeline(PipelineHandle handle)
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        if ( _graphics_pipelines.get( handle.id ) != nullptr )
        {
            waitIdle();
            vkDestroyPipeline( _device, _graphics_pipelines.get( handle.id )->pipeline, nullptr );
            _graphics_pipelines.get( handle.id )->pipeline = VK_NULL_HANDLE;
            _graphics_pipelines.free( handle.id );
        }
    }

    kege::PipelineHandle Device::createComputePipeline( const kege::ComputePipelineDesc& desc )
    {
        VkPipelineLayout layout = _descriptor_manager.getVkPipelineLayout( desc.pipeline_layout );
        if ( layout == VK_NULL_HANDLE )
        {
            KEGE_LOG_ERROR << "Invalid pipeline-layout in createGraphicsPipeline!"<<Log::nl;
            return {-1};
        }

        Shader* shader = _shaders.get( desc.compute_shader.id );

        VkPipelineShaderStageCreateInfo create_shader_info = {};
        create_shader_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        create_shader_info.stage = convertShaderStage( shader->desc.stage );
        create_shader_info.module = shader->shader_module;
        create_shader_info.pName = shader->desc.entry_point.data();

        VkComputePipelineCreateInfo create_pipeline_info{};
        create_pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        create_pipeline_info.layout = layout;
        create_pipeline_info.stage = create_shader_info;

        VkPipeline pipeline;
        VkResult result = vkCreateComputePipelines
        (
            _device,
            VK_NULL_HANDLE,
            1,
            &create_pipeline_info,
            nullptr,
            &pipeline
        );

        if ( result != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "createComputePipeline" <<Log::nl;
            return {-1};
        }

        int32_t id = _compute_pipelines.gen();
        ComputePipeline* compute = _compute_pipelines.get( id );
        compute->pipeline = pipeline;
        compute->desc = desc;
        return { id };
    }

    void Device::destroyComputePipeline( kege::PipelineHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        if ( _compute_pipelines.get( handle.id ) != nullptr )
        {
            waitIdle();
            vkDestroyPipeline( _device, _compute_pipelines.get( handle.id )->pipeline, nullptr );
            _compute_pipelines.get( handle.id )->pipeline = VK_NULL_HANDLE;
            _compute_pipelines.free( handle.id );
        }
    }

    bool Device::updateDescriptorSets( const std::vector< kege::WriteDescriptorSet >& writes )
    {
        return _descriptor_manager.updateDescriptorSets( writes );
    }

    kege::DescriptorSetHandle Device::allocateDescriptorSet( kege::DescriptorSetLayoutHandle layout_handle )
    {
        return _descriptor_manager.allocateDescriptorSet( layout_handle );
    }

    kege::DescriptorSetHandle Device::allocateDescriptorSet( const std::vector< kege::DescriptorSetLayoutBinding >& bindings )
    {
        return _descriptor_manager.allocateDescriptorSet( bindings );
    }

    const vk::DescriptorSet* Device::getDescriptorSet(DescriptorSetHandle handle) const
    {
        return _descriptor_manager.getDescriptorSet( handle );
    }

    kege::DescriptorSetLayoutHandle Device::createDescriptorSetLayout( const std::vector< kege::DescriptorSetLayoutBinding >& bindings )
    {
        return _descriptor_manager.createDescriptorSetLayout( bindings );
    }

    const DescriptorSetLayout* Device::getDescriptorSetLayout( kege::DescriptorSetLayoutHandle handle ) const
    {
        return _descriptor_manager.getDescriptorSetLayout( handle );
    }

    void Device::destroyDescriptorSetLayout( kege::DescriptorSetLayoutHandle handle )
    {
        _descriptor_manager.destroyDescriptorSetLayout( handle );
    }

    void Device::freeDescriptorSet( kege::DescriptorSetHandle handle )
    {
        _descriptor_manager.freeDescriptorSet( handle );
    }

    // --- Wait Idle ---
    void Device::waitIdle()
    {
        if ( _device == VK_NULL_HANDLE || _device == VK_NULL_HANDLE) return;
        vkDeviceWaitIdle(_device);
    }

    // --- Buffer Mapping ---
    void* Device::mapBuffer( kege::BufferHandle handle, size_t offset, size_t size )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return nullptr;

        Buffer* buffer = _buffers.get( handle.id );
        if ( buffer == nullptr )
        {
            KEGE_LOG_ERROR << "Attempting to map a buffer with incompatible memory usage!" <<Log::nl;
            return nullptr;
        }
        // Only map if memory allows it and not already mapped
        if
        (
            buffer->mapped_ptr == nullptr &&
            (buffer->desc.memory_usage == MemoryUsage::CpuToGpu ||
             buffer->desc.memory_usage == MemoryUsage::GpuToCpu)
        )
        {
            if ( size == 0 )
            {
                size = buffer->desc.size - offset;
            }

            VkResult result = vkMapMemory( _device, buffer->memory, offset, size, 0, &buffer->mapped_ptr );
            if (result == VK_SUCCESS)
            {
                // Return pointer adjusted by offset
                return static_cast< uint8_t* >( buffer->mapped_ptr ) + offset;
            }
            else
            {
                switch ( result )
                {
                    case VK_ERROR_OUT_OF_HOST_MEMORY:
                        KEGE_LOG_ERROR << "VK_ERROR_OUT_OF_HOST_MEMORY in mapBuffer()"<<Log::nl;
                        break;
                    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        KEGE_LOG_ERROR << "VK_ERROR_OUT_OF_DEVICE_MEMORY in mapBuffer()"<<Log::nl;
                        break;
                    case VK_ERROR_MEMORY_MAP_FAILED:
                        KEGE_LOG_ERROR << "VK_ERROR_MEMORY_MAP_FAILED in mapBuffer()"<<Log::nl;
                        break;
                    default:
                        KEGE_LOG_ERROR << "unablde to map device memory in mapBuffer()"<<Log::nl;
                        break;
                }
                return nullptr;
            }
        }
        else if ( buffer->mapped_ptr != nullptr )
        {
            // Already mapped, return adjusted pointer
            return static_cast< uint8_t* >( buffer->mapped_ptr ) + offset;
        }
        return nullptr; // Handle not found
    }

    void Device::unmapBuffer( kege::BufferHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        Buffer* buffer = _buffers.get( handle.id );
        if ( buffer != nullptr )
        {
             if (buffer->mapped_ptr != nullptr)
             {
                 vkUnmapMemory( _device, buffer->memory );
                 buffer->mapped_ptr = nullptr;
             }
        }
    }

    kege::FenceHandle Device::createFence( bool initially_signaled )
    {
        FenceHandle handle = { _fences.gen() };
        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Ensure the fence starts as signaled
        if ( vkCreateFence( _device, &fence_info, nullptr, &_fences.get( handle.id )->fence ) != VK_SUCCESS )
        {
            _fences.free( handle.id );
            return {-1};
        }
        return handle;
    }

    void Device::destroyFence( kege::FenceHandle handle )
    {
        if ( _fences.get( handle.id ) != nullptr )
        {
            vkDestroyFence( _device, _fences.get( handle.id )->fence, nullptr );
            _fences.get( handle.id )->fence = VK_NULL_HANDLE;
            _fences.free( handle.id );
        }
    }

    kege::SemaphoreHandle Device::createSemaphore()
    {
        SemaphoreHandle handle = { _semaphores.gen() };
        VkSemaphoreCreateInfo crate_info{};
        crate_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if ( vkCreateSemaphore( _device, &crate_info, nullptr, &_semaphores.get( handle.id )->semaphore ) != VK_SUCCESS )
        {
            _semaphores.free( handle.id );
            return {-1};
        }
        return handle;
    }

    void Device::destroySemaphore( kege::SemaphoreHandle handle )
    {
        if ( _semaphores.get( handle.id ) != nullptr )
        {
            vkDestroySemaphore( _device, _semaphores.get( handle.id )->semaphore, nullptr );
            _semaphores.get( handle.id )->semaphore = VK_NULL_HANDLE;
            _semaphores.free( handle.id );
        }
    }

    bool Device::waitForFence( uint32_t count, kege::FenceHandle* fences, uint32_t wait_all, uint64_t timeout_nanoseconds)
    {
        if ( count )
        {
            VkFence vk_fences[ count ];
            for ( int i=0; i<count; ++i)
            {
                vk_fences[i] = _fences.get( fences[i].id )->fence;
            }
            VkResult result = vkWaitForFences
            (
                _device,
                count,
                vk_fences,
                wait_all,
                timeout_nanoseconds
            );
            if ( result != VK_SUCCESS )
            {
                KEGE_LOG_ERROR << "error while waiting for fences. in waitForFence()"<<Log::nl;
                return false;
            }
            return true;
        }
        return 0;
    }

    void Device::resetFence( uint32_t count, kege::FenceHandle* fences )
    {
        if ( count )
        {
            VkFence vk_fences[ count ];
            for ( int i=0; i<count; ++i)
            {
                vk_fences[i] = _fences.get( fences[i].id )->fence;
            }
            vkResetFences( _device, count, vk_fences );
        }
    }

    FenceStatus Device::getFenceStatus( kege::FenceHandle fence )
    {
        switch ( vkGetFenceStatus( _device, _fences.get( fence.id )->fence ) )
        {
            case VK_SUCCESS: return FenceStatus::Success;
            case VK_NOT_READY: return FenceStatus::NotReady;
            case VK_ERROR_DEVICE_LOST: return FenceStatus::DeviceLost;
            default: return {};
        };
    }

    VkResult Device::allocateDeviceMemory
    (
        VkMemoryRequirements memory_requirements,
        VkMemoryPropertyFlags memory_properties,
        VkDeviceMemory* memory
    )
    {
        VkPhysicalDeviceMemoryProperties physical_device_memory_properties = _physical_device->getMemoryProperties();
        uint32_t memory_type_index = 0;
        for( ; memory_type_index < physical_device_memory_properties.memoryTypeCount; ++memory_type_index )
        {
            VkMemoryPropertyFlags type = physical_device_memory_properties.memoryTypes[ memory_type_index ].propertyFlags;
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

    void Device::cleanupSwapchains()
    {
        for ( int32_t i = 0; i < _swapchains.count(); ++i )
        {
            if ( _swapchains.get( i ) != nullptr )
            {
                destroySwapchain({ i });
            };
        }
    }

    void Device::cleanupResources()
    {
        for ( int32_t i = 0; i < _buffers.count(); ++i )
        {
            if ( _buffers.get( i ) != nullptr )
            {
                destroyBuffer({ i });
            };
        }
        for ( int32_t i = 0; i < _textures.count(); ++i )
        {
            if ( _textures.get( i ) != nullptr )
            {
                destroyImage({ i });
            };
        }
        for ( int32_t i = 0; i < _samplers.count(); ++i )
        {
            if ( _samplers.get( i ) != nullptr )
            {
                destroySampler({ i });
            };
        }
    }

    void Device::cleanupSyncPrimitives()
    {
        for ( int32_t i = 0; i < _fences.count(); ++i )
        {
            if ( _fences.get( i ) != nullptr )
            {
                destroyFence({ i });
            };
        }
        for ( int32_t i = 0; i < _semaphores.count(); ++i )
        {
            if ( _semaphores.get( i ) != nullptr )
            {
                destroySemaphore({ i });
            };
        }
    }

    void Device::cleanupPipelines()
    {
        for ( int32_t i = 0; i < _graphics_pipelines.count(); ++i )
        {
            if ( _graphics_pipelines.get( i ) != nullptr )
            {
                destroyGraphicsPipeline({ i });
            };
        }
        for ( int32_t i = 0; i < _compute_pipelines.count(); ++i )
        {
            if ( _compute_pipelines.get( i ) != nullptr )
            {
                destroyComputePipeline({ i });
            };
        }
    }

    void Device::cleanupShaders()
    {
        for ( int32_t i = 0; i < _shaders.count(); ++i )
        {
            if ( _shaders.get( i ) != nullptr )
            {
                destroyShader({ i });
            };
        }
    }

    void Device::cleanupCommandPools()
    {
        // Destroy command pools
        for(auto const& [key, val] : _command_pools)
        {
            if (val != VK_NULL_HANDLE)
            {
                vkDestroyCommandPool(_device, val, nullptr);
            }
        }
        _command_pools.clear();
    }

    const Buffer* Device::getBuffer(BufferHandle handle) const
    {
        return _buffers.get( handle.id );
    }

    Buffer* Device::getBuffer(BufferHandle handle)
    {
        return _buffers.get( handle.id );
    }

    const Image* Device::getImage(ImageHandle handle) const
    {
        return _textures.get( handle.id );
    }

    const Sampler* Device::getSampler(SamplerHandle handle) const
    {
        return _samplers.get( handle.id );
    }

    const Shader* Device::getShader(ShaderHandle handle) const
    {
        return _shaders.get( handle.id );
    }

    const GraphicsPipeline* Device::getGraphicsPipeline(PipelineHandle handle) const
    {
        return _graphics_pipelines.get( handle.id );
    }

    const ComputePipeline* Device::getComputePipeline(PipelineHandle handle) const
    {
        return _compute_pipelines.get( handle.id );
    }

    const Fence* Device::getFence(FenceHandle handle) const
    {
        return _fences.get( handle.id );
    }

    const Semaphore* Device::getSemaphore(SemaphoreHandle handle) const
    {
        return _semaphores.get( handle.id );
    }

    void Device::copyBufferToTexture
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


    void Device::debugSetObjectName( uint64_t object_handle, VkObjectType object_type, const char* name)
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

    void Device::destroySurface( VkSurfaceKHR surface, const VkAllocationCallbacks *allocator )
    {
        _instance->destroySurface( surface, allocator );
    }

    VkSurfaceKHR Device::surface()
    {
        return _surface;
    }
    
    VkDevice Device::handle()
    {
        return _device;
    }
}
