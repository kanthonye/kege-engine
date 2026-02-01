//
//  shader-library.cpp
//  editor
//
//  Created by Kenneth Esdaile on 1/29/26.
//

#include "pipeline-loader.hpp"
#include "shader-library.hpp"

namespace kege{

//    std::string getFilePath( const std::string& filename )
//    {
//        std::string path = filename;
//        int count;
//        for ( count=int(filename.length() - 1); count > 0 && path[count] != '/'; --count )
//        {
//            path[count] = 0;
//        }
//        path.resize(count + 1);
//        return path;
//    }
//
    kege::ShaderStageFlag parseShaderStages( kege::Json json )
    {
        kege::ShaderStageFlag stages = kege::ShaderStageFlag::Invalid;
        for (int i = 0; i < json.count(); ++i)
        {
            stages |= kege::stringToShaderStageFlag( json[i].toStr() );
        }
        return stages;
    }

    kege::StencilOpState parseStencilOpState( const kege::Json& json )
    {
        return kege::StencilOpState
        {
            .fail_op = kege::stringToStencilOp( json[ "fail_op" ].toStr( "keep" ) ),
            .depth_fail_op = kege::stringToStencilOp( json[ "depth_fail_op" ].toStr( "keep" ) ),
            .compare_op = kege::stringToCompareOp( json[ "compare_op" ].toStr( "always" ) ),
            .compare_mask = (uint32_t)json[ "compare_mask" ].toInt( 0xFFFFFFFF ),
            .write_mask = (uint32_t)json[ "write_mask" ].toInt( 0xFFFFFFFF ),
            .reference = (uint32_t)json[ "reference" ].toInt( 0 ),
        };
    }

    kege::DepthStencilStateDesc parseDepthStencilStates( const kege::Json& json )
    {
        return kege::DepthStencilStateDesc
        {
            .depth_test_enable = json[ "depth_test" ].toBool( false ),
            .depth_write_enable = json[ "depth_write" ].toBool( true ),
            .depth_compare_op = kege::stringToCompareOp( json[ "depth_compare_op" ].toStr( "less" ) ),
            .stencil_test_enable = json[ "stencil_test_enable" ].toBool( false ),
            .front_op = parseStencilOpState( json[ "front_op" ] ),
            .back_op = parseStencilOpState( json[ "back_op" ] ),
        };
    }

    kege::ColorBlendAttachmentState getColorBlendAttachmentState( const kege::Json& json )
    {
        return kege::ColorBlendAttachmentState
        {
            .src_color_blend_factor = kege::stringToBlendFactor( json[ "src_color_blend_factor" ].toStr( "one" ) ),
            .dst_color_blend_factor = kege::stringToBlendFactor( json[ "dst_color_blend_factor" ].toStr( "zero" ) ),
            .color_blend_op = kege::stringToBlendOp( json[ "color_blend_op" ].toStr( "add" ) ),

            .src_alpha_blend_factor = kege::stringToBlendFactor( json[ "src_alpha_blend_factor" ].toStr( "one" ) ),
            .dst_alpha_blend_factor = kege::stringToBlendFactor( json[ "dst_alpha_blend_factor" ].toStr( "zero" ) ),
            .alpha_blend_op = kege::stringToBlendOp( json[ "alpha_blend_op" ].toStr( "add" ) ),

            .blend_enable = json[ "blend_enable" ].toBool( false ),
            .color_write_mask = kege::ColorComponentFlags::All,
        };
    }

    kege::ColorBlendStateDesc parseColorBlendStates( const kege::Json& json )
    {
        return kege::ColorBlendStateDesc
        {
            .logic_op_enable = json[ "logic_op_enable" ].toBool( false ),
            .logic_op = kege::stringToColorBlendLogicOp( json[ "logic_op" ].toStr( "copy") ),
            .attachments = json[ "color_blend_attachments" ].toVector( getColorBlendAttachmentState )
        };
    }

