//
//  pipeline-loader.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//


#include "pipeline-loader.hpp"
#include "string-to-enum-types.hpp"

namespace kege{

    bool loadTextFile( std::vector< char >& source, const char* filename )
    {
        FILE* file = fopen( filename, "rb" );
        if ( file )
        {
            fseek( file, 0, SEEK_END );
            uint64_t size = ftell( file );
            rewind( file );

            source.resize( size + 1 );
            fread( source.data(), sizeof( char ), size, file );
            fclose( file );

            source[ size ] = 0;
            return true;
        }
        return false;
    }

    std::string getFilePath( const std::string& filename )
    {
        std::string path = filename;
        int count;
        for ( count=int(filename.length() - 1); count > 0 && path[count] != '/'; --count )
        {
            path[count] = 0;
        }
        path.resize(count + 1);
        return path;
    }

    kege::ShaderStageFlag convertShaderStageFlages( kege::Json json )
    {
        kege::ShaderStageFlag stages = kege::ShaderStageFlag::Invalid;
        for (int i = 0; i < json.count(); ++i)
        {
            stages |= convertShaderStage( json[i].toStr() );
        }
        return stages;
    }

    void parseDepthStencilStates( std::vector< DepthStencilStateDesc >* depth_stencil_states, kege::Json json )
    {
        for (int i = 0; i < json.count(); ++i)
        {
            kege::Json elems = json[i];

            DepthStencilStateDesc state = {};
            state.depth_test_enable = elems[ "depth_test" ].toBool( false );
            state.depth_write_enable = elems[ "depth_write" ].toBool( true );
            state.depth_compare_op = stringToCompareOp( elems[ "depth_compare_op" ].toStr( "less" ) );

            state.stencil_test_enable = elems[ "stencil_test_enable" ].toBool( false );
            if ( state.stencil_test_enable )
            {
                Json front_op = elems[ "front_op" ];
                state.front_op.fail_op = stringToStencilOp( front_op[ "fail_op" ].toStr( "keep" ) );
                state.front_op.depth_fail_op = stringToStencilOp( front_op[ "depth_fail_op" ].toStr( "keep" ) );
                state.front_op.compare_op = stringToCompareOp( front_op[ "compare_op" ].toStr( "always" ) );
                state.front_op.compare_mask = front_op[ "compare_mask" ].toInt( 0xFFFFFFFF );
                state.front_op.write_mask = front_op[ "write_mask" ].toInt( 0xFFFFFFFF );
                state.front_op.reference = front_op[ "reference" ].toInt( 0 );

                Json back_op = elems[ "back_op" ];
                state.back_op.fail_op = stringToStencilOp( back_op[ "fail_op" ].toStr( "keep" ) );
                state.back_op.depth_fail_op = stringToStencilOp( back_op[ "depth_fail_op" ].toStr( "keep" ) );
                state.back_op.compare_op = stringToCompareOp( back_op[ "compare_op" ].toStr( "keep" ) );
                state.back_op.compare_mask = back_op[ "compare_mask" ].toInt( 0xFFFFFFFF );
                state.back_op.write_mask = back_op[ "write_mask" ].toInt( 0xFFFFFFFF );
                state.back_op.reference = back_op[ "reference" ].toInt( 0 );
            }

            depth_stencil_states->push_back( state );
        }
    }

    ColorBlendAttachmentState getColorBlendAttachmentState( kege::Json json )
    {
        ColorBlendAttachmentState state = {};
        if ( json )
        {
            state.src_color_blend_factor = stringToBlendFactor( json[ "src_color_blend_factor" ].toStr( "one" ) );
            state.dst_color_blend_factor = stringToBlendFactor( json[ "dst_color_blend_factor" ].toStr( "zero" ) );
            state.color_blend_op = stringToBlendOp( json[ "color_blend_op" ].toStr( "add" ) );

            state.src_alpha_blend_factor = stringToBlendFactor( json[ "src_alpha_blend_factor" ].toStr( "one" ) );
            state.dst_alpha_blend_factor = stringToBlendFactor( json[ "dst_alpha_blend_factor" ].toStr( "zero" ) );
            state.alpha_blend_op = stringToBlendOp( json[ "alpha_blend_op" ].toStr( "add" ) );

            state.blend_enable = json[ "blend_enable" ].toBool( false );
            state.color_write_mask = ColorComponentFlags::All;
        }
        return state;
    }

