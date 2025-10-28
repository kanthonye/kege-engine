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

    bool createShaderModules
    (
        kege::Graphics* graphics,
        kege::GraphicsPipelineDesc* info,
        const std::string& path,
        kege::Json json
    )
    {
        json = json[ "stages" ];
        if ( json.count() == 0 )
        {
            kege::Log::error <<"empty shader stages not allowed in loadShaderPipelineParams()" << Log::nl;
            return {};
        }

        for (int i = 0; i < json.count(); ++i)
        {
            std::string shader_filename = path + json[i][ "file" ].value();
            std::vector< char > source;
            if( !loadTextFile( source, shader_filename.data()) )
            {
                kege::Log::error << "fail open shader file -> " << shader_filename << Log::nl;
                return false;
            }

            kege::ShaderDesc shader_desc;
            shader_desc.stage = converShaderStage( json[i][ "type" ].value() );
            shader_desc.entry_point = json[i][ "entry-point" ].value();
            shader_desc.name = json[i][ "debug-name" ].value();
            shader_desc.byte_code = compileGlslToSpv( shader_desc.name.data(), shader_desc.stage, source );
            if( shader_desc.byte_code.empty() )
            {
                return false;
            }
            kege::ShaderHandle shader_handle = graphics->createShader( shader_desc );
            info->shader_stages.push_back( shader_handle );
        }
        return true;
    }

    bool createDescriptorSetLayout
    (
        kege::Graphics* graphics,
        std::vector< kege::UniformSetLayout >& descriptor_set_layouts,
        kege::Json descriptors
    )
    {
        if ( descriptors.count() == 0 )
        {
            return false;
        }
        using LayoutBinding = std::map< int, kege::UniformSetDesc >;
        LayoutBinding dslb_map;
        for (int i = 0; i < descriptors.count(); ++i)
        {
            kege::UniformDesc dslb;

            kege::Json descriptor =  descriptors[i];
            dslb.descriptor_type = convertDescriptorType( descriptor[ "type" ].toStr() );
            dslb.stage_flags = convertShaderStageFlages( descriptor[ "stages" ] );
            dslb.binding = descriptor[ "binding" ].toInt();
            dslb.count = descriptor[ "count" ].toInt();
            dslb.name = descriptor[ "name" ].toStr();

            kege::UniformSetDesc& dsl = dslb_map[ descriptor[ "set" ].toInt() ];
            dsl.push_back( dslb );
        }

        for ( LayoutBinding::iterator m = dslb_map.begin(); m != dslb_map.end(); m++ )
        {
            //kege::UniformSetLayout handle = graphics->createUniformSetLayout( m->second );
            //descriptor_set_layouts.push_back( handle );
        }
        return true;
    }

    bool parsePushConstantRange
    (
        kege::Graphics* graphics,
        std::vector< kege::PushConstantRange >& push_constants_ranges,
        kege::Json json
    )
    {
        if ( json.count() == 0 )
        {
            return false;
        }

        for (int i = 0; i < json.count(); ++i)
        {
            kege::PushConstantRange constant;
            constant.stage_flags = convertShaderStageFlages( json[i][ "stages" ] );
            constant.offset = json[i][ "offset" ].toInt();
            constant.size = json[i][ "size" ].toInt();
            push_constants_ranges.push_back( constant );
        }

        return true;
    }

    bool createPipelineLayout
    (
        kege::Graphics* graphics,
        kege::GraphicsPipelineDesc* info,
        const std::string& name,
        kege::Json json
    )
    {
        if ( json.count() == 0 )
        {
            return false;
        }
        kege::PipelineLayoutDesc layout_info;
        createDescriptorSetLayout( graphics, layout_info.descriptor_set_layouts, json[ "descriptor_set_layouts" ] );
        parsePushConstantRange( graphics, layout_info.push_constant_ranges, json[ "push_constants_ranges" ] );
        info->pipeline_layout = graphics->createPipelineLayout( layout_info );
        info->name = name;
        if ( info->pipeline_layout.id < 0 )
        {
            kege::Log::error << "pipeline-layout create failed in createPipelineLayout()" << Log::nl;
            return false;
        }
        return true;
    }

    bool parseVertexInput
    (
        kege::Graphics* graphics,
        kege::GraphicsPipelineDesc* info,
        kege::Json json
    )
    {
        kege::Json params = json[ "vertex_bindings" ];
        if ( params.count() != 0 )
        {
            for (int i = 0; i < params.count(); ++i)
            {
                //kege::VertexInputBindingDesc v;
                info->vertex_input_state.strides.push_back( params[i]["stride"].toInt() );
            }
        }
        params = json[ "vertex_inputs" ];
        if ( params.count() != 0 )
        {
            for (int i = 0; i < params.count(); ++i)
            {
                kege::VertexInputAttributeDesc v;
                v.location = params[i]["location"].toInt();
                v.binding = params[i]["binding"].toInt();
                v.offset = params[i]["offset"].toInt();
                v.type = convertVertexInputType( params[i][ "format" ].toStr() );
                //v.stride = params[i]["stride"].toInt();
                v.binding = params[i]["binding"].toInt();
                v.input_rate = ( strcmp("vertex", params[i]["input_rate"].toStr()) == 0 )
                ? kege::VertexInputRate::Vertex : kege::VertexInputRate::Instance;
                info->vertex_input_state.attributes.push_back( v );
            }
        }
        return true;
    }

    DepthStencilStateDesc getDepthStencilState( kege::Json json )
    {
        DepthStencilStateDesc desc = {};
        if ( json )
        {
            desc.depth_test_enable = json[ "depth_test" ].toBool( false );
            desc.depth_write_enable = json[ "depth_write" ].toBool( true );
            desc.depth_compare_op = stringToCompareOp( json[ "depth_compare_op" ].getStr( "less" ) );

            desc.stencil_test_enable = json[ "stencil_test_enable" ].toBool( false );
            if ( desc.stencil_test_enable )
            {
                Json front_op = json[ "front_op" ];
                desc.front_op.fail_op = stringToStencilOp( front_op[ "fail_op" ].getStr( "keep" ) );
                desc.front_op.depth_fail_op = stringToStencilOp( front_op[ "depth_fail_op" ].getStr( "keep" ) );
                desc.front_op.compare_op = stringToCompareOp( front_op[ "compare_op" ].getStr( "always" ) );
                desc.front_op.compare_mask = front_op[ "compare_mask" ].toInt( 0xFFFFFFFF );
                desc.front_op.write_mask = front_op[ "write_mask" ].toInt( 0xFFFFFFFF );
                desc.front_op.reference = front_op[ "reference" ].toInt( 0 );

                Json back_op = json[ "back_op" ];
                desc.back_op.fail_op = stringToStencilOp( back_op[ "fail_op" ].getStr( "keep" ) );
                desc.back_op.depth_fail_op = stringToStencilOp( back_op[ "depth_fail_op" ].getStr( "keep" ) );
                desc.back_op.compare_op = stringToCompareOp( back_op[ "compare_op" ].getStr( "keep" ) );
                desc.back_op.compare_mask = back_op[ "compare_mask" ].toInt( 0xFFFFFFFF );
                desc.back_op.write_mask = back_op[ "write_mask" ].toInt( 0xFFFFFFFF );
                desc.back_op.reference = back_op[ "reference" ].toInt( 0 );
            }
        }
        return desc;
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

    ColorBlendStateDesc getColorBlendState( kege::Json json )
    {
        ColorBlendStateDesc desc = {};
        if ( json )
        {
            desc.logic_op_enable = json[ "logic_op_enable" ].toBool( false );
            desc.logic_op = stringToColorBlendLogicOp( json[ "logic_op" ].getStr( "copy") );

            Json color_blend_attachments = json[ "color_blend_attachments" ];
            for (int i=0; i<color_blend_attachments.count(); ++i)
            {
                desc.attachments.push_back( getColorBlendAttachmentState( color_blend_attachments[i] ) );
            }
        }
        else
        {
            desc.attachments.push_back( ColorBlendAttachmentState::createAlphaBlending() );

        }
        return desc;
    }

    RasterizationStateDesc getRasterizationState( kege::Json json )
    {
        RasterizationStateDesc desc = {};
        if ( json )
        {
            Json rasterizer_disable = json["rasterizer_disable"];
            if( rasterizer_disable )
            {
                if ( strcmp(rasterizer_disable.value(), "true") == 0 )
                {
                    desc.rasterizer_disable = true;
                }
                else
                {
                    desc.rasterizer_disable = false;
                }
            }

            Json polygon_mode = json["polygon_mode"];
            desc.polygon_mode = stringToPolygonMode( polygon_mode.value() );

            Json cull_mode = json["cull_mode"];
            desc.cull_mode = stringToCullMode( cull_mode.value() );

            Json front_face = json["front_face"];
            desc.front_face = stringToFrontFace( front_face.value() );

            Json line_width = json["line_width"];
            Json depth_clamp_enable = json["depth_clamp_enable"];
            Json depth_bias_constant_factor = json["depth_bias_constant_factor"];
            Json depth_bias_clamp = json["depth_bias_clamp"];
            Json depth_bias_slope_factor = json["depth_bias_slope_factor"];
        }
        return desc;
    }
}




