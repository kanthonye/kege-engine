//
//  vulkan-swapchain.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/26/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-swapchain.hpp"
#include "vk-utils.hpp"

#include <limits>
#include <algorithm>

namespace kege::vk{

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

    VkExtent2D chooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& preferred_extent )
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            // Window system dictates extent
            return capabilities.currentExtent;
        }
        else
        {
            // We can choose extent within limits
            VkExtent2D actualExtent = preferred_extent;

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

    std::vector< ImageHandle > Swapchain::getColorImages()const
    {
        std::vector< ImageHandle > images( _image_count );
        for (int i=0; i<_image_count; ++i) {
            images[i] = _frames[i].color;
        }
        return images;
    }
    std::vector< ImageHandle > Swapchain::getDepthImages()const
    {
        std::vector< ImageHandle > images( _image_count );
        for (int i=0; i<_image_count; ++i) {
            images[i] = _frames[i].depth;
        }
        return images;
    }
    kege::Format Swapchain::getDepthFormat()const
    {
        return vk::convertVkFormat( _depth_format );
    }
    kege::Format Swapchain::getColorFormat()const
    {
        return vk::convertVkFormat( _surface_format.format );
    }
    uint32_t Swapchain::getImageCount()const
    {
        return _image_count;
    }
    uint32_t Swapchain::getImageIndex()const
    {
        return _image_index;
    }
    Extent2D Swapchain::getExtent()const
    {
        return {_extent.width, _extent.height};
    }

    FrameSync& Swapchain::getFrameSync( uint32_t frame_index )
    {
        return _frame_syncs [ _image_index ];
    }
    
    int32_t Swapchain::acquireNextImage()
    {
        _curr_frame_index = (_curr_frame_index + 1) % _frame_syncs.size();
        VkSemaphore image_available_semaphore = _frame_syncs[ _curr_frame_index ].image_available_semaphore;

        VkResult result = vkAcquireNextImageKHR
        (
            _device->_device,
            _swapchain,
            UINT64_MAX, // Timeout (no timeout)
            image_available_semaphore, // The Semaphore to signal when the image is ready for use
            VK_NULL_HANDLE, // kege::Fence (not using fence here)
            &_image_index
        );

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            kege::Log::debug << "Swapchain out of date during acquire. Needs recreation." <<Log::nl;
            _needs_recreation = true;
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            kege::Log::debug << "Warning: Swapchain suboptimal during acquire. Needs recreation soon." <<Log::nl;
            _needs_recreation = true;
        }
        else if (result != VK_SUCCESS)
        {
            kege::Log::error << "Failed to acquire swap chain image!" << Log::nl;
            return -1;
        }

        if( _needs_recreation )
        {
            if ( recreate() != VK_SUCCESS )
            {
                kege::Log::error << "Failed to recreate Swapchain!" << Log::nl;
                return -1;
            };
            _needs_recreation = false;// Reset flag on successful acquire (if not suboptimal)
        }
        return _image_index;
    }

    vk::FrameData& Swapchain::getFrame( uint32_t curr_frame )
    {
        return _frames[ curr_frame ];
    }

    Viewport Swapchain::getViewport()const
    {
        return _viewport;
    }
    
    Scissor Swapchain::getScissor()const
    {
        return _scissor;
    }


    VkResult Swapchain::create( const SwapchainDesc& desc )
    {
        vk::PhysicalDevice* physical_device = _device->getVkPhysicalDevice();

        std::vector< VkSurfaceFormatKHR > formats = physical_device->getPhysicalDeviceSurfaceFormats( _device->_surface );
        if ( formats.empty() )
        {
            KEGE_LOG_ERROR << "No surface formats available!" <<Log::nl;
            return {};
        }

        std::vector< VkPresentModeKHR > present_modes = physical_device->getPhysicalDeviceSurfacePresentModes( _device->_surface );
        if ( present_modes.empty() )
        {
            KEGE_LOG_ERROR << "No present modes available!" <<Log::nl;
            return {};
        }

        _surface_format = chooseSwapSurfaceFormat( formats, desc.color_format );
        _present_mode = chooseSwapPresentMode( present_modes, desc.present_mode );
        _depth_format = vk::convertFormat( desc.depth_format );
        _image_count = desc.image_count;

        return recreate();
    }

    VkResult Swapchain::recreate()
    {
        _device->waitIdle();

        VkSwapchainKHR new_swapchain;
        VkResult result = createSwapchain( &new_swapchain );

        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return result; // ERROR_SWAPCHAIN_CREATEION_FAILED
        }

        destroy();

        _swapchain = new_swapchain;
        _frames = createFrames( _image_count );
        if ( _frames.empty() )
        {
            destroy();
            return VK_ERROR_INITIALIZATION_FAILED; // ERROR_SWAPCHAIN_FRAMEBUFFER_CREATEION_FAILED
        }

        _frame_syncs.resize( _image_count );
        for (int i=0; i<_frame_syncs.size(); ++i)
        {
            _frame_syncs[i].image_available_semaphore = _device->_manager.createSemaphore();
            if( _frame_syncs[i].image_available_semaphore == VK_NULL_HANDLE )
            {
                destroy();
                return VK_ERROR_INITIALIZATION_FAILED;
            }

            _frame_syncs[i].render_complete_semaphore = _device->_manager.createSemaphore();
            if( _frame_syncs[i].render_complete_semaphore == VK_NULL_HANDLE )
            {
                destroy();
                return VK_ERROR_INITIALIZATION_FAILED;
            }
        }
        return result;
    }

    VkResult Swapchain::createSwapchain( VkSwapchainKHR* swapchain )
    {
        vk::PhysicalDevice* physical_device = _device->getVkPhysicalDevice();

        // 2. Query Surface Support Details
        VkSurfaceCapabilitiesKHR capabilities = physical_device->getPhysicalDeviceSurfaceCapabilities(  _device->_surface );
        if ( capabilities.maxImageCount == 0 )
        {
            KEGE_LOG_ERROR << "Failed to get surface capabilities!" <<Log::nl;
            return {};
        }

        // 3. Choose Settings
        VkImageUsageFlags image_usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        _extent = chooseSwapExtent( capabilities, capabilities.currentExtent );

        _viewport.height = _extent.height;
        _viewport.width = _extent.width;
        _viewport.min_depth = 0.0;
        _viewport.max_depth = 1.0;
        _viewport.x = 0.0;
        _viewport.y = 0.0;

        _scissor.height = _extent.height;
        _scissor.width = _extent.width;
        _scissor.x = 0.0;
        _scissor.y = 0.0;

        // 4. Create Swapchain Info Struct
        VkSwapchainCreateInfoKHR create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface =  _device->_surface;
        create_info.minImageCount = _image_count;
        create_info.imageFormat = _surface_format.format;
        create_info.imageColorSpace = _surface_format.colorSpace;
        create_info.imageExtent.height = _extent.height;
        create_info.imageExtent.width = _extent.width;
        create_info.imageArrayLayers = 1; // Non-stereoscopic
        create_info.imageUsage = image_usage; // Use conversion helper
        create_info.preTransform = capabilities.currentTransform; // Use current transform
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Assume opaque window background
        create_info.presentMode = _present_mode;
        create_info.oldSwapchain = _swapchain;
        create_info.clipped = VK_TRUE; // Allow clipping unseen pixels

        // Handle Queue Family Sharing (if graphics and present are different)
        const QueueFamilyIndices& indices = _device->_queue_family_indices;
        uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value() };

        if (indices.graphics_family != indices.present_family)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queueFamilyIndices;
            KEGE_LOG_INFO << "Swapchain using concurrent sharing mode." <<Log::nl;
            KEGE_LOG_INFO << indices.graphics_family.value() <<"," << indices.present_family.value() <<Log::nl;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0; // Optional
            create_info.pQueueFamilyIndices = nullptr; // Optional
        }

        // 5. Create the Vulkan Swapchai
        return _device->_manager.createSwapchain( &create_info, nullptr, swapchain );
    }

    std::vector< vk::FrameData > Swapchain::createFrames( uint32_t image_count )
    {
        // 6. Get Swapchain Images
        vkGetSwapchainImagesKHR( _device->handle(), _swapchain, &image_count, nullptr ); // Get actual count
        std::vector< VkImage > images( image_count );
        vkGetSwapchainImagesKHR( _device->handle(), _swapchain, &image_count, images.data());

        std::vector< vk::FrameData > frames( image_count );
        // 7. Create Image Views and Abstract Handles
        for (uint32_t i = 0; i < image_count; ++i)
        {
            frames[i].color = { _device->_textures.gen() };

            Image* color_image = _device->_textures.get( frames[i].color.id );
            color_image->image = images[i];
            color_image->format = _surface_format.format;

            color_image->view = _device->_manager.createImageView
            (
                images[i],
                VK_IMAGE_VIEW_TYPE_2D,
                _surface_format.format,
                1, 1,
                vkFormatToVkImageAspect( _surface_format.format )
            );

            if ( color_image->view == VK_NULL_HANDLE )
            {
                kege::Log::error << "Failed to create swapchain image view " << i << "!" <<Log::nl;
                // Cleanup already created views, swapchain, surface
                for (uint32_t j = 0; j < i; ++j)
                {
                    Image* tex = _device->_textures.get( frames[j].color.id );
                    _device->_manager.destroyImageView( tex->view);
                }
                return {};
            }

            // Create corresponding abstract Texture entry
            color_image->desc.type = ImageType::Type2D;
            color_image->desc.width = _extent.width;
            color_image->desc.height = _extent.height;
            color_image->desc.depth = 1;
            color_image->desc.mip_levels = 1;
            color_image->desc.format = convertVkFormat( _surface_format.format ); // Need inverse translation
            color_image->desc.sample_count = SampleCount::Count1;
            color_image->desc.usage = ImageUsage::Color | ImageUsage::Present | ImageUsage::Present; // Store intended usage
            color_image->desc.memory_usage = MemoryUsage::GpuOnly; // Implicitly GPU only
            color_image->desc.name = "swapchain-image-" + std::to_string(i);
            color_image->current_layout = VK_IMAGE_LAYOUT_UNDEFINED; // Swapchain images start as undefined
            color_image->aspect = VK_IMAGE_ASPECT_COLOR_BIT;


            if ( _depth_format != VkFormat::VK_FORMAT_UNDEFINED )
            {
                kege::ImageDesc depth_info = {};
                depth_info.type = ImageType::Type2D;
                depth_info.width = _extent.width;
                depth_info.height = _extent.height;
                depth_info.depth = 1;
                depth_info.mip_levels = 1;
                depth_info.format = convertVkFormat( _depth_format );
                depth_info.sample_count = SampleCount::Count1;
                depth_info.usage = ImageUsage::DepthStencil | ImageUsage::Present | ImageUsage::Present;;
                depth_info.memory_usage = MemoryUsage::GpuOnly;
                depth_info.name = "swapchain-depth-image-" + std::to_string(i);
                frames[i].depth = _device->createImage( depth_info );
            }
        }

        return frames;
    }

    void Swapchain::destroy()
    {
        if ( _device )
        {
            // Wait for device to be idle before destroying swapchain resources
            _device->waitIdle(); // Simplest synchronization

            // Destroy Color Image
            for( FrameData& fbo : _frames )
            {
                Image* img = _device->_textures.get( fbo.color.id );
                if ( img != nullptr )
                {
                    // Don't destroy VkImage, just the view and map entry
                    _device->_manager.destroyImageView( img->view );
                    _device->destroyImage( fbo.color );
                }

                img = _device->_textures.get( fbo.depth.id );
                if ( img != nullptr )
                {
                    // Don't destroy VkImage, just the view and map entry
                    _device->destroyImage( fbo.depth );
                }
            }
            _frames.clear();

            for (int i=0; i<_frame_syncs.size(); ++i)
            {
                _device->_manager.destroySemaphore( _frame_syncs[i].image_available_semaphore );
                _device->_manager.destroySemaphore( _frame_syncs[i].render_complete_semaphore );
            }
            _frame_syncs.clear();

            // Destroy Swapchain
            if ( _swapchain != VK_NULL_HANDLE)
            {
                vkDestroySwapchainKHR( _device->_device, _swapchain, nullptr );
                _swapchain = VK_NULL_HANDLE;
            }
        }
    }

    void Swapchain::setShouldRecreate( bool state )
    {
        _needs_recreation = state;
    }

    bool Swapchain::shouldRecreate()const
    {
        return _needs_recreation;
    }

    const VkSwapchainKHR& Swapchain::getHandle()const
    {
        return _swapchain;
    }

    Swapchain::Swapchain( Device* device )
    :   _device( device )
    ,   _swapchain( VK_NULL_HANDLE )
    ,   _curr_frame_index( -1 )
    ,   _image_count( 0 )
    ,   _image_index( 0 )
    ,   _needs_recreation( false )
    {}

    Swapchain::~ Swapchain()
    {
        destroy();
        _device = nullptr;
    }
}
