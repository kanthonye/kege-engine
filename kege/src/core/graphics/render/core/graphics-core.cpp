//
//  graphics-enums-and-types.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/24/25.
//

#include "graphics-core.hpp"

namespace kege{

    /**
     * @brief Convert engine descriptor type to a human-readable C string
     *
     * @param type Engine descriptor type
     * @return Null-terminated C string name of the descriptor type
     */
    const char* descriptorTypeCString( DescriptorType type )
    {
        switch ( type )
        {
            case DescriptorType::CombinedImageSampler: return "CombinedImageSampler";
            case DescriptorType::SampledImage: return "SampledImage";
            case DescriptorType::StorageImage: return "StorageImage";
            case DescriptorType::UniformTexelBuffer: return "UniformTexelBuffer";
            case DescriptorType::StorageTexelBuffer: return "StorageTexelBuffer";
            case DescriptorType::UniformBuffer: return "UniformBuffer";
            case DescriptorType::StorageBuffer: return "StorageBuffer";
            case DescriptorType::UniformBufferDynamic: return "UniformBufferDynamic";
            case DescriptorType::StorageBufferDynamic: return "StorageBufferDynamic";
            case DescriptorType::InputAttachment: return "InputAttachment";
            case DescriptorType::Sampler: return "Sampler";
            default: return "Invalid";
        }
    }
    
    int sizeOf( ShaderVarType t )
    {
        switch ( t )
        {
            case ShaderVarType::Bool:    return 1;

            case ShaderVarType::Sint:
            case ShaderVarType::Uint:
            case ShaderVarType::Float:   return 4;

            case ShaderVarType::Vec2:
            case ShaderVarType::Double:  return 8;
            case ShaderVarType::Vec3:    return 12;
            case ShaderVarType::Vec4:    return 16;

            case ShaderVarType::Vec2D:   return 16;
            case ShaderVarType::Vec3D:   return 24;
            case ShaderVarType::Vec4D:   return 32;

            case ShaderVarType::Mat2:    return 12;
            case ShaderVarType::Mat3:    return 36;
            case ShaderVarType::Mat4:    return 64;

            default: break;
        }
        return 0;
    }

    std::string toString( ShaderVarType t )
    {
        switch ( t )
        {
            case ShaderVarType::Bool:      return "bool";
            case ShaderVarType::Sint:      return "int";
            case ShaderVarType::Uint:      return "uint";
            case ShaderVarType::Float:     return "float";
            case ShaderVarType::Double:    return "double";
            case ShaderVarType::Vec2:      return "vec2";
            case ShaderVarType::Vec3:      return "vec3";
            case ShaderVarType::Vec4:      return "vec4";
            case ShaderVarType::Vec2D:     return "dvec2";
            case ShaderVarType::Vec3D:     return "dvec3";
            case ShaderVarType::Vec4D:     return "dvec4";
            case ShaderVarType::Mat2:      return "mat2";
            case ShaderVarType::Mat3:      return "mat3";
            case ShaderVarType::Mat4:      return "mat4";
            default: break;
        }
        return "";
    }
}
