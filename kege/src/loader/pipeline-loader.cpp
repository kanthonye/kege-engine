//
//  pipeline-loader.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//


#include "shaderc/shaderc.hpp"
#include "pipeline-loader.hpp"

namespace kege{

    std::vector< uint32_t > compileGlslToSpv
    (
        const char* shader_name,
        ShaderStage shader_stage,
        const std::vector< char >& source
    )
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        shaderc_shader_kind shaderc_shader_type;

        switch ( shader_stage )
        {
            case kege::ShaderStage::Compute:
            {
                shaderc_shader_type = shaderc_compute_shader;
                break;
            }
            case kege::ShaderStage::Vertex:
            {
                shaderc_shader_type = shaderc_vertex_shader;
                break;
            }
            case kege::ShaderStage::Fragment:
            {
                shaderc_shader_type = shaderc_fragment_shader;
                break;
            }
            case kege::ShaderStage::Geometry:
            {
                shaderc_shader_type = shaderc_geometry_shader;
                break;
            }
            case kege::ShaderStage::TessellationEvaluation:
            {
                shaderc_shader_type = shaderc_tess_evaluation_shader;
                break;
            }
            case kege::ShaderStage::TessellationControl:
            {
                shaderc_shader_type = shaderc_tess_control_shader;
                break;
            }
            default:
            {
                KEGE_LOG_ERROR <<"Invalid shader state in compileGlslTextToByteCode()" << Log::nl;
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
            KEGE_LOG_ERROR <<"error in compileGlslTextToByteCode(): "<< result.GetErrorMessage().data() << Log::nl;
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

    kege::ShaderStage converShaderStage( const std::string& stage )
    {
        kege::ShaderStage shader_type = {};
        if ( stage == "vertex" )
        {
            shader_type = kege::ShaderStage::Vertex;
        }
        else if ( stage == "fragment" )
        {
            shader_type = kege::ShaderStage::Fragment;
        }
        else if ( stage == "compute" )
        {
            shader_type = kege::ShaderStage::Compute;
        }
        else if ( stage == "geometry" )
        {
            shader_type = kege::ShaderStage::Geometry;
        }
        else if ( stage == "tess-control" )
        {
            shader_type = kege::ShaderStage::TessellationControl;
        }
        else if ( stage == "tess-evaluation" )
        {
            shader_type = kege::ShaderStage::TessellationEvaluation;
        }
        else
        {
            return kege::ShaderStage::Invalid;
        }
        return shader_type;
    }

    kege::ShaderStage convertShaderStageFlages( kege::Json json )
    {
        kege::ShaderStage stages = kege::ShaderStage::Invalid;
        for (int i = 0; i < json.count(); ++i)
        {
            stages |= convertShaderStage( json[i].getString() );
        }
        return stages;
    }

    bool createShaderModules
    (
        kege::GraphicsDevice* device,
        kege::GraphicsPipelineDesc* info,
        const std::string& path,
        kege::Json json
    )
    {
        json = json[ "stages" ];
        if ( json.count() == 0 )
        {
            KEGE_LOG_ERROR <<"empty shader stages not allowed in loadShaderPipelineParams()" << Log::nl;
            return {};
        }

        for (int i = 0; i < json.count(); ++i)
        {
            std::string shader_filename = path + json[i][ "file" ].value();
            std::vector< char > source;
            if( !loadTextFile( source, shader_filename.data()) )
            {
                KEGE_LOG_ERROR << "fail open shader file -> " << shader_filename << Log::nl;
                return false;
            }

            kege::ShaderDesc shader_desc;
            shader_desc.stage = converShaderStage( json[i][ "type" ].value() );
            shader_desc.entry_point = json[i][ "entry-point" ].value();
            shader_desc.debug_name = json[i][ "debug-name" ].value();
            shader_desc.byte_code = compileGlslToSpv( shader_desc.debug_name.data(), shader_desc.stage, source );
            if( shader_desc.byte_code.empty() )
            {
                return false;
            }
            kege::ShaderHandle shader_handle = device->createShader( shader_desc );
            info->shader_stages.push_back( shader_handle );
        }
        return true;
    }

    bool createDescriptorSetLayout
    (
        kege::GraphicsDevice* device,
        std::vector< kege::DescriptorSetLayoutHandle >& descriptor_set_layouts,
        kege::Json descriptors
    )
    {
        if ( descriptors.count() == 0 )
        {
            return false;
        }
        using LayoutBinding = std::map< int, kege::DescriptorSetLayoutBindings >;
        LayoutBinding dslb_map;
        for (int i = 0; i < descriptors.count(); ++i)
        {
            kege::DescriptorSetLayoutBinding dslb;

            kege::Json descriptor =  descriptors[i];
            dslb.descriptor_type = convertDescriptorType( descriptor[ "type" ].getString() );
            dslb.stage_flags = convertShaderStageFlages( descriptor[ "stages" ] );
            dslb.binding = descriptor[ "binding" ].getInt();
            dslb.count = descriptor[ "count" ].getInt();
            dslb.name = descriptor[ "name" ].getString();

            kege::DescriptorSetLayoutBindings& dsl = dslb_map[ descriptor[ "set" ].getInt() ];
            dsl.push_back( dslb );
        }

        for ( LayoutBinding::iterator m = dslb_map.begin(); m != dslb_map.end(); m++ )
        {
            kege::DescriptorSetLayoutHandle handle = device->createDescriptorSetLayout( m->second );
            descriptor_set_layouts.push_back( handle );
        }
        return true;
    }

    bool parsePushConstantRange
    (
        kege::GraphicsDevice* device,
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
            constant.offset = json[i][ "offset" ].getInt();
            constant.size = json[i][ "size" ].getInt();
            push_constants_ranges.push_back( constant );
        }

        return true;
    }

    bool createPipelineLayout
    (
        kege::GraphicsDevice* device,
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
        createDescriptorSetLayout( device, layout_info.descriptor_set_layouts, json[ "descriptor_set_layouts" ] );
        parsePushConstantRange( device, layout_info.push_constant_ranges, json[ "push_constants_ranges" ] );
        info->pipeline_layout = device->createPipelineLayout( layout_info );
        info->debug_name = name;
        if ( info->pipeline_layout.id < 0 )
        {
            KEGE_LOG_ERROR << "pipeline-layout create failed in createPipelineLayout()" << Log::nl;
            return false;
        }
        return true;
    }

    bool parseVertexInput
    (
        kege::GraphicsDevice* device,
        kege::GraphicsPipelineDesc* info,
        kege::Json json
    )
    {
        kege::Json params = json[ "vertex_bindings" ];
        if ( params.count() != 0 )
        {
            for (int i = 0; i < params.count(); ++i)
            {
                kege::VertexInputBindingDesc v;
                v.stride = params[i]["stride"].getInt();
                v.binding = params[i]["binding"].getInt();
                v.input_rate = ( strcmp("vertex", params[i]["input_rate"].getString()) == 0 )
                ? kege::VertexInputRate::Vertex : kege::VertexInputRate::Instance;
                info->vertex_input_state.bindings.push_back( v );
            }
        }
        params = json[ "vertex_inputs" ];
        if ( params.count() != 0 )
        {
            for (int i = 0; i < params.count(); ++i)
            {
                kege::VertexInputAttributeDesc v;
                v.location = params[i]["location"].getInt();
                v.binding = params[i]["binding"].getInt();
                v.offset = params[i]["offset"].getInt();
                v.format = convertVertexInputType( params[i][ "format" ].getString() );
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
            desc.depth_test_enable = json[ "depth_test_enable" ].getBool( true );
            desc.depth_write_enable = json[ "depth_write_enable" ].getBool( true );
            desc.depth_bounds_test_enable = json[ "depth_bounds_test_enable" ].getBool( false );
            desc.stencil_test_enable = json[ "stencil_test_enable" ].getBool( false );
            desc.min_depth_bounds = json[ "min_depth_bounds" ].getFloat( 0.0f );
            desc.max_depth_bounds = json[ "max_depth_bounds" ].getFloat( 1.0f );

            desc.depth_compare_op = stringToCompareOp( json[ "depth_compare_op" ].getString( "less" ) );

            Json front_op = json[ "front_op" ];
            desc.front_op.fail_op = stringToStencilOp( front_op[ "fail_op" ].getString( "keep" ) );
            desc.front_op.depth_fail_op = stringToStencilOp( front_op[ "depth_fail_op" ].getString( "keep" ) );
            desc.front_op.compare_op = stringToCompareOp( front_op[ "compare_op" ].getString( "always" ) );
            desc.front_op.compare_mask = front_op[ "compare_mask" ].getInt( 0xFFFFFFFF );
            desc.front_op.write_mask = front_op[ "write_mask" ].getInt( 0xFFFFFFFF );
            desc.front_op.reference = front_op[ "reference" ].getInt( 0 );

            Json back_op = json[ "back_op" ];
            desc.back_op.fail_op = stringToStencilOp( back_op[ "fail_op" ].getString( "keep" ) );
            desc.back_op.depth_fail_op = stringToStencilOp( back_op[ "depth_fail_op" ].getString( "keep" ) );
            desc.back_op.compare_op = stringToCompareOp( back_op[ "compare_op" ].getString( "keep" ) );
            desc.back_op.compare_mask = back_op[ "compare_mask" ].getInt( 0xFFFFFFFF );
            desc.back_op.write_mask = back_op[ "write_mask" ].getInt( 0xFFFFFFFF );
            desc.back_op.reference = back_op[ "reference" ].getInt( 0 );
        }
        return desc;
    }

    ColorBlendAttachmentState getColorBlendAttachmentState( kege::Json json )
    {
        ColorBlendAttachmentState state = {};
        if ( json )
        {
            state.blend_enable = json[ "blend_enable" ].getBool( false );
            state.src_color_blend_factor = stringToBlendFactor( json[ "src_color_blend_factor" ].getString( "one" ) );
            state.dst_color_blend_factor = stringToBlendFactor( json[ "dst_color_blend_factor" ].getString( "zero" ) );
            state.color_blend_op = stringToBlendOp( json[ "color_blend_op" ].getString( "add" ) );
            state.src_alpha_blend_factor = stringToBlendFactor( json[ "src_alpha_blend_factor" ].getString( "one" ) );
            state.dst_alpha_blend_factor = stringToBlendFactor( json[ "dst_alpha_blend_factor" ].getString( "zero" ) );
            state.alpha_blend_op = stringToBlendOp( json[ "alpha_blend_op" ].getString( "add" ) );
            state.color_write_mask = ColorComponentFlags::All;
        }
        return state;
    }

    ColorBlendStateDesc getColorBlendState( kege::Json json )
    {
        ColorBlendStateDesc desc = {};
        if ( json )
        {
            desc.logic_op_enable = json[ "depth_test_enable" ].getBool( false );
            desc.logic_op = stringToColorBlendLogicOp( json[ "depth_bounds_test_enable" ].getString( "copy") );

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
            Json rasterizer_discard_enable = json["rasterizer_discard_enable"];
            if( rasterizer_discard_enable )
            {
                if ( strcmp(rasterizer_discard_enable.value(), "true") == 0 )
                {
                    desc.rasterizer_discard_enable = true;
                }
                else
                {
                    desc.rasterizer_discard_enable = false;
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

    kege::PipelineHandle createPipelineFromFile
    (
        kege::GraphicsDevice* device,
        kege::Json json,
        const std::string& path
    )
    {
        kege::GraphicsPipelineDesc info = {};
        info.debug_name = json[ "name" ].value();

        info.depth_stencil_state = getDepthStencilState( json[ "depth_stencil_state" ] );
        info.color_blend_state = getColorBlendState( json[ "color_blend_state" ] );
        info.rasterization_state = getRasterizationState( json[ "rasterization_state" ] );
        info.input_assembly_state.primitive_restart_enable = true;
        info.input_assembly_state.topology = stringToPrimitiveTopology( json["primatives"][0].getString( "triangle-strip" ) );

        Json rendering_output = json[ "rendering_output" ];
        if ( rendering_output )
        {
            Json color_output_formats = rendering_output["color_output_formats"];
            for (int i = 0; i < color_output_formats.count(); ++i)
            {
                Format format = stringToFormat( color_output_formats[i].value() );
                info.color_attachment_formats.push_back( format );
            }
            Json depth_output_format = rendering_output["depth_output_format"];
            if ( depth_output_format )
            {
                Format format = stringToFormat( depth_output_format.value() );
                info.depth_attachment_format = format;
            }
            Json stencil_output_format = rendering_output["stencil_output_format"];
            if ( stencil_output_format )
            {
                Format format = stringToFormat( stencil_output_format.value() );
                info.stencil_attachment_format = format;
            }
        }

        if( !createShaderModules( device, &info, path, json ) )
        {
            return {-1};
        }
        createPipelineLayout( device, &info, info.debug_name, json );
        parseVertexInput( device, &info, json );
        return device->createGraphicsPipeline( info );
    }

    kege::PipelineHandle createPipelineFromFile
    (
        kege::GraphicsDevice* device,
        const std::string& filename
    )
    {
        kege::Json json = kege::JsonParser::load( filename.data() );
        if ( !json )
        {
            KEGE_LOG_ERROR <<"fail to open file -> " << filename << Log::nl;
            return {};
        }
        std::string path = getFilePath( filename );
        return createPipelineFromFile( device, json, path );
    }
}