    std::vector< int > parseShaderStageIndices( kege::Json json )
    {
        std::vector< int > indices( json.count() );
        for (int i = 0; i < json.count(); ++i )
        {
            indices[i] = json[i].toInt();
        }
        return indices;
    }

    InputAssemblyStateDesc inputAssembly( kege::Json json )
    {
        InputAssemblyStateDesc state = {};
        if ( json )
        {
            state.topology = stringToPrimitiveTopology( json["topology"].toStr() );
        }
        return state;
    }

    PipelineOutputs parsePipelineOutputs( kege::Json json )
    {
        PipelineOutputs outputs = {};
        if ( json )
        {
            for (int i = 0; i < json.count(); ++i)
            {
                Json output = json[i];
                if ( strcmp( output[ "semantic"].toStr(), "COLOR" ) == 0 )
                {
                    outputs.color_attachment_formats.push_back( stringToFormat( output[ "format"].toStr() ) );
                }
                else if ( strcmp( output[ "semantic"].toStr(), "DEPTH" ) == 0 )
                {
                    outputs.depth_attachment_format = stringToFormat( output[ "format"].toStr() );
                }
                else if ( strcmp( output[ "semantic"].toStr(), "STENCIL" ) == 0 )
                {
                    outputs.stencil_attachment_format = stringToFormat( output[ "format"].toStr() );
                }
            }
        }
        return outputs;
    }

    std::vector< std::pair< int,int > > parsePipelineSpecializationConstants( kege::Json json )
    {
        static std::map< std::string,int > specialization_constants;
        std::vector< std::pair< int,int > > constants = {};
        if ( json.count() )
        {
            constants.resize( json.count() );
            for (int i = 0; i < json.count(); ++i)
            {
                Json element = json[i];
                int id = element[ "id" ].toInt();

                auto m = specialization_constants.find( element[ "name" ].value() );
                if ( m != specialization_constants.end() )
                {
                    constants[ id ].first = m->second;
                    constants[ id ].second = element[ "value" ].toInt();
                }
            }
        }
        return constants;
    }

    void parseColorBlendStates( std::vector< ColorBlendStateDesc >* color_blend_states, kege::Json json )
    {
        for (int i = 0; i < json.count(); ++i)
        {
            kege::Json desc = json[i];

            ColorBlendStateDesc state = {};
            state.logic_op_enable = desc[ "logic_op_enable" ].toBool( false );
            state.logic_op = stringToColorBlendLogicOp( desc[ "logic_op" ].toStr( "copy") );

            Json color_blend_attachments = desc[ "color_blend_attachments" ];
            for (int j=0; j<color_blend_attachments.count(); ++j)
            {
                state.attachments.push_back( getColorBlendAttachmentState( color_blend_attachments[j] ) );
            }
            color_blend_states->push_back( state );
        }
    }

    void parseRasterizationState( std::vector< RasterizationStateDesc >* rasterizer_states, kege::Json json )
    {
        for (int i = 0; i < json.count(); ++i)
        {
            kege::Json elems = json[i];

            RasterizationStateDesc state = {};
            state.rasterizer_disable = json.getBool("rasterizer_disable");
            state.polygon_mode = json["polygon_mode"]( stringToPolygonMode, kege::PolygonMode::Fill );
            state.cull_mode = json["cull_mode"]( stringToCullMode, kege::CullMode::None );
            state.front_face = json["front_face"]( stringToFrontFace, kege::FrontFace::CounterClockwise );

            //Json line_width = json["line_width"];
            //Json depth_clamp_enable = json["depth_clamp_enable"];
            //Json depth_bias_constant_factor = json["depth_bias_constant_factor"];
            //Json depth_bias_clamp = json["depth_bias_clamp"];
            //Json depth_bias_slope_factor = json["depth_bias_slope_factor"];

            rasterizer_states->push_back( state );
        }
    }

