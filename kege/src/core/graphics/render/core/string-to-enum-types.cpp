//
//  string-to-enum-types.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/3/25.
//

#include "string-to-enum-types.hpp"

namespace kege{
    
    ShaderVarType stringToShaderVarType( const std::string& str )
    {
        static std::map< std::string, kege::ShaderVarType > table;
        if( table.empty() )
        {
            table[ "sint" ] = kege::ShaderVarType::Sint;
            table[ "uint" ] = kege::ShaderVarType::Uint;
            table[ "float" ] = kege::ShaderVarType::Float;
            table[ "double" ] = kege::ShaderVarType::Double;
            table[ "vec2" ] = kege::ShaderVarType::Vec2;
            table[ "vec3" ] = kege::ShaderVarType::Vec3;
            table[ "vec4" ] = kege::ShaderVarType::Vec4;
            table[ "dvec2" ] = kege::ShaderVarType::Vec2D;
            table[ "dvec3" ] = kege::ShaderVarType::Vec3D;
            table[ "dvec4" ] = kege::ShaderVarType::Vec4D;
            table[ "ivec2" ] = kege::ShaderVarType::Vec2I;
            table[ "ivec3" ] = kege::ShaderVarType::Vec3I;
            table[ "ivec4" ] = kege::ShaderVarType::Vec4I;
            table[ "uvec2" ] = kege::ShaderVarType::Vec2U;
            table[ "uvec3" ] = kege::ShaderVarType::Vec3U;
            table[ "uvec4" ] = kege::ShaderVarType::Vec4U;
        }
        auto m = table.find( str );
        if ( m != table.end() )
        {
            return m->second;
        }
        return kege::ShaderVarType::Unknown;
    }

//    UsageTarget stringToUsageTarget( const char* str )
//    {
//        static std::map< std::string, kege::UsageTarget > table;
//        if( table.empty() )
//        {
//            table[ "Object" ] = kege::UsageTarget::Object;
//            table[ "Material" ] = kege::UsageTarget::Material;
//            table[ "Camera" ] = kege::UsageTarget::Camera;
//            table[ "Light" ] = kege::UsageTarget::Light;
//            table[ "Mesh" ] = kege::UsageTarget::Mesh;
//        }
//        auto m = table.find( str );
//        if ( m != table.end() )
//        {
//            return m->second;
//        }
//        Log::error << "invalid UsageTarget -> " << str <<Log::nl;
//        return kege::UsageTarget::Camera;
//    }

//    UsageSource stringToUsageSource( const char* str )
//    {
//        static std::map< std::string, kege::UsageSource > table;
//        if( table.empty() )
//        {
//            table[ "Scale" ] = kege::UsageSource::Scale;
//            table[ "Rotation" ] = kege::UsageSource::Rotation;
//            table[ "Translation" ] = kege::UsageSource::Translation;
//            table[ "ViewTransform" ] = kege::UsageSource::ViewTransform;
//            table[ "ViewPerspective" ] = kege::UsageSource::ViewPerspective;
//            table[ "ViewOrthographics" ] = kege::UsageSource::ViewOrthographics;
//            table[ "ObjectTransform" ] = kege::UsageSource::ObjectTransform;
//            table[ "Lights" ] = kege::UsageSource::Lights;
//            table[ "Material" ] = kege::UsageSource::Material;
//            table[ "MaterialTextures" ] = kege::UsageSource::MaterialTextures;
//            table[ "MaterialParameters" ] = kege::UsageSource::MaterialParameters;
//            table[ "InstanceBufferList" ] = kege::UsageSource::InstanceBufferList;
//            table[ "IndirectDrawBufferList" ] = kege::UsageSource::IndirectDrawBufferList;
//            table[ "ShadowMap" ] = kege::UsageSource::ShadowMap;
//            table[ "EVMap" ] = kege::UsageSource::EVMap;
//        }
//        auto m = table.find( str );
//        if ( m != table.end() )
//        {
//            return m->second;
//        }
//        Log::error << "invalid UsageSource -> " << str <<Log::nl;
//        return kege::UsageSource::Scale;
//    }

