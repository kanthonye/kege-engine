//
//  json-to-shader.cpp
//  editor
//
//  Created by Kenneth Esdaile on 2/16/26.
//

#include "json-shader-loader.hpp"

namespace kege{

    Format cstrToFormat( const char* name )
    {
        return stringToFormat( name );
    }

    kege::VertexBit stringToVertexBit( const std::string& name )
    {
        static std::map< std::string, kege::VertexBit > types;

        if ( types.empty() )
        {
            types[ "POSITION"  ] = kege::VertexBit::POSITION;
            types[ "NORMAL"  ] = kege::VertexBit::NORMAL;
            types[ "TEXCOORD"  ] = kege::VertexBit::TEXCOORD;
            types[ "TANGENT"  ] = kege::VertexBit::TANGENT;
            types[ "BITANGENT" ] = kege::VertexBit::BITANGENT;
            types[ "WEIGHTS" ] = kege::VertexBit::WEIGHTS;
            types[ "JOINTS" ] = kege::VertexBit::JOINTS;
            types[ "COLOR" ] = kege::VertexBit::COLOR;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported VertexBit -> " <<name <<kege::Log::nl;
        return VertexBit::POSITION;
    }

    SampleCount stringToSampleCount( const std::string& name )
    {
        static std::map< std::string, SampleCount > types;

        if ( types.empty() )
        {
            types[ "Count1"  ] = SampleCount::Count1;
            types[ "Count2"  ] = SampleCount::Count2;
            types[ "Count4"  ] = SampleCount::Count4;
            types[ "Count8"  ] = SampleCount::Count8;
            types[ "Count16" ] = SampleCount::Count16;
            types[ "Count32" ] = SampleCount::Count32;
            types[ "Count64" ] = SampleCount::Count64;
            types[ "1"  ] = SampleCount::Count1;
            types[ "2"  ] = SampleCount::Count2;
            types[ "4"  ] = SampleCount::Count4;
            types[ "8"  ] = SampleCount::Count8;
            types[ "16" ] = SampleCount::Count16;
            types[ "32" ] = SampleCount::Count32;
            types[ "64" ] = SampleCount::Count64;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported SampleCount -> " <<name <<kege::Log::nl;
        return SampleCount::Count1;
    }

    kege::ShaderStageFlag parseShaderStages( kege::Json json )
    {
        kege::ShaderStageFlag stages = kege::ShaderStageFlag::Invalid;
        for (int i = 0; i < json.count(); ++i)
        {
            stages |= kege::stringToShaderStageFlag( json[i].toStr() );
        }
        return stages;
    }

    kege::FaceOperations parseStencilOpState( const kege::Json& json )
    {
        return kege::FaceOperations
        {
            .fail_op = kege::stringToStencilOp( json[ "fail_op" ].toStr( "keep" ) ),
            .depth_fail_op = kege::stringToStencilOp( json[ "depth_fail_op" ].toStr( "keep" ) ),
            .compare_op = kege::stringToCompareOp( json[ "compare_op" ].toStr( "always" ) ),
            .compare_mask = (uint32_t)json[ "compare_mask" ].toInt( 0xFFFFFFFF ),
            .write_mask = (uint32_t)json[ "write_mask" ].toInt( 0xFFFFFFFF ),
        };
    }

    kege::DepthStencil parseDepthStencilStates( const kege::Json& json )
    {
        return kege::DepthStencil
        {
            .depth.enable = json[ "depth_test" ].toBool( false ),
            .depth.write = json[ "depth_write" ].toBool( true ),
            .depth.compare_op = kege::stringToCompareOp( json[ "depth_compare_op" ].toStr( "less" ) ),
            .stencil.enable = json[ "stencil_test_enable" ].toBool( false ),
            .stencil.front = parseStencilOpState( json[ "front_op" ] ),
            .stencil.back = parseStencilOpState( json[ "back_op" ] ),
            .stencil.reference = (uint32_t)json[ "reference_value" ].toInt( 0 ),
        };
    }

    kege::BlendState getColorBlendAttachmentState( const kege::Json& json )
    {
        return kege::BlendState
        {
            .src_color_blend = kege::stringToBlendFactor( json[ "src_color_blend" ].toStr( "one" ) ),
            .dst_color_blend = kege::stringToBlendFactor( json[ "dst_color_blend" ].toStr( "zero" ) ),
            .color_blend_op = kege::stringToBlendOp( json[ "color_blend_op" ].toStr( "add" ) ),

            .src_alpha_blend = kege::stringToBlendFactor( json[ "src_alpha_blend" ].toStr( "one" ) ),
            .dst_alpha_blend = kege::stringToBlendFactor( json[ "dst_alpha_blend" ].toStr( "zero" ) ),
            .alpha_blend_op = kege::stringToBlendOp( json[ "alpha_blend_op" ].toStr( "add" ) ),

            .enable = json[ "blend_enable" ].toBool( false ),
            .color_write_mask = kege::ColorWriteMask::All,
        };
    }

    kege::ColorBlendState parseColorBlendStates( const kege::Json& json )
    {
        return kege::ColorBlendState
        {
            .logic_op_enable = json[ "logic_op_enable" ].toBool( false ),
            .logic_op = kege::stringToColorBlendLogicOp( json[ "logic_op" ].toStr( "copy") ),
            .attachments = json[ "color_blend_attachments" ].toVector( getColorBlendAttachmentState )
        };
    }

    kege::RasterizerState parseRasterizationState( const kege::Json& json )
    {
        return kege::RasterizerState
        {
            .fill_mode = json["polygon_mode"]( kege::stringToPolygonMode, kege::FillMode::Fill ),
            .cull_mode = json["cull_mode"]( kege::stringToCullMode, kege::CullMode::None ),
            .front_face = json["front_face"]( kege::stringToFrontFace, kege::FrontFace::CounterClockwise ),
            .depth_bias.enable = json.getBool("depth_bias_enable"),
            .depth_bias.constant_factor = json.getFloat("depth_bias_constant_factor", 0.f),
            .depth_bias.slope_factor = json.getFloat("depth_bias_slope_factor", 0.f),
            .depth_bias.clamp = json.getFloat("depth_bias_clamp", 0.f),
            .depth_clip_enable = json.getBool("depth_clip_enable", true),
            .scissor_enable = json.getBool("scissor_enable", false),
            .multisample_enable = json.getBool("multisample_enable", true),
            .line_width = json.getFloat("line_width", 1.0f),
        };
    }

    kege::PipelineRendering parsePipelineOutput( const kege::Json& json )
    {
        return kege::PipelineRendering
        {
            .color_attachment_formats = json[ "color_attachments" ].toVector( cstrToFormat ),
            .depth_attachment_format = json["depth_attachment"](stringToFormat, Format::depth_32),
            .stencil_attachment_format = json["stencil_attachment"]( stringToFormat, Format::undefined ),
            .sample_count = json["sample_count"](stringToSampleCount, SampleCount::Count1),
        };
    }

    kege::JsonShaderContent::Attribute parseVertexInput( const kege::Json& json )
    {
        return kege::JsonShaderContent::Attribute
        {
            .name = json["name"].toStr(),
            .location = json["location"].toInt(),
            .binding = json["binding"].toInt(),
            .semantic = stringToVertexBit( json.getStr("semantic") ),
            .format = kege::convertVertexInputType( json[ "type" ].toStr() ),
        };
    }

    kege::JsonShaderContent::AttributeBinding parseVertexBinding( const kege::Json& json )
    {
        return kege::JsonShaderContent::AttributeBinding
        {
            .stride = json["stride"].toInt(),
            .input_rate = stringToVertexInputRate( json.getStr("input_rate") )
        };
    }

    kege::JsonShaderContent::VertexAttribute parseVertexAttribute( const kege::Json& json )
    {
        return kege::JsonShaderContent::VertexAttribute
        {
            .attributes = json[ "vertex_inputs" ].toVector( parseVertexInput ),
            .bindings = json[ "vertex_bindings" ].toVector( parseVertexBinding ),
        };
    }

    kege::JsonShaderContent::Shader parseShaders( const kege::Json& json )
    {
        return kege::JsonShaderContent::Shader
        {
            .uri = json.getStr("uri"),
            .name = json.getStr("name"),
            .entry_point = json.getStr("entry-point"),
            .type = kege::stringToShaderStageFlag( json[ "type" ].toStr() )
        };
    }

    kege::JsonShaderContent::Field parseField( const kege::Json& json )
    {
        return kege::JsonShaderContent::Field
        {
            .count = json.getInt( "count" ),
            .name = json.getStr( "name" ),
            .type = json.getStr( "type" )
        };
    }

    kege::JsonShaderContent::Struct parseStruct( const kege::Json& json )
    {
        return kege::JsonShaderContent::Struct
        {
            .name = json.getStr( "name" ),
            .fields = json["fields"].toVector( parseField )
        };
    }

    kege::JsonShaderContent::PushBlockDesc parsePushBlock( const kege::Json& json )
    {
        return kege::JsonShaderContent::PushBlockDesc
        {
            .id = json["id"].toStr(),
            .name = json["name"].toStr(),
            .size = json["size"].toInt(),
            .count = json["count"].toInt(),
            .offset = json["offset"].toInt(),
            .fields = json[ "fields" ].toVector( parseField ),
            .stages = kege::parseShaderStages( json[ "stages" ] ),
        };
    }


    kege::JsonShaderContent::Set::Binding parseSetBinding( const kege::Json& json )
    {
        return kege::JsonShaderContent::Set::Binding
        {
            .id = json["id"].toStr(),
            .name = json["name"].toStr(),
            .count = json.getInt("count"),
            .index = json.getInt("index"),
            .fields = json[ "fields" ].toVector( parseField ),
            .stages = kege::parseShaderStages( json[ "stages" ] ),
            .usage = kege::stringToBindingUsage( json.getStr("usage") ),
        };
    }

    kege::JsonShaderContent::Set parseSet( const kege::Json& json )
    {
        return kege::JsonShaderContent::Set
        {
            .index = json.getInt("set"),
            .bindings = json[ "bindings" ].toVector( parseSetBinding )
        };
    }

    kege::JsonShaderContent::PipelineLayout parsePipelineLayout( const kege::Json& json )
    {
        return kege::JsonShaderContent::PipelineLayout
        {
            .blocks = json["blocks"].toVector( atoi ),
            .sets = json["sets"].toVector( atoi )
        };
    }

    kege::JsonShaderContent::Pipeline parsePipeline( const kege::Json& json )
    {
        return kege::JsonShaderContent::Pipeline
        {
            .name = json.getStr( "name" ),
            .topology = kege::stringToPrimitiveTopology( json["topology"].toStr() ),
            .type = kege::stringToPipelineType( json.getStr( "type" ) ),
            .pipeline_layout = json.getInt("pipeline_layout"),
            .rasterizer_state = json.getInt("rasterizer_state"),
            .depth_state = json.getInt("depth_state"),
            .blend_state = json.getInt("blend_state"),
            .shaders = json[ "stages" ].toVector( atoi ),
            .outputs = json[ "outputs" ].toVector( atoi )
        };
    }

    kege::JsonShaderContent JsonShaderContent::load(const std::string& filename)
    {
        std::ifstream file( filename );
        std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if ( source.empty() )
        {
            return {.error = "LOAD_ERROR: JsonShaderContent::load(): file not found ->" + filename};
        }
        Json json = kege::JsonParser::parse( source.c_str() );
        if ( !json )
        {
            return {.error = "PARSING_ERROR: JsonShaderContent::load() -> kege::JsonParser::parse(): failed."};
        }
        return parse(json);

    }
    kege::JsonShaderContent JsonShaderContent::parse(const Json& json)
    {
        return kege::JsonShaderContent
        {
            .rasterizer_states   = json[ "rasterizer_states" ].toVector( parseRasterizationState ),
            .depth_states        = json[ "depth_states" ].toVector( parseDepthStencilStates ),
            .blend_states        = json[ "blend_states" ].toVector( parseColorBlendStates ),
            .attributes          = json[ "vertex_layout" ].toVector( parseVertexAttribute ),
            .push_blocks         = json[ "blocks" ].toVector( parsePushBlock ),
            .sets                = json[ "sets" ].toVector( parseSet ),
            .shaders             = json[ "shaders" ].toVector( parseShaders ),
            .structs             = json[ "structs" ].toVector( parseStruct ),
            .pipeline_layouts    = json[ "pipeline_layouts" ].toVector( parsePipelineLayout ),
            .pipeline_outputs    = json[ "pipeline_outputs" ].toVector( parsePipelineOutput ),
            .pipelines           = json[ "pipelines" ].toVector( parsePipeline ),
        };
    }

}


namespace kege{

    kege::ref::ShaderStructBlock createShaderStructBlock
    (
        const std::string& name,
        const std::vector< JsonShaderContent::Field >& fields
    )
    {
        int offset = 0;
        std::vector< kege::ShaderStructField > f(fields.size());
        for (int i=0; i<fields.size(); ++i)
        {
            f[i].size = sizeOf( kege::stringToShaderVarType(fields[i].type) );
            f[i].count = fields[i].count;
            f[i].type = fields[i].type;
            f[i].name = fields[i].name;
            f[i].offset = offset;
            offset += f[i].size;
        }
        return new ShaderStructBlock( ShaderStructType::Buffer, name, f );
    }

    ref::ShaderPipeline JsonShaderLoader::load
    (
        const std::string parent_path,
        const JsonShaderContent content,
        int pipeline_index
    )
    {
        const auto& pipeline_meta = content.pipelines[ pipeline_index ];

        kege::ShaderPipelineDesc pipeline_description;
        pipeline_description.pipeline_type = pipeline_meta.type;
        pipeline_description.input_assembly.topology = pipeline_meta.topology;
        pipeline_description.name = pipeline_meta.name;

        // -------------------------------------------------
        // LOAD SHADERS
        // -------------------------------------------------

        for (int shader_index : pipeline_meta.shaders)
        {
            const auto& shader_meta = content.shaders[shader_index];

            std::string shader_filename = parent_path + "/" + shader_meta.uri;

            std::vector<char> source;
            loadTextFile(source, shader_filename.c_str());

            ShaderDesc desc;
            desc.entry_point = "main";
            desc.name = shader_meta.name;
            desc.stage = shader_meta.type;

            desc.byte_code = _graphics->compileGlslToSpv
            (
                shader_meta.type,
                shader_meta.name.c_str(),
                source.data(),
                source.size()
            );

            if( desc.byte_code.empty() )
            {
                kege::Log::error << "LOAD_FAILED: JsonShaderLoader::load() - > ";
                kege::Log::error << "Graphics::compileGlslToSpv(): failed to compile shader. ->";
                kege::Log::error << shader_filename << kege::Log::nl;
                return {};
            }

            pipeline_description.shaders.push_back(_graphics->createShader( desc ));
        }

        // -------------------------------------------------
        // PIPELINE LAYOUT
        // -------------------------------------------------

        if ( !content.pipeline_layouts.empty() )
        {
            const auto& pipeline_layout_meta = content.pipeline_layouts[ pipeline_meta.pipeline_layout ];
            pipeline_description.shader_layout.name = pipeline_meta.name;

            pipeline_description.shader_layout.push_blocks.resize(pipeline_layout_meta.blocks.size());
            uint32_t offset = 0;
            for (int block_index : pipeline_layout_meta.blocks)
            {
                const auto& block = content.push_blocks[ block_index ];
                auto& push_block = pipeline_description.shader_layout.push_blocks[ block_index ];

                push_block.id = block.id;
                push_block.name = block.name;
                push_block.count = block.count;
                push_block.size = block.size;
                push_block.stages = block.stages;
                push_block.offset = offset;
                push_block.block = createShaderStructBlock( block.id, block.fields );
                offset += block.size;
            }

            pipeline_description.shader_layout.bind_sets.resize(pipeline_layout_meta.sets.size());
            offset = 0;
            for (int set_index : pipeline_layout_meta.sets)
            {
                const auto& set = content.sets[ set_index ];
                auto& set_layout = pipeline_description.shader_layout.bind_sets[ set_index ];

                set_layout.index = set.index;
                set_layout.bindings.resize( set.bindings.size() );

                for (const auto& b : set.bindings)
                {
                    auto& binding = set_layout.bindings[b.index];
                    binding.index = b.index;
                    binding.count = b.count;
                    binding.name  = b.id;
                    binding.stages = b.stages;
                    binding.usage = b.usage;
                    binding.block = createShaderStructBlock( b.id, b.fields );
                    switch (b.usage) {
                        case BindingUsage::UniformBuffer:
                        case BindingUsage::UniformTexelBuffer:
                        case BindingUsage::UniformBufferDynamic:
                        case BindingUsage::StorageBuffer:
                        case BindingUsage::StorageTexelBuffer:
                        case BindingUsage::StorageBufferDynamic:
                            binding.type = BindType::Buffer;
                            break;
                        case BindingUsage::SampledImage:
                        case BindingUsage::StorageImage:
                        case BindingUsage::CombinedImageSampler:
                        case BindingUsage::InputAttachment:
                            binding.type = BindType::Image;
                            break;
                        case BindingUsage::Sampler:
                            binding.type = BindType::Invalid;
                            break;

                        default:
                            binding.type = BindType::Invalid;
                            break;
                    }
                }

            }
        }

        // -------------------------------------------------
        // VRETEX LAYOUT
        // -------------------------------------------------

        if ( !content.attributes.empty() )
        {
            auto& vertex = content.attributes[ pipeline_meta.attributes ];
            pipeline_description.vertex_layout.attributes.resize( vertex.attributes.size() );
            for (auto& a : vertex.attributes)
            {
                auto& layout = pipeline_description.vertex_layout.attributes[ a.location ];
                layout.binding = a.binding;
                layout.location = a.location;
                layout.name = a.name;
                layout.type = a.format;
                layout.semantic = a.semantic;
            }

            pipeline_description.vertex_layout.input_rates.resize( vertex.bindings.size() );
            int buffer_index = 0;
            for (auto& b : vertex.bindings)
            {
                auto& input = pipeline_description.vertex_layout.input_rates[ buffer_index++ ];
                input.input_rate = b.input_rate;
                input.stride = b.stride;
            }
        }

        // -------------------------------------------------
        // PIPELINE STATES
        // -------------------------------------------------

        pipeline_description.rasterizer = (!content.rasterizer_states.empty())
        ? content.rasterizer_states[ pipeline_meta.rasterizer_state ] : RasterizerStatePresets::cullFront();

        pipeline_description.depth_stencil = (!content.depth_states.empty())
        ? content.depth_states[ pipeline_meta.depth_state ] : DepthStencilPresets::depthTestWrite();

        pipeline_description.color_blend = (!content.blend_states.empty())
        ? content.blend_states[ pipeline_meta.blend_state ] : ColorBlendState{.attachments = {BlendStatePreset::alphaBlend()}};

        pipeline_description.pipeline_rendering = content.pipeline_outputs[ pipeline_meta.pipeline_output ];

        return _graphics->createShaderPipeline( pipeline_description );
    }

    ref::ShaderPipeline JsonShaderLoader::load( const std::string& filename )
    {
        if (_graphics == nullptr) // make sure the required Graphics is set before loading anything
        {
            CallbackRequest< kege::Graphics > request(this, &JsonShaderLoader::operator() );
            Communication::broadcast< CallbackRequest< kege::Graphics >& >( request );
            if (_graphics == nullptr)
            {
                kege::Log::error << "FAILED: CallbackRequest -> Graphics. Null graphics pointer." << kege::Log::nl;
                return {};
            }
        }

        try
        {
            JsonShaderContent content = JsonShaderContent::load(filename);
            if (!content.error.empty())
            {
                kege::Log::error << content.error << kege::Log::nl;
                return {};
            }

            std::filesystem::path path = filename;
            std::string parent_path = path.parent_path().string();

            kege::ShaderPipelineDesc pipeline_description;

            return load( parent_path, content, 0 );
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return {};
        }
    }

    void JsonShaderLoader::operator()( kege::Graphics* graphics )
    {
        _graphics = graphics;
    }

    JsonShaderLoader::JsonShaderLoader( AssetManager* am )
    : kege::AssetLoaderT< ref::ShaderPipeline >( am )
    {
    }
}

