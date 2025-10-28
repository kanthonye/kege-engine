//
//  shader-data-types.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#ifndef shader_data_types_hpp
#define shader_data_types_hpp

#include "graphics-core.hpp"

namespace kege{
    
    /**
     * @brief Enumeration of shader data types.
     */
    enum struct ShaderDataType
    {
        Undefined,
        Bool,
        Sint,
        Uint,
        Float,
        Double,
        Vec2,
        Vec3,
        Vec4,
        DVec2,
        DVec3,
        DVec4,
        Mat2,
        Mat3,
        Mat4,
    };

    int sizeOf( ShaderDataType t );
    std::string toString( ShaderDataType t );
}


namespace kege{

    /**
     * @brief Describes how vertex attributes are fetched from a binding.
     */
    struct VertexAttributeDesc {
        /**
         * @brief Shader input location (layout(location=X) in GLSL).
         */
        uint32_t location = 0;

        /**
         * @brief Binding number this attribute pulls from.
         *
         * Must match a binding description's binding number.
         */
        uint32_t binding = 0;

        /**
         * @brief Format/type of the attribute data.
         *
         * Common formats:
         * - Format::RGB32_SFLOAT for vec3
         * - Format::RGBA8_UNORM for normalized uchar4
         * - Format::RG32_UINT for uvec2
         */
        ShaderDataType type = ShaderDataType::Undefined;

        /**
         * @brief Byte offset from start of vertex data.
         *
         * For interleaved attributes, this is the offset within the vertex structure.
         */
        uint32_t offset = 0;

        /**
         * @brief Specifies whether data is per-vertex or per-instance.
         *
         * @note For instanced rendering, typically:
         * - Vertex-rate data contains mesh geometry
         * - Instance-rate data contains transformation matrices
         */
        VertexInputRate input_rate = VertexInputRate::Vertex;

        std::string name;
    };
}
#endif /* shader_data_types_hpp */
