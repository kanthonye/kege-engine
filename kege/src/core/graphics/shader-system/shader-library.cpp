//
//  shader-library.cpp
//  editor
//
//  Created by Kenneth Esdaile on 1/29/26.
//

#include "pipeline-loader.hpp"
#include "shader-library.hpp"

namespace kege{
//
////    std::string getFilePath( const std::string& filename )
////    {
////        std::string path = filename;
////        int count;
////        for ( count=int(filename.length() - 1); count > 0 && path[count] != '/'; --count )
////        {
////            path[count] = 0;
////        }
////        path.resize(count + 1);
////        return path;
////    }
////
//
//
//    kege::LibraryContext::Attribute parseAttribute( const kege::Json& json )
//    {
//        return kege::LibraryContext::Attribute
//        {
//            .name = json["name"].toStr(),
//            .location = json["location"].toInt(),
//            .binding = json["binding"].toInt(),
//            .format = kege::convertVertexInputType( json[ "format" ].toStr() ),
//
//            .input_rate = ( strcmp("vertex", json["input_rate"].toStr()) == 0 )
//            ? kege::VertexInputRate::Vertex : kege::VertexInputRate::Instance,
//        };
//    }
//
//    kege::LibraryContext::AttributeBinding parseAttributeBinding( const kege::Json& json )
//    {
//        return kege::LibraryContext::AttributeBinding
//        {
//            .stride = json["stride"].toInt(),
//            .binding = json["binding"].toInt()
//        };
//    }
//
//    kege::LibraryContext::VertexAttribute parseVertexAttribute( const kege::Json& json )
//    {
//        return kege::LibraryContext::VertexAttribute
//        {
//            .attributes = json[ "attributes" ].toVector( parseAttribute ),
//            .bindings = json[ "attributes" ].toVector( parseAttributeBinding ),
//        };
//    }
//
//    kege::LibraryContext::Shader parseShaders( const kege::Json& json )
//    {
//        return kege::LibraryContext::Shader
//        {
//            .uri = json.getStr("uri"),
//            .name = json.getStr("name"),
//            .entry_point = json.getStr("entry-point"),
//            .type = kege::stringToShaderStageFlag( json[ "type" ].toStr() )
//        };
//    }
//
//    kege::LibraryContext::Field parseField( const kege::Json& json )
//    {
//        return kege::LibraryContext::Field
//        {
//            .count = json.getInt( "count" ),
//            .name = json.getStr( "name" ),
//            .type = json.getStr( "type" )
//        };
//    }
//
//    kege::LibraryContext::Struct parseStruct( const kege::Json& json )
//    {
//        return kege::LibraryContext::Struct
//        {
//            .name = json.getStr( "name" ),
//            .fields = json["fields"].toVector( parseField )
//        };
//    }
//
//    kege::LibraryContext::Uniform parseUniform( const kege::Json& json )
//    {
//        return kege::LibraryContext::Uniform
//        {
//            .name = json.getStr( "name" ),
//            .fields = json["fields"].toVector( parseField )
//        };
//    }
//
//    kege::LibraryContext::Storage parseStorage( const kege::Json& json )
//    {
//        return kege::LibraryContext::Storage
//        {
//            .name = json.getStr( "name" ),
//            .fields = json["fields"].toVector( parseField )
//        };
//    }
//
//    kege::LibraryContext::Sampler parseSampler( const kege::Json& json )
//    {
//        return kege::LibraryContext::Sampler
//        {
//            .name = json.getStr( "name" ),
//            .type = json.getStr( "type" ),
//            .count = json["count"].toInt()
//        };
//    }
//
//    kege::LibraryContext::PushConst parsePushConst( const kege::Json& json )
//    {
//        return kege::LibraryContext::PushConst
//        {
//            .name = json["name"].toStr(),
//            .size = json["size"].toInt(),
//            .count = json["count"].toInt(),
//            .offset = json["offset"].toInt(),
//            .fields = json[ "fields" ].toVector( parseField )
//        };
//    }
//
//    kege::LibraryContext::Block parseBlock( const kege::Json& json )
//    {
//        return kege::LibraryContext::Block
//        {
//            .usage = kege::stringToBindingUsage( json.getStr("usage") ),
//            .index = json.getInt("index"),
//            .target = json.getStr("target")
//        };
//    }
//
//    kege::LibraryContext::BindLayout parseBindLayout( const kege::Json& json )
//    {
//        return kege::LibraryContext::BindLayout
//        {
//            .block = parseBlock( json[ "block" ] ),
//            .stages = kege::parseShaderStages( json[ "stages" ] ),
//            .binding = json.getInt("binding"),
//            .count = json.getInt("count"),
//            .name = json.getStr("name")
//        };
//    }
//
//    kege::LibraryContext::BindConstant parseBindConstant( const kege::Json& json )
//    {
//        return kege::LibraryContext::BindConstant
//        {
//            .stages = kege::parseShaderStages( json[ "stages" ] ),
//            .push_constant = json.getInt("push_constant"),
//            .count = json.getInt("count"),
//            .name = json.getStr("name")
//        };
//    }
//
//    kege::LibraryContext::BindSetDesc parseBindSet( const kege::Json& json )
//    {
//        return kege::LibraryContext::BindSetDesc
//        {
//            .set = json.getInt("set"),
//            .bindings = json[ "bindings" ].toVector< kege::LibraryContext::BindLayout >( parseBindLayout )
//        };
//    }
//
//    kege::LibraryContext::PipelineLayout parsePipelineLayout( const kege::Json& json )
//    {
//        return kege::LibraryContext::PipelineLayout
//        {
//            .push_constants = json["push_constants"].toVector( parseBindConstant ),
//            .uniform_layouts = json["uniform_layouts"].toVector( parseBindSet )
//        };
//    }
//
//    kege::LibraryContext::Output parseOutput( const kege::Json& json )
//    {
//        return kege::LibraryContext::Output
//        {
//            .name = json.getStr("name"),
//            .semantic = json.getStr("semantic"),
//            .location = json.getInt( "location" ),
//            .format = kege::stringToFormat( json.getStr("format") ),
//        };
//    }
//
//    kege::LibraryContext::Pipeline parsePipeline( const kege::Json& json )
//    {
//        return kege::LibraryContext::Pipeline
//        {
//            .name = json.getStr( "name" ),
//            .topology = kege::stringToPrimitiveTopology( json["topology"].toStr() ),
//            .type = kege::stringToPipelineType( json.getStr( "type" ) ),
//            .rasterizer = json.getInt("push_constants"),
//            .depth_stencil = json.getInt("depth_stencil"),
//            .color_blend = json.getInt("color_blend"),
//            .pipeline_layout = json.getInt("pipeline_layout"),
//            .shaders = json[ "stages" ].toVector( atoi ),
//            .outputs = json[ "outputs" ].toVector( atoi )
//        };
//    }
//
//    bool loadLibraryContext(LibraryContext& context, const std::string& filename)
//    {
//        Json json = kege::JsonParser::load( filename.c_str() );
//
//        if ( !json )
//        {
//            return false;
//        }
//
//        context.rasterizer_states = json[ "rasterizer_states" ].toVector( parseRasterizationState );
//        context.depth_stencil_states = json[ "depth_stencil_states" ].toVector( parseDepthStencilStates );
//        context.color_blend_states = json[ "color_blend_states" ].toVector( parseColorBlendStates );
//        context.attributes = json[ "attributes" ].toVector( parseVertexAttribute );
//        context.shaders = json[ "shaders" ].toVector( parseShaders );
//        context.structs = json[ "structs" ].toVector( parseStruct );
//        context.uniforms = json[ "uniforms" ].toVector( parseUniform );
//        context.storages = json[ "storages" ].toVector( parseStorage );
//        context.push_constants = json[ "push_constants" ].toVector( parsePushConst );
//        context.pipeline_layouts = json[ "pipeline_layouts" ].toVector( parsePipelineLayout );
//        context.outputs = json[ "outputs" ].toVector( parseOutput );
//        context.samplers = json[ "samplers" ].toVector( parseSampler );
//        context.pipelines = json[ "pipelines" ].toVector( parsePipeline );
//
//        std::filesystem::path path = filename;
//        context.path = path.parent_path();//.string();
//        return true;
//    }
//
//    int createShaderStructBlock
//    (
//     std::vector< kege::ref::ShaderStructBlock >& shader_struct_blocks,
//     const std::string& name,
//     const std::vector< LibraryContext::Field >& fields
//    )
//    {
//        int offset = 0;
//        std::vector< kege::ShaderStructField > f(fields.size());
//        for (int i=0; i<fields.size(); ++i)
//        {
//            f[i].count = fields[i].count;
//            f[i].type = fields[i].type;
//            f[i].name = fields[i].name;
//            f[i].size = sizeOf( stringToShaderVarType(fields[i].type) );
//            f[i].offset = offset;
//            offset += f[i].size;
//        }
//        shader_struct_blocks.push_back( new ShaderStructBlock( ShaderStructType::Buffer, name, f ) );
//        return int(shader_struct_blocks.size() - 1);
//    }
//
//    ref::ShaderPipeline createShaderPipeline
//    (
//        kege::Graphics* graphics,
//        std::vector< kege::ref::ShaderStructBlock >& shader_struct_blocks,
//        kege::LibraryContext& context,
//        uint64_t pipeline_index
//    )
//    {
//        const LibraryContext::Pipeline& pipeline_record = context.pipelines[ pipeline_index ];
//
//        kege::ShaderPipelineDesc shader_pipeline_desc;
//        shader_pipeline_desc.name = pipeline_record.name;
//        shader_pipeline_desc.pipeline_type = pipeline_record.type;
//        shader_pipeline_desc.topology = pipeline_record.topology;
//        shader_pipeline_desc.raster_state = context.rasterizer_states[ pipeline_record.rasterizer ];
//        shader_pipeline_desc.color_blend_state = context.color_blend_states[ pipeline_record.color_blend ];
//        shader_pipeline_desc.depth_state = context.depth_stencil_states[ pipeline_record.depth_stencil ];
//
//        shader_pipeline_desc.pipeline_rendering.color_attachment_formats.resize(pipeline_record.outputs.size());
//        for (int i=0; i<pipeline_record.outputs.size(); ++i)
//        {
//            LibraryContext::Output& output = context.outputs[pipeline_record.outputs[i]];
//            shader_pipeline_desc.pipeline_rendering.color_attachment_formats[i] = output.format;
//        }
//
//        // process vertex attribute inputs
//        if ( !context.attributes.empty() )
//        {
//            LibraryContext::VertexAttribute* attributes = &context.attributes[pipeline_record.attributes];
//            shader_pipeline_desc.vertex_layout.attributes.resize(attributes->attributes.size());
//            shader_pipeline_desc.vertex_layout.input_rates.resize(attributes->bindings.size());
//            for (int i=0; i<attributes->attributes.size(); ++i)
//            {
//                kege::VertexAttribute& a = shader_pipeline_desc.vertex_layout.attributes[i];
//                a.name = attributes->attributes[i].name;
//                a.location = attributes->attributes[i].location;
//                a.buffer_index = attributes->attributes[i].binding;
//                a.type = attributes->attributes[i].format;
//            }
//            for (int i=0; i<attributes->bindings.size(); ++i)
//            {
//                shader_pipeline_desc.vertex_layout.input_rates[i].input_rate = attributes->attributes[i].input_rate;
//                shader_pipeline_desc.vertex_layout.input_rates[i].stride = attributes->bindings[i].stride;
//            }
//        }
////
////        LibraryContext::PipelineLayout& pipeline_layout = context.pipeline_layouts[ pipeline_record.pipeline_layout ];
////        shader_pipeline_desc.shader_layout.name = pipeline_record.name;
////        // process push constants
////        if ( !pipeline_layout.push_constants.empty() )
////        {
//////            const std::vector< LibraryContext::BindConstant >& bind_constants = pipeline_layout.push_constants;
////            PushBlockDescs& push_blocks = shader_pipeline_desc.shader_layout.push_blocks;
////            push_blocks.resize( pipeline_layout.push_constants.size() );
////            for (int i=0; i < pipeline_layout.push_constants.size(); ++i)
////            {
////                const LibraryContext::BindConstant& bind_constant = pipeline_layout.push_constants[i];
////                LibraryContext::PushConst& push_const = context.push_constants[ bind_constant.push_constant ];
////
////                if (push_const.shader_struct_block_index < 0)
////                {
////                    push_const.shader_struct_block_index = createShaderStructBlock( shader_struct_blocks, push_const.name, push_const.fields );
////                }
////
////                PushBlockDesc& push_block = push_blocks[i];
////                push_block.block = shader_struct_blocks[ push_const.shader_struct_block_index ];
////                push_block.size = push_const.size;
////                push_block.offset = push_const.offset;
////                push_block.count = push_const.count;
////                push_block.name = push_const.name;
////                push_block.stages = bind_constant.stages;
////            }
////        }
////
////        if ( !pipeline_layout.uniform_layouts.empty() )
////        {
////            // process descriptors
////            BindSetDescs& bind_sets = shader_pipeline_desc.shader_layout.bind_sets;
////            bind_sets.resize(pipeline_layout.uniform_layouts.size());
////            for (int i=0; i<pipeline_layout.uniform_layouts.size(); ++i)
////            {
////                const LibraryContext::BindSetDesc& bind_set = pipeline_layout.uniform_layouts[i];
////                BindSetDesc& bindings = bind_sets[i];
////
////                bindings.index = bind_set.set;
////                bindings.bindings.resize(bind_set.bindings.size());
////                for (int j=0; j<bind_set.bindings.size(); ++j)
////                {
////                    bindings.bindings[j].usage = bind_set.bindings[j].block.usage;
////                    bindings.bindings[j].instance_name = bind_set.bindings[j].name;
////                    bindings.bindings[j].count = bind_set.bindings[j].count;
////                    bindings.bindings[j].index = bind_set.bindings[j].binding;
////                    bindings.bindings[j].stages = bind_set.bindings[j].stages;
////                    switch (bind_set.bindings[j].block.usage)
////                    {
////                        case kege::BindingUsage::UniformBuffer:
////                        case kege::BindingUsage::UniformBufferDynamic:
////                        {
////                            LibraryContext::Uniform& uniform = context.uniforms[ bind_set.bindings[j].block.index ];
////                            if (uniform.target_index < 0)
////                            {
////                                uniform.target_index = createShaderStructBlock( shader_struct_blocks, uniform.name, uniform.fields );
////                            }
////                            bindings.bindings[j].block = shader_struct_blocks[ uniform.target_index ];
////                            bindings.bindings[j].name = uniform.name;
////                            break;
////                        }
////
////                        case kege::BindingUsage::StorageBuffer:
////                        case kege::BindingUsage::StorageBufferDynamic:
////                        {
////                            LibraryContext::Storage& storage = context.storages[ bind_set.bindings[j].block.index ];
////                            if (storage.target_index < 0)
////                            {
////                                storage.target_index = createShaderStructBlock( shader_struct_blocks, storage.name, storage.fields );
////                            }
////                            bindings.bindings[j].block = shader_struct_blocks[ storage.target_index ];
////                            bindings.bindings[j].name = storage.name;
////                            break;
////                        }
////
////                        case kege::BindingUsage::Sampler:
////                        case kege::BindingUsage::SampledImage:
////                        case kege::BindingUsage::CombinedImageSampler:
////                        {
////                            LibraryContext::Sampler& sampler = context.samplers[ bind_set.bindings[j].block.index ];
////
////                            //bindings.bindings[j].block = shader_struct_blocks[ uniform.shader_struct_block_index ];
////                            bindings.bindings[j].name = sampler.name;
////                            break;
////                        }
////
////                        default: break;
////                    }
////                }
////            }
////        }
////
////        // process shader source code
////        for (int i=0; i<pipeline_record.shaders.size(); ++i)
////        {
////            LibraryContext::Shader& shader = context.shaders[ pipeline_record.shaders[i] ];
////            kege::string filename = kege::vfs( shader.uri.c_str() );
////            ShaderDesc desc;
////            desc.name = shader.name;
////            desc.stage = shader.type;
////            desc.entry_point = shader.entry_point;
////
////            std::string shader_filename = context.path + "/" +shader.uri;
////
////            std::vector< char > source;
////            if( !kege::loadTextFile( source, shader_filename.c_str()) )
////            {
////                kege::Log::error << "FILE_LOAD_FAILED -> " << filename << Log::nl;
////                return 0;
////            }
////
////            desc.byte_code = graphics->compileGlslToSpv( desc.name.data(), desc.stage, source );
////            if( desc.byte_code.empty() )
////            {
////                return 0;
////            }
////
////            shader_pipeline_desc.shaders.push_back( graphics->createShader( desc ) );
////        }
////
////        return graphics->createShaderPipeline( shader_pipeline_desc );
//        return {};
//    }
//}
//
//
//namespace kege{
//
//    bool ShaderLibrary::load( const std::string& filename )
//    {
//        return loadLibraryContext(_context, filename);
//    }
//
//    ref::ShaderPipeline* ShaderLibrary::find( uint32_t guid )
//    {
//        if ( _records.size() < guid )
//        {
//            return _manager->get(_error_shader_handle);
//        }
//        
//        Record& record = _records[ guid ];
//        if( record.physical_handle == 0 )
//        {
//            ref::ShaderPipeline pipeline = createShaderPipeline( _graphics, _shader_struct_blocks, _context, record.pipeline_index );
//            record.physical_handle = _manager->add( _context.pipelines[record.pipeline_index].name, pipeline );
//        }
//        return _manager->get( record.physical_handle );
//    }
//
//    ShaderLibrary::ShaderLibrary(kege::Graphics* graphics, uint64_t error_shader_handle)
//    :   _graphics( graphics )
//    ,   _error_shader_handle( error_shader_handle )
//    {
//    }
//
//    ShaderLibrary::~ShaderLibrary()
//    {}
}

