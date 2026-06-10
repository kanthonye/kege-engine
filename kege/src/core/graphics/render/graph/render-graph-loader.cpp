//
//  render-graph-loader.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/25/25.
//

#include "render-graph.hpp"
#include "render-graph-loader.hpp"

namespace kege{

    RenderPassType stringToRenderPass( const std::string& name )
    {
        static std::map< std::string, RenderPassType > types;
        if ( types.empty() )
        {
            types[ "UI" ] = RenderPassType::UI;
            types[ "ShadowMap" ] = RenderPassType::Shadow;
            types[ "DepthPrePass" ] = RenderPassType::DepthPrePass;
            types[ "Geometry" ] = RenderPassType::Geometry;
            types[ "Lighting" ] = RenderPassType::Lighting;
            types[ "Forward" ] = RenderPassType::Forward;
            types[ "PostProcess" ] = RenderPassType::PostProcess;
            types[ "Sky" ] = RenderPassType::Sky;
            types[ "Forward" ] = RenderPassType::Forward;
            types[ "VolumetricFog" ] = RenderPassType::VolumetricFog;
            types[ "Transparent" ] = RenderPassType::Transparent;
            types[ "Particles" ] = RenderPassType::Particles;
            types[ "Debug" ] = RenderPassType::Debug;
            types[ "Compute" ] = RenderPassType::Compute;
            types[ "SSAO" ] = RenderPassType::SSAO;
            types[ "SSAOBlur" ] = RenderPassType::SSAOBlur;
            types[ "DepthOfField" ] = RenderPassType::DepthOfField;
            types[ "DepthOfFieldCoC" ] = RenderPassType::DepthOfFieldCoC;
            types[ "BloomHighpass" ] = RenderPassType::BloomHighpass;
            types[ "BloomDownsample" ] = RenderPassType::BloomDownsample;
            types[ "BloomUpsample" ] = RenderPassType::BloomUpsample;
            types[ "BloomCombine" ] = RenderPassType::BloomCombine;
            types[ "TemporalAA" ] = RenderPassType::TemporalAA;
            types[ "Tonemapping" ] = RenderPassType::Tonemapping;
            types[ "Present" ] = RenderPassType::Present;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported BarrierTransition -> " <<name <<kege::Log::nl;
        return RenderPassType::BarrierTransition;
    }

    RgResrcType stringToRgResrcType( const std::string& name )
    {
        static std::map< std::string, RgResrcType > types;
        if ( types.empty() )
        {
            types[ "buffer" ] = RgResrcType::Buffer;
            types[ "image" ] = RgResrcType::Image;
            types[ "sampler" ] = RgResrcType::Sampler;
            types[ "buffer-view" ] = RgResrcType::BufferView;
            types[ "shader-resource" ] = RgResrcType::ShaderResource;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported RgResrcType -> " <<name <<kege::Log::nl;
        return RgResrcType::Invalid;
    }

    int parseInt( const RenderGraphLoader::StringToIntMap& intmap, const std::string& str )
    {
        auto i = intmap.find( str );
        if ( i != intmap.end() )
        {
            return i->second;
        }
        return 0;
    }

    ImageUsage parseImageUsage( const Json& json )
    {
        ImageUsage usage = stringToImageUsage( json[ 0 ].value() );
        for (int i=1; i<json.count(); ++i)
        {
            usage = usage | stringToImageUsage( json[ i ].value() );
        }
        return usage;
    }

    PipelineStageFlag parsePipelineStageFlags( const Json& json )
    {
        if( !json )
            return PipelineStageFlag::None;

        PipelineStageFlag stages = stringToPipelineStageFlags( json[ 0 ].value() );
        for (int i=1; i<json.count(); ++i)
        {
            stages = stages | stringToPipelineStageFlags( json[ i ].value() );
        }
        return stages;
    }

    RgReadResrcDesc parseReadResrcDesc( const Json& json )
    {
        return RgReadResrcDesc
        {
            .name = json[ "name" ].value(),
            .type = json[ "type" ]( stringToRgResrcType, RgResrcType::Invalid ),
            .access = json[ "access" ]( stringToAccessFlags, AccessFlags::None ),
            .layout = json[ "layout" ]( stringToImageLayout, ImageLayout::Undefined ),
            .stage = parsePipelineStageFlags( json[ "stage" ] ),
            .semantic = stringToSemantic( json.getStr( "semantic" ) )
        };
    }

    RgWriteResrcDesc parseWriteResrcDesc( const Json& json )
    {
        //bool has_clear_value = false;
        ClearValue clear_value = {};
        if ( json[ "clear_color" ] )
        {
            arr< double,4 > vals = json[ "clear_color" ].getArray<double, 4>(atof);
            clear_value.color[0] = vals.data[0];
            clear_value.color[1] = vals.data[1];
            clear_value.color[2] = vals.data[2];
            clear_value.color[3] = vals.data[3];
        }
        else if ( json[ "clear_depth" ] )
        {
            clear_value.depth_stencil.depth = json.getFloat( "clear_depth" );
        }

        return RgWriteResrcDesc
        {
            .name = json[ "name" ].value(),
            .type = json[ "type" ]( stringToRgResrcType, RgResrcType::Invalid ),
            .access = json[ "access" ]( stringToAccessFlags, AccessFlags::None ),
            .stage = parsePipelineStageFlags( json[ "stage" ] ),
            .layout = json[ "layout" ]( stringToImageLayout, ImageLayout::Undefined ),
            .load_op = json[ "load_op" ]( stringToAttachmentLoadOp, AttachmentLoadOp::Clear ),
            .clear_value = clear_value,
        };
    }

    std::vector< RgReadResrcDesc > parseReadResrcDescs( const Json& json )
    {
        std::vector< RgReadResrcDesc > reads;
        json.foreach([ &reads ]( const Json& j ){ reads.push_back( parseReadResrcDesc( j ) ); });
        return reads;
    }

    std::vector< RgWriteResrcDesc > parseWriteResrcDescs( const Json& json )
    {
        std::vector< RgWriteResrcDesc > writes;
        json.foreach([ &writes ]( const Json& j ){ writes.push_back( parseWriteResrcDesc( j ) ); });
        return writes;
    }

    void parseRenderStage( const RenderGraphLoader::StringToIntMap& intmap, kege::RenderGraphDescriptor& graph, const Json& json )
    {
        graph.insert(kege::RgRenderPassDesc{
            .name = json[ "name" ].value(),
            .type = stringToQueueType( json[ "type" ].value() ),
            .pass = stringToRenderPass( json[ "pass" ].value() ),
            .reads = parseReadResrcDescs( json[ "reads" ] ),
            .writes = parseWriteResrcDescs( json[ "writes" ] ),
            .execute = renderDeferredStaticGeometry, // getExeFn( json[ "execute" ] ),
            //.bindings = parseWriteResrcDescs( json[ "writes" ] ),
        });
    }

    void parseSamplerDefn( const RenderGraphLoader::StringToIntMap& intmap, kege::RenderGraphDescriptor& graph, const Json& json )
    {
        graph.insert( kege::RgSamplerDesc
        {
            .name = json[ "name" ].value(),
            .mag_filter = json[ "mag_filter" ]( stringToFilter, Filter::Linear ),
            .min_filter = json[ "min_filter" ]( stringToFilter, Filter::Linear ),
            .mipmap_mode = json[ "mipmap_mode" ]( stringToMipmapMode, MipmapMode::Linear ),
            .address_mode_u = json[ "address_mode_u" ]( stringToAddressMode, AddressMode::Repeat ),
            .address_mode_v = json[ "address_mode_v" ]( stringToAddressMode, AddressMode::Repeat ),
            .address_mode_w = json[ "address_mode_w" ]( stringToAddressMode, AddressMode::Repeat )
        });
    }

    void parseBufferDefn( const RenderGraphLoader::StringToIntMap& intmap, kege::RenderGraphDescriptor& graph, const Json& json )
    {
        graph.insert( kege::RgBufferDesc
        {
            .name               = json[ "name" ].value(),
            .frames_in_flight   = (uint32_t) json.getInt( "frames" ),
            .size               = (uint32_t) json.getInt( "size" ),
            .usage              = stringToBufferUsage( json.getStr( "usage" ) ),
            .memory_usage       = stringToMemoryUsage( json.getStr( "memory_usage" ) ),
        });
    }

    void parseImageDefn( const RenderGraphLoader::StringToIntMap& intmap, kege::RenderGraphDescriptor& graph, const Json& json )
    {
        kege::RgImageDesc desc = {};

        desc.name      = json[ "name" ].value();
        desc.usages    = parseImageUsage( json[ "usages" ] );
        desc.width     = parseInt( intmap, json[ "width" ].toStr() );
        desc.height    = parseInt( intmap, json[ "height" ].toStr() );
        desc.depth     = parseInt( intmap, json[ "depth" ].toStr() );
        desc.type      = stringToImageType( json.getStr( "type" ) );
        desc.format    = stringToFormat( json.getStr( "format" ) );
        desc.frames_in_flight = json.getInt( "frames" );
        desc.is_swapcain_color = false;
        desc.is_swapcain_depth = false;

        if( desc.depth == 0 ) desc.depth = 1;
        if( desc.array_layers == 0 ) desc.array_layers = 1;

        Json target = json[ "target" ];
        if ( target )
        {
            if (strcmp(target.value(),"SWAPCHAIN_COLOR_IMAGES") == 0)
            {
                desc.is_swapcain_color = true;
            }
            else if (strcmp(target.value(),"SWAPCHAIN_DEPTH_IMAGES") == 0)
            {
                desc.is_swapcain_depth = true;
            }
        }
        else
        {
            if (desc.frames_in_flight == 0)
            {
                desc.frames_in_flight = 1;
                kege::Log::error << "render graph image resource -> '" <<desc.name;
                kege::Log::error <<"' parameter 'frames' must be a value greater than 0." << kege::Log::nl;
            }
        }
        graph.insert( desc );
    }

    void parseJons
    (
        kege::RenderGraphLoader::Function parser,
        const RenderGraphLoader::StringToIntMap& intmap,
        kege::RenderGraphDescriptor& graph,
        Json json
    )
    {
        if ( parser != nullptr )
        {
            for( uint32_t i = 0; i < json.count(); ++i )
            {
                parser( intmap, graph, json[i] );
            }
        }
    }

    bool RenderGraphLoader::load
    (
        kege::RenderGraphDescriptor& graph,
        const kege::Swapchain* swapchain,
        const std::string& filename
    )
    {
        Json json = kege::JsonParser::load( filename.c_str() );
        if ( !json )
        {
            return false;
        }

        kege::RenderGraphLoader::FunctionMap callbacks;
        callbacks[ "GRAPHICS" ] = parseRenderStage;
        callbacks[ "COMPUTE" ] = parseRenderStage;
        callbacks[ "sampler" ] = parseSamplerDefn;
        callbacks[ "buffer" ] = parseBufferDefn;
        callbacks[ "image" ] = parseImageDefn;

        kege::RenderGraphLoader::StringToIntMap intmap;
        intmap[ "SWAPCHAIN_IMAGE_COUNT" ] = swapchain->getImageCount();
        intmap[ "SWAPCHAIN_IMAGE_WIDTH" ] = swapchain->getViewport().width;
        intmap[ "SWAPCHAIN_IMAGE_HEIGHT" ] = swapchain->getViewport().height;
        intmap[ "SWAPCHAIN_IMAGE_WIDTH_HALF" ] = swapchain->getViewport().width * 0.5;
        intmap[ "SWAPCHAIN_IMAGE_HEIGHT_HALF" ] = swapchain->getViewport().height * 0.5;
        intmap[ "SWAPCHAIN_IMAGE_WIDTH_QUARTER" ] = swapchain->getViewport().width * 0.25;
        intmap[ "SWAPCHAIN_IMAGE_HEIGHT_QUARTER" ] = swapchain->getViewport().height * 0.25;
        intmap[ "SWAPCHAIN_IMAGE_WIDTH_EIGHTH" ] = swapchain->getViewport().width * 0.125;
        intmap[ "SWAPCHAIN_IMAGE_HEIGHT_EIGHTH" ] = swapchain->getViewport().height * 0.125;

        kege::Json resources = json[ "resources" ];
        kege::Json passes = json[ "passes" ];

        parseJons( parseSamplerDefn, intmap, graph, resources[ "samplers" ] );
        parseJons( parseBufferDefn, intmap, graph, resources[ "buffers" ] );
        parseJons( parseImageDefn, intmap, graph, resources[ "images" ] );
        parseJons( parseRenderStage, intmap, graph, passes );

        return true;
    }
}