    kege::RasterizationStateDesc parseRasterizationState( const kege::Json& json )
    {
        return kege::RasterizationStateDesc
        {
            .rasterizer_disable = json.getBool("rasterizer_disable"),
            .polygon_mode = json["polygon_mode"]( kege::stringToPolygonMode, kege::PolygonMode::Fill ),
            .cull_mode = json["cull_mode"]( kege::stringToCullMode, kege::CullMode::None ),
            .front_face = json["front_face"]( kege::stringToFrontFace, kege::FrontFace::CounterClockwise ),
        };
    }

    kege::LibraryContext::Attribute parseAttribute( const kege::Json& json )
    {
        return kege::LibraryContext::Attribute
        {
            .name = json["name"].toStr(),
            .location = json["location"].toInt(),
            .binding = json["binding"].toInt(),
            .format = kege::convertVertexInputType( json[ "format" ].toStr() ),

            .input_rate = ( strcmp("vertex", json["input_rate"].toStr()) == 0 )
            ? kege::VertexInputRate::Vertex : kege::VertexInputRate::Instance,
        };
    }

    kege::LibraryContext::AttributeBinding parseAttributeBinding( const kege::Json& json )
    {
        return kege::LibraryContext::AttributeBinding
        {
            .stride = json["stride"].toInt(),
            .binding = json["binding"].toInt()
        };
    }

    kege::LibraryContext::VertexAttribute parseVertexAttribute( const kege::Json& json )
    {
        return kege::LibraryContext::VertexAttribute
        {
            .attributes = json[ "attributes" ].toVector( parseAttribute ),
            .bindings = json[ "attributes" ].toVector( parseAttributeBinding ),
        };
    }

    kege::LibraryContext::Shader parseShaders( const kege::Json& json )
    {
        return kege::LibraryContext::Shader
        {
            .uri = json.getStr("uri"),
            .name = json.getStr("name"),
            .entry_point = json.getStr("entry-point"),
            .type = kege::stringToShaderStageFlag( json[ "type" ].toStr() )
        };
    }

    kege::LibraryContext::Field parseField( const kege::Json& json )
    {
        return kege::LibraryContext::Field
        {
            .count = json.getInt( "count" ),
            .name = json.getStr( "name" ),
            .type = json.getStr( "type" )
        };
    }

    kege::LibraryContext::Struct parseStruct( const kege::Json& json )
    {
        return kege::LibraryContext::Struct
        {
            .name = json.getStr( "name" ),
            .fields = json["fields"].toVector( parseField )
        };
    }

    kege::LibraryContext::Uniform parseUniform( const kege::Json& json )
    {
        return kege::LibraryContext::Uniform
        {
            .name = json.getStr( "name" ),
            .fields = json["fields"].toVector( parseField )
        };
    }

    kege::LibraryContext::Storage parseStorage( const kege::Json& json )
    {
        return kege::LibraryContext::Storage
        {
            .name = json.getStr( "name" ),
            .fields = json["fields"].toVector( parseField )
        };
    }

    kege::LibraryContext::PushConst parsePushConst( const kege::Json& json )
    {
        return kege::LibraryContext::PushConst
        {
            .name = json["name"].toStr(),
            .size = json["size"].toInt(),
            .count = json["count"].toInt(),
            .offset = json["offset"].toInt(),
            .stages = parseShaderStages( json[ "stages" ] ),
            .fields = json[ "fields" ].toVector( parseField )
        };
    }

    kege::LibraryContext::Block parseBlock( const kege::Json& json )
    {
        return kege::LibraryContext::Block
        {
            .usage = kege::stringToBindingUsage( json.getStr("usage") ),
            .index = json.getInt("index"),
            .target = json.getStr("target")
        };
    }

