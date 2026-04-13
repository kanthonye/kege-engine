//
//  vertex-format.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 2/7/26.
//


#ifndef kege_vertex_format_hpp
#define kege_vertex_format_hpp

#include "../core/graphics-core.hpp"


namespace kege{

    /**
     * @brief Describes the usage of a member within a push constant block.
     */
    enum struct ShaderVar : char
    {
        Unknown,

        Bool,
        Sint,
        Uint,
        Float,
        Double,

        Vec2,  Vec3,  Vec4,
        Vec2I, Vec3I, Vec4I,
        Vec2U, Vec3U, Vec4U,
        Vec2D, Vec3D, Vec4D,
        Mat2,  Mat3,  Mat4
    };

    int sizeOf( ShaderVar t );
    std::string shaderVarToString( ShaderVar t );
    kege::ShaderVar stringToShaderVarType( const std::string& str );

    enum VertexBit
    {
        // ====== VERTEX SHADER FEATURES ======
        POSITION    = 1 << 0,   // CPU/GPU vertex animation
        NORMAL      = 1 << 1,   // Blend shapes/morph targets
        TEXCOORD    = 1 << 2,   // Skin with bone matrices on GPU
        TANGENT     = 1 << 3,   // Displace vertices by texture
        BITANGENT   = 1 << 4,   // Foliage/wind animation
        WEIGHTS     = 1 << 5,   // Billboard particles/quads
        JOINTS      = 1 << 6,   // GPU instancing
        COLOR       = 1 << 7,   // Use vertex color attribute

        ALBEDO      = 1 << 8,
        EMISSIVE    = 1 << 9,
    };

    typedef int VertexSignature;

    struct VertexAttribute
    {
        std::string name;
        uint8_t location;
        uint8_t binding;
        ShaderVar type;
        VertexBit semantic;
    };

    enum struct VertexInputRate
    {
        Vertex,   ///< Data advances per vertex (standard vertex buffers)
        Instance  ///< Data advances per instance (instanced rendering)
    };

   struct VertexInput
   {
       uint32_t index = 0;

       /**
        * @brief Byte stride between consecutive elements.
        *
        * For vertex rate: distance between vertices
        * For instance rate: distance between instances
        */
       uint32_t stride = 0;

       /**
        * @brief Specifies whether data is per-vertex or per-instance.
        *
        * @note For instanced rendering, typically:
        * - Vertex-rate data contains mesh geometry
        * - Instance-rate data contains transformation matrices
        */
       VertexInputRate input_rate = VertexInputRate::Vertex;
   };

    struct VertexLayout
    {
        /**
         * @brief List of vertex attributes.
         *
         * Defines how to interpret data from the bound buffers.
         */
        std::vector< VertexAttribute > attributes;

        /**
         * @brief Byte stride between consecutive elements.
         *
         * For vertex rate: distance between vertices
         * For instance rate: distance between instances
         */
        std::vector< VertexInput > input_rates;

        /**
         */
        VertexSignature signature;



        static VertexLayout staticMesh()
        {
            return VertexLayout
            {
                .attributes = std::vector< VertexAttribute >
                {
                    VertexAttribute
                    {
                        .semantic = VertexBit::POSITION,
                        .name = "in_position",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 0,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::NORMAL,
                        .name = "in_normal",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 1,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::TEXCOORD,
                        .name = "in_texcoord",
                        .type = ShaderVar::Vec2,
                        .binding = 0,
                        .location = 2,
                    },
                },
                .input_rates = std::vector< VertexInput >
                {
                    VertexInput
                    {
                        .stride = 24,
                        .input_rate = VertexInputRate::Vertex,
                    }
                },
                .signature = VertexBit::POSITION | VertexBit::NORMAL | VertexBit::TEXCOORD
            };
        }
        
        static VertexLayout staticMeshNormalMapped()
        {
            return VertexLayout
            {
                .attributes = std::vector< VertexAttribute >
                {
                    VertexAttribute
                    {
                        .semantic = VertexBit::POSITION,
                        .name = "in_position",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 0,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::NORMAL,
                        .name = "in_normal",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 1,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::TEXCOORD,
                        .name = "in_texcoord",
                        .type = ShaderVar::Vec2,
                        .binding = 0,
                        .location = 2,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::TANGENT,
                        .name = "in_tangent",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 3,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::BITANGENT,
                        .name = "in_bitangent",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 4,
                    },
                },
                .input_rates = std::vector< VertexInput >
                {
                    VertexInput
                    {
                        .stride = 56,
                        .input_rate = VertexInputRate::Vertex,
                    }
                },
                .signature = VertexBit::POSITION
                | VertexBit::NORMAL  | VertexBit::TEXCOORD
                | VertexBit::TANGENT | VertexBit::BITANGENT
            };
        }

