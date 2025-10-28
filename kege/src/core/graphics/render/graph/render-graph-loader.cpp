//
//  render-graph-loader.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/25/25.
//

#include "render-graph.hpp"
#include "render-graph-loader.hpp"

namespace kege{

    struct RGParser
    {
        typedef std::function< void( kege::RenderGraph&, Json ) > Function;
        typedef std::map< std::string, Function > FunctionMap;

        typedef std::function< int( kege::RenderGraph&, Json ) > IntFunct;
        typedef std::map< std::string, IntFunct > IntFunctMap;

        void parse( std::string type, kege::RenderGraph& graph, Json& json )
        {
            FunctionMap::iterator parser = parsers.find( type );
            if ( parser != parsers.end() )
            {
                parser->second( graph, json );
            }
        }

        static RGParser& instance()
        {
            static RGParser instance;
            return instance;
        }

        RGParser();

        FunctionMap parsers;
        IntFunctMap int_funct_maps;
    };

    int parseInt( kege::RenderGraph& graph, Json json )
    {
        RGParser::IntFunctMap::iterator m = RGParser::instance().int_funct_maps.find( json.value() );
        if ( m != RGParser::instance().int_funct_maps.end() )
        {
            return m->second( graph, json );
        }
        return json.toInt();
    }

    Format getFormat( kege::RenderGraph& graph, const std::string& name )
    {
        if ( name == "SWAPCHAIN_DEPTH_FORMAT()" )
        {
            return graph.getGraphics()->getSwapchain()->getDepthFormat();
        }
        else if ( name == "SWAPCHAIN_COLOR_FORMAT()" )
        {
            return graph.getGraphics()->getSwapchain()->getColorFormat();
        }
        return stringToFormat( name );
    }

    int getSwapchainImageCount( kege::RenderGraph& graph, Json json )
    {
        return graph.getGraphics()->getSwapchain()->getImageCount();
    }

    int getSwapchainExtentHeight( kege::RenderGraph& graph, Json json )
    {
        return graph.getGraphics()->getSwapchain()->getExtent().height;
    }