namespace kege{

    kege::PipelineHandle PipelineLoader::load
    (
        kege::Graphics* graphics,
        const std::string& filename
    )    
    {
        kege::Json json = kege::JsonParser::load( filename.data() );
        if ( !json )
        {
            kege::Log::error <<"fail to open file -> " << filename << Log::nl;
            return {};
        }
        kege::GraphicsPipelineDesc info;
        std::string path = getFilePath( filename );
        PipelineLoader::createPipelineFromFile( graphics, &info, json, path );
        return {};
    }

    bool PipelineLoader::createPipelineFromFile
    (
        kege::Graphics* graphics,
        kege::GraphicsPipelineDesc* info,
        kege::Json json,
        const std::string& path
    )
    {
        info->name = json[ "name" ].value();

        info->color_blend_state = getColorBlendState( json[ "color_blend_state" ] );
        info->color_blend_state = getColorBlendState( json[ "color_blend_state" ] );
        info->depth_stencil_state = getDepthStencilState( json[ "depth_stencil_state" ] );
        info->rasterization_state = getRasterizationState( json[ "rasterization_state" ] );
        info->input_assembly_state.primitive_restart_enable = true;
        info->input_assembly_state.topology = stringToPrimitiveTopology( json["primatives"][0].getStr( "triangle-strip" ) );

        Json rendering_output = json[ "rendering_output" ];
        if ( rendering_output )
        {
            Json color_output_formats = rendering_output["color_output_formats"];
            for (int i = 0; i < color_output_formats.count(); ++i)
            {
                Format format = stringToFormat( color_output_formats[i].value() );
                info->color_attachment_formats.push_back( format );
            }
            Json depth_output_format = rendering_output["depth_output_format"];
            if ( depth_output_format )
            {
                Format format = stringToFormat( depth_output_format.value() );
                info->depth_attachment_format = format;
            }
            Json stencil_output_format = rendering_output["stencil_output_format"];
            if ( stencil_output_format )
            {
                Format format = stringToFormat( stencil_output_format.value() );
                info->stencil_attachment_format = format;
            }
        }

        if( !createShaderModules( graphics, info, path, json ) )
        {
            return false;
        }
        createPipelineLayout( graphics, info, info->name, json );
        parseVertexInput( graphics, info, json );
        return true;
    }

}







