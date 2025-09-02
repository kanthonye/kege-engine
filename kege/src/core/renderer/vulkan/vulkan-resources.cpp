//
//  vulkan_resources.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#include "vulkan-resources.hpp"

namespace kege::vk{

    VkBufferUsageFlags convertBufferUsageFlags( BufferUsage flags )
    {
        VkBufferUsageFlags usage = {0};
        if (( flags & BufferUsage::CopySrc ) == BufferUsage::CopySrc )
        {
            usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }
        if (( flags & BufferUsage::CopyDst ) == BufferUsage::CopyDst )
        {
            usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
        if (( flags & BufferUsage::VertexBuffer ) == BufferUsage::VertexBuffer )
        {
            usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        else if (( flags & BufferUsage::IndexBuffer ) == BufferUsage::IndexBuffer )
        {
            usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        else if (( flags & BufferUsage::UniformBuffer ) == BufferUsage::UniformBuffer )
        {
            usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        else if (( flags & BufferUsage::StorageBuffer ) == BufferUsage::StorageBuffer )
        {
            usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        else if (( flags & BufferUsage::IndirectBuffer ) == BufferUsage::IndirectBuffer )
        {
            usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }
        else if (( flags & BufferUsage::UniformTexelBuffer ) == BufferUsage::UniformTexelBuffer )
        {
            usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
        }
        else if (( flags & BufferUsage::StorageTexelBuffer ) == BufferUsage::StorageTexelBuffer )
        {
            usage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
        }
        return usage;
    }
    
    VkMemoryPropertyFlags convertMemoryPropertyFlags( MemoryUsage memory_usage )
    {
        VkMemoryPropertyFlags memory_properties = 0;

        switch ( memory_usage )
        {
            case MemoryUsage::GpuOnly:
            {
                memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                break;
            }

            case MemoryUsage::CpuToGpu:
            {
                memory_properties = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                break;
            }

            case MemoryUsage::GpuToCpu:
            {
                memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | // MUST be mappable
                                    VK_MEMORY_PROPERTY_HOST_CACHED_BIT;  // Request cached memory for faster CPU reads
                break;
            }

            case MemoryUsage::CpuOnly:
                memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                break;

            default:break;
        }
        return memory_properties;
    }


    PhysicalDeviceType convertPhysicalDeviceType( VkPhysicalDeviceType type )
    {
        switch ( type )
        {
            default:
            case VK_PHYSICAL_DEVICE_TYPE_OTHER: return PhysicalDeviceType::Other;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return PhysicalDeviceType::IntegratedGpu;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return PhysicalDeviceType::DiscreteGpu;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return PhysicalDeviceType::VirtualGpu;
            case VK_PHYSICAL_DEVICE_TYPE_CPU: return PhysicalDeviceType::DeviceCpu;
        }
    }

    VkAttachmentLoadOp convertAttachmentLoadOp( AttachmentLoadOp op )
    {
        switch ( op )
        {
            case AttachmentLoadOp::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
            case AttachmentLoadOp::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case AttachmentLoadOp::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            //case LoadOp::None: return VK_ATTACHMENT_LOAD_OP_NONE_EXT;
        }
    }

    VkAttachmentStoreOp convertAttachmentStoreOp( AttachmentStoreOp op )
    {
        switch ( op )
        {
            case AttachmentStoreOp::None: return VK_ATTACHMENT_STORE_OP_NONE;
            case AttachmentStoreOp::Store: return VK_ATTACHMENT_STORE_OP_STORE;
            case AttachmentStoreOp::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
            //case LoadOp::None: return VK_ATTACHMENT_LOAD_OP_NONE_EXT;
        }
    }

    VkImageAspectFlags convertImageAspect(ImageAspectFlag aspect)
    {
        VkImageAspectFlags vk_aspect = 0;
        if ( (aspect & ImageAspectFlag::Color) != ImageAspectFlag::None ) {
            vk_aspect |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if ( (aspect & ImageAspectFlag::Depth) != ImageAspectFlag::None ) {
            vk_aspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if ( (aspect & ImageAspectFlag::Stencil) != ImageAspectFlag::None ) {
            vk_aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        return vk_aspect;
    }


    VkFormat convertFormat( Format format )
    {
        switch (format) {
            case Format::undefined:         return VK_FORMAT_UNDEFINED;

            // 8-bit unsigned
            case Format::r_u8:             return VK_FORMAT_R8_UINT;
            case Format::rg_u8:            return VK_FORMAT_R8G8_UINT;
            case Format::rgb_u8:           return VK_FORMAT_R8G8B8_UINT;
            case Format::rgba_u8:          return VK_FORMAT_R8G8B8A8_UINT;

            // 8-bit signed
            case Format::r_s8:             return VK_FORMAT_R8_SINT;
            case Format::rg_s8:            return VK_FORMAT_R8G8_SINT;
            case Format::rgb_s8:           return VK_FORMAT_R8G8B8_SINT;
            case Format::rgba_s8:          return VK_FORMAT_R8G8B8A8_SINT;

            // 8-bit unsigned normalized
            case Format::r_u8_norm:        return VK_FORMAT_R8_UNORM;
            case Format::rg_u8_norm:       return VK_FORMAT_R8G8_UNORM;
            case Format::rgb_u8_norm:      return VK_FORMAT_R8G8B8_UNORM;
            case Format::rgba_u8_norm:     return VK_FORMAT_R8G8B8A8_UNORM;

            // 8-bit signed normalized
            case Format::r_s8_norm:        return VK_FORMAT_R8_SNORM;
            case Format::rg_s8_norm:       return VK_FORMAT_R8G8_SNORM;
            case Format::rgb_s8_norm:      return VK_FORMAT_R8G8B8_SNORM;
            case Format::rgba_s8_norm:     return VK_FORMAT_R8G8B8A8_SNORM;

            // 8-bit sRGB
            case Format::r_8_srgb:         return VK_FORMAT_R8_SRGB;
            case Format::rg_8_srgb:        return VK_FORMAT_R8G8_SRGB;
            case Format::rgb_8_srgb:       return VK_FORMAT_R8G8B8_SRGB;
            case Format::rgba_8_srgb:      return VK_FORMAT_R8G8B8A8_SRGB;

            // BGR formats
            case Format::bgr_8_srbg:       return VK_FORMAT_B8G8R8_SRGB;
            case Format::bgra_8_srbg:      return VK_FORMAT_B8G8R8A8_SRGB;
            case Format::bgr_u8:           return VK_FORMAT_B8G8R8_UINT;
            case Format::bgr_s8:           return VK_FORMAT_B8G8R8_SINT;
            case Format::bgr_s8_norm:      return VK_FORMAT_B8G8R8_SNORM;
            case Format::bgr_u8_norm:      return VK_FORMAT_B8G8R8_UNORM;
            case Format::bgra_u8:          return VK_FORMAT_B8G8R8A8_UINT;
            case Format::bgra_s8:          return VK_FORMAT_B8G8R8A8_SINT;
            case Format::bgra_s8_norm:     return VK_FORMAT_B8G8R8A8_SNORM;
            case Format::bgra_u8_norm:     return VK_FORMAT_B8G8R8A8_UNORM;

            // 16-bit unsigned
            case Format::r_u16:            return VK_FORMAT_R16_UINT;
            case Format::rg_u16:           return VK_FORMAT_R16G16_UINT;
            case Format::rgb_u16:          return VK_FORMAT_R16G16B16_UINT;
            case Format::rgba_u16:         return VK_FORMAT_R16G16B16A16_UINT;

            // 16-bit signed
            case Format::r_s16:            return VK_FORMAT_R16_SINT;
            case Format::rg_s16:           return VK_FORMAT_R16G16_SINT;
            case Format::rgb_s16:          return VK_FORMAT_R16G16B16_SINT;
            case Format::rgba_s16:         return VK_FORMAT_R16G16B16A16_SINT;

            // 16-bit unsigned normalized
            case Format::r_u16_norm:       return VK_FORMAT_R16_UNORM;
            case Format::rg_u16_norm:      return VK_FORMAT_R16G16_UNORM;
            case Format::rgb_u16_norm:     return VK_FORMAT_R16G16B16_UNORM;
            case Format::rgba_u16_norm:    return VK_FORMAT_R16G16B16A16_UNORM;

            // 16-bit signed normalized
            case Format::r_s16_norm:       return VK_FORMAT_R16_SNORM;
            case Format::rg_s16_norm:      return VK_FORMAT_R16G16_SNORM;
            case Format::rgb_s16_norm:     return VK_FORMAT_R16G16B16_SNORM;
            case Format::rgba_s16_norm:    return VK_FORMAT_R16G16B16A16_SNORM;

            // 16-bit signed float
            case Format::r_f16:            return VK_FORMAT_R16_SFLOAT;
            case Format::rg_f16:           return VK_FORMAT_R16G16_SFLOAT;
            case Format::rgb_f16:          return VK_FORMAT_R16G16B16_SFLOAT;
            case Format::rgba_f16:         return VK_FORMAT_R16G16B16A16_SFLOAT;

            // 32-bit unsigned
            case Format::r_u32:            return VK_FORMAT_R32_UINT;
            case Format::rg_u32:           return VK_FORMAT_R32G32_UINT;
            case Format::rgb_u32:          return VK_FORMAT_R32G32B32_UINT;
            case Format::rgba_u32:         return VK_FORMAT_R32G32B32A32_UINT;

            // 32-bit signed
            case Format::r_s32:            return VK_FORMAT_R32_SINT;
            case Format::rg_s32:           return VK_FORMAT_R32G32_SINT;
            case Format::rgb_s32:          return VK_FORMAT_R32G32B32_SINT;
            case Format::rgba_s32:         return VK_FORMAT_R32G32B32A32_SINT;

            // 32-bit float
            case Format::r_f32:            return VK_FORMAT_R32_SFLOAT;
            case Format::rg_f32:           return VK_FORMAT_R32G32_SFLOAT;
            case Format::rgb_f32:          return VK_FORMAT_R32G32B32_SFLOAT;
            case Format::rgba_f32:         return VK_FORMAT_R32G32B32A32_SFLOAT;

            // 64-bit unsigned
            case Format::r_u64:            return VK_FORMAT_R64_UINT;
            case Format::rg_u64:           return VK_FORMAT_R64G64_UINT;
            case Format::rgb_u64:          return VK_FORMAT_R64G64B64_UINT;
            case Format::rgba_u64:         return VK_FORMAT_R64G64B64A64_UINT;

            // 64-bit signed
            case Format::r_s64:            return VK_FORMAT_R64_SINT;
            case Format::rg_s64:           return VK_FORMAT_R64G64_SINT;
            case Format::rgb_s64:          return VK_FORMAT_R64G64B64_SINT;
            case Format::rgba_s64:         return VK_FORMAT_R64G64B64A64_SINT;

            // 64-bit float
            case Format::r_f64:            return VK_FORMAT_R64_SFLOAT;
            case Format::rg_f64:           return VK_FORMAT_R64G64_SFLOAT;
            case Format::rgb_f64:          return VK_FORMAT_R64G64B64_SFLOAT;
            case Format::rgba_f64:         return VK_FORMAT_R64G64B64A64_SFLOAT;

            // Depth formats
            case Format::depth_16:         return VK_FORMAT_D16_UNORM;
            case Format::depth_32:         return VK_FORMAT_D32_SFLOAT;

            case Format::depth_16_stencil_8: return VK_FORMAT_D16_UNORM_S8_UINT;
            case Format::depth_24_stencil_8: return VK_FORMAT_D24_UNORM_S8_UINT;
            case Format::depth_32_stencil_8: return VK_FORMAT_D32_SFLOAT_S8_UINT;

            // stencil formats
            case Format::stencil_u8:       return VK_FORMAT_S8_UINT;

            default:
                // Fallback for unknown formats
                return VK_FORMAT_UNDEFINED;
        }
    }

    Format convertVkFormat( VkFormat format )
    {
        switch (format) {
            case VK_FORMAT_UNDEFINED:        return Format::undefined;

            // 8-bit unsigned
            case VK_FORMAT_R8_UINT:           return Format::r_u8;
            case VK_FORMAT_R8G8_UINT:         return Format::rg_u8;
            case VK_FORMAT_R8G8B8_UINT:       return Format::rgb_u8;
            case VK_FORMAT_R8G8B8A8_UINT:     return Format::rgba_u8;

            // 8-bit signed
            case VK_FORMAT_R8_SINT:           return Format::r_s8;
            case VK_FORMAT_R8G8_SINT:         return Format::rg_s8;
            case VK_FORMAT_R8G8B8_SINT:       return Format::rgb_s8;
            case VK_FORMAT_R8G8B8A8_SINT:     return Format::rgba_s8;

            // 8-bit unsigned normalized
            case VK_FORMAT_R8_UNORM:          return Format::r_u8_norm;
            case VK_FORMAT_R8G8_UNORM:        return Format::rg_u8_norm;
            case VK_FORMAT_R8G8B8_UNORM:      return Format::rgb_u8_norm;
            case VK_FORMAT_R8G8B8A8_UNORM:    return Format::rgba_u8_norm;

            // 8-bit signed normalized
            case VK_FORMAT_R8_SNORM:          return Format::r_s8_norm;
            case VK_FORMAT_R8G8_SNORM:        return Format::rg_s8_norm;
            case VK_FORMAT_R8G8B8_SNORM:      return Format::rgb_s8_norm;
            case VK_FORMAT_R8G8B8A8_SNORM:    return Format::rgba_s8_norm;

            // 8-bit sRGB
            case VK_FORMAT_R8_SRGB:           return Format::r_8_srgb;
            case VK_FORMAT_R8G8_SRGB:         return Format::rg_8_srgb;
            case VK_FORMAT_R8G8B8_SRGB:       return Format::rgb_8_srgb;
            case VK_FORMAT_R8G8B8A8_SRGB:     return Format::rgba_8_srgb;

            // BGR formats
            case VK_FORMAT_B8G8R8_SRGB:       return Format::bgr_8_srbg;
            case VK_FORMAT_B8G8R8A8_SRGB:     return Format::bgra_8_srbg;
            case VK_FORMAT_B8G8R8_UINT:       return Format::bgr_u8;
            case VK_FORMAT_B8G8R8_SINT:       return Format::bgr_s8;
            case VK_FORMAT_B8G8R8_SNORM:      return Format::bgr_s8_norm;
            case VK_FORMAT_B8G8R8_UNORM:      return Format::bgr_u8_norm;
            case VK_FORMAT_B8G8R8A8_UINT:     return Format::bgra_u8;
            case VK_FORMAT_B8G8R8A8_SINT:     return Format::bgra_s8;
            case VK_FORMAT_B8G8R8A8_SNORM:    return Format::bgra_s8_norm;
            case VK_FORMAT_B8G8R8A8_UNORM:    return Format::bgra_u8_norm;

            // 16-bit unsigned
            case VK_FORMAT_R16_UINT:          return Format::r_u16;
            case VK_FORMAT_R16G16_UINT:       return Format::rg_u16;
            case VK_FORMAT_R16G16B16_UINT:    return Format::rgb_u16;
            case VK_FORMAT_R16G16B16A16_UINT: return Format::rgba_u16;

            // 16-bit signed
            case VK_FORMAT_R16_SINT:          return Format::r_s16;
            case VK_FORMAT_R16G16_SINT:       return Format::rg_s16;
            case VK_FORMAT_R16G16B16_SINT:    return Format::rgb_s16;
            case VK_FORMAT_R16G16B16A16_SINT: return Format::rgba_s16;

            // 16-bit unsigned normalized
            case VK_FORMAT_R16_UNORM:         return Format::r_u16_norm;
            case VK_FORMAT_R16G16_UNORM:      return Format::rg_u16_norm;
            case VK_FORMAT_R16G16B16_UNORM:   return Format::rgb_u16_norm;
            case VK_FORMAT_R16G16B16A16_UNORM:return Format::rgba_u16_norm;

            // 16-bit signed normalized
            case VK_FORMAT_R16_SNORM:         return Format::r_s16_norm;
            case VK_FORMAT_R16G16_SNORM:      return Format::rg_s16_norm;
            case VK_FORMAT_R16G16B16_SNORM:   return Format::rgb_s16_norm;
            case VK_FORMAT_R16G16B16A16_SNORM:return Format::rgba_s16_norm;

            // 32-bit unsigned
            case VK_FORMAT_R32_UINT:          return Format::r_u32;
            case VK_FORMAT_R32G32_UINT:       return Format::rg_u32;
            case VK_FORMAT_R32G32B32_UINT:    return Format::rgb_u32;
            case VK_FORMAT_R32G32B32A32_UINT: return Format::rgba_u32;

            // 32-bit signed
            case VK_FORMAT_R32_SINT:          return Format::r_s32;
            case VK_FORMAT_R32G32_SINT:       return Format::rg_s32;
            case VK_FORMAT_R32G32B32_SINT:    return Format::rgb_s32;
            case VK_FORMAT_R32G32B32A32_SINT: return Format::rgba_s32;

            // 32-bit float
            case VK_FORMAT_R32_SFLOAT:        return Format::r_f32;
            case VK_FORMAT_R32G32_SFLOAT:     return Format::rg_f32;
            case VK_FORMAT_R32G32B32_SFLOAT:  return Format::rgb_f32;
            case VK_FORMAT_R32G32B32A32_SFLOAT:return Format::rgba_f32;

            // 64-bit unsigned
            case VK_FORMAT_R64_UINT:          return Format::r_u64;
            case VK_FORMAT_R64G64_UINT:       return Format::rg_u64;
            case VK_FORMAT_R64G64B64_UINT:    return Format::rgb_u64;
            case VK_FORMAT_R64G64B64A64_UINT: return Format::rgba_u64;

            // 64-bit signed
            case VK_FORMAT_R64_SINT:          return Format::r_s64;
            case VK_FORMAT_R64G64_SINT:       return Format::rg_s64;
            case VK_FORMAT_R64G64B64_SINT:    return Format::rgb_s64;
            case VK_FORMAT_R64G64B64A64_SINT: return Format::rgba_s64;

            // 64-bit float
            case VK_FORMAT_R64_SFLOAT:        return Format::r_f64;
            case VK_FORMAT_R64G64_SFLOAT:     return Format::rg_f64;
            case VK_FORMAT_R64G64B64_SFLOAT:  return Format::rgb_f64;
            case VK_FORMAT_R64G64B64A64_SFLOAT:return Format::rgba_f64;

            // Depth formats
            case VK_FORMAT_D16_UNORM:         return Format::depth_16;
            case VK_FORMAT_D32_SFLOAT:        return Format::depth_32;

            // Stencil formats
            case VK_FORMAT_S8_UINT:           return Format::stencil_u8;

            default:
                // Fallback for unknown formats
                return Format::undefined;
        }
    }

    VkImageAspectFlags getImageAspectFlags( Format format )
    {
        switch (format)
        {
            // --- Depth+Stencil Formats ---
            case Format::depth_16_stencil_8:
            case Format::depth_24_stencil_8:
            case Format::depth_32_stencil_8: // Combined depth and stencil
                return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

            // --- Depth-Only Formats ---
            case Format::depth_16:
            case Format::depth_32:
                return VK_IMAGE_ASPECT_DEPTH_BIT;

            // --- Stencil-Only Format ---
            case Format::stencil_u8:
                return VK_IMAGE_ASPECT_STENCIL_BIT;

            // --- Color Formats ---

            // 8-bit unsigned
            case Format::r_u8:
            case Format::rg_u8:
            case Format::rgb_u8:
            case Format::rgba_u8:

            // 8-bit signed
            case Format::r_s8:
            case Format::rg_s8:
            case Format::rgb_s8:
            case Format::rgba_s8:

            // 8-bit unsigned normalized
            case Format::r_u8_norm:
            case Format::rg_u8_norm:
            case Format::rgb_u8_norm:
            case Format::rgba_u8_norm:

            // 8-bit signed normalized
            case Format::r_s8_norm:
            case Format::rg_s8_norm:
            case Format::rgb_s8_norm:
            case Format::rgba_s8_norm:

            // 8-bit sRGB
            case Format::r_8_srgb:
            case Format::rg_8_srgb:
            case Format::rgb_8_srgb:
            case Format::rgba_8_srgb:

            // BGR formats
            case Format::bgr_8_srbg:
            case Format::bgra_8_srbg:
            case Format::bgr_u8:
            case Format::bgr_s8:
            case Format::bgr_s8_norm:
            case Format::bgr_u8_norm:
            case Format::bgra_u8:
            case Format::bgra_s8:
            case Format::bgra_s8_norm:
            case Format::bgra_u8_norm:

            // 16-bit unsigned
            case Format::r_u16:
            case Format::rg_u16:
            case Format::rgb_u16:
            case Format::rgba_u16:

            // 16-bit signed
            case Format::r_s16:
            case Format::rg_s16:
            case Format::rgb_s16:
            case Format::rgba_s16:

            // 16-bit unsigned normalized
            case Format::r_u16_norm:
            case Format::rg_u16_norm:
            case Format::rgb_u16_norm:
            case Format::rgba_u16_norm:

            // 16-bit signed normalized
            case Format::r_s16_norm:
            case Format::rg_s16_norm:
            case Format::rgb_s16_norm:
            case Format::rgba_s16_norm:

            // 32-bit unsigned
            case Format::r_u32:
            case Format::rg_u32:
            case Format::rgb_u32:
            case Format::rgba_u32:

            // 32-bit signed
            case Format::r_s32:
            case Format::rg_s32:
            case Format::rgb_s32:
            case Format::rgba_s32:

            // 32-bit float
            case Format::r_f32:
            case Format::rg_f32:
            case Format::rgb_f32:
            case Format::rgba_f32:

            // 64-bit unsigned
            case Format::r_u64:
            case Format::rg_u64:
            case Format::rgb_u64:
            case Format::rgba_u64:

            // 64-bit signed
            case Format::r_s64:
            case Format::rg_s64:
            case Format::rgb_s64:
            case Format::rgba_s64:

            // 64-bit float
            case Format::r_f64:
            case Format::rg_f64:
            case Format::rgb_f64:
            case Format::rgba_f64:
                return VK_IMAGE_ASPECT_COLOR_BIT;

                // --- Undefined Format ---
            case Format::undefined:
                Log::warning << "getImageAspectFlags called with Undefined format."  <<Log::nl;
                return VK_IMAGE_ASPECT_NONE; // Or maybe COLOR_BIT? None is safer.

            default:
                // This case ideally shouldn't be hit if all formats are listed,
                // but serves as a fallback.
                Log::warning << "Unhandled Format in getImageAspectFlags. Assuming COLOR_BIT."  <<Log::nl;
                return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }


    VkImageType convertTextureType(ImageType type)
    {
        switch (type)
        {
            case ImageType::Type1D:
                return VK_IMAGE_TYPE_1D;
            case ImageType::Type1DArray:
                return VK_IMAGE_TYPE_1D;
            case ImageType::Type2D:
                return VK_IMAGE_TYPE_2D;
            case ImageType::Type2DArray:
                return VK_IMAGE_TYPE_2D;
            case ImageType::TypeCube:
                return VK_IMAGE_TYPE_2D;
            case ImageType::TypeCubeArray:
                return VK_IMAGE_TYPE_2D;
            case ImageType::Type3D:
                return VK_IMAGE_TYPE_3D;
            default:
                // Default to 2D for unknown types
                //assert(false && "Unknown texture type");
                return VK_IMAGE_TYPE_2D;
        }
    }

    VkVertexInputRate convertVertexInputRate( VertexInputRate input_rate )
    {
        switch ( input_rate )
        {
            case kege::VertexInputRate::Vertex: return VK_VERTEX_INPUT_RATE_VERTEX;
            case kege::VertexInputRate::Instance: return VK_VERTEX_INPUT_RATE_INSTANCE;
            default: return VK_VERTEX_INPUT_RATE_MAX_ENUM;
        }
    }

    VkCullModeFlags convertCullMode( CullMode mode )
    {
        switch ( mode )
        {
            case kege::CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
            case kege::CullMode::Back: return VK_CULL_MODE_BACK_BIT;
            case kege::CullMode::FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;
            case kege::CullMode::None: return VK_CULL_MODE_NONE;
            default: KEGE_LOG_ERROR << "Unsupported CullMode in convertCullMode()" <<Log::nl;
        }
    }

    VkFrontFace convertFrontFace( FrontFace face )
    {
        switch ( face )
        {
            case kege::FrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
            case kege::FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
            default: KEGE_LOG_ERROR << "Unsupported FrontFace in convertFrontFace()" <<Log::nl;
        }
    }
    
    VkPolygonMode convertPolygonMode( PolygonMode mode )
    {
        switch (mode)
        {
            default: KEGE_LOG_ERROR << "Unsupported PolygonMode in convertPolygonMode()" <<Log::nl;

            case PolygonMode::Fill: return VK_POLYGON_MODE_FILL;
            case PolygonMode::Point: return VK_POLYGON_MODE_POINT;
            case PolygonMode::Line: return VK_POLYGON_MODE_LINE;
        }
    }
    
    VkPrimitiveTopology convertPrimitiveTopology( kege::PrimitiveTopology topology )
    {
        switch (topology)
        {
            case kege::PrimitiveTopology::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case kege::PrimitiveTopology::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case kege::PrimitiveTopology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case kege::PrimitiveTopology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case kege::PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            //case kege::PrimitiveTopology::: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            case kege::PrimitiveTopology::LineListAdjacency: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
            case kege::PrimitiveTopology::LineStripAdjacency: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
            case kege::PrimitiveTopology::TriangleListAdjacency: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
            case kege::PrimitiveTopology::TriangleStripAdjacency: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
            case kege::PrimitiveTopology::PatchList: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
            default: return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
        }
    }

    VkImageViewType convertTextureViewType(ImageType type)
    {
        switch ( type )
        {
            case ImageType::Type1D:
                return VK_IMAGE_VIEW_TYPE_1D;

            case ImageType::Type1DArray:
                return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

            case ImageType::Type2D:
                return VK_IMAGE_VIEW_TYPE_2D;

            case ImageType::Type2DArray:
                return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

            case ImageType::Type3D:
                return VK_IMAGE_VIEW_TYPE_3D;

            case ImageType::TypeCube:
                return VK_IMAGE_VIEW_TYPE_CUBE;

            case ImageType::TypeCubeArray:
                return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

            default: KEGE_LOG_ERROR << "UNKNOW ImageType in convertTextureViewType()" <<Log::nl;
                break;
        }
    }

    VkImageUsageFlags convertTextureUsage(ImageUsageFlags usage)
    {
        VkImageUsageFlags vk_usage = 0; // Start with no flags

        // --- Direct Mappings ---

        if ((usage & ImageUsageFlags::CopySrc) != ImageUsageFlags::None) {
            vk_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if ((usage & ImageUsageFlags::CopyDst) != ImageUsageFlags::None) {
            vk_usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }
        if ((usage & ImageUsageFlags::ShaderResource) != ImageUsageFlags::None) {
            vk_usage |= VK_IMAGE_USAGE_SAMPLED_BIT; // Allows binding as sampled image (texture())
        }
        if ((usage & ImageUsageFlags::Storage) != ImageUsageFlags::None) {
            vk_usage |= VK_IMAGE_USAGE_STORAGE_BIT; // Allows binding as storage image (imageLoad/Store)
        }
        if ((usage & ImageUsageFlags::ColorAttachment) != ImageUsageFlags::None) {
            vk_usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if ((usage & ImageUsageFlags::DepthStencilAttachment) != ImageUsageFlags::None) {
            vk_usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        if ((usage & ImageUsageFlags::InputAttachment) != ImageUsageFlags::None) {
            // Primarily for Vulkan subpasses (less common with dynamic rendering focus)
            vk_usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        }

        // --- Implied Mappings / Notes ---

        // ImageUsageFlags::TransientAttachment:
        // This flag is primarily a *hint* for the memory allocator (like VMA).
        // It often implies ColorAttachment or DepthStencilAttachment usage as well,
        // which should be explicitly set in the abstract flags.
        // It doesn't usually translate to a direct VkImageUsageFlagBits itself,
        // but VMA might use it along with other usage flags to select lazy allocation.
        // So, ensure that if you use TransientAttachment, you also set ColorAttachment or DepthStencilAttachment.
        if ((usage & ImageUsageFlags::TransientAttachment) != ImageUsageFlags::None)
        {
            // No direct Vulkan usage flag, but good practice checks:
            if (((usage & ImageUsageFlags::ColorAttachment) == ImageUsageFlags::None) &&
                ((usage & ImageUsageFlags::DepthStencilAttachment) == ImageUsageFlags::None))
            {
                // Optional: Log a warning - Transient usually implies render target usage
                // std::cerr << "Warning: TransientAttachment flag set without ColorAttachment or DepthStencilAttachment." << std::endl;
            }
        }

        // ImageUsageFlags::ResolveSrc / ImageUsageFlags::ResolveDst:
        // These don't have their own dedicated VkImageUsageFlagBits.
        // The ability to be a resolve source/destination is implied by other flags:
        // - Resolve Source: Typically needs VK_IMAGE_USAGE_TRANSFER_SRC_BIT (for vkCmdResolveImage)
        //                   or potentially VK_IMAGE_USAGE_SAMPLED_BIT if resolving via shader.
        //                   Ensure the corresponding flag is set if ResolveSrc is used.
        // - Resolve Destination: Typically needs VK_IMAGE_USAGE_TRANSFER_DST_BIT (for vkCmdResolveImage)
        //                       or VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT / DEPTH_STENCIL_ATTACHMENT_BIT
        //                       if resolving via shader write.
        //                       Ensure the corresponding flag is set if ResolveDst is used.
        // Your Render Graph logic for resolving should ensure the concrete flags are present.

        // ImageUsageFlags::Present:
        // As discussed, this flag does NOT translate to a VkImageUsageFlagBits here.
        // The necessary usage flags (like ColorAttachment, TransferDst) should be set
        // based on how the application intends to write to the swapchain image *before*
        // presentation. These flags are then passed during vkCreateSwapchainKHR.
        if ((usage & ImageUsageFlags::Present) != ImageUsageFlags::None) {
            // No direct flag added. Ensure other flags like ColorAttachment/TransferDst are set.
        }


        // Ensure *some* usage is specified if not explicitly 'None'
        // (Though creating a texture with no usage is valid but useless)
        // if (usage != ImageUsageFlags::None && vkUsage == 0) {
        //     // Optional: Log warning/error for unusual usage combination
        // }

        return vk_usage;
    }

    VkBufferUsageFlags convertBufferUsage(BufferUsage usage)
    {
        VkBufferUsageFlags vk_usage = 0; // Start with no flags

        if ((usage & BufferUsage::CopySrc) != BufferUsage::None) {
            vk_usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }
        if ((usage & BufferUsage::CopyDst) != BufferUsage::None) {
            vk_usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
        if ((usage & BufferUsage::VertexBuffer) != BufferUsage::None) {
            vk_usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        if ((usage & BufferUsage::IndexBuffer) != BufferUsage::None) {
            vk_usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        if ((usage & BufferUsage::UniformBuffer) != BufferUsage::None) {
            vk_usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        if ((usage & BufferUsage::StorageBuffer) != BufferUsage::None) {
            // Covers both read and write access from the buffer's perspective
            // Access control (read/write) is often managed by descriptor type and shader capabilities
            vk_usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        if ((usage & BufferUsage::IndirectBuffer) != BufferUsage::None) {
            vk_usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }
        if ((usage & BufferUsage::ShaderDeviceAddress) != BufferUsage::None) {
            // Note: Requires enabling the bufferDeviceAddress feature on the VkDevice
            // and potentially the VK_KHR_buffer_device_address extension if not using Vulkan 1.2+
            vk_usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT; // _KHR or core version
        }
        // Texel Buffer Usage (if you add corresponding abstract flags)
        // if ((usage & BufferUsage::UniformTexelBuffer) != BufferUsage::None) {
        //     vkUsage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
        // }
        // if ((usage & BufferUsage::StorageTexelBuffer) != BufferUsage::None) {
        //     vkUsage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
        // }

        return vk_usage;
    }

//    VmaMemoryUsage convertMemoryUsage(MemoryUsage usage)
//    {
//        switch (usage)
//        {
//            case MemoryUsage::GpuOnly:
//                // Prefer device-local memory. VMA will handle fallback if necessary.
//                return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; // Changed from GPU_ONLY for flexibility
//
//            case MemoryUsage::CpuToGpu:
//                // Prefer host-visible, coherent (if possible), suitable for uploading.
//                // VMA_MEMORY_USAGE_CPU_TO_GPU is deprecated favouring AUTO.
//                // VMA_MEMORY_USAGE_AUTO with HOST_ACCESS flags often achieves the right thing.
//                return VMA_MEMORY_USAGE_AUTO; // Rely on VMA + Allocation Flags (like MAPPED + HOST_ACCESS_SEQUENTIAL_WRITE)
//
//            case MemoryUsage::GpuToCpu:
//                // Prefer host-visible, cached (if possible), suitable for readback.
//                // VMA_MEMORY_USAGE_GPU_TO_CPU is deprecated favouring AUTO.
//                 // Rely on VMA + Allocation Flags (like MAPPED + HOST_ACCESS_RANDOM)
//                return VMA_MEMORY_USAGE_AUTO;
//
//            case MemoryUsage::CpuOnly: // Less common for GPU resources
//                // Could potentially map to HOST_VISIBLE + HOST_CACHED if needed for pure CPU work
//                 return VMA_MEMORY_USAGE_AUTO; // Or map specifically if VMA offers CPU only hints
//
//            // Add other specific cases if needed, e.g., mapping directly to
//            // VMA_MEMORY_USAGE_CPU_ONLY, VMA_MEMORY_USAGE_GPU_LAZY_ALLOCATION etc.
//            // if your abstraction requires that level of control.
//
//            default:
//                // Default to letting VMA decide based on Vulkan usage flags.
//                return VMA_MEMORY_USAGE_AUTO;
//        }
//        // NOTE: The most effective way to control VMA memory is often by combining
//        // VMA_MEMORY_USAGE_AUTO with specific VmaAllocationCreateFlags (like
//        // VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT or
//        // VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT) and required/preferred
//        // VkMemoryPropertyFlags in VmaAllocationCreateInfo.
//        // This simple switch provides basic intent, but check VMA docs for advanced usage.
//    }

    VkSampleCountFlagBits convertSampleCount(SampleCount count)
    {
        switch (count)
        {
            case SampleCount::Count1:  return VK_SAMPLE_COUNT_1_BIT;
            case SampleCount::Count2:  return VK_SAMPLE_COUNT_2_BIT;
            case SampleCount::Count4:  return VK_SAMPLE_COUNT_4_BIT;
            case SampleCount::Count8:  return VK_SAMPLE_COUNT_8_BIT;
            case SampleCount::Count16: return VK_SAMPLE_COUNT_16_BIT;
            case SampleCount::Count32: return VK_SAMPLE_COUNT_32_BIT;
            case SampleCount::Count64: return VK_SAMPLE_COUNT_64_BIT;
            default:
                // Handle invalid input, perhaps default to 1 or assert/log error
                Log::warning << "Invalid SampleCount value encountered. Defaulting to 1." <<Log::nl;
                return VK_SAMPLE_COUNT_1_BIT;
        }
    }

    VkShaderStageFlagBits convertShaderStage(ShaderStage stage)
    {
        switch (stage)
        {
            case ShaderStage::Vertex:  return VK_SHADER_STAGE_VERTEX_BIT;
            case ShaderStage::Fragment:  return VK_SHADER_STAGE_FRAGMENT_BIT;
            case ShaderStage::Compute:  return VK_SHADER_STAGE_COMPUTE_BIT;
            case ShaderStage::Geometry:  return VK_SHADER_STAGE_GEOMETRY_BIT;
            case ShaderStage::TessellationControl: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            case ShaderStage::TessellationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            default:
                // Handle invalid input, perhaps default to 1 or assert/log error
                KEGE_LOG_ERROR << "Invalid shader stage encountered." <<Log::nl;
                return {};
        }
    }

    VkShaderStageFlags convertShaderStageMask(ShaderStage stage)
    {
        VkShaderStageFlags vkFlags = 0; // Start with no flags

        if ((stage & ShaderStage::Vertex) != static_cast<ShaderStage>(0)) { // Use static_cast for type safety with None=0
            vkFlags |= VK_SHADER_STAGE_VERTEX_BIT;
        }
        if ((stage & ShaderStage::Fragment) != static_cast<ShaderStage>(0)) {
            vkFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        if ((stage & ShaderStage::Compute) != static_cast<ShaderStage>(0)) {
            vkFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
        }
        if ((stage & ShaderStage::Geometry) != static_cast<ShaderStage>(0)) {
            vkFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
        }
        if ((stage & ShaderStage::TessellationControl) != static_cast<ShaderStage>(0)) {
            vkFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        }
        if ((stage & ShaderStage::TessellationEvaluation) != static_cast<ShaderStage>(0)) {
            vkFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        }

        return vkFlags;
    }

    VkFilter convertFilter(Filter filter)
    {
        switch ( filter )
        {
            case Filter::Linear:
                return VK_FILTER_LINEAR;

            case Filter::Nearest:
                return VK_FILTER_NEAREST;

            default: KEGE_LOG_ERROR << "Unknown Filter in convertFilter()" <<Log::nl;
                break;
        }
    }
    
    VkSamplerMipmapMode convertMipmapMode(MipmapMode filter)
    {
        switch ( filter )
        {
            case MipmapMode::Linear:
                return VK_SAMPLER_MIPMAP_MODE_LINEAR;

            case MipmapMode::Nearest:
                return VK_SAMPLER_MIPMAP_MODE_NEAREST;

            default: KEGE_LOG_ERROR << "Unknown Filter in convertMipmapMode()" <<Log::nl;
                break;
        }
    }

    VkSamplerAddressMode convertAddressMode(AddressMode mode)
    {
        switch ( mode )
        {
            case AddressMode::Repeat:
                return VK_SAMPLER_ADDRESS_MODE_REPEAT;

            case AddressMode::MirroredRepeat:
                return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

            case AddressMode::ClampToEdge:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

            case AddressMode::ClampToBorder:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

            case AddressMode::MirrorClampToEdge:
                return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;

            default: KEGE_LOG_ERROR << "Unknown AddressMode in convertAddressMode()" <<Log::nl;
                break;
        }
    }

    VkStencilOp convertStencilOp(StencilOp op)
    {
        switch ( op )
        {
            case StencilOp::Zero:
                return VK_STENCIL_OP_ZERO;

            case StencilOp::Keep:
                return VK_STENCIL_OP_KEEP;

            case StencilOp::Invert:
                return VK_STENCIL_OP_INVERT;

            case StencilOp::Replace:
                return VK_STENCIL_OP_REPLACE;

            case StencilOp::IncrementAndWrap:
                return VK_STENCIL_OP_INCREMENT_AND_WRAP;

            case StencilOp::IncrementAndClamp:
                return VK_STENCIL_OP_INCREMENT_AND_CLAMP;

            case StencilOp::DecrementAndClamp:
                return VK_STENCIL_OP_DECREMENT_AND_CLAMP;

            case StencilOp::DecrementAndWrap:
                return VK_STENCIL_OP_DECREMENT_AND_WRAP;

            default: KEGE_LOG_ERROR << "Unknown StencilOp in convertStencilOp()" <<Log::nl;
                break;
        }
    }

    VkColorComponentFlags convertColorComponentMask( ColorComponentFlags flags )
    {
        VkColorComponentFlags components = 0;

        if ( flags == ColorComponentFlags::All )
        {
            components = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        }
        else
        {
            if (( flags & ColorComponentFlags::R ) == ColorComponentFlags::R )
            {
                components |= VK_COLOR_COMPONENT_R_BIT;
            }
            if (( flags & ColorComponentFlags::G ) == ColorComponentFlags::G )
            {
                components |= VK_COLOR_COMPONENT_G_BIT;
            }
            if (( flags & ColorComponentFlags::B ) == ColorComponentFlags::B )
            {
                components |= VK_COLOR_COMPONENT_B_BIT;
            }
            if (( flags & ColorComponentFlags::A ) == ColorComponentFlags::A )
            {
                components |= VK_COLOR_COMPONENT_A_BIT;
            }
        }
        return components;
    }

    VkBlendFactor convertBlendFactor(BlendFactor factor)
    {
        switch (factor)
        {
        case BlendFactor::Zero:
            return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::One:
            return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SrcColor:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DstColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::OneMinusDstColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendFactor::ConstantColor:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case BlendFactor::OneMinusConstantColor:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::ConstantAlpha:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SrcAlphaSaturate:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case BlendFactor::Src1Color:
            return VK_BLEND_FACTOR_SRC1_COLOR;
        case BlendFactor::OneMinusSrc1Color:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::Src1Alpha:
            return VK_BLEND_FACTOR_SRC1_ALPHA;
        case BlendFactor::OneMinusSrc1Alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        default:
            return VK_BLEND_FACTOR_ZERO; // Default case
        }
    }

    VkBlendOp convertBlendOp( BlendOp op )
    {
        switch (op)
        {
        case BlendOp::Add:
            return VK_BLEND_OP_ADD;
        case BlendOp::Subtract:
            return VK_BLEND_OP_SUBTRACT;
        case BlendOp::ReverseSubtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::Min:
            return VK_BLEND_OP_MIN;
        case BlendOp::Max:
            return VK_BLEND_OP_MAX;
        default:
            // Handle the case where an unknown BlendOp is provided.
            // You might want to return a default value, throw an exception,
            // or log an error depending on your application's needs.
            return VK_BLEND_OP_MAX; // Returning a default value for now
        }
    }


    VkCompareOp convertCompareOp(CompareOp compare)
    {
        switch ( compare )
        {
            case CompareOp::Never:
                return VK_COMPARE_OP_NEVER;

            case CompareOp::Less:
                return VK_COMPARE_OP_LESS;

            case CompareOp::Equal:
                return VK_COMPARE_OP_EQUAL;

            case CompareOp::LessEqual:
                return VK_COMPARE_OP_LESS;

            case CompareOp::NotEqual:
                return VK_COMPARE_OP_NOT_EQUAL;

            case CompareOp::GreaterEqual:
                return VK_COMPARE_OP_GREATER_OR_EQUAL;

            case CompareOp::Always:
                return VK_COMPARE_OP_ALWAYS;

            default: KEGE_LOG_ERROR << "Unknown CompareOp in convertCompareOp()" <<Log::nl;
                break;
        }
        return VK_COMPARE_OP_ALWAYS;
    }

    VkBorderColor convertBorderColor(BorderColor color)
    {
        switch ( color )
        {
            case BorderColor::FloatTransparentBlack:
                return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
            case BorderColor::IntTransparentBlack:
                return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
            case BorderColor::FloatOpaqueBlack:
                return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
            case BorderColor::IntOpaqueBlack:
                return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            case BorderColor::FloatOpaqueWhite:
                return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            case BorderColor::IntOpaqueWhite:
                return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
            default: KEGE_LOG_ERROR << "Unknown BorderColor in convertBorderColor()" <<Log::nl;
                break;
        }
    }

    VkDescriptorType convertDescriptorType(DescriptorType type)
    {
        switch (type) {
            case DescriptorType::Sampler:
                return VK_DESCRIPTOR_TYPE_SAMPLER;
            case DescriptorType::CombinedImageSampler:
                return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case DescriptorType::SampledImage:
                return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            case DescriptorType::StorageImage:
                return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            case DescriptorType::UniformTexelBuffer:
                return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
            case DescriptorType::StorageTexelBuffer:
                return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
            case DescriptorType::UniformBuffer:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case DescriptorType::StorageBuffer:
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case DescriptorType::UniformBufferDynamic:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            case DescriptorType::StorageBufferDynamic:
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            case DescriptorType::InputAttachment:
                return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
            default:
                // Default to combined image sampler for unknown values
                //assert(false && "Unknown descriptor type");
                return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        }
    }


    VkImageLayout convertImageLayout( ImageLayout image_layout )
    {
        switch ( image_layout )
        {

            case ImageLayout::Undefined: return VK_IMAGE_LAYOUT_UNDEFINED;
            case ImageLayout::General: return VK_IMAGE_LAYOUT_GENERAL;
            case ImageLayout::ColorAttachment: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case ImageLayout::DepthStencilAttachment: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case ImageLayout::DepthStencilRead: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            case ImageLayout::ShaderReadOnly: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case ImageLayout::TransferSrc: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case ImageLayout::TransferDst: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case ImageLayout::PreInitialized: return VK_IMAGE_LAYOUT_PREINITIALIZED;
            //case ImageLayout:: IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMA: return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
            //case ImageLayout:: IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
            //case ImageLayout:: IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            //case ImageLayout:: IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL: return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
            //case ImageLayout:: IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL: return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
            //case ImageLayout:: IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL: return VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
            //case ImageLayout:: IMAGE_LAYOUT_READ_ONLY_OPTIMAL: return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
            //case ImageLayout:: IMAGE_LAYOUT_ATTACHMENT_OPTIMAL: return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
            case ImageLayout::Present: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            default: KEGE_LOG_ERROR << "Unsupported ImageLayout" <<Log::nl;
        }
        return {};
    }


    VkAccessFlags convertAccessFlag(AccessFlags access)
    {
        VkAccessFlags vk_flags = 0;

        if ((access & AccessFlags::IndirectCommandRead) != AccessFlags::None) vk_flags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        if ((access & AccessFlags::IndexRead) != AccessFlags::None) vk_flags |= VK_ACCESS_INDEX_READ_BIT;
        if ((access & AccessFlags::VertexBufferRead) != AccessFlags::None) vk_flags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        if ((access & AccessFlags::UniformRead) != AccessFlags::None) vk_flags |= VK_ACCESS_UNIFORM_READ_BIT;
        if ((access & AccessFlags::InputAttachmentRead) != AccessFlags::None) vk_flags |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; // Subpass specific
        if ((access & AccessFlags::ShaderRead) != AccessFlags::None) vk_flags |= VK_ACCESS_SHADER_READ_BIT;
        if ((access & AccessFlags::ShaderWrite) != AccessFlags::None) vk_flags |= VK_ACCESS_SHADER_WRITE_BIT;
        if ((access & AccessFlags::ColorAttachmentRead) != AccessFlags::None) vk_flags |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        if ((access & AccessFlags::ColorAttachmentWrite) != AccessFlags::None) vk_flags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        if ((access & AccessFlags::DepthStencilAttachmentRead) != AccessFlags::None) vk_flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        if ((access & AccessFlags::DepthStencilAttachmentWrite) != AccessFlags::None) vk_flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        if ((access & AccessFlags::TransferRead) != AccessFlags::None) vk_flags |= VK_ACCESS_TRANSFER_READ_BIT;
        if ((access & AccessFlags::TransferWrite) != AccessFlags::None) vk_flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
        if ((access & AccessFlags::HostRead) != AccessFlags::None) vk_flags |= VK_ACCESS_HOST_READ_BIT;
        if ((access & AccessFlags::HostWrite) != AccessFlags::None) vk_flags |= VK_ACCESS_HOST_WRITE_BIT;
        if ((access & AccessFlags::MemoryRead) != AccessFlags::None) vk_flags |= VK_ACCESS_MEMORY_READ_BIT;
        if ((access & AccessFlags::MemoryWrite) != AccessFlags::None) vk_flags |= VK_ACCESS_MEMORY_WRITE_BIT;
        // Add Acceleration Structure flags if needed (VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR, etc.)

        // Note: VK_ACCESS_NONE is 0, so if 'access' is None, vk_flags will correctly be 0.

        return vk_flags;
    }


    VkPipelineStageFlags convertPipelineStageFlag(PipelineStageFlag stages)
    {
        VkPipelineStageFlags flags = 0;

        if ((stages & PipelineStageFlag::TopOfPipe) != 0) flags |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        if ((stages & PipelineStageFlag::DrawIndirect) != 0) flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
        if ((stages & PipelineStageFlag::VertexInput) != 0) flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        if ((stages & PipelineStageFlag::VertexShader) != 0) flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
        if ((stages & PipelineStageFlag::TessellationControlShader) != 0) flags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
        if ((stages & PipelineStageFlag::TessellationEvaluationShader) != 0) flags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
        if ((stages & PipelineStageFlag::GeometryShader) != 0) flags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT; // Requires feature
        if ((stages & PipelineStageFlag::FragmentShader) != 0) flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        if ((stages & PipelineStageFlag::EarlyFragmentTests) != 0) flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        if ((stages & PipelineStageFlag::LateFragmentTests) != 0) flags |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        if ((stages & PipelineStageFlag::ColorAttachmentOutput) != 0) flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        if ((stages & PipelineStageFlag::ComputeShader) != 0) flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        if ((stages & PipelineStageFlag::Transfer) != 0) flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        if ((stages & PipelineStageFlag::BottomOfPipe) != 0) flags |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        if ((stages & PipelineStageFlag::Host) != 0) flags |= VK_PIPELINE_STAGE_HOST_BIT;
        if ((stages & PipelineStageFlag::RayTrace) != 0) flags |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;

        // Handle composite flags (optional, as individual bits are usually enough)
        if ((stages & PipelineStageFlag::AllGraphics) != 0) flags |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
        if ((stages & PipelineStageFlag::AllCommands) != 0) flags |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

        // Ensure that if no specific stage was requested (potentially an error in barrier logic),
        // we use sensible defaults for src/dst to avoid validation errors.
        // This logic might better belong in the barrier calculation itself.
        if (flags == 0)
        {
            // Maybe default to ALL_COMMANDS for safety? Or TOP/BOTTOM?
             flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; // Or BOTTOM?
        }

        return flags;
    }


    // Translates abstract resource state to appropriate Vulkan image layout
    VkImageLayout translatesToVkLayout(ResourceState state, Format format)
    {
        switch ( state )
        {
            case ResourceState::Undefined:
                return VK_IMAGE_LAYOUT_UNDEFINED;

            case ResourceState::RenderTargetColor:
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            case ResourceState::RenderTargetDepthStencil:
                // Could refine based on format if separating depth/stencil-only layouts
                // if (isDepthOnlyFormat(format)) return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                // if (isStencilOnlyFormat(format)) return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            case ResourceState::ShaderResource:
                // Read-only access in shaders (textures, uniform texel buffers)
                if ( isDepthStencilFormat(format) )
                {
                     // Depth/stencil formats often need a specific read-only layout
                     return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                     // Could refine to DEPTH_READ_ONLY / STENCIL_READ_ONLY if needed
                }
                else
                {
                    return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                }

            case ResourceState::UnorderedAccess:
                // Read/Write access in shaders (storage images/texel buffers)
                return VK_IMAGE_LAYOUT_GENERAL; // General layout is usually required for storage images

            case ResourceState::CopySrc:
                return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

            case ResourceState::CopyDst:
                return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

            case ResourceState::Present:
                return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            // States primarily relevant for Buffers (Layout doesn't apply)
            case ResourceState::VertexBuffer:
            case ResourceState::IndexBuffer:
            case ResourceState::UniformBuffer:
            case ResourceState::StorageBufferRead:
            case ResourceState::StorageBufferWrite:
            case ResourceState::IndirectArgument:
                // Buffers don't have image layouts. Return something reasonable or assert.
                // Returning UNDEFINED might be safest if called erroneously for a buffer.
                // std::cerr << "Warning: stateToVkLayout called for buffer-specific state." << std::endl;
                return VK_IMAGE_LAYOUT_UNDEFINED; // Or VK_IMAGE_LAYOUT_GENERAL? Needs care.

            // Add ResolveSrc/Dst cases if needed -> TRANSFER_SRC/DST usually appropriate
            // case ResourceState::ResolveSrc: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            // case ResourceState::ResolveDst: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

            default:
                std::cerr << "Warning: Unhandled ResourceState in stateToVkLayout. Defaulting to UNDEFINED." << std::endl;
                return VK_IMAGE_LAYOUT_UNDEFINED;
        }
        return {};
    }



    void logVkError(VkResult result, const char* from, const char* where )
    {
        switch ( result )
        {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                Log::error <<"( "<< from <<" ) ->" << where << "() ->VK_ERROR_OUT_OF_HOST_MEMORY in " <<Log::nl;
                break;

            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                Log::error <<"( "<< from <<" ) ->" << where << "() ->VK_ERROR_OUT_OF_DEVICE_MEMORY in " <<Log::nl;
                break;

            case VK_ERROR_FRAGMENTED_POOL:
                Log::error <<"( "<< from <<" ) ->" << where << "() ->VK_ERROR_FRAGMENTED_POOL in " <<Log::nl;
                break;

            case VK_ERROR_OUT_OF_POOL_MEMORY:
                Log::error <<"( "<< from <<" ) ->" << where << "() ->VK_ERROR_OUT_OF_POOL_MEMORY in " <<Log::nl;
                break;

            case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR:
                Log::error <<"( "<< from <<" ) ->" << where << "() -> VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR in createBuffer";
                break;

            default:
                Log::error <<"( "<< from <<" ) ->" << where << "() -> " << Log::nl;
                break;
        }
    }

    QueueFamilyIndices findQueueFamilies( VkPhysicalDevice physical_device, VkSurfaceKHR surface )
    {
        QueueFamilyIndices indices;
        bool requiresPresent = (surface != VK_NULL_HANDLE);

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &queueFamilyCount, nullptr );
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &queueFamilyCount, queueFamilies.data() );

        int i = 0;
        std::map< QueueType, int > queuemap;
        std::map< int, QueueType > queue_indxmap;
        for (const auto& queueFamily : queueFamilies)
        {
            // Check for Graphics Queue
            if (!indices.graphics_family.has_value() && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                if ( queuemap.find( QueueType::Graphics ) == queuemap.end() )
                {
                    indices.graphics_family = i;
                    queuemap[ QueueType::Graphics ] = i;
                    queue_indxmap[i] = QueueType::Graphics;
                }
            }

            // Check for Compute Queue
            if (!indices.compute_family.has_value() && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
            {
                if (queue_indxmap.find(i) == queue_indxmap.end())
                {
                    indices.compute_family = i;
                    queuemap[ QueueType::Compute ] = i;
                    queue_indxmap[i] = QueueType::Compute;
                }
            }

            // Check for Transfer Queue
            if (!indices.transfer_family.has_value() && (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT))
            {
                if (queue_indxmap.find(i) == queue_indxmap.end())
                {
                    indices.transfer_family = i;
                    queuemap[ QueueType::Transfer ] = i;
                    queue_indxmap[i] = QueueType::Transfer;
                }
            }

            // Check for Presentation Queue (if needed)
            if ( requiresPresent && !indices.present_family.has_value() )
            {
                VkBool32 support_present = VK_FALSE;

                // Corrected check: Use the function directly if loaded
                // It's better to load this function pointer once after instance creation.
                // Let's assume vkGetPhysicalDeviceSurfaceSupportKHR is available globally or via a loader.

//                vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice , <#uint32_t queueFamilyIndex#>, VkSurfaceKHR, <#VkBool32 *pSupported#>)
                VkResult present_check_result = vkGetPhysicalDeviceSurfaceSupportKHR( physical_device, i, surface, &support_present );
                if ( present_check_result == VK_SUCCESS && support_present )
                {
                    if (queue_indxmap.find(i) == queue_indxmap.end())
                    {
                        indices.present_family = i;
                        queuemap[ QueueType::Graphics ] = i;
                        queue_indxmap[i] = QueueType::Graphics;
                    }
                }
                // If vkGetPhysicalDeviceSurfaceSupportKHR isn't found, we can't check, so fail gracefully.
                // A robust implementation loads this function pointer reliably after instance creation.
                else if (present_check_result != VK_SUCCESS)
                {
                    // Consider logging an error - unable to check presentation support
                }
            }

            if ( indices.isComplete() )
            {
                break;
            }
            i++;
        }

        if ( !indices.isComplete() )
        {
            if ( !indices.present_family.has_value() )
            {
                indices.present_family = queuemap[ QueueType::Graphics ];
            }
            if ( !indices.compute_family.has_value() )
            {
                indices.compute_family = (queuemap.find( QueueType::Transfer ) != queuemap.end())
                ? queuemap[ QueueType::Transfer ] : queuemap[ QueueType::Graphics ];
            }
            if ( !indices.transfer_family.has_value() )
            {
                indices.transfer_family = (queuemap.find( QueueType::Compute ) != queuemap.end())
                ? queuemap[ QueueType::Compute ] : queuemap[ QueueType::Graphics ];
            }
            if ( !indices.present_family.has_value() )
            {
                indices.present_family = queuemap[ QueueType::Graphics ];
            }
        }
        return indices;
    }

}