    int getSwapchainExtentWidth( kege::RenderGraph& graph, Json json )
    {
        return graph.getGraphics()->getSwapchain()->getExtent().width;
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

    ImageUsage parseImageUsage( Json json )
    {
        ImageUsage usage = stringToImageUsage( json[ 0 ].value() );
        for (int i=1; i<json.count(); ++i)
        {
            usage = usage | stringToImageUsage( json[ i ].value() );
        }
        return usage;
    }

    PipelineStageFlag parsePipelineStageFlags( Json json )
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

    kege::SamplerDesc parseSamplerDesc( const Json& json )
    {
        Json info = json[ "info" ];
        return  kege::SamplerDesc
        {
            .name = json[ "name" ].value(),
            .mag_filter = info[ "mag_filter" ]( stringToFilter, Filter::Linear ),
            .min_filter = info[ "min_filter" ]( stringToFilter, Filter::Linear ),
            .mipmap_mode = info[ "mipmap_mode" ]( stringToMipmapMode, MipmapMode::Linear ),
            .address_mode_u = info[ "address_mode_u" ]( stringToAddressMode, AddressMode::Repeat ),
            .address_mode_v = info[ "address_mode_v" ]( stringToAddressMode, AddressMode::Repeat ),
            .address_mode_w = info[ "address_mode_w" ]( stringToAddressMode, AddressMode::Repeat )
        };
    }

    RgReadResrcDesc parseReadResrcDesc( const Json& json )
    {
        return RgReadResrcDesc
        {
            .name = json[ "name" ].value(),
            .type = json[ "type" ]( stringToRgResrcType, RgResrcType::Invalid ),
            .usage =
            {
                .access = json[ "access" ]( stringToAccessFlags, AccessFlags::None ),
                .stage = parsePipelineStageFlags( json[ "stage" ] ),
                .layout = json[ "layout" ]( stringToImageLayout, ImageLayout::Undefined ),
                .load_op = json[ "load_op" ]( stringToAttachmentLoadOp, AttachmentLoadOp::Clear )
            }
        };
    }

    RgWriteResrcDesc parseWriteResrcDesc( const Json& json )
    {
        bool has_clear_value = false;
        ClearValue clear_value;
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
            .usage =
            {
                .layout = json[ "layout" ]( stringToImageLayout, ImageLayout::Undefined ),
                .access = json[ "access" ]( stringToAccessFlags, AccessFlags::None ),
                .stage = parsePipelineStageFlags( json[ "stage" ] ),
                .load_op = json[ "load_op" ]( stringToAttachmentLoadOp, AttachmentLoadOp::Clear )
            },
            .clear_value = ((has_clear_value)? clear_value : std::optional< ClearValue >{}),
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

    std::vector< ShaderPipeline > parseShaderPipelines( const Json& json )
    {
        return {};
    }

    void parseRenderStage( kege::RenderGraph& graph, Json json )
    {
        graph.addPass
        ({
            .name = json[ "name" ].value(),
            .type = stringToQueueType( json[ "type" ].value() ),
            .reads = parseReadResrcDescs( json[ "reads" ] ),
            .writes = parseWriteResrcDescs( json[ "writes" ] ),
            .pipelines = parseShaderPipelines( json[ "shaders" ] ),
        });
    }

    void parseShaderResourceDefn( kege::RenderGraph& graph, Json json )
    {
//        uint32_t frames_in_flight = getInt( graph, json[ "frames_in_flight" ] );
//        Json json_bindings = json[ "bindings" ];
//        Json json_targets = json[ "targets" ];

//        std::vector< kege::UniformDesc > bindings;
//        std::vector< kege::RgShaderResource > targets;
//
//        for( uint32_t i = 0; i < json_bindings.count(); ++i )
//        {
//            Json binding = json_bindings[ i ];
//
//            kege::UniformDesc layout;
//            layout.name = binding[ "name" ].value();
//            layout.count = binding[ "count" ].getInt();
//            layout.binding = binding[ "binding" ].getInt();
//            layout.descriptor_type = getDescriptorType( binding[ "descriptor_type" ] );
//            layout.stage_flags = getShaderStageFlag( binding[ "stage_flags" ] );
//
//            bindings.push_back( layout );
//        }
//
//        for( uint32_t i = 0; i < json_targets.count(); ++i )
//        {
//            Json target = json_targets[ i ];
//            targets.push_back( getRgShaderResource( target[ "type" ] ) );
//        }

        //kege::RgResrcHandle handle = graph.defineShaderResource( json[ "id" ].value(), frames_in_flight, bindings );
        //graph.updateShaderResource( handle, targets );
    }

    void parseSamplerDefn( kege::RenderGraph& graph, Json json )
    {
        graph.defnSampler
        ({
            .name = json[ "name" ].value(),
            .desc = parseSamplerDesc( json )
        });
    }

    void parseBufferDefn( kege::RenderGraph& graph, Json json )
    {
        Json info = json[ "info" ];
        graph.defnBuffer
        ({
            .name              = json[ "name" ].value(),
            .frames            = (uint32_t) json.getInt( "frames" ),
            .info.size         = (uint32_t) info.getInt( "size" ),
            .info.usage        = stringToBufferUsage( info.getStr( "usage" ) ),
            .info.memory_usage = stringToMemoryUsage( info.getStr( "memory_usage" ) ),
            .info.data         = nullptr,
        });
    }

    void parseImageDefn( kege::RenderGraph& graph, Json json )
    {
        kege::ImageDefn desc = {};
        Json info = json[ "info" ];

        desc.name = json[ "name" ].value();
        desc.usages = parseImageUsage( json[ "usages" ] );

        Json physical_handle = json[ "physical_handle" ];
        if ( physical_handle )
        {
            kege::Swapchain* swapchain = graph.getGraphics()->getSwapchain();
            if ( strcmp( physical_handle.value(), "SWAPCHAIN_COLOR_IMAGES" ) == 0 )
            {
                desc.physical_handle = swapchain->getColorImages();
                desc.frames         = swapchain->getImageCount();
                desc.info.format    = swapchain->getColorFormat();
                desc.info.width     = swapchain->getExtent().width;
                desc.info.height    = swapchain->getExtent().height;
                desc.info.type      = ImageType::Type2D;
                desc.info.depth     = 1;
            }
            else if ( strcmp( physical_handle.value(), "SWAPCHAIN_DEPTH_IMAGES" ) == 0 )
            {
                desc.physical_handle = graph.getGraphics()->getSwapchain()->getDepthImages();
                desc.frames         = swapchain->getImageCount();
                desc.info.format    = swapchain->getDepthFormat();
                desc.info.width     = swapchain->getExtent().width;
                desc.info.height    = swapchain->getExtent().height;
                desc.info.type      = ImageType::Type2D;
                desc.info.depth     = 1;
            }
        }
        else
        {
            desc.frames         = json.getInt( "frames" );
            desc.info.width     = parseInt( graph, info[ "width" ] );
            desc.info.height    = parseInt( graph, info[ "width" ] );
            desc.info.depth     = parseInt( graph, info[ "depth" ] );
            desc.info.type      = stringToImageType( info.getStr( "type" ) );
            desc.info.format    = getFormat( graph, info.getStr( "format" ) );
        }

        graph.defnImage( desc );
    }

    RGParser::RGParser()
    {
        parsers[ "GRAPHICS" ] = parseRenderStage;
        parsers[ "COMPUTE" ] = parseRenderStage;

        parsers[ "shader_resource" ] = parseShaderResourceDefn;
        parsers[ "sampler" ] = parseSamplerDefn;
        parsers[ "buffer" ] = parseBufferDefn;
        parsers[ "image" ] = parseImageDefn;

        int_funct_maps[ "SWAPCHAIN_IMAGE_COUNT" ] = getSwapchainImageCount;
        int_funct_maps[ "SWAPCHAIN_IMAGE_HEIGHT" ] = getSwapchainExtentHeight;
        int_funct_maps[ "SWAPCHAIN_IMAGE_WIDTH" ] = getSwapchainExtentWidth;
    };

    bool RenderGraphLoader::load( kege::RenderGraph& graph, const std::string& filename )
    {
        Json json = kege::JsonParser::load( filename.c_str() );
        if ( !json )
        {
            return false;
        }

        Json resources = json[ "resources" ];
        Json stages = json[ "stages" ];
        if ( !resources || !stages )
        {
            return false;
        }

        RGParser& parser = RGParser::instance();
        for( uint32_t i = 0; i < resources.count(); ++i )
        {
            Json res = resources[i];
            std::string type = res[ "type" ].value();
            parser.parse( type, graph, res );
        }
        for( uint32_t i = 0; i < stages.count(); ++i )
        {
            Json res = stages[i];
            std::string type = res[ "type" ].value();
            parser.parse( type, graph, res );
        }

        return true;
    }
}
