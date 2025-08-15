//
//  vulkan-swapchain.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/26/25.
//

#include "vulkan-device.hpp"
#include "vulkan-instance.hpp"
#include "vulkan-swapchain.hpp"
#include "vulkan-resources.hpp"

#include <limits>
#include <algorithm>

namespace kege::vk{

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector< VkSurfaceFormatKHR >& availableFormats, Format preferredFormat);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes, PresentMode preferredMode);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t preferredWidth, uint32_t preferredHeight);

    VkResult Swapchain::create( Device* device, const kege::SwapchainDesc& desc )
    {
        vk::PhysicalDevice* physical_device = device->getVkPhysicalDevice();

        // 1. Create Surface (Platform Specific - Must be implemented!)
        _device = device;
        _surface = device->surface();// static_cast< VkSurfaceKHR >( desc.surface );
        if (_surface == VK_NULL_HANDLE)
        {
            KEGE_LOG_ERROR << "Failed to create Vulkan surface!" <<Log::nl;
            return {};
        }

        // 2. Query Surface Support Details
        VkSurfaceCapabilitiesKHR capabilities = physical_device->getPhysicalDeviceSurfaceCapabilities( _surface );
        if ( capabilities.maxImageCount == 0 )
        {
            KEGE_LOG_ERROR << "Failed to get surface capabilities!" <<Log::nl;
            device->destroySurface( _surface, nullptr ); // Clean up surface
            return {};
        }

        std::vector< VkSurfaceFormatKHR > formats = physical_device->getPhysicalDeviceSurfaceFormats( _surface );
        if ( formats.empty() )
        {
            KEGE_LOG_ERROR << "No surface formats available!" <<Log::nl;
            vkDestroySurfaceKHR( device->_instance->_instance, _surface, nullptr );
            return {};
        }

        std::vector< VkPresentModeKHR > present_modes = physical_device->getPhysicalDeviceSurfacePresentModes( _surface );
        if ( present_modes.empty() )
        {
            KEGE_LOG_ERROR << "No present modes available!" <<Log::nl;
            vkDestroySurfaceKHR( device->_instance->_instance, _surface, nullptr );
            return {};
        }

        // 3. Choose Settings
        VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat( formats, desc.color_format );
        VkPresentModeKHR present_mode = chooseSwapPresentMode( present_modes, desc.present_mode );
        VkExtent2D extent = chooseSwapExtent( capabilities, desc.width, desc.height );

        _image_count = desc.image_count;
        if ( capabilities.maxImageCount > 0 && _image_count > capabilities.maxImageCount )
        {
            _image_count = capabilities.maxImageCount;
            KEGE_LOG_INFO << "Warning: Clamped swapchain image count to max supported: " << _image_count <<Log::nl;
        }
        if ( _image_count < capabilities.minImageCount )
        {
            _image_count = capabilities.minImageCount;
            KEGE_LOG_INFO << "Warning: Increased swapchain image count to min supported: " << _image_count <<Log::nl;
        }

        VkResult result = createSwapchain
        (
            device,
            desc,
            extent,
            surface_format,
            present_mode,
            capabilities,
            _image_count
        );

        if ( result != VK_SUCCESS )
        {
            return result;
        }
        
        _desc = desc; // Store original abstract desc
        _extent = { extent.width, extent.height };
        _depth_format = desc.depth_format;
        _color_format = convertVkFormat( surface_format.format );

        return createSwapchainImages
        (
            device,
            desc,
            surface_format.format,
            _image_count,
            _swapchain
        );
    }

    VkResult Swapchain::createSwapchain
    (
        Device* device,
        const kege::SwapchainDesc& desc,
        VkExtent2D extent,
        VkSurfaceFormatKHR surface_format,
        VkPresentModeKHR present_mode,
        VkSurfaceCapabilitiesKHR capabilities,
        uint32_t image_count
    )
    {
        // 4. Create Swapchain Info Struct
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _surface;
        createInfo.minImageCount = image_count;
        createInfo.imageFormat = surface_format.format;
        createInfo.imageColorSpace = surface_format.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1; // Non-stereoscopic
        createInfo.imageUsage = convertTextureUsage( desc.image_usage ); // Use conversion helper

        // Handle Queue Family Sharing (if graphics and present are different)
        const QueueFamilyIndices& indices = device->_queue_family_indices;
        uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.graphics_family.value() };

        if (indices.graphics_family != indices.present_family)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
            KEGE_LOG_INFO << "Swapchain using concurrent sharing mode." <<Log::nl;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = capabilities.currentTransform; // Use current transform
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Assume opaque window background
        createInfo.presentMode = present_mode;
        createInfo.clipped = VK_TRUE; // Allow clipping unseen pixels

        // Handle recreation
        Swapchain* old_swapchain = nullptr;
        if ( desc.old_swapchain.id >= 0 )
        {
            old_swapchain = device->_swapchains.get( desc.old_swapchain.id );
            if ( old_swapchain != nullptr )
            {
                createInfo.oldSwapchain = old_swapchain->_swapchain;
            }
            else
            {
                KEGE_LOG_WARN << "oldSwapchain handle provided but not found!" <<Log::nl;
            }
        }
        else
        {
            createInfo.oldSwapchain = VK_NULL_HANDLE;
        }

        // 5. Create the Vulkan Swapchai
        VkResult result;
        if ((result = device->createSwapchain( &createInfo, nullptr, &_swapchain ) ) != VK_SUCCESS )
        {
            logVkError( result, "vulkan-swapchain", "createSwapchain()" );
            device->destroySurface( _surface, nullptr ); // Clean up surface
            return result;
        }
        KEGE_LOG_INFO << "Vulkan Swapchain Created." <<Log::nl;

        // Destroy the old swapchain *after* the new one potentially reuses its resources
        if ( old_swapchain )
        {
            KEGE_LOG_WARN << "Destroying old Vulkan swapchain." <<Log::nl;
            // Need to ensure the old one is really not in use (WaitIdle is safest)
            device->waitIdle();
            // Destroy associated views and abstract handles of the OLD swapchain
            for( ImageHandle& handle : old_swapchain->_color_images )
            {
                Image* texture = device->_textures.get( handle.id );
                if ( texture != nullptr )
                {
                    // Don't destroy VkImage, just the view and map entry
                    device->destroyImageView( texture->view, nullptr );
                    device->destroyImage( handle );
                }
            }
            for( ImageHandle& handle : old_swapchain->_depth_images )
            {
                _device->destroyImage( handle );
            }
            // Clear the list
            // Image views are destroyed implicitly by vkDestroySwapchainKHR? Check spec.
            // Safest to destroy explicitly if unsure, but often views are destroyed with swapchain.
            // vkDestroyImageView for oldSwapchain.imageViews... (Assuming they weren't destroyed yet)
            old_swapchain->_color_images.clear();
            old_swapchain->_depth_images.clear();

            device->destroySwapchain( desc.old_swapchain );
            if (old_swapchain->_surface != _surface)
            {
                // If surface changed (unlikely but possible)
                device->destroySurface( old_swapchain->_surface, nullptr );
            }
        }
        return VK_SUCCESS;
    }

    VkResult Swapchain::createSwapchainImages
    (
        Device* device,
        const kege::SwapchainDesc& desc,
        VkFormat format,
        uint32_t image_count,
        VkSwapchainKHR swapchain
    )
    {
        // 6. Get Swapchain Images
        vkGetSwapchainImagesKHR( device->_device, swapchain, &image_count, nullptr ); // Get actual count
        std::vector< VkImage > images( image_count );
        vkGetSwapchainImagesKHR( device->_device, swapchain, &image_count, images.data());

        // 7. Create Image Views and Abstract Handles
        for (uint32_t i = 0; i < image_count; ++i)
        {
            ImageHandle texr = {device->_textures.gen()};
            _color_images.push_back( texr );

            Image* color_image = device->_textures.get( texr.id );
            color_image->image = images[i];
            color_image->format = format;

            // Create Image View
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = format;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if ( device->createImageView( &viewInfo, nullptr, &color_image->view ) != VK_SUCCESS )
            {
                KEGE_LOG_ERROR << "Failed to create swapchain image view " << i << "!" <<Log::nl;
                // Cleanup already created views, swapchain, surface
                for (uint32_t j = 0; j < i; ++j)
                {
                    Image* tex = device->_textures.get( _color_images[j].id );
                    device->destroyImageView( tex->view, nullptr);
                }
                device->destroySwapchain( swapchain, nullptr);
                device->destroySurface( _surface, nullptr );
                return {};
            }

            // Create corresponding abstract Texture entry
            color_image->desc.type = ImageType::Type2D;
            color_image->desc.width = _extent.width;
            color_image->desc.height = _extent.height;
            color_image->desc.depth = 1;
            color_image->desc.mip_levels = 1;
            color_image->desc.format = convertVkFormat( format ); // Need inverse translation
            color_image->desc.sample_count = SampleCount::Count1;
            color_image->desc.usage = desc.image_usage; // Store intended usage
            color_image->desc.memory_usage = MemoryUsage::GpuOnly; // Implicitly GPU only
            color_image->desc.debug_name = "swapchain-image-" + std::to_string(i);
            color_image->current_layout = VK_IMAGE_LAYOUT_UNDEFINED; // Swapchain images start as undefined
        }

        if ( desc.depth_format != kege::Format::undefined )
        {
            for (uint32_t i = 0; i < image_count; ++i)
            {
                kege::ImageDesc depth_info = {};
                depth_info.type = ImageType::Type2D;
                depth_info.width = _extent.width;
                depth_info.height = _extent.height;
                depth_info.depth = 1;
                depth_info.mip_levels = 1;
                depth_info.format = desc.depth_format;
                depth_info.sample_count = SampleCount::Count1;
                depth_info.usage = ImageUsageFlags::DepthStencilAttachment;
                depth_info.memory_usage = MemoryUsage::GpuOnly;
                depth_info.debug_name = "swapchain-depth-image-" + std::to_string(i);
                _depth_images.push_back( _device->createImage( depth_info ) );
            }
        }
        return VK_SUCCESS;
    }

    void Swapchain::destroy()
    {
        // Wait for device to be idle before destroying swapchain resources
        _device->waitIdle(); // Simplest synchronization

        // Destroy Color Image
        for (int i=0; i<_color_images.size(); ++i )
        {
            if( _device->getImage( _color_images[i] ) )
            {
                _device->destroyImage( _color_images[i] );
            }
            
            if( _device->getImage( _depth_images[i] ) )
            {
                _device->destroyImage( _depth_images[i] );
            }
        }
        _color_images.clear();

        // Destroy Depth Image
        for (const auto& handle : _depth_images)
        {
            if( _device->getImage( handle ) )
            {
                _device->destroyImage( handle );
            }
        }
        _depth_images.clear();

        // Destroy Swapchain
        if ( _swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR( _device->_device, _swapchain, nullptr );
        }

        KEGE_LOG_INFO << "Vulkan Swapchain Destroyed (Handle: " << _id << ")" <<Log::nl;
    }

    bool Swapchain::acquireNextImage( SemaphoreHandle signalSemaphore, uint32_t* out_image_index )
    {
        if ( _id == 0 || signalSemaphore.id == 0) return false;

        VkResult result = vkAcquireNextImageKHR
        (
            _device->_device,
            _swapchain,
            UINT64_MAX, // Timeout (no timeout)
            _device->_semaphores.get( signalSemaphore.id )->semaphore, // Semaphore to signal
            VK_NULL_HANDLE, // Fence (not using fence here)
            out_image_index
        );

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            KEGE_LOG_ERROR << "Swapchain out of date during acquire. Needs recreation." <<Log::nl;
            _needs_recreation = true;
            return false; // Cannot render to this image index
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            KEGE_LOG_ERROR << "Warning: Swapchain suboptimal during acquire. Needs recreation soon." <<Log::nl;
            _needs_recreation = true;
            // Still okay to render to this image for now
            return true;
        }
        else if (result != VK_SUCCESS)
        {
            KEGE_LOG_ERROR << "Failed to acquire swap chain image! Error: " << result;
            // This could be a more serious error
            return false;
        }

        // Success
        _needs_recreation = false; // Reset flag on successful acquire (if not suboptimal)
        return true;
    }

    ImageHandle Swapchain::getColorImage( uint32_t image_index )
    {
        if ( _id < 0)
        {
            KEGE_LOG_ERROR << "Error: Invalid Swapchain in getColorImage!" <<Log::nl;
            return {-1};
        }

        if ( image_index < _color_images.size() )
        {
            return _color_images[ image_index ];
        }
        else
        {
            KEGE_LOG_ERROR << "Error: imageIndex out of bounds in getColorImage!" <<Log::nl;
            return {-1};
        }
        return {-1}; // Invalid handle
    }

    ImageHandle Swapchain::getDepthImage( uint32_t image_index )
    {
        if ( _id < 0)
        {
            KEGE_LOG_ERROR << "Error: Invalid Swapchain in getDepthImage!" <<Log::nl;
            return {-1};
        }

        if ( image_index < _depth_images.size() )
        {
            return _depth_images[ image_index ];
        }
        else
        {
            KEGE_LOG_ERROR << "Error: imageIndex out of bounds in getDepthImage!";
            return {-1};
        }
        return {-1}; // Invalid handle
    }

    bool Swapchain::present( SemaphoreHandle wait_semaphore, uint32_t image_index)
    {
        if ( _id == 0 || wait_semaphore.id == 0 )
        {
            return false; // QueueHandle needs proper lookup
        }

        Semaphore* sem = _device->_semaphores.get( wait_semaphore.id );
        if ( sem == nullptr )
        {
            KEGE_LOG_ERROR << "Invalid semaphore in present." <<Log::nl;
            return false;
        }

        /** --- Prepare Present Info --- */
        VkSemaphore wait_semaphores[] = { sem->semaphore };
        VkSwapchainKHR swapchains[] = { _swapchain };

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = wait_semaphores;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr; // Optional

        /** --- Submit Present Request --- */
        VkResult result = vkQueuePresentKHR( _device->_present_queue.queue, &present_info );

        /** --- Handle Results --- */
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            KEGE_LOG_WARN << "Out of date image during present. Needs recreation." <<Log::nl;
            _needs_recreation = true;
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            KEGE_LOG_WARN << "Suboptimal image during present. Needs recreation soon." <<Log::nl;
            _needs_recreation = true;
        }
        else if (result != VK_SUCCESS)
        {
            /** This could be a more serious error (e.g., device lost) */
            KEGE_LOG_ERROR << "Failed to queue present! Error: " << result <<Log::nl;
            /** Consider handling VK_ERROR_DEVICE_LOST */
            return false;
        }
        /** On success, no specific action needed here, but render loop needs to check needsRecreation flag. */
        return false;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats, Format preferred_format )
    {
        VkFormat preferred_vk_format = convertFormat( preferred_format ); // Need conversion helper

        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == preferred_vk_format && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        // Fallback to the first available format if preferred not found
        KEGE_LOG_WARN << "Preferred swapchain format not found, using first available.";
        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_modes, PresentMode preferred_mode)
    {
        VkPresentModeKHR mode;
        switch(preferred_mode)
        {
            case PresentMode::Immediate: mode = VK_PRESENT_MODE_IMMEDIATE_KHR; break;
            case PresentMode::Mailbox: mode = VK_PRESENT_MODE_MAILBOX_KHR; break;
            case PresentMode::FifoRelaxed: mode = VK_PRESENT_MODE_FIFO_RELAXED_KHR; break;
            case PresentMode::Fifo: // Fallthrough intended
            default: mode = VK_PRESENT_MODE_FIFO_KHR; break;
        }

        for (const auto& availableMode : available_modes)
        {
            if (availableMode == mode)
            {
                return availableMode; // Found preferred
            }
        }

        // Fallback - Mailbox if available (good alternative), otherwise FIFO (guaranteed)
         for (const auto& availableMode : available_modes)
         {
            if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                KEGE_LOG_WARN << "Preferred present mode not found, using Mailbox.";
                 return VK_PRESENT_MODE_MAILBOX_KHR;
            }
        }

        KEGE_LOG_WARN << "Preferred present mode not found, using FIFO.";
        return VK_PRESENT_MODE_FIFO_KHR; // Always supported
    }

    VkExtent2D chooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities, uint32_t preferred_width, uint32_t preferred_height )
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            // Window system dictates extent
            return capabilities.currentExtent;
        }
        else
        {
            // We can choose extent within limits
            VkExtent2D actualExtent = { preferred_width, preferred_height };

            actualExtent.width = std::max
            (
                capabilities.minImageExtent.width,
                std::min(capabilities.maxImageExtent.width, actualExtent.width)
            );

            actualExtent.height = std::max
            (
                capabilities.minImageExtent.height,
                std::min(capabilities.maxImageExtent.height, actualExtent.height)
            );

            return actualExtent;
        }
    }

}