namespace kege{

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

    PipelineStates parsePipelineStates( kege::Json json )
    {
        PipelineStates states = {};
        if ( json )
        {
            states.color_blend   = getColorBlendState( json[ "color_blend" ] );
            states.depth_stencil = getDepthStencilState( json[ "depth_stencil" ] );
            states.multisample   = {};//getMultisampleStateDesc( json[ "multisample" ] );
            states.rasterization = getRasterizationState( json[ "rasterizer" ] );
            states.input_assembly = inputAssembly( json["input_assembly"] );
        }
        return states;
    }

    UniformDescriptorSets parsePipelineUniformLayouts( kege::Json json )
    {
        using LayoutBinding = std::map< int, kege::UniformDescriptorSet >;
        LayoutBinding dslb_map;
        for (int i = 0; i < json.count(); ++i)
        {
            kege::Json layout =  json[i];
            int set = layout[ "set" ].toInt();
            std::string name = layout[ "name" ].toStr();
            Json bindings = layout[ "bindings" ];

            kege::UniformDescriptorSet& dsl = dslb_map[ set ];
            dsl.set = set;
            dsl.descriptors.reserve( bindings.count() );
            for (int i = 0; i < bindings.count(); ++i)
            {
                Json bind = bindings[i];

                kege::UniformDescriptor ud = {};
                ud.name = bind[ "name" ].toStr();
                ud.descriptor_type = convertDescriptorType( bind[ "type" ].toStr() );
                //dslb.stage_flags = convertShaderStageFlages( bind[ "stages" ] );
                ud.binding = bind[ "binding" ].toInt();
                ud.count = bind[ "count" ].toInt();
                dsl.descriptors.push_back( ud );
            }
        }

        UniformDescriptorSets desc_sets;
        if ( !dslb_map.empty() )
        {
            desc_sets.reserve( dslb_map.size() );
            for ( LayoutBinding::iterator m = dslb_map.begin(); m != dslb_map.end(); m++ )
            {
                desc_sets.push_back( m->second );
            }
        }
        return desc_sets;
    }