    void parsePipelineVertexInput( std::vector< VertexBufferLayout >* vertex_layouts, kege::Json json )
    {
        VertexBufferLayout desc;
        if ( json )
        {
            kege::Json attributes = json[ "attributes" ];
            for (int i = 0; i < attributes.count(); ++i)
            {
                kege::Json attribute = attributes[i];

                kege::VertexInputAttributeDesc v;
                v.location = attribute["location"].toInt();
                v.binding = attribute["binding"].toInt();
                v.offset = attribute["offset"].toInt();
                v.type = convertVertexInputType( attribute[ "format" ].toStr() );
                v.binding = attribute["binding"].toInt();
                v.input_rate = ( strcmp("vertex", attribute["input_rate"].toStr()) == 0 )
                ? kege::VertexInputRate::Vertex : kege::VertexInputRate::Instance;
                desc.attributes.push_back( v );
            }

            kege::Json bindings = json[ "bindings" ];
            for (int i = 0; i < bindings.count(); ++i)
            {
                kege::Json binding = bindings[i];
                desc.strides.push_back( binding["stride"].toInt() );
            }
        }
    }

    ShaderStructType stringToShaderStructType( const std::string& s )
    {
        if ( s == "buffer" ) return ShaderStructType::Buffer;
        if ( s == "data_block" ) return ShaderStructType::DataBlock;
        if ( s == "struct" ) return ShaderStructType::Struct;
        kege::Log::error << "unsupported ShaderStructType -> " <<s <<kege::Log::nl;
        return {};
    }

    bool parseShaderStructBlocks( std::vector< ref::ShaderStructBlock >* uniforms, kege::Json json )
    {
        for (int i = 0; i < json.count(); ++i)
        {
            kege::Json uniform = json[i];
            kege::Json members = uniform["fields"];

            std::string name = uniform.getStr("name");
            ShaderStructType type = uniform["type"](stringToShaderStructType, {});

            std::vector< ShaderStructField > fields( members.count() );
            int offset = 0;
            for (int k = 0; k < members.count(); ++k)
            {
                Json field = members[k];
                fields[k].name = field.getStr("name");
                fields[k].type = field.getStr("type");
                //fields[k].type = stringToShaderVarType( fields[k].stype );
                fields[k].count = field.getInt("count");
                fields[k].size = field.getInt("size");

                fields[k].offset = offset;
                offset += fields[k].size;
            }

            uniforms->push_back( new kege::ShaderStructBlock( type, name, fields ) );
        }
        return true;
    }

    ShaderStageFlag parseShaderStageFlag( const Json& json )
    {
        ShaderStageFlag stages = stringToShaderStageFlag( json[0].value() );
        for (int i = 1; i < json.count(); ++i )
        {
            stages = stages | stringToShaderStageFlag( json[i].value() );
        }
        return stages;
    }

    void parseOutputs( const Json& json, std::vector< glsl::Output >* outputs )
    {
        outputs->resize( json.count() );
        for (int i = 0; i < json.count(); ++i )
        {
            outputs->at(i).location = json[i]["location"].toInt();
            outputs->at(i).format = stringToFormat( json[i]["format"].toStr() );
            outputs->at(i).semantic = json[i]["semantic"].toStr();
            outputs->at(i).name = json[i]["name"].toStr();
        }
    }

    bool parsePipelineLayouts( kege::Json json, std::vector< glsl::PipelineLayout >* pipeline_layouts )
    {
        pipeline_layouts->resize( json.count() );
        for (int i = 0; i < json.count(); ++i)
        {
            Json layout_data = json[i];
            pipeline_layouts->at(i).name = layout_data.getStr("name");
            {
                Json layouts = layout_data[ "layouts" ];
                pipeline_layouts->at(i).set_layouts.resize( layouts.count() );
                
                for (int k = 0; k < layouts.count(); ++k)
                {
                    Json layout = layouts[k];
                    Json bindings = layout[ "bindings" ];

                    glsl::SetLayout& set_layouts = pipeline_layouts->at(i).set_layouts[k];
                    set_layouts.bindings.resize( bindings.count() );
                    set_layouts.set = layout.getInt("set");

                    for (int b = 0; b < bindings.count(); ++b)
                    {
                        Json binding = bindings[i];
                        set_layouts.bindings[b].name = binding.getStr("name");
                        set_layouts.bindings[b].count = binding.getInt("count");
                        set_layouts.bindings[b].binding = binding.getInt("binding");
                        set_layouts.bindings[b].block_ref = binding.getInt("block_ref");
                        set_layouts.bindings[b].usage = stringToBindingUsage( binding.getStr("usage") );
                        set_layouts.bindings[b].stages = parseShaderStageFlag( binding["stages"] );
                    }
                }

                Json push_constants = layout_data[ "push_constants" ];
                {
                    pipeline_layouts->at(i).push_layouts.resize( push_constants.count() );
                    for (int k = 0; k < push_constants.count(); ++k)
                    {
                        glsl::PushLayout& push_layout = pipeline_layouts->at(i).push_layouts[k];
                        Json push_blocks = push_constants[k];
                        push_layout.name = push_blocks.getStr("name");
                        push_layout.size = push_blocks.getInt("size");
                        push_layout.count = push_blocks.getInt("count");
                        push_layout.offset = push_blocks.getInt("offset");
                        push_layout.block_ref = push_blocks.getInt("block_ref");
                        push_layout.stages = parseShaderStageFlag( push_blocks["stages"] );
                    }
                }
            }
        }
        return true;
    }