    kege::LibraryContext::BindLayout parseBindLayout( const kege::Json& json )
    {
        return kege::LibraryContext::BindLayout
        {
            .block = parseBlock( json[ "block" ] ),
            .stages = kege::parseShaderStages( json[ "stages" ] ),
            .binding = json.getInt("binding"),
            .count = json.getInt("count"),
            .name = json.getStr("name")
        };
    }

    kege::LibraryContext::BindConstant parseBindConstant( const kege::Json& json )
    {
        return kege::LibraryContext::BindConstant
        {
            .push_constant = json.getInt("push_constant"),
            .count = json.getInt("count"),
            .name = json.getStr("name")
        };
    }

    kege::LibraryContext::BindSet parseBindSet( const kege::Json& json )
    {
        return kege::LibraryContext::BindSet
        {
            .set = json.getInt("set"),
            .bindings = json[ "bindings" ].toVector< kege::LibraryContext::BindLayout >( parseBindLayout )
        };
    }

    kege::LibraryContext::PipelineLayout parsePipelineLayout( const kege::Json& json )
    {
        return kege::LibraryContext::PipelineLayout
        {
            .push_constants = json["push_constants"].toVector( parseBindConstant ),
            .uniform_layouts = json["uniform_layouts"].toVector( parseBindSet )
        };
    }

    kege::LibraryContext::Output parseOutput( const kege::Json& json )
    {
        return kege::LibraryContext::Output
        {
            .name = json.getStr("name"),
            .semantic = json.getStr("semantic"),
            .location = json.getInt( "location" ),
            .format = kege::stringToFormat( json.getStr("format") ),
        };
    }

    kege::LibraryContext::Pipeline parsePipeline( const kege::Json& json )
    {
        return kege::LibraryContext::Pipeline
        {
            .name = json.getStr( "name" ),
            .topology = kege::stringToPrimitiveTopology( json["topology"].toStr() ),
            .type = kege::stringToPipelineType( json.getStr( "type" ) ),
            .rasterizer = json.getInt("push_constants"),
            .depth_stencil = json.getInt("depth_stencil"),
            .color_blend = json.getInt("color_blend"),
            .pipeline_layout = json.getInt("pipeline_layout"),
            .shaders = json[ "stages" ].toVector( atoi ),
            .outputs = json[ "outputs" ].toVector( atoi )
        };
    }

    bool loadLibraryContext(LibraryContext& context, const std::string& filename)
    {
        Json json = kege::JsonParser::load( filename.c_str() );

        if ( !json )
        {
            return false;
        }

        context.rasterizer_states = json[ "rasterizer_states" ].toVector( parseRasterizationState );
        context.depth_stencil_states = json[ "depth_stencil_states" ].toVector( parseDepthStencilStates );
        context.color_blend_states = json[ "color_blend_states" ].toVector( parseColorBlendStates );
        context.attributes = json[ "attributes" ].toVector( parseVertexAttribute );
        context.shaders = json[ "shaders" ].toVector( parseShaders );
        context.structs = json[ "structs" ].toVector( parseStruct );
        context.uniforms = json[ "uniforms" ].toVector( parseUniform );
        context.storages = json[ "storages" ].toVector( parseStorage );
        context.push_constants = json[ "push_constants" ].toVector( parsePushConst );
        context.pipeline_layouts = json[ "pipeline_layouts" ].toVector( parsePipelineLayout );
        context.outputs = json[ "outputs" ].toVector( parseOutput );
        context.pipelines = json[ "pipelines" ].toVector( parsePipeline );

        std::filesystem::path path = filename;
        context.path = path.parent_path();//.string();
        return true;
    }

    int createShaderStructBlock
    (
     std::vector< kege::ref::ShaderStructBlock >& shader_struct_blocks,
     const std::string& name,
     const std::vector< LibraryContext::Field >& fields
    )
    {
        int offset = 0;
        std::vector< kege::ShaderStructField > f(fields.size());
        for (int i=0; i<fields.size(); ++i)
        {
            f[i].count = fields[i].count;
            f[i].type = fields[i].type;
            f[i].name = fields[i].name;
            f[i].size = sizeOf( stringToShaderVarType(fields[i].type) );
            f[i].offset = offset;
            offset += f[i].size;
        }
        shader_struct_blocks.push_back( new ShaderStructBlock( ShaderStructType::Buffer, name, f ) );
        return int(shader_struct_blocks.size() - 1);
    }