    kege::CompareOp stringToCompareOp( const std::string& type )
    {
        static std::map< std::string, kege::CompareOp > table;
        if( table.empty() )
        {
            table[ "never" ] = kege::CompareOp::Never;
            table[ "less" ] = kege::CompareOp::Less;
            table[ "equal" ] = kege::CompareOp::Equal;
            table[ "less_equal" ] = kege::CompareOp::LessEqual;
            table[ "greater" ] = kege::CompareOp::Greater;
            table[ "not_equal" ] = kege::CompareOp::NotEqual;
            table[ "greater_equal" ] = kege::CompareOp::GreaterEqual;
            table[ "always" ] = kege::CompareOp::Always;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        kege::Log::error << "invalid CompareOp -> " << type <<Log::nl;
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
            table[ "increment_and_clamp" ] = kege::StencilOp::IncrementAndClamp;
            table[ "decrement_and_clamp" ] = kege::StencilOp::DecrementAndClamp;
            table[ "invert" ] = kege::StencilOp::Invert;
            table[ "increment_and_wrap" ] = kege::StencilOp::IncrementAndWrap;
            table[ "decrement_and_wrap" ] = kege::StencilOp::DecrementAndWrap;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        kege::Log::error << "invalid StencilOp -> " << type <<Log::nl;
        return kege::StencilOp::Zero;
    }

    ColorBlendLogicOp stringToColorBlendLogicOp( const std::string& type )
    {
        static std::map< std::string, kege::ColorBlendLogicOp > table;
        if( table.empty() )
        {
            table[ "clear" ] = kege::ColorBlendLogicOp::Clear;
            table[ "and" ] = kege::ColorBlendLogicOp::And;
            table[ "and_reverse" ] = kege::ColorBlendLogicOp::AndReverse;
            table[ "copy" ] = kege::ColorBlendLogicOp::Copy;
            table[ "and_inverted" ] = kege::ColorBlendLogicOp::AndInverted;
            table[ "no_op" ] = kege::ColorBlendLogicOp::NoOp;
            table[ "xor" ] = kege::ColorBlendLogicOp::Xor;
            table[ "or" ] = kege::ColorBlendLogicOp::Or;
            table[ "Nor" ] = kege::ColorBlendLogicOp::Nor;
            table[ "equiv" ] = kege::ColorBlendLogicOp::Equiv;
            table[ "invert" ] = kege::ColorBlendLogicOp::Invert;
            table[ "or_reverse" ] = kege::ColorBlendLogicOp::OrReverse;
            table[ "copy_inverted" ] = kege::ColorBlendLogicOp::CopyInverted;
            table[ "or_inverted" ] = kege::ColorBlendLogicOp::OrInverted;
            table[ "nand" ] = kege::ColorBlendLogicOp::Nand;
            table[ "set" ] = kege::ColorBlendLogicOp::Set;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        kege::Log::error << "invalid ColorBlendLogicOp -> " << type <<Log::nl;
        return kege::ColorBlendLogicOp::Clear;
    }

    kege::BlendFactor stringToBlendFactor( const std::string& type )
    {
        static std::map< std::string, kege::BlendFactor > table;
        if( table.empty() )
        {
            table[ "zero" ] = kege::BlendFactor::Zero;
            table[ "one" ] = kege::BlendFactor::One;
            table[ "src_color" ] = kege::BlendFactor::SrcColor;
            table[ "src_alpha" ] = kege::BlendFactor::SrcAlpha;
            table[ "dst_color" ] = kege::BlendFactor::DstColor;
            table[ "dst_alpha" ] = kege::BlendFactor::DstAlpha;
            table[ "src1_color" ] = kege::BlendFactor::Src1Color;
            table[ "src1_alpha" ] = kege::BlendFactor::Src1Alpha;
            table[ "constant_color" ] = kege::BlendFactor::ConstantColor;
            table[ "constant_alpha" ] = kege::BlendFactor::ConstantAlpha;
            table[ "one_minus_src_color" ] = kege::BlendFactor::OneMinusSrcColor;
            table[ "one_minus_dst_color" ] = kege::BlendFactor::OneMinusDstColor;
            table[ "one_minus_src_alpha" ] = kege::BlendFactor::OneMinusSrcAlpha;
            table[ "one_minus_constant_color" ] = kege::BlendFactor::OneMinusConstantColor;
            table[ "one_minus_constant_alpha" ] = kege::BlendFactor::OneMinusConstantAlpha;
            table[ "one_minus_src1_alpha" ] = kege::BlendFactor::OneMinusSrc1Alpha;
            table[ "src_alpha_saturate" ] = kege::BlendFactor::SrcAlphaSaturate;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        kege::Log::error << "invalid BlendFactor -> " << type <<Log::nl;
        return kege::BlendFactor::Zero;
    }

    kege::BlendOp stringToBlendOp( const std::string& type )
    {
        static std::map< std::string, kege::BlendOp > table;
        if( table.empty() )
        {
            table[ "add" ] = kege::BlendOp::Add;
            table[ "subtract" ] = kege::BlendOp::Subtract;
            table[ "reverse_subtract" ] = kege::BlendOp::ReverseSubtract;
            table[ "min" ] = kege::BlendOp::Min;
            table[ "max" ] = kege::BlendOp::Max;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        kege::Log::error << "invalid BlendOp -> " << type <<Log::nl;
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
            table[ "cull_none" ] = kege::CullMode::None;
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
        kege::Log::error << "invalid DescriptorType -> " << type <<" in convertDescriptorType()" <<Log::nl;
        return kege::DescriptorType::Invalid;
    }

    kege::ShaderStageFlag convertShaderStage( const std::string& type )
    {
        static std::map< std::string, kege::ShaderStageFlag > table;
        if( table.empty() )
        {
            table[ "vertex"          ] = kege::ShaderStageFlag::Vertex;
            table[ "fragment"        ] = kege::ShaderStageFlag::Fragment;
            table[ "tess-control"    ] = kege::ShaderStageFlag::TessellationControl;
            table[ "tess-evaliation" ] = kege::ShaderStageFlag::TessellationEvaluation;
            table[ "geometry"        ] = kege::ShaderStageFlag::Geometry;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        return kege::ShaderStageFlag::Invalid;
    }

    kege::ShaderVarType convertVertexInputType( const std::string& type )
    {
        static std::map< std::string, kege::ShaderVarType > table;
        if( table.empty() )
        {
            table[ "int" ] = kege::ShaderVarType::Sint;
            table[ "uint" ] = kege::ShaderVarType::Uint;
            table[ "float" ] = kege::ShaderVarType::Float;
            table[ "double" ] = kege::ShaderVarType::Double;

            table[ "vec2"  ] = kege::ShaderVarType::Vec2;
            table[ "dvec2" ] = kege::ShaderVarType::Vec2D;
            table[ "ivec2" ] = kege::ShaderVarType::Vec2I;
            table[ "uvec2" ] = kege::ShaderVarType::Vec2U;

            table[ "vec3"  ] = kege::ShaderVarType::Vec3;
            table[ "dvec3" ] = kege::ShaderVarType::Vec3D;
            table[ "ivec3" ] = kege::ShaderVarType::Vec3I;
            table[ "uvec3" ] = kege::ShaderVarType::Vec3U;

            table[ "vec4"  ] = kege::ShaderVarType::Vec4;
            table[ "dvec4" ] = kege::ShaderVarType::Vec4D;
            table[ "ivec4" ] = kege::ShaderVarType::Vec4I;
            table[ "uvec4" ] = kege::ShaderVarType::Vec4U;
        }
        auto m = table.find( type );
        if ( m != table.end() )
        {
            return m->second;
        }
        kege::Log::error << "invalid vertex input type";
        return {};
    }


    kege::Filter stringToFilter( const std::string& name )
    {
        if ( name == "Nearest" ) return kege::Filter::Nearest;
        if ( name == "Linear" ) return kege::Filter::Linear;
        kege::Log::error << "unsupported Filter -> " <<name <<kege::Log::nl;
        return kege::Filter::Linear;
    }

    kege::MipmapMode stringToMipmapMode( const std::string& name )
    {
        if ( name == "Nearest" ) return kege::MipmapMode::Nearest;
        if ( name == "Linear" ) return kege::MipmapMode::Linear;
        kege::Log::error << "unsupported MipmapMode -> " <<name <<kege::Log::nl;
        return kege::MipmapMode::Linear;
    }

    ImageType stringToImageType( const std::string& name )
    {
        static std::map< std::string, ImageType > types;
        if ( types.empty() )
        {
            types[ "Type1D" ] = ImageType::Type1D;
            types[ "Type1DArray" ] = ImageType::Type1DArray;
            types[ "Type2D" ] = ImageType::Type2D;
            types[ "Type2DArray" ] = ImageType::Type2DArray;
            types[ "TypeCube" ] = ImageType::TypeCube;
            types[ "TypeCubeArray" ] = ImageType::TypeCubeArray;
            types[ "Type3D" ] = ImageType::Type3D;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported ImageType -> " <<name <<kege::Log::nl;
        return ImageType::Type2D;
    }

    BufferUsages stringToBufferUsage( const std::string& name )
    {
        static std::map< std::string, BufferUsages > types;
        if ( types.empty() )
        {
            types[ "None" ] = BufferUsages::None;
            types[ "CopySrc" ] = BufferUsages::CopySrc;
            types[ "CopyDst" ] = BufferUsages::CopyDst;
            types[ "VertexBuffer" ] = BufferUsages::VertexBuffer;
            types[ "IndexBuffer" ] = BufferUsages::IndexBuffer;
            types[ "UniformBuffer" ] = BufferUsages::UniformBuffer;
            types[ "StorageBuffer" ] = BufferUsages::StorageBuffer;
            types[ "IndirectBuffer" ] = BufferUsages::IndirectBuffer;
            types[ "UniformTexelBuffer" ] = BufferUsages::UniformTexelBuffer;
            types[ "StorageTexelBuffer" ] = BufferUsages::StorageTexelBuffer;
            types[ "ShaderDeviceAddress" ] = BufferUsages::ShaderDeviceAddress;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported BufferUsages -> " <<name <<kege::Log::nl;
        return {};
    }

    MemoryUsage stringToMemoryUsage( const std::string& name )
    {
        static std::map< std::string, MemoryUsage > types;
        if ( types.empty() )
        {
            types[ "GpuOnly" ] = MemoryUsage::GpuOnly;
            types[ "CpuToGpu" ] = MemoryUsage::CpuToGpu;
            types[ "GpuToCpu" ] = MemoryUsage::GpuToCpu;
            types[ "CpuOnly" ] = MemoryUsage::CpuOnly;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported MemoryUsage -> " <<name <<kege::Log::nl;
        return {};
    }

    ImageUsage stringToImageUsage( const std::string& name )
    {
        static std::map< std::string, ImageUsage > types;
        if ( types.empty() )
        {
            types[ "None" ] = ImageUsage::Undefined;
            types[ "CopySrc" ] = ImageUsage::TransferSrc;
            types[ "CopyDst" ] = ImageUsage::TransferDst;
            types[ "ShaderRead" ] = ImageUsage::Sampled;
            types[ "Storage" ] = ImageUsage::Storage;
            types[ "Color" ] = ImageUsage::Color;
            types[ "Depth" ] = ImageUsage::DepthStencil;
            types[ "DepthStencil" ] = ImageUsage::DepthStencil;
            types[ "TransientAttachment" ] = ImageUsage::Transient;
            types[ "InputAttachment" ] = ImageUsage::Input;
            types[ "Present" ] = ImageUsage::Present;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported ImageUsage -> " <<name <<kege::Log::nl;
        return ImageUsage::Undefined;
    }

    kege::BindingUsage stringToBindingUsage( const std::string& name )
    {
        static std::map< std::string, kege::BindingUsage > types;

        if ( types.empty() )
        {
            types[ "Invalid" ] = kege::BindingUsage::Invalid;
            types[ "Sampler" ] = kege::BindingUsage::Sampler;
            types[ "CombinedImageSampler" ] = kege::BindingUsage::CombinedImageSampler;
            types[ "SampledImage" ] = kege::BindingUsage::SampledImage;
            types[ "StorageImage" ] = kege::BindingUsage::StorageImage;
            types[ "UniformTexelBuffer" ] = kege::BindingUsage::UniformTexelBuffer;
            types[ "StorageTexelBuffer" ] = kege::BindingUsage::StorageTexelBuffer;
            types[ "UniformBuffer" ] = kege::BindingUsage::UniformBuffer;
            types[ "StorageBuffer" ] = kege::BindingUsage::StorageBuffer;
            types[ "UniformBufferDynamic" ] = kege::BindingUsage::UniformBufferDynamic;
            types[ "StorageBufferDynamic" ] = kege::BindingUsage::StorageBufferDynamic;
            types[ "InputAttachment" ] = kege::BindingUsage::InputAttachment;

            types[ "sampler" ] = kege::BindingUsage::Sampler;
            types[ "combined_image_sampler" ] = kege::BindingUsage::CombinedImageSampler;
            types[ "sampled_image" ] = kege::BindingUsage::SampledImage;
            types[ "storage_image" ] = kege::BindingUsage::StorageImage;
            types[ "uniform_texel_buffer" ] = kege::BindingUsage::UniformTexelBuffer;
            types[ "storage_texel_buffer" ] = kege::BindingUsage::StorageTexelBuffer;
            types[ "uniform_buffer" ] = kege::BindingUsage::UniformBuffer;
            types[ "storage_buffer" ] = kege::BindingUsage::StorageBuffer;
            types[ "uniform_buffer_dynamic" ] = kege::BindingUsage::UniformBufferDynamic;
            types[ "storage_buffer_dynamic" ] = kege::BindingUsage::StorageBufferDynamic;
            types[ "input_attachment" ] = kege::BindingUsage::InputAttachment;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported BindingUsage -> " <<name <<kege::Log::nl;
        return {};
    }

    ShaderStageFlag stringToShaderStageFlag( const std::string& name )
    {
        static std::map< std::string, ShaderStageFlag > types;

        if ( types.empty() )
        {
            types[ "Invalid" ] = ShaderStageFlag::Invalid;
            types[ "vertex" ] = ShaderStageFlag::Vertex;
            types[ "fragment" ] = ShaderStageFlag::Fragment;
            types[ "compute" ] = ShaderStageFlag::Compute;
            types[ "geometry" ] = ShaderStageFlag::Geometry;
            types[ "tessellation-control" ] = ShaderStageFlag::TessellationControl;
            types[ "tessellation-evaluation" ] = ShaderStageFlag::TessellationEvaluation;
            types[ "all-graphics" ] = ShaderStageFlag::AllGraphics;
            types[ "all" ] = ShaderStageFlag::All;
            types[ "vs" ] = ShaderStageFlag::Vertex;
            types[ "fs" ] = ShaderStageFlag::Fragment;
            types[ "cs" ] = ShaderStageFlag::Compute;
            types[ "gs" ] = ShaderStageFlag::Geometry;
            types[ "tcs" ] = ShaderStageFlag::TessellationControl;
            types[ "tes" ] = ShaderStageFlag::TessellationEvaluation;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported ShaderStageFlag -> " <<name <<kege::Log::nl;
        return {};
    }

    kege::PipelineType stringToPipelineType( const std::string& name )
    {
        static std::map< std::string, kege::PipelineType > types;

        if ( types.empty() )
        {
            types[ "Graphics" ] = kege::PipelineType::Graphics;
            types[ "Compute" ] = kege::PipelineType::Graphics;
            types[ "RayTracing" ] = kege::PipelineType::Graphics;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported PipelineType -> " <<name <<kege::Log::nl;
        return kege::PipelineType::Graphics;
    }

    kege::AddressMode stringToAddressMode( const std::string& name )
    {
        static std::map< std::string, AddressMode > types;

        if ( types.empty() )
        {
            types[ "Repeat" ] = AddressMode::Repeat;
            types[ "MirroredRepeat" ] = AddressMode::MirroredRepeat;
            types[ "ClampToEdge" ] = AddressMode::ClampToEdge;
            types[ "ClampToBorder" ] = AddressMode::ClampToBorder;
            types[ "MirrorClampToEdge" ] = AddressMode::MirrorClampToEdge;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported AddressMode -> " <<name <<kege::Log::nl;
        return AddressMode::Repeat;
    }

    QueueType stringToQueueType( const std::string& name )
    {
        if ( name == "GRAPHICS" ) return QueueType::Graphics;
        else if ( name == "COMPUTE" ) return QueueType::Compute;
        return QueueType::Invalid;
    }

    ImageLayout stringToImageLayout( const std::string& name )
    {
        static std::map< std::string, ImageLayout > types;
        if ( types.empty() )
        {
            types[ "General" ] = ImageLayout::General;
            types[ "Color" ] = ImageLayout::Color;
            types[ "Depth" ] = ImageLayout::Depth;
            types[ "DepthRead" ] = ImageLayout::DepthRead;
            types[ "Stencil" ] = ImageLayout::Stencil;
            types[ "StencilRead" ] = ImageLayout::StencilRead;
            types[ "DepthStencil" ] = ImageLayout::DepthStencil;
            types[ "DepthStencilRead" ] = ImageLayout::DepthStencilRead;
            types[ "DepthRead_Stencil" ] = ImageLayout::DepthRead_Stencil;
            types[ "Depth_StencilRead" ] = ImageLayout::Depth_StencilRead;
            types[ "ShaderRead" ] = ImageLayout::ShaderRead;
            types[ "TransferSrc" ] = ImageLayout::TransferSrc;
            types[ "TransferDst" ] = ImageLayout::TransferDst;
            types[ "PreInitialized" ] = ImageLayout::PreInitialized;
            types[ "Present" ] = ImageLayout::Present;
            types[ "HostRead" ] = ImageLayout::HostRead;
            types[ "HostWrite" ] = ImageLayout::HostWrite;
            types[ "Undefined" ] = ImageLayout::Undefined;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported ImageLayout -> " <<name <<kege::Log::nl;
        return ImageLayout::Undefined;
    }

    AccessFlags stringToAccessFlags( const std::string& name )
    {
        static std::map< std::string, AccessFlags > types;
        if ( types.empty() )
        {
            types[ "None" ] = AccessFlags::None;
            types[ "IndirectCommandRead" ] = AccessFlags::IndirectCommandRead;
            types[ "IndexRead" ] = AccessFlags::IndexRead;
            types[ "InputRead" ] = AccessFlags::InputRead;
            types[ "VertexBufferRead" ] = AccessFlags::VertexBufferRead;
            types[ "UniformRead" ] = AccessFlags::UniformRead;
            types[ "ShaderRead" ] = AccessFlags::ShaderRead;
            types[ "ShaderWrite" ] = AccessFlags::ShaderWrite;
            types[ "ColorRead" ] = AccessFlags::ColorRead;
            types[ "ColorWrite" ] = AccessFlags::ColorWrite;
            types[ "DepthStencilRead" ] = AccessFlags::DepthStencilRead;
            types[ "DepthStencilWrite" ] = AccessFlags::DepthStencilWrite;
            types[ "TransferRead" ] = AccessFlags::TransferRead;
            types[ "TransferWrite" ] = AccessFlags::TransferWrite;
            types[ "HostRead" ] = AccessFlags::HostRead;
            types[ "HostWrite" ] = AccessFlags::HostWrite;
            types[ "MemoryRead" ] = AccessFlags::MemoryRead;
            types[ "MemoryWrite" ] = AccessFlags::MemoryWrite;
            types[ "All" ] = AccessFlags::HostWrite;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported AccessFlags -> " <<name <<kege::Log::nl;
        return {};
    }

    PipelineStageFlag stringToPipelineStageFlags( const std::string& name )
    {
        static std::map< std::string, PipelineStageFlag > types;
        if ( types.empty() )
        {
            types[ "None" ] = PipelineStageFlag::None;
            types[ "TopOfPipe" ] = PipelineStageFlag::TopOfPipe;
            types[ "DrawIndirect" ] = PipelineStageFlag::DrawIndirect;
            types[ "VertexInput" ] = PipelineStageFlag::VertexInput;
            types[ "VertexShader" ] = PipelineStageFlag::VertexShader;
            types[ "TessellationControlShader" ] = PipelineStageFlag::TessellationControlShader;
            types[ "TessellationEvaluationShader" ] = PipelineStageFlag::TessellationEvaluationShader;
            types[ "GeometryShader" ] = PipelineStageFlag::GeometryShader;
            types[ "FragmentShader" ] = PipelineStageFlag::FragmentShader;
            types[ "EarlyFragmentTests" ] = PipelineStageFlag::EarlyFragmentTests;
            types[ "LateFragmentTests" ] = PipelineStageFlag::LateFragmentTests;
            types[ "ColorOutput" ] = PipelineStageFlag::ColorOutput;

            types[ "ComputeShader" ] = PipelineStageFlag::ComputeShader;
            types[ "Transfer" ] = PipelineStageFlag::Transfer;
            types[ "Host" ] = PipelineStageFlag::Host;
            types[ "RayTrace" ] = PipelineStageFlag::RayTrace;
            types[ "BottomOfPipe" ] = PipelineStageFlag::BottomOfPipe;
            types[ "AllCommands" ] = PipelineStageFlag::AllCommands;
            types[ "AllGraphics" ] = PipelineStageFlag::AllGraphics;
            types[ "AllTransfer" ] = PipelineStageFlag::AllTransfer;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported PipelineStageFlag -> " <<name <<kege::Log::nl;
        return {};
    }

    AttachmentLoadOp stringToAttachmentLoadOp( const std::string& name )
    {
        if ( name == "Load" ) return AttachmentLoadOp::Load;
        if ( name == "Clear" ) return AttachmentLoadOp::Clear;
        if ( name == "DontCare" ) return AttachmentLoadOp::DontCare;
        kege::Log::error << "unsupported AttachmentLoadOp -> " <<name <<kege::Log::nl;
        return AttachmentLoadOp::Clear;
    }

    Format stringToFormat( const std::string& name )
    {
        static std::map< std::string, Format > types;
        if ( types.empty() )
        {
            types[ "r_s8" ] = Format::r_s8;
            types[ "rg_s8" ] = Format::rg_s8;
            types[ "rgb_s8" ] = Format::rgb_s8;
            types[ "rgba_s8" ] = Format::rgba_s8;

            types[ "r_u8" ] = Format::r_u8;
            types[ "rg_u8" ] = Format::rg_u8;
            types[ "rgb_u8" ] = Format::rgb_u8;
            types[ "rgba_u8" ] = Format::rgba_u8;

            types[ "r_s16" ] = Format::r_s16;
            types[ "rg_s16" ] = Format::rg_s16;
            types[ "rgb_s16" ] = Format::rgb_s16;
            types[ "rgba_s16" ] = Format::rgba_s16;

            types[ "r_u16" ] = Format::r_u16;
            types[ "rg_u16" ] = Format::rg_u16;
            types[ "rgb_u16" ] = Format::rgb_u16;
            types[ "rgba_u16" ] = Format::rgba_u16;

            types[ "r_s32" ] = Format::r_s32;
            types[ "rg_s32" ] = Format::rg_s32;
            types[ "rgb_s32" ] = Format::rgb_s32;
            types[ "rgba_s23" ] = Format::rgba_s32;

            types[ "r_u32" ] = Format::r_u32;
            types[ "rg_u32" ] = Format::rg_u32;
            types[ "rgb_u32" ] = Format::rgb_u32;
            types[ "rgba_u32" ] = Format::rgba_u32;

            types[ "r_s64" ] = Format::r_s64;
            types[ "rg_s64" ] = Format::rg_s64;
            types[ "rgb_s46" ] = Format::rgb_s64;
            types[ "rgba_s64" ] = Format::rgba_s64;

            types[ "r_u64" ] = Format::r_u64;
            types[ "rg_u64" ] = Format::rg_u64;
            types[ "rgb_u46" ] = Format::rgb_u64;
            types[ "rgba_u64" ] = Format::rgba_u64;


            types[ "r_s8_norm" ] = Format::r_s8_norm;
            types[ "rg_s8_norm" ] = Format::rg_s8_norm;
            types[ "rgb_s8_norm" ] = Format::rgb_s8_norm;
            types[ "rgba_s8_norm" ] = Format::rgba_s8_norm;

            types[ "r_u8_norm" ] = Format::r_u8_norm;
            types[ "rg_u8_norm" ] = Format::rg_u8_norm;
            types[ "rgb_u8_norm" ] = Format::rgb_u8_norm;
            types[ "rgba_u8_norm" ] = Format::rgba_u8_norm;


            types[ "r_8_srgb" ] = Format::r_8_srgb;
            types[ "rg_8_srgb" ] = Format::rg_8_srgb;
            types[ "rgb_8_srgb" ] = Format::rgb_8_srgb;
            types[ "rgba_8_srgb" ] = Format::rgba_8_srgb;

            types[ "bgr_8_srgb" ] = Format::rgba_8_srgb;
            types[ "bgra_8_srgb" ] = Format::rgba_8_srgb;


            types[ "bgr_u8" ] = Format::bgr_u8;
            types[ "bgr_s8" ] = Format::bgr_s8;
            types[ "bgr_s8_norm" ] = Format::bgr_s8_norm;
            types[ "bgr_u8_norm" ] = Format::bgr_u8_norm;
            types[ "bgra_u8" ] = Format::bgra_u8;
            types[ "bgra_s8" ] = Format::bgra_s8;
            types[ "bgra_s8_norm" ] = Format::bgra_s8_norm;
            types[ "bgra_u8_norm" ] = Format::bgra_u8_norm;
            types[ "bgra_u8_norm_srbg" ] = Format::bgra_u8_norm_srbg;

            types[ "depth_16" ] = kege::Format::depth_16;
            types[ "depth_32" ] = kege::Format::depth_32;
            types[ "depth_16_stencil_8" ] = kege::Format::depth_16_stencil_8;
            types[ "depth_24_stencil_8" ] = kege::Format::depth_24_stencil_8;
            types[ "depth_32_stencil_8" ] = kege::Format::depth_32_stencil_8;
            types[ "stencil_u8" ] = kege::Format::stencil_u8;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported Format -> " <<name <<kege::Log::nl;
        return {};
    }
}
