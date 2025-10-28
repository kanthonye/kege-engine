//
//  shader-data-types.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#include "shader-data-types.hpp"

namespace kege{
    
    int sizeOf( ShaderDataType t )
    {
        switch ( t )
        {
            case ShaderDataType::Bool: return 1;

            case ShaderDataType::Sint:
            case ShaderDataType::Uint:
            case ShaderDataType::Float: return 4;

            case ShaderDataType::Vec2:
            case ShaderDataType::Double:  return 8;
            case ShaderDataType::Vec3:    return 12;
            case ShaderDataType::Vec4:    return 16;

            case ShaderDataType::DVec2:   return 16;
            case ShaderDataType::DVec3:   return 24;
            case ShaderDataType::DVec4:   return 32;

            case ShaderDataType::Mat2:    return 12;
            case ShaderDataType::Mat3:    return 36;
            case ShaderDataType::Mat4:    return 64;

            default: break;
        }
        return 0;
    }

    std::string toString( ShaderDataType t )
    {
        switch ( t )
        {
            case ShaderDataType::Bool:      return "bool";
            case ShaderDataType::Sint:      return "int";
            case ShaderDataType::Uint:      return "uint";
            case ShaderDataType::Float:     return "float";
            case ShaderDataType::Double:    return "double";
            case ShaderDataType::Vec2:      return "vec2";
            case ShaderDataType::Vec3:      return "vec3";
            case ShaderDataType::Vec4:      return "vec4";
            case ShaderDataType::DVec2:     return "dvec2";
            case ShaderDataType::DVec3:     return "dvec3";
            case ShaderDataType::DVec4:     return "dvec4";
            case ShaderDataType::Mat2:      return "mat2";
            case ShaderDataType::Mat3:      return "mat3";
            case ShaderDataType::Mat4:      return "mat4";
            default: break;
        }
        return "";
    }
}