    ref::ShaderPipeline createShaderPipeline
    (
        kege::Graphics* graphics,
        std::vector< kege::ref::ShaderStructBlock >& shader_struct_blocks,
        kege::LibraryContext& context,
        uint64_t pipeline_index
    )
    {
        const LibraryContext::Pipeline& pipeline_record = context.pipelines[ pipeline_index ];

        kege::PipelineCreateInfo shader_pipeline_desc;
        shader_pipeline_desc.name = pipeline_record.name;
        shader_pipeline_desc.pipeline_type = pipeline_record.type;
        shader_pipeline_desc.input_assembly.topology = pipeline_record.topology;
        shader_pipeline_desc.rasterization = context.rasterizer_states[ pipeline_record.rasterizer ];
        shader_pipeline_desc.color_blend = context.color_blend_states[ pipeline_record.color_blend ];
        shader_pipeline_desc.depth_stencil = context.depth_stencil_states[ pipeline_record.depth_stencil ];

        // process vertex attribute inputs
        LibraryContext::VertexAttribute& attributes = context.attributes[pipeline_record.attributes];
        shader_pipeline_desc.vertex_input.attributes.resize(attributes.attributes.size());
        shader_pipeline_desc.vertex_input.strides.resize(attributes.bindings.size());
        for (int i=0; i<attributes.attributes.size(); ++i)
        {
            VertexInputAttributeDesc& a = shader_pipeline_desc.vertex_input.attributes[i];
            a.name = attributes.attributes[i].name;
            a.location = attributes.attributes[i].location;
            a.binding = attributes.attributes[i].binding;
            a.type = attributes.attributes[i].format;
            a.input_rate = attributes.attributes[i].input_rate;
        }
        for (int i=0; i<attributes.bindings.size(); ++i)
        {
            shader_pipeline_desc.vertex_input.strides[i] = attributes.bindings[i].stride;
        }

        // process push constants
        const std::vector< LibraryContext::BindConstant >& bind_constants = context.pipeline_layouts[ pipeline_record.pipeline_layout ].push_constants;
        PushBlockLayout& push_block_layout = shader_pipeline_desc.shader_layout.push_block_layout;
        push_block_layout.resize(bind_constants.size());
        for (int i=0; i<bind_constants.size(); ++i)
        {
            const LibraryContext::BindConstant& bind_constant = bind_constants[i];
            LibraryContext::PushConst& push_const = context.push_constants[ bind_constant.push_constant ];

            if (push_const.shader_struct_block_index < 0)
            {
                push_const.shader_struct_block_index = createShaderStructBlock( shader_struct_blocks, push_const.name, push_const.fields );
            }

            PushBlock& push_block = push_block_layout[i];
            push_block.block = shader_struct_blocks[ push_const.shader_struct_block_index ];
            push_block.size = push_const.size;
            push_block.offset = push_const.offset;
            push_block.count = push_const.count;
            push_block.name = push_const.name;
            push_block.stages = push_const.stages;
        }

        // process descriptors
        const std::vector< LibraryContext::BindSet >& bind_sets = context.pipeline_layouts[ pipeline_record.pipeline_layout ].uniform_layouts;
        PipelineSetLayoutBindings& set_layout_bindings = shader_pipeline_desc.shader_layout.set_layout_bindings;
        set_layout_bindings.resize(bind_sets.size());
        for (int i=0; i<bind_sets.size(); ++i)
        {
            const LibraryContext::BindSet& bind_set = bind_sets[i];
            SetLayoutBindings& bindings = set_layout_bindings[i];

            bindings.set_index = bind_set.set;
            bindings.bindings.resize(bind_set.bindings.size());
            for (int j=0; j<bind_set.bindings.size(); ++j)
            {
                bindings.bindings[j].usage = bind_set.bindings[j].block.usage;
                bindings.bindings[j].instance_name = bind_set.bindings[j].name;
                bindings.bindings[j].count = bind_set.bindings[j].count;
                bindings.bindings[j].index = bind_set.bindings[j].binding;
                bindings.bindings[j].stages = bind_set.bindings[j].stages;
                switch (bind_set.bindings[j].block.usage)
                {
                    case kege::BindingUsage::UniformBuffer:
                    case kege::BindingUsage::UniformBufferDynamic:
                    {
                        LibraryContext::Uniform& uniform = context.uniforms[ bind_set.bindings[j].block.index ];
                        if (uniform.shader_struct_block_index < 0)
                        {
                            uniform.shader_struct_block_index = createShaderStructBlock( shader_struct_blocks, uniform.name, uniform.fields );
                        }
                        bindings.bindings[j].block = shader_struct_blocks[ uniform.shader_struct_block_index ];
                        bindings.bindings[j].name = uniform.name;
                        break;
                    }

                    case kege::BindingUsage::StorageBuffer:
                    case kege::BindingUsage::StorageBufferDynamic:
                    {
                        LibraryContext::Uniform& uniform = context.uniforms[ bind_set.bindings[j].block.index ];
                        if (uniform.shader_struct_block_index < 0)
                        {
                            uniform.shader_struct_block_index = createShaderStructBlock( shader_struct_blocks, uniform.name, uniform.fields );
                        }
                        bindings.bindings[j].block = shader_struct_blocks[ uniform.shader_struct_block_index ];
                        bindings.bindings[j].name = uniform.name;
                        break;
                    }

                    default: break;
                }
            }
        }

        // process shader source code
        for (int i=0; i<pipeline_record.shaders.size(); ++i)
        {
            LibraryContext::Shader& shader = context.shaders[ pipeline_record.shaders[i] ];
            kege::string filename = kege::vfs( shader.uri.c_str() );
            ShaderDesc desc;
            desc.name = shader.name;
            desc.stage = shader.type;
            desc.entry_point = shader.entry_point;

            std::string shader_filename = context.path + "/" +shader.uri;

            std::vector< char > source;
            if( !kege::loadTextFile( source, shader_filename.c_str()) )
            {
                kege::Log::error << "FILE_LOAD_FAILED -> " << filename << Log::nl;
                return 0;
            }

            desc.byte_code = graphics->compileGlslToSpv( desc.name.data(), desc.stage, source );
            if( desc.byte_code.empty() )
            {
                return 0;
            }

            shader_pipeline_desc.shaders.push_back( graphics->createShader( desc ) );
        }

        return graphics->createShaderPipeline( shader_pipeline_desc );
    }
}


namespace kege{

    bool ShaderLibrary::load( const std::string& filename )
    {
        return loadLibraryContext(_context, filename);
    }

    ref::ShaderPipeline* ShaderLibrary::find( uint32_t guid )
    {
        if ( _records.size() < guid )
        {
            return _manager->get(_error_shader_handle);
        }
        
        Record& record = _records[ guid ];
        if( record.physical_handle == 0 )
        {
            ref::ShaderPipeline pipeline = createShaderPipeline( _graphics, _shader_struct_blocks, _context, record.pipeline_index );
            record.physical_handle = _manager->add( _context.pipelines[record.pipeline_index].name, pipeline );
        }
        return _manager->get( record.physical_handle );
    }

    ShaderLibrary::ShaderLibrary(kege::Graphics* graphics, uint64_t error_shader_handle)
    :   _graphics( graphics )
    ,   _error_shader_handle( error_shader_handle )
    {
    }

    ShaderLibrary::~ShaderLibrary()
    {}
}