    void parsePipelines( kege::Json json, std::vector< glsl::Pipeline >* pipelines )
    {
        pipelines->resize( json.count() );
        for (int i = 0; i < json.count(); ++i )
        {
            kege::Json pipeline_data = json[i];
            pipelines->at(i).name = pipeline_data.getStr("name");
            pipelines->at(i).type = stringToPipelineType( json[i].getStr( "type" ) );
            pipelines->at(i).topology = stringToPrimitiveTopology( pipeline_data.getStr("topology") );
            pipelines->at(i).rasterizer = pipeline_data.getInt("rasterizer");
            pipelines->at(i).depth_stencil = pipeline_data.getInt("depth_stencil");
            pipelines->at(i).color_blend = pipeline_data.getInt("color_blend");
            pipelines->at(i).pipeline_layout_ref = pipeline_data.getInt("pipeline_layout_ref");
            pipelines->at(i).stages = pipeline_data[ "stage_refs" ].toVector<int>( atoi );
            parseOutputs( pipeline_data[ "outputs" ], &pipelines->at(i).outputs );
        }
    }

    bool parseShaderStages
    (
        kege::Graphics* graphics, std::vector< ref::Shader >* shaders, kege::Json json, const std::string& path
    )
    {
        shaders->reserve( json.count() );
        for (int i = 0; i < json.count(); ++i)
        {
            Json element = json[i];

            kege::ShaderDesc shader_desc;
            shader_desc.stage = stringToShaderStageFlag( element[ "type" ].toStr() );
            shader_desc.entry_point = element[ "entry-point" ].toStr();
            shader_desc.name = element[ "name" ].toStr();

            const char* uri = element[ "uri" ].toStr();
            std::string filename = path + "/" + uri;

            std::vector< char > source;
            if( !loadTextFile( source, filename.data()) )
            {
                kege::Log::error << "FILE_LOAD_FAILED -> " << filename << Log::nl;
                return false;
            }

            shader_desc.byte_code = graphics->compileGlslToSpv( shader_desc.name.data(), shader_desc.stage, source );
            if( shader_desc.byte_code.empty() )
            {
                return false;
            }
            shaders->push_back( graphics->createShader( shader_desc ) );
        }
        return true;
    }

    bool parseShaderPipelineLib
    (
        kege::Json& json,
        const std::string& path,
        kege::Graphics* graphics,
        glsl::LibraryContext* context
    )
    {
        parseShaderStructBlocks( &context->data_blocks, json[ "data_blocks" ] );
        parsePipelineLayouts( json[ "pipeline_layouts" ], &context->pipeline_layouts );
        parseColorBlendStates( &context->color_blend_states, json[ "color_blend_states" ] );
        parseDepthStencilStates( &context->depth_stencil_states, json[ "depth_stencil_states" ] );
        parseRasterizationState( &context->rasterizer_states, json[ "rasterizer_states" ] );
        parsePipelineVertexInput( &context->vertex_layouts, json[ "vertex_layouts" ] );
        parseShaderStages( graphics, &context->shaders, json[ "shaders" ], path );
        parsePipelines( json[ "pipelines" ], &context->pipelines );
        return true;
    }

