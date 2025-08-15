//
//  string-to-enum-types.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/3/25.
//

#include "../../graphics/core/string-to-enum-types.hpp"

namespace kege{

    kege::CompareOp stringToCompareOp( const std::string& type )
    {
        static std::map< std::string, kege::CompareOp > table;
        if( table.empty() )
        {
            table[ "never" ] = kege::CompareOp::Never;
            table[ "less" ] = kege::CompareOp::Less;
            table[ "equal" ] = kege::CompareOp::Equal;
            table[ "less-equal" ] = kege::CompareOp::LessEqual;
            table[ "greater" ] = kege::CompareOp::Greater;
            table[ "not-equal" ] = kege::CompareOp::NotEqual;
            table[ "greater-equal" ] = kege::CompareOp::GreaterEqual;
            table[ "always" ] = kege::CompareOp::Always;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        KEGE_LOG_ERROR << "invalid CompareOp -> " << type <<Log::nl;
        return kege::CompareOp::Always;
    }

    StencilOp stringToStencilOp( const std::string& type )
    {
        static std::map< std::string, kege::StencilOp > table;
        if( table.empty() )
        {
            table[ "keep" ] = kege::StencilOp::Keep;
            table[ "zero" ] = kege::StencilOp::Zero;
            table[ "replace" ] = kege::StencilOp::Replace;
            table[ "increment-and-clamp" ] = kege::StencilOp::IncrementAndClamp;
            table[ "decrement-and-clamp" ] = kege::StencilOp::DecrementAndClamp;
            table[ "invert" ] = kege::StencilOp::Invert;
            table[ "increment-and-wrap" ] = kege::StencilOp::IncrementAndWrap;
            table[ "decrement-and-wrap" ] = kege::StencilOp::DecrementAndWrap;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        KEGE_LOG_ERROR << "invalid StencilOp -> " << type <<Log::nl;
        return kege::StencilOp::Zero;
    }

    ColorBlendLogicOp stringToColorBlendLogicOp( const std::string& type )
    {
        static std::map< std::string, kege::ColorBlendLogicOp > table;
        if( table.empty() )
        {
            table[ "clear" ] = kege::ColorBlendLogicOp::Clear;
            table[ "and" ] = kege::ColorBlendLogicOp::And;
            table[ "and-reverse" ] = kege::ColorBlendLogicOp::AndReverse;
            table[ "copy" ] = kege::ColorBlendLogicOp::Copy;
            table[ "and-inverted" ] = kege::ColorBlendLogicOp::AndInverted;
            table[ "no-op" ] = kege::ColorBlendLogicOp::NoOp;
            table[ "xor" ] = kege::ColorBlendLogicOp::Xor;
            table[ "or" ] = kege::ColorBlendLogicOp::Or;
            table[ "Nor" ] = kege::ColorBlendLogicOp::Nor;
            table[ "equiv" ] = kege::ColorBlendLogicOp::Equiv;
            table[ "invert" ] = kege::ColorBlendLogicOp::Invert;
            table[ "or-reverse" ] = kege::ColorBlendLogicOp::OrReverse;
            table[ "copy-inverted" ] = kege::ColorBlendLogicOp::CopyInverted;
            table[ "or-inverted" ] = kege::ColorBlendLogicOp::OrInverted;
            table[ "nand" ] = kege::ColorBlendLogicOp::Nand;
            table[ "set" ] = kege::ColorBlendLogicOp::Set;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        KEGE_LOG_ERROR << "invalid ColorBlendLogicOp -> " << type <<Log::nl;
        return kege::ColorBlendLogicOp::Clear;
    }

    kege::BlendFactor stringToBlendFactor( const std::string& type )
    {
        static std::map< std::string, kege::BlendFactor > table;
        if( table.empty() )
        {
            table[ "zero" ] = kege::BlendFactor::Zero;
            table[ "one" ] = kege::BlendFactor::One;
            table[ "src-color" ] = kege::BlendFactor::SrcColor;
            table[ "src-alpha" ] = kege::BlendFactor::SrcAlpha;
            table[ "dst-color" ] = kege::BlendFactor::DstColor;
            table[ "dst-alpha" ] = kege::BlendFactor::DstAlpha;
            table[ "src1-color" ] = kege::BlendFactor::Src1Color;
            table[ "src1-alpha" ] = kege::BlendFactor::Src1Alpha;
            table[ "constant-color" ] = kege::BlendFactor::ConstantColor;
            table[ "constant-alpha" ] = kege::BlendFactor::ConstantAlpha;
            table[ "one-minus-src-color" ] = kege::BlendFactor::OneMinusSrcColor;
            table[ "one-minus-dst-color" ] = kege::BlendFactor::OneMinusDstColor;
            table[ "one-minus-src-alpha" ] = kege::BlendFactor::OneMinusSrcAlpha;
            table[ "one-minus-constant-color" ] = kege::BlendFactor::OneMinusConstantColor;
            table[ "one-minus-constant-alpha" ] = kege::BlendFactor::OneMinusConstantAlpha;
            table[ "one-minus-src1-alpha" ] = kege::BlendFactor::OneMinusSrc1Alpha;
            table[ "src-alpha-saturate" ] = kege::BlendFactor::SrcAlphaSaturate;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        KEGE_LOG_ERROR << "invalid BlendFactor -> " << type <<Log::nl;
        return kege::BlendFactor::Zero;
    }

    kege::BlendOp stringToBlendOp( const std::string& type )
    {
        static std::map< std::string, kege::BlendOp > table;
        if( table.empty() )
        {
            table[ "add" ] = kege::BlendOp::Add;
            table[ "subtract" ] = kege::BlendOp::Subtract;
            table[ "reverse-subtract" ] = kege::BlendOp::ReverseSubtract;
            table[ "min" ] = kege::BlendOp::Min;
            table[ "max" ] = kege::BlendOp::Max;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        KEGE_LOG_ERROR << "invalid BlendOp -> " << type <<Log::nl;
        return kege::BlendOp::Add;
    }

    FrontFace stringToFrontFace( const std::string& type )
    {
        static std::map< std::string, kege::FrontFace > table;
        if( table.empty() )
        {
            table[ "cw" ] = kege::FrontFace::Clockwise;
            table[ "ccw" ] = kege::FrontFace::CounterClockwise;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        return kege::FrontFace::CounterClockwise;
    }

    kege::PolygonMode stringToPolygonMode( const std::string& type )
    {
        static std::map< std::string, kege::PolygonMode > table;
        if( table.empty() )
        {
            table[ "point" ] = kege::PolygonMode::Point;
            table[ "line" ] = kege::PolygonMode::Line;
            table[ "fill" ] = kege::PolygonMode::Fill;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        return kege::PolygonMode::Invalid;
    }

    kege::CullMode stringToCullMode( const std::string& type )
    {
        static std::map< std::string, kege::CullMode > table;
        if( table.empty() )
        {
            table[ "back" ] = kege::CullMode::Back;
            table[ "front" ] = kege::CullMode::Front;
            table[ "front_back" ] = kege::CullMode::FrontAndBack;
            table[ "cull_node" ] = kege::CullMode::None;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        return kege::CullMode::None;
    }

    kege::PrimitiveTopology stringToPrimitiveTopology( const std::string& type )
    {
        static std::map< std::string, kege::PrimitiveTopology > table;
        if( table.empty() )
        {
            table[ "point-list" ] = kege::PrimitiveTopology::PointList;
            table[ "line-list" ] = kege::PrimitiveTopology::LineList;
            table[ "line-strip" ] = kege::PrimitiveTopology::LineStrip;
            table[ "triangle-fan" ] = kege::PrimitiveTopology::TriangleList;
            table[ "triangle-list" ] = kege::PrimitiveTopology::TriangleList;
            table[ "triangle-strip" ] = kege::PrimitiveTopology::TriangleStrip;
            table[ "line-list-with-adjacency" ] = kege::PrimitiveTopology::LineListAdjacency;
            table[ "line-strip-with-adjacency" ] = kege::PrimitiveTopology::LineStripAdjacency;
            table[ "triangle-list-with-adjacency" ] = kege::PrimitiveTopology::TriangleListAdjacency;
            table[ "triangle-strip-with-adjacency" ] = kege::PrimitiveTopology::TriangleStripAdjacency;
            table[ "patch-list" ] = kege::PrimitiveTopology::PatchList;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        return kege::PrimitiveTopology::Invalid;
    }

    kege::DescriptorType convertDescriptorType( const std::string& type )
    {
        static std::map< std::string, kege::DescriptorType > table;
        if( table.empty() )
        {
            table[ "sampler"                ] = kege::DescriptorType::Sampler;
            table[ "sampled-image"          ] = kege::DescriptorType::SampledImage;
            table[ "combined-image-sampler" ] = kege::DescriptorType::CombinedImageSampler;
            table[ "storage-buffer"         ] = kege::DescriptorType::StorageBuffer;
            table[ "storage-image"          ] = kege::DescriptorType::StorageImage;
            table[ "storage-buffer-dynamic" ] = kege::DescriptorType::StorageBufferDynamic;
            table[ "storage-texel-buffer"   ] = kege::DescriptorType::StorageTexelBuffer;
            table[ "uniform-buffer"         ] = kege::DescriptorType::UniformBuffer;
            table[ "uniform-buffer-dynamic" ] = kege::DescriptorType::UniformBufferDynamic;
            table[ "uniform-texel-buffer"   ] = kege::DescriptorType::UniformTexelBuffer;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        KEGE_LOG_ERROR << "invalid DescriptorType -> " << type <<" in convertDescriptorType()" <<Log::nl;
        return kege::DescriptorType::Invalid;
    }

    kege::ShaderStage convertShaderStage( const std::string& type )
    {
        static std::map< std::string, kege::ShaderStage > table;
        if( table.empty() )
        {
            table[ "vertex"          ] = kege::ShaderStage::Vertex;
            table[ "fragment"        ] = kege::ShaderStage::Fragment;
            table[ "tess-control"    ] = kege::ShaderStage::TessellationControl;
            table[ "tess-evaliation" ] = kege::ShaderStage::TessellationEvaluation;
            table[ "geometry"        ] = kege::ShaderStage::Geometry;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        return kege::ShaderStage::Invalid;
    }

    kege::Format convertVertexInputType( const std::string& type )
    {
        static std::map< std::string, kege::Format > table;
        if( table.empty() )
        {
            table[ "int" ] = kege::Format::r_s32;
            table[ "uint" ] = kege::Format::r_u32;
            table[ "float" ] = kege::Format::r_f32;
            table[ "double" ] = kege::Format::r_f64;

            table[ "vec2"  ] = kege::Format::rg_f32;
            table[ "dvec2" ] = kege::Format::rg_f64;
            table[ "ivec2" ] = kege::Format::rg_s32;
            table[ "uvec2" ] = kege::Format::rg_u32;

            table[ "vec3"  ] = kege::Format::rgb_f32;
            table[ "dvec3" ] = kege::Format::rgb_f64;
            table[ "ivec3" ] = kege::Format::rgb_s32;
            table[ "uvec3" ] = kege::Format::rgb_u32;

            table[ "vec4"  ] = kege::Format::rgba_f32;
            table[ "dvec4" ] = kege::Format::rgba_f64;
            table[ "ivec4" ] = kege::Format::rgba_s32;
            table[ "uvec4" ] = kege::Format::rgba_u32;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        KEGE_LOG_ERROR << "invalid vertex input type";
        return {};
    }


    kege::Format stringToFormat( const std::string& type )
    {
        static std::map< std::string, kege::Format > table;
        if( table.empty() )
        {
            table[ "r_s32" ] = kege::Format::r_s32;
            table[ "r_u32" ] = kege::Format::r_u32;
            table[ "r_f32" ] = kege::Format::r_f32;
            table[ "r_f64" ] = kege::Format::r_f64;

            table[ "rg_f32"  ] = kege::Format::rg_f32;
            table[ "rg_f64" ] = kege::Format::rg_f64;
            table[ "rg_s32" ] = kege::Format::rg_s32;
            table[ "rg_u32" ] = kege::Format::rg_u32;

            table[ "rgb_f32"  ] = kege::Format::rgb_f32;
            table[ "rgb_f64" ] = kege::Format::rgb_f64;
            table[ "rgb_s32" ] = kege::Format::rgb_s32;
            table[ "rgb_u32" ] = kege::Format::rgb_u32;

            table[ "rgba_f32"  ] = kege::Format::rgba_f32;
            table[ "rgba_f64" ] = kege::Format::rgba_f64;
            table[ "rgba_s32" ] = kege::Format::rgba_s32;
            table[ "rgba_u32" ] = kege::Format::rgba_u32;

            table[ "bgra_u8" ] = kege::Format::bgra_u8;
            table[ "bgra_s8" ] = kege::Format::bgra_s8;
            table[ "bgra_s8_norm" ] = kege::Format::bgra_s8_norm;
            table[ "bgra_u8_norm" ] = kege::Format::bgra_u8_norm;
            table[ "bgra_u8_norm_srbg" ] = kege::Format::bgra_u8_norm_srbg;

            table[ "depth_16" ] = kege::Format::depth_16;
            table[ "depth_32" ] = kege::Format::depth_32;
            table[ "depth_16_stencil_8" ] = kege::Format::depth_16_stencil_8;
            table[ "depth_24_stencil_8" ] = kege::Format::depth_24_stencil_8;
            table[ "depth_32_stencil_8" ] = kege::Format::depth_32_stencil_8;
            table[ "stencil_u8" ] = kege::Format::stencil_u8;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        KEGE_LOG_ERROR << "invalid format";
        return {};
    }
}
