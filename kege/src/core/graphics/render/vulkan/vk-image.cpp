//
//  vk-image.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#include "vk-image.hpp"
#include "vk-device.hpp"
#include "vk-instance.hpp"

namespace kege::vk{

    Image::Image( vk::Device* device, const kege::Image::Desc& desc )
    :   kege::Image( desc )
    ,   _current_layout( VK_IMAGE_LAYOUT_UNDEFINED )
    ,   _device( device )
    {
        _aspect = getImageAspectFlags( desc.format );

        /** ---- Create Image Handle ---- */

        _size = desc.extent.width * desc.extent.height * desc.extent.depth * sizeOfFormat( desc.format );
        VkImageCreateFlags flags = ((desc.type == ImageType::TypeCube || desc.type == ImageType::TypeCubeArray))
        ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
        : 0;

        _vkformat = convertFormat( desc.format );

        _image = device->core().createImage
        (
            convertTextureType( desc.type ),
            _vkformat,
            desc.extent.width,
            desc.extent.height,
            desc.extent.depth,
            desc.array_layers,
            desc.mip_levels,
            convertImageUsage(desc.usage),
            convertSampleCount(desc.samples),
            flags
        );

        if ( _image == VK_NULL_HANDLE )
        {
            kege::Log::error << "Failed to create VkImage handle."<<Log::nl;
            return;
        }
        /** ---- Create Image Memory ---- */

        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements( device->handle(), _image, &memory_requirements );

        device->core().allocateDeviceMemory( memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_memory );
        if( vkBindImageMemory( device->handle(), _image, _memory, 0 ) != VK_SUCCESS )
        {
            kege::Log::error << "Failed to create image memory in createImage."<<Log::nl;
            device->core().destroyImage( _image );
            return;
        }

        /** ---- Create Image View ---- */

        _view = device->core().createImageView
        (
            _image,
            convertTextureViewType( desc.type ),
            _vkformat,
            desc.array_layers,
            desc.mip_levels,
            _aspect
        );
        if ( _view == VK_NULL_HANDLE )
        {
            kege::Log::error << "Failed to create image view in createImage."<<Log::nl;
            device->core().destroyMemory( _memory );
            device->core().destroyImage( _image );
            return;
        }

        // Set Debug Name (requires VK_EXT_debug_utils)
        if ( device->getVkInstance()->isValidationEnabled() && !desc.debug_name.empty() )
        {
            device->core().debugSetObjectName( (uint64_t)_memory, VK_OBJECT_TYPE_DEVICE_MEMORY, desc.debug_name.c_str() );
            device->core().debugSetObjectName( (uint64_t)_view, VK_OBJECT_TYPE_IMAGE_VIEW, desc.debug_name.c_str() );
            device->core().debugSetObjectName( (uint64_t)_image, VK_OBJECT_TYPE_IMAGE, desc.debug_name.c_str() );
        }

        if ( desc.data != nullptr )
        {
            device->copyBufferToImage
            (
                _size, desc.data, _image,
                desc.extent.width, desc.extent.height,
                _aspect,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );
        }
    }
    
    Image::Image()
    :   kege::Image({})
    ,   _image( VK_NULL_HANDLE )
    ,   _view( VK_NULL_HANDLE )
    ,   _memory( VK_NULL_HANDLE )
    ,   _device( nullptr )
    ,   _swapchain_owned( false )
    {}

    Image::~Image()
    {
        if ( _device )
        {
            _device->destroyImage( this );
            _device = nullptr;
        }
    }

    Sampler::Sampler( vk::Device* device, const kege::SamplerDesc& desc )
    :   _device( device )
    {
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
        ? std::min(desc.max_anisotropy, device->getLimits().max_sampler_anisotropy)
        : 1.0f; // Clamp to device limits

        samplerInfo.compareEnable = desc.compare_enable ? VK_TRUE : VK_FALSE;
        samplerInfo.compareOp = convertCompareOp(desc.compare_op);
        samplerInfo.minLod = desc.min_lod;
        samplerInfo.maxLod = desc.max_lod;
        samplerInfo.borderColor = convertBorderColor(desc.border_color);
        samplerInfo.unnormalizedCoordinates = VK_FALSE; // Default

        if (vkCreateSampler(device->handle(), &samplerInfo, nullptr, &_sampler) != VK_SUCCESS)
        {
            kege::Log::error << "Failed to create sampler!";
            return;
        }

        // Set Debug Name (requires VK_EXT_debug_utils)
        if ( device->getVkInstance()->isValidationEnabled() && desc.name )
        {
            device->core().debugSetObjectName( (uint64_t)_sampler, VK_OBJECT_TYPE_SAMPLER, desc.name );
        }
        _device = device;
    }
    Sampler::~Sampler()
    {
        if ( _device )
        {
            _device->destroySampler( this );
            _device = nullptr;
        }
    }
}