    ref::ShaderPipeline createShaderPipeline
    (
        kege::Graphics* graphics,
        glsl::LibraryContext& context,
        int pipeline_index
    )
    {
        const glsl::Pipeline& pipeline = context.pipelines[ pipeline_index ];

        PipelineCreateInfo create_info = {};
        create_info.name = pipeline.name;
        create_info.pipeline_type = pipeline.type;
        create_info.input_assembly.topology = pipeline.topology;
        if( 0 <= pipeline.color_blend )   create_info.color_blend = context.color_blend_states[ pipeline.color_blend ];
        if( 0 <= pipeline.depth_stencil ) create_info.depth_stencil = context.depth_stencil_states[ pipeline.depth_stencil ];
        if( 0 <= pipeline.rasterizer )    create_info.rasterization = context.rasterizer_states[ pipeline.rasterizer ];
        if( 0 <= pipeline.vertex_layout ) create_info.vertex_input = context.vertex_layouts[ pipeline.vertex_layout ];
        create_info.shaders.reserve( pipeline.stages.size() );

        create_info.outputs = {};
        create_info.outputs.render_sample_count = SampleCount::Count1;
        for (int i = 0; i<pipeline.outputs.size(); ++i)
        {
            switch ( pipeline.outputs[i].format )
            {
                case kege::Format::depth_16:
                case kege::Format::depth_32:
                case kege::Format::depth_16_stencil_8:
                case kege::Format::depth_24_stencil_8:
                case kege::Format::depth_32_stencil_8:
                    create_info.outputs.depth_attachment_format = pipeline.outputs[i].format;
                    break;

                default:
                    create_info.outputs.color_attachment_formats.push_back({
                        pipeline.outputs[i].format
                    });
                    break;
            }

        }

        for (int i = 0; i<pipeline.stages.size(); ++i)
        {
            create_info.shaders.push_back( context.shaders[ pipeline.stages[i] ] );
        }

        const glsl::PipelineLayout& pipeline_layout_data = context.pipeline_layouts[ pipeline.pipeline_layout_ref ];
        {
            PipelineSetLayoutBindings& set_layout_bindings = create_info.shader_layout.set_layout_bindings;
            {
                set_layout_bindings.resize( pipeline_layout_data.set_layouts.size() );
                for ( int set_index = 0; set_index < pipeline_layout_data.set_layouts.size(); ++set_index )
                {
                    const glsl::SetLayout& set_layout_data = pipeline_layout_data.set_layouts[ set_index ];
                    SetLayoutBindings& set = set_layout_bindings[ set_index ];

                    set.bindings.resize( set_layout_data.bindings.size() );
                    set.set_index = set_layout_data.set;

                    for (int i = 0; i < set_layout_data.bindings.size(); ++i)
                    {
                        set.bindings[i].index   = set_layout_data.bindings[i].binding;
                        set.bindings[i].name    = set_layout_data.bindings[i].name;
                        set.bindings[i].count   = set_layout_data.bindings[i].count;
                        set.bindings[i].stages  = set_layout_data.bindings[i].stages;
                        set.bindings[i].usage   = set_layout_data.bindings[i].usage;

                        if ( set_layout_data.bindings[i].block_ref >= 0 )
                        {
                            set.bindings[i].block = context.data_blocks[ set_layout_data.bindings[i].block_ref ];
                        }
                    }
                }
            }

            create_info.shader_layout.push_block_layout.resize( pipeline_layout_data.push_layouts.size() );
            for (int i = 0; i < pipeline_layout_data.push_layouts.size(); ++i)
            {
                PushBlock& block = create_info.shader_layout.push_block_layout[i];
                block.size = pipeline_layout_data.push_layouts[i].size;
                block.offset = pipeline_layout_data.push_layouts[i].offset;
                block.name = pipeline_layout_data.push_layouts[i].name;
                block.stages = pipeline_layout_data.push_layouts[i].stages;
                block.count = pipeline_layout_data.push_layouts[i].count;
                if ( pipeline_layout_data.push_layouts[i].block_ref >= 0 )
                {
                    block.block = context.data_blocks[ pipeline_layout_data.push_layouts[i].block_ref ];
                }
            }
        }

        return graphics->createShaderPipeline( create_info );
    }

}