        static VertexLayout skinMesh()
        {
            return VertexLayout
            {
                .attributes = std::vector< VertexAttribute >
                {
                    VertexAttribute
                    {
                        .semantic = VertexBit::POSITION,
                        .name = "in_position",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 0,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::NORMAL,
                        .name = "in_normal",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 1,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::TEXCOORD,
                        .name = "in_texcoord",
                        .type = ShaderVar::Vec2,
                        .binding = 0,
                        .location = 2,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::JOINTS,
                        .name = "in_joints",
                        .type = ShaderVar::Vec4,
                        .binding = 0,
                        .location = 5,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::WEIGHTS,
                        .name = "in_weights",
                        .type = ShaderVar::Vec4,
                        .binding = 0,
                        .location = 6,
                    },
                },
                .input_rates = std::vector< VertexInput >
                {
                    VertexInput
                    {
                        .stride = 64,
                        .input_rate = VertexInputRate::Vertex,
                    }
                },
                .signature = VertexBit::POSITION
                | VertexBit::NORMAL  | VertexBit::TEXCOORD
                | VertexBit::JOINTS  | VertexBit::WEIGHTS
            };
        }

        static VertexLayout skinMeshNormalMapped()
        {
            return VertexLayout
            {
                .attributes = std::vector< VertexAttribute >
                {
                    VertexAttribute
                    {
                        .semantic = VertexBit::POSITION,
                        .name = "in_position",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 0,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::NORMAL,
                        .name = "in_normal",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 1,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::TEXCOORD,
                        .name = "in_texcoord",
                        .type = ShaderVar::Vec2,
                        .binding = 0,
                        .location = 2,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::TANGENT,
                        .name = "in_tangent",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 3,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::BITANGENT,
                        .name = "in_bitangent",
                        .type = ShaderVar::Vec3,
                        .binding = 0,
                        .location = 4,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::JOINTS,
                        .name = "in_joints",
                        .type = ShaderVar::Vec4,
                        .binding = 0,
                        .location = 5,
                    },
                    VertexAttribute
                    {
                        .semantic = VertexBit::WEIGHTS,
                        .name = "in_weights",
                        .type = ShaderVar::Vec4,
                        .binding = 0,
                        .location = 6,
                    },
                },
                .input_rates = std::vector< VertexInput >
                {
                    VertexInput
                    {
                        .stride = 88,
                        .input_rate = VertexInputRate::Vertex,
                    }
                },
                .signature = VertexBit::POSITION
                | VertexBit::NORMAL  | VertexBit::TEXCOORD
                | VertexBit::TANGENT | VertexBit::BITANGENT
                | VertexBit::JOINTS  | VertexBit::WEIGHTS
            };
        }
    };



//    inline constexpr VertexBitFlags operator|(VertexBitFlags a, VertexBitFlags b)
//    {
//        return static_cast<VertexBitFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
//    }
//
//    inline constexpr VertexBitFlags operator&(VertexBitFlags a, VertexBitFlags b)
//    {
//        return static_cast<VertexBitFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
//    }
//
//    inline constexpr VertexBitFlags& operator|=(VertexBitFlags& a, VertexBitFlags b)
//    {
//        a = a | b;
//        return a;
//    }
//
//    inline constexpr VertexBitFlags& operator&=(VertexBitFlags& a, VertexBitFlags b)
//    {
//        a = a & b;
//        return a;
//    }
//
//    inline constexpr bool operator==(VertexBitFlags& a, VertexBitFlags b)
//    {
//        return (static_cast<uint32_t>(a) == static_cast<uint32_t>(b));
//    }
//
//    inline constexpr bool operator!=(VertexBitFlags& a, VertexBitFlags b)
//    {
//        return (static_cast<uint32_t>(a) != static_cast<uint32_t>(b));
//    }

}
#endif //kege_vertex_format_hpp
