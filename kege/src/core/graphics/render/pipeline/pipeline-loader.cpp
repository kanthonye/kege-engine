//
//  pipeline-loader.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//


#include "shaderc/shaderc.hpp"
#include "pipeline-loader.hpp"
#include "string-to-enum-types.hpp"

namespace kege{

    std::vector< uint32_t > compileGlslToSpv
    (
        const char* shader_name,
        ShaderStageFlag shader_stage,
        const std::vector< char >& source
    )
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        shaderc_shader_kind shaderc_shader_type;

        switch ( shader_stage )
        {
            case kege::ShaderStageFlag::Compute:
            {
                shaderc_shader_type = shaderc_compute_shader;
                break;
            }
            case kege::ShaderStageFlag::Vertex:
            {
                shaderc_shader_type = shaderc_vertex_shader;
                break;
            }
            case kege::ShaderStageFlag::Fragment:
            {
                shaderc_shader_type = shaderc_fragment_shader;
                break;
            }
            case kege::ShaderStageFlag::Geometry:
            {
                shaderc_shader_type = shaderc_geometry_shader;
                break;
            }
            case kege::ShaderStageFlag::TessellationEvaluation:
            {
                shaderc_shader_type = shaderc_tess_evaluation_shader;
                break;
            }
            case kege::ShaderStageFlag::TessellationControl:
            {
                shaderc_shader_type = shaderc_tess_control_shader;
                break;
            }
            default:
            {
                kege::Log::error <<"Invalid shader state in compileGlslTextToByteCode()" << Log::nl;
                return {};
            }
        }
        shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv
        (
            source.data(),
            source.size() - 1,
            shaderc_shader_type,
            shader_name,
            options
        );
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            kege::Log::error <<"error in compileGlslTextToByteCode(): "<< result.GetErrorMessage().data() << Log::nl;
            return {};
        }
        return std::vector< uint32_t >(result.cbegin(), result.cend());
    }

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

    kege::ShaderStageFlag converShaderStage( const std::string& stage )
    {
        kege::ShaderStageFlag shader_type = {};
        if ( stage == "vertex" )
        {
            shader_type = kege::ShaderStageFlag::Vertex;
        }
        else if ( stage == "fragment" )
        {
            shader_type = kege::ShaderStageFlag::Fragment;
        }
        else if ( stage == "compute" )
        {
            shader_type = kege::ShaderStageFlag::Compute;
        }
        else if ( stage == "geometry" )
        {
            shader_type = kege::ShaderStageFlag::Geometry;
        }
        else if ( stage == "tess-control" )
        {
            shader_type = kege::ShaderStageFlag::TessellationControl;
        }
        else if ( stage == "tess-evaluation" )
        {
            shader_type = kege::ShaderStageFlag::TessellationEvaluation;
        }
        else
        {
            return kege::ShaderStageFlag::Invalid;
        }
        return shader_type;
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

//    bool createShaderModules
//    (
//        kege::Graphics* graphics,
//        kege::GraphicsPipelineDesc* info,
//        const std::string& path,
//        kege::Json json
//    )

//
//    bool createDescriptorSetLayout
//    (
//        kege::Graphics* graphics,
//        std::vector< kege::UniformSetLayout >& descriptor_set_layouts,
//        kege::Json descriptors
//    )
//    {
//        if ( descriptors.count() == 0 )
//        {
//            return false;
//        }
//        using LayoutBinding = std::map< int, kege::UniformSetDesc >;
//        LayoutBinding dslb_map;
//        for (int i = 0; i < descriptors.count(); ++i)
//        {
//            kege::UniformDesc dslb;
//
//            kege::Json descriptor =  descriptors[i];
//            dslb.descriptor_type = convertDescriptorType( descriptor[ "type" ].toStr() );
//            dslb.stage_flags = convertShaderStageFlages( descriptor[ "stages" ] );
//            dslb.binding = descriptor[ "binding" ].toInt();
//            dslb.count = descriptor[ "count" ].toInt();
//            dslb.name = descriptor[ "name" ].toStr();
//
//            kege::UniformSetDesc& dsl = dslb_map[ descriptor[ "set" ].toInt() ];
//            dsl.push_back( dslb );
//        }
//
//        for ( LayoutBinding::iterator m = dslb_map.begin(); m != dslb_map.end(); m++ )
//        {
//            //kege::UniformSetLayout handle = graphics->createUniformSetLayout( m->second );
//            //descriptor_set_layouts.push_back( handle );
//        }
//        return true;
//    }
//
//    bool parsePushConstantRange
//    (
//        kege::Graphics* graphics,
//        std::vector< kege::PushConstantRange >& push_constants_ranges,
//        kege::Json json
//    )
//    {
//        if ( json.count() == 0 )
//        {
//            return false;
//        }
//
//        for (int i = 0; i < json.count(); ++i)
//        {
//            kege::PushConstantRange constant;
//            constant.stage_flags = convertShaderStageFlages( json[i][ "stages" ] );
//            constant.offset = json[i][ "offset" ].toInt();
//            constant.size = json[i][ "size" ].toInt();
//            push_constants_ranges.push_back( constant );
//        }
//
//        return true;
//    }
//
//    bool createPipelineLayout
//    (
//        kege::Graphics* graphics,
//        kege::GraphicsPipelineDesc* info,
//        const std::string& name,
//        kege::Json json
//    )
//    {
//        if ( json.count() == 0 )
//        {
//            return false;
//        }
//        kege::PipelineLayoutDesc layout_info;
//        createDescriptorSetLayout( graphics, layout_info.descriptor_set_layouts, json[ "descriptor_set_layouts" ] );
//        parsePushConstantRange( graphics, layout_info.push_constant_ranges, json[ "push_constants_ranges" ] );
//        info->pipeline_layout = graphics->createPipelineLayout( layout_info );
//        info->name = name;
//        if ( info->pipeline_layout.id < 0 )
//        {
//            kege::Log::error << "pipeline-layout create failed in createPipelineLayout()" << Log::nl;
//            return false;
//        }
//        return true;
//    }
//
//    bool parseVertexInput
//    (
//        kege::Graphics* graphics,
//        kege::GraphicsPipelineDesc* info,
//        kege::Json json
//    )
//    {
//        kege::Json params = json[ "vertex_bindings" ];
//        if ( params.count() != 0 )
//        {
//            for (int i = 0; i < params.count(); ++i)
//            {
//                //kege::VertexInputBindingDesc v;
//                info->vertex_input_state.strides.push_back( params[i]["stride"].toInt() );
//            }
//        }
//        params = json[ "vertex_inputs" ];
//        if ( params.count() != 0 )
//        {
//            for (int i = 0; i < params.count(); ++i)
//            {
//                kege::VertexInputAttributeDesc v;
//                v.location = params[i]["location"].toInt();
//                v.binding = params[i]["binding"].toInt();
//                v.offset = params[i]["offset"].toInt();
//                v.type = convertVertexInputType( params[i][ "format" ].toStr() );
//                //v.stride = params[i]["stride"].toInt();
//                v.binding = params[i]["binding"].toInt();
//                v.input_rate = ( strcmp("vertex", params[i]["input_rate"].toStr()) == 0 )
//                ? kege::VertexInputRate::Vertex : kege::VertexInputRate::Instance;
//                info->vertex_input_state.attributes.push_back( v );
//            }
//        }
//        return true;
//    }
//
    void parseDepthStencilStates( std::vector< DepthStencilStateDesc >* depth_stencil_states, kege::Json json )
    {
        for (int i = 0; i < json.count(); ++i)
        {
            kege::Json elems = json[i];

            DepthStencilStateDesc state = {};
            state.depth_test_enable = elems[ "depth_test" ].toBool( false );
            state.depth_write_enable = elems[ "depth_write" ].toBool( true );
            state.depth_compare_op = stringToCompareOp( elems[ "depth_compare_op" ].getStr( "less" ) );

            state.stencil_test_enable = elems[ "stencil_test_enable" ].toBool( false );
            if ( state.stencil_test_enable )
            {
                Json front_op = elems[ "front_op" ];
                state.front_op.fail_op = stringToStencilOp( front_op[ "fail_op" ].getStr( "keep" ) );
                state.front_op.depth_fail_op = stringToStencilOp( front_op[ "depth_fail_op" ].getStr( "keep" ) );
                state.front_op.compare_op = stringToCompareOp( front_op[ "compare_op" ].getStr( "always" ) );
                state.front_op.compare_mask = front_op[ "compare_mask" ].toInt( 0xFFFFFFFF );
                state.front_op.write_mask = front_op[ "write_mask" ].toInt( 0xFFFFFFFF );
                state.front_op.reference = front_op[ "reference" ].toInt( 0 );

                Json back_op = elems[ "back_op" ];
                state.back_op.fail_op = stringToStencilOp( back_op[ "fail_op" ].getStr( "keep" ) );
                state.back_op.depth_fail_op = stringToStencilOp( back_op[ "depth_fail_op" ].getStr( "keep" ) );
                state.back_op.compare_op = stringToCompareOp( back_op[ "compare_op" ].getStr( "keep" ) );
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
            state.blend_enable = json[ "blend_enable" ].toBool( false );
            state.src_color_blend_factor = stringToBlendFactor( json[ "src_color_blend_factor" ].getStr( "one" ) );
            state.dst_color_blend_factor = stringToBlendFactor( json[ "dst_color_blend_factor" ].getStr( "zero" ) );
            state.color_blend_op = stringToBlendOp( json[ "color_blend_op" ].getStr( "add" ) );
            state.src_alpha_blend_factor = stringToBlendFactor( json[ "src_alpha_blend_factor" ].getStr( "one" ) );
            state.dst_alpha_blend_factor = stringToBlendFactor( json[ "dst_alpha_blend_factor" ].getStr( "zero" ) );
            state.alpha_blend_op = stringToBlendOp( json[ "alpha_blend_op" ].getStr( "add" ) );
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
            state.primitive_restart_enable = true;
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

//    BindingType stringToBindingType( const char* str )
//    {
//        static std::map< std::string, kege::BindingType > table;
//        if( table.empty() )
//        {
//            table[ "BUFFER" ] = kege::BindingType::BUFFER;
//            table[ "TEXTURE" ] = kege::BindingType::TEXTURE;
//            table[ "PUSH_CONSTANT" ] = kege::BindingType::PUSH_CONSTANTS;
//            table[ "SHADER_RESOURCE" ] = kege::BindingType::SHADER_RESOURCE;
//        }
//        auto m = table.find( str );
//        if ( m != table.end() )
//        {
//            return m->second;
//        }
//        Log::error << "invalid BindingType -> " << str <<Log::nl;
//        return kege::BindingType::BUFFER;
//    }

    std::vector< std::string > parsePipelineGlobals( kege::Json json )
    {
        std::vector< std::string > bindings;
//        if ( json.count() )
//        {
//            bindings.resize( json.count() );
//            for (int i = 0; i < json.count(); ++i)
//            {
//                Json element = json[i];
//                PipelineResourceBinding binding;
//                binding.name = element[ "name" ].value();
//                //TODO: binding.type = stringToBindingType( element[ "type" ].value() );
//                bindings[i] = binding;
//            }
//        }
        return bindings;
    }

    void parseColorBlendStates( std::vector< ColorBlendStateDesc >* color_blend_states, kege::Json json )
    {
        for (int i = 0; i < json.count(); ++i)
        {
            kege::Json desc = json[i];

            ColorBlendStateDesc state = {};
            state.logic_op_enable = desc[ "logic_op_enable" ].toBool( false );
            state.logic_op = stringToColorBlendLogicOp( desc[ "logic_op" ].getStr( "copy") );

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
        if ( s == "data-block" ) return ShaderStructType::DataBlock;
        if ( s == "image" ) return ShaderStructType::Image;
        if ( s == "sampler" ) return ShaderStructType::Sampler;
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
                fields[k].type = stringToShaderVarType( field.getStr("type") );//, ShaderVarType::Vec3 );
                fields[k].count = field.getInt("count");
                fields[k].size = field.getInt("size");

                fields[k].offset = offset;
                offset += fields[k].size;
            }

            uniforms->push_back( new kege::ShaderStructBlock( type, name, fields ) );
        }
        return true;
    }

//    UniformDescriptorSets parsePipelineUniformLayouts( kege::Json json )
//    {
//        using LayoutBinding = std::map< int, kege::UniformDescriptorSet >;
//        LayoutBinding dslb_map;
//        for (int i = 0; i < json.count(); ++i)
//        {
//            kege::Json layout =  json[i];
//            int set = layout[ "set" ].toInt();
//            std::string name = layout[ "name" ].toStr();
//            Json bindings = layout[ "bindings" ];
//
//            kege::UniformDescriptorSet& dsl = dslb_map[ set ];
//            dsl.set = set;
//            dsl.descriptors.reserve( bindings.count() );
//            for (int i = 0; i < bindings.count(); ++i)
//            {
//                Json bind = bindings[i];
//
//                kege::UniformDescriptor ud = {};
//                ud.name = bind[ "name" ].toStr();
//                ud.descriptor_type = convertDescriptorType( bind[ "type" ].toStr() );
//                //dslb.stage_flags = convertShaderStageFlages( bind[ "stages" ] );
//                ud.binding = bind[ "binding" ].toInt();
//                ud.count = bind[ "count" ].toInt();
//                dsl.descriptors.push_back( ud );
//            }
//        }
//
//        UniformDescriptorSets desc_sets;
//        if ( !dslb_map.empty() )
//        {
//            desc_sets.reserve( dslb_map.size() );
//            for ( LayoutBinding::iterator m = dslb_map.begin(); m != dslb_map.end(); m++ )
//            {
//                desc_sets.push_back( m->second );
//            }
//        }
//        return desc_sets;
//    }

//    std::vector< ref::ShaderBindingPointConstant > parsePipelinePushCnstants( kege::Json json )
//    {
//        std::vector< ref::ShaderBindingPointConstant > push_constants( json.count() );
//        for (int i = 0; i < json.count(); ++i)
//        {
//            push_constants[i]->stages = convertShaderStageFlages( json[i][ "stages" ] );
//            push_constants[i]->offset = json[i][ "offset" ].toInt();
//            push_constants[i]->size = json[i][ "size" ].toInt();
//            push_constants[i]->name = json[i][ "name" ].toStr();
//
//            Json fields = json[ "fields" ];
//            std::vector< ShaderStructField > shader_block_fields( json[ "fields" ].count() );
//            for (int f = 0; f < fields.count(); ++f)
//            {
//                shader_block_fields[f].name   = fields[ "name" ].value();
//                shader_block_fields[f].type   = stringToMemberType( fields[ "type" ].value() );
//                shader_block_fields[f].offset = fields[ "offset" ].toInt();
//                shader_block_fields[f].size   = fields[ "size" ].toInt();
//            }
//            push_constants[i]->struct_block = new ShaderStructBlock( shader_block_fields );
//        }
//        return push_constants;
//    }

    ShaderStageFlag parseShaderStageFlag( const Json& json )
    {
        ShaderStageFlag stages = stringToShaderStageFlag( json[0].value() );
        for (int i = 1; i < json.count(); ++i )
        {
            stages = stages | stringToShaderStageFlag( json[i].value() );
        }
        return stages;
    }

    PipelinInfo::Push parsePush( const Json& json )
    {
        PipelinInfo::Push l = {};
        l.size = json["size"].toInt();
        l.offset = json["offset"].toInt();
        l.count = json["count"].toInt();
        l.uniform = json["uniform_ref"].toInt();
        l.name = json["name"].toStr();
        l.stages = parseShaderStageFlag( json["stages"] );
        return l;
    }

    void parsePushs( const Json& json, std::vector< PipelinInfo::Push >* pushs )
    {
        pushs->reserve( json.count() );
        for (int i = 0; i < json.count(); ++i )
        {
            pushs->push_back( parsePush( json[i] ) );
        }
    }

    PipelinInfo::Layout parseLayout( const Json& json )
    {
        PipelinInfo::Layout l = {};
        l.set = json["set"].toInt();
        l.binding = json["binding"].toInt();
        l.count = json["count"].toInt();
        l.uniform = json["uniform_ref"].toInt();
        l.name = json["name"].toStr();
        l.usage = stringToBindingUsage( json["usage"].value() );
        l.stages = parseShaderStageFlag( json["stages"] );
        return l;
    }

    void parseLayouts( const Json& json, std::map< int, std::vector< PipelinInfo::Layout > >* layouts )
    {
        for (int i = 0; i < json.count(); ++i )
        {
            PipelinInfo::Layout layout = parseLayout( json[i] );
            (*layouts)[ layout.set ].push_back( layout );
        }
    }

    void parseOutputs( const Json& json, std::vector< PipelinInfo::Output >* outputs )
    {
        outputs->reserve( json.count() );
        for (int i = 0; i < json.count(); ++i )
        {
            outputs->at(i).location = json["location"].toInt();
            outputs->at(i).format = stringToFormat( json["format"].toStr() );
            outputs->at(i).semantic = json["semantic"].toStr();
            outputs->at(i).name = json["name"].toStr();
        }
    }

    int atoI( const kege::Json& json );
    void parsePipelines( std::vector< PipelinInfo >* pipelines, kege::Json json )
    {
        pipelines->reserve( json.count() );
        for (int i = 0; i < json.count(); ++i )
        {
            PipelinInfo info = {};
            pipelines->at(i).name = json.getStr("name");
            pipelines->at(i).topology = stringToPrimitiveTopology( json.getStr("topology") );
            pipelines->at(i).rasterizer = json.getInt("rasterizer");
            pipelines->at(i).depth_stencil = json.getInt("depth_stencil");
            pipelines->at(i).color_blend = json.getInt("color_blend");
            parseOutputs( json[ "outputs" ], &pipelines->at(i).outputs );
            parsePushs( json["layout"], &pipelines->at(i).push_constants );
            parseLayouts( json["layout"], &pipelines->at(i).sets );
            pipelines->at(i).stages = json.toVector<int>( atoi );
        }
    }

    bool parseShaderStages
    (
        kege::Graphics* graphics, std::vector< ref::Shader >* shaders, kege::Json json, const std::string& path
    )
    {
        std::vector< ref::Shader > shader_stages;
        for (int i = 0; i < json.count(); ++i)
        {
            Json element = json[i];

            kege::ShaderDesc shader_desc;
            shader_desc.stage = converShaderStage( element[ "type" ].toStr() );
            shader_desc.entry_point = element[ "entry-point" ].toStr();
            shader_desc.name = element[ "name" ].toStr();

            const char* uri = element[ "uri" ].toStr();
            std::string filename = path + uri;

            std::vector< char > source;
            if( !loadTextFile( source, filename.data()) )
            {
                kege::Log::error << "fail open shader file -> " << filename << Log::nl;
                return false;
            }

            shader_desc.byte_code = compileGlslToSpv( shader_desc.name.data(), shader_desc.stage, source );
            if( shader_desc.byte_code.empty() )
            {
                return false;
            }
            shader_stages.push_back( graphics->createShader( shader_desc ) );
        }
        return true;
    }

    bool parseShaderPipeline
    (
        kege::Graphics* graphics,
        kege::Json json,
        const std::string& path,
        std::vector< ref::ShaderStructBlock >* uniforms,
        std::vector< RasterizationStateDesc >* rasterizer_states,
        std::vector< DepthStencilStateDesc >* depth_stencil_states,
        std::vector< ColorBlendStateDesc >* color_blend_states,
        std::vector< VertexBufferLayout >* vertex_layouts,
        std::vector< PipelinInfo >* pipelines,
        std::vector< ref::Shader >* shaders
    )
    {
        parseShaderStructBlocks( uniforms, json[ "uniforms" ] );
        parseColorBlendStates( color_blend_states, json[ "color_blend_states" ] );
        parseDepthStencilStates( depth_stencil_states, json[ "depth_stencil_states" ] );
        parseRasterizationState( rasterizer_states, json[ "rasterizer_states" ] );
        parsePipelineVertexInput( vertex_layouts, json[ "vertex_layouts" ] );
        parseShaderStages( graphics, shaders, json[ "shaders" ], path );
        parsePipelines( pipelines, json[ "pipelines" ] );
        return true;
    }



    std::vector< ref::ShaderPipeline > PipelineLoader::load
    (
        kege::Graphics* graphics, const std::string& filename
    )
    {
        Json json = JsonParser::load( filename.data() );
        if ( json )
        {
            kege::Log::error << "FILE_LOAD_FAILED -> " << filename << Log::nl;
            return {};
        }

        std::string path;

        std::vector< ref::ShaderStructBlock > uniforms;
        std::vector< RasterizationStateDesc > rasterizer_states;
        std::vector< DepthStencilStateDesc > depth_stencil_states;
        std::vector< ColorBlendStateDesc > color_blend_states;
        std::vector< VertexBufferLayout > vertex_layouts;
        std::vector< PipelinInfo > pipelines;
        std::vector< ref::Shader > shaders;

        parseShaderPipeline
        (
            graphics, json, path,
            &uniforms,
            &rasterizer_states,
            &depth_stencil_states,
            &color_blend_states,
            &vertex_layouts,
            &pipelines,
            &shaders
        );


        std::vector< ref::ShaderPipeline > shader_pipelines;
        for (const PipelinInfo& info : pipelines)
        {
            PipelineCreateInfo create_info = {};

            create_info.name = info.name;
            create_info.pipeline_type = info.type;
            create_info.input_assembly.topology = info.topology;
            create_info.color_blend = color_blend_states[ info.color_blend ];
            create_info.depth_stencil = depth_stencil_states[ info.depth_stencil ];
            create_info.rasterization = rasterizer_states[ info.rasterizer ];
            create_info.vertex_input = vertex_layouts[ info.vertex_layout ];
            create_info.shaders.reserve( info.stages.size() );

            for (int i = 0; i<info.stages.size(); ++i)
            {
                create_info.shaders.push_back( shaders[ info.stages[i] ] );
            }

            create_info.shader_layout.set_layout_config.resize( info.sets.size() );
            for (const auto& [set_index, layout] : info.sets)
            {
                IndexedSetConfig& set = create_info.shader_layout.set_layout_config[ set_index ];
                set.bindings.resize( layout.size() );
                set.index = set_index;

                for (int i = 0; i<layout.size(); ++i)
                {
                    set.bindings[i].binding = layout[i].binding;
                    set.bindings[i].name    = layout[i].name;
                    set.bindings[i].count   = layout[i].count;
                    set.bindings[i].stages  = layout[i].stages;
                    set.bindings[i].usage   = layout[i].usage;

                    if ( layout[i].uniform >= 0 )
                    {
                        set.bindings[i].block = uniforms[ layout[i].uniform ];
                    }
                }
            }

            create_info.shader_layout.push_block_layout.resize( info.push_constants.size() );
            for (int i = 0; i<info.push_constants.size(); ++i)
            {
                PushBlock& block = create_info.shader_layout.push_block_layout[i];
                block.size = info.push_constants[i].size;
                block.offset = info.push_constants[i].offset;
                block.name = info.push_constants[i].name;
                block.stages = info.push_constants[i].stages;
                block.count = info.push_constants[i].count;
                if ( info.push_constants[i].uniform >= 0 )
                {
                    block.block = uniforms[ info.push_constants[i].uniform ];
                }
            }

            ref::ShaderPipeline sp = graphics->createShaderPipeline( create_info );
            shader_pipelines.push_back( sp );
        }
        return {};
    }
}