    std::vector< PushConstantInfo > parsePipelinePushCnstants( kege::Json json )
    {
        std::vector< PushConstantInfo > push_constants( json.count() );
        for (int i = 0; i < json.count(); ++i)
        {
            push_constants[i].stages = convertShaderStageFlages( json[i][ "stages" ] );
            push_constants[i].offset = json[i][ "offset" ].toInt();
            push_constants[i].size = json[i][ "size" ].toInt();
            push_constants[i].name = json[i][ "name" ].toStr();

            Json fields = json[ "fields" ];
            push_constants[i].fields.resize( fields.count() );
            for (int f = 0; f < fields.count(); ++f)
            {
                push_constants[i].fields[f].name   = fields[ "name" ].value();
                push_constants[i].fields[f].type   = stringToMemberType( fields[ "type" ].value() );
                push_constants[i].fields[f].offset = fields[ "offset" ].toInt();
                push_constants[i].fields[f].size   = fields[ "size" ].toInt();
            }
        }
        return push_constants;
    }

    VertexBufferLayout parsePipelineVertexInput( kege::Json json )
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
        return desc;
    }
//
//    enum SemanticType
//    {
//        POSITION, NORMAL, TEXCOORD, COLOR, TANGENT, WEIGHTHS, JOINTS
//    };
//    struct Output
//    {
//        int location;
//        Format format;
//        SemanticType semantic;
//        std::string name;
//    };

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

    BindingType stringToBindingType( const char* str )
    {
        static std::map< std::string, kege::BindingType > table;
        if( table.empty() )
        {
            table[ "BUFFER" ] = kege::BindingType::BUFFER;
            table[ "TEXTURE" ] = kege::BindingType::TEXTURE;
            table[ "PUSH_CONSTANT" ] = kege::BindingType::PUSH_CONSTANTS;
            table[ "SHADER_RESOURCE" ] = kege::BindingType::SHADER_RESOURCE;
        }
        auto m = table.find( str );
        if ( m != table.end() )
        {
            return m->second;
        }
        Log::error << "invalid BindingType -> " << str <<Log::nl;
        return kege::BindingType::BUFFER;

    }

    std::vector< PipelineResourceBinding > parsePipelineGlobals( kege::Json json )
    {
        std::vector< PipelineResourceBinding > bindings;
        if ( json.count() )
        {
            bindings.resize( json.count() );
            for (int i = 0; i < json.count(); ++i)
            {
                Json element = json[i];
                PipelineResourceBinding binding;
                binding.name = element[ "name" ].value();
                //TODO: binding.type = stringToBindingType( element[ "type" ].value() );
                bindings[i] = binding;
            }
        }
        return bindings;
    }

    std::vector< kege::PipelineInfo > parsePipelines( kege::Graphics* graphics, kege::Json json )
    {
        std::vector< kege::PipelineInfo > pipelines( json.count() );
        for (int i = 0; i < json.count(); ++i )
        {
            Json pipeline = json[i];
            kege::PipelineInfo* info = &pipelines[i];

            info->name = pipeline[ "name" ].toStr();
            info->stages = parseShaderStageIndices( pipeline[ "stages" ] ) ;
            info->states = parsePipelineStates( pipeline[ "states" ] );
            info->layouts = parsePipelineUniformLayouts( pipeline[ "layouts" ] );
            info->push_constants = parsePipelinePushCnstants( pipeline[ "push_constants" ] );
            info->vertex_input = parsePipelineVertexInput( pipeline[ "vertex_input" ] );
            info->outputs = parsePipelineOutputs( pipeline[ "outputs" ] );

            info->specialization_constants = parsePipelineSpecializationConstants( pipeline[ "specialization_constants" ] );
            info->global_resources = parsePipelineGlobals( pipeline[ "globals" ] );
        }
        return pipelines;
    }

    std::vector< kege::ShaderHandle > parseShaderStages
    (
        kege::Graphics* graphics, kege::Json shaders, const std::string& path
    )
    {
        std::vector< kege::ShaderHandle > shader_stages;
        for (int i = 0; i < shaders.count(); ++i)
        {
            Json element = shaders[i];

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
                return {};
            }

            shader_desc.byte_code = compileGlslToSpv( shader_desc.name.data(), shader_desc.stage, source );
            if( shader_desc.byte_code.empty() )
            {
                return {};
            }
            shader_stages.push_back( graphics->createShader( shader_desc ) );
        }
        return shader_stages;
    }

    bool parseShaderPipelineInfo
    (
        kege::Graphics* graphics,
        kege::CreateShaderPipelineInfo* info,
        kege::Json json,
        const std::string& path
    )
    {
        info->stages = parseShaderStages( graphics, json[ "shaders" ], path );
        info->pipelines = parsePipelines( graphics, json[ "pipelines" ] );
        return true;
    }




}
