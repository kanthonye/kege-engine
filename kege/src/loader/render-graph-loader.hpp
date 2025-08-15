//
//  render-graph-loader.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/25/25.
//

#ifndef render_graph_loader_hpp
#define render_graph_loader_hpp

#include "json-parser.hpp"
#include "render-graph.hpp"

namespace kege{

    class RenderGraphLoader
    {
    public:


        static bool load( kege::RenderGraph& graph, const std::string& filename );

        static void graphicsPass( kege::RenderGraph& graph, Json json );
        static void computePass( kege::RenderGraph& graph, Json json );
        static void shaderResource( kege::RenderGraph& graph, Json json );
        static void sampler( kege::RenderGraph& graph, Json json );
        static void buffer( kege::RenderGraph& graph, Json json );
        static void image( kege::RenderGraph& graph, Json json );

        typedef std::function< void( kege::RenderGraph&, Json ) > Function;
        typedef std::map< std::string, Function > FunctionMap;

        typedef std::function< int( kege::RenderGraph&, Json ) > IntFunct;
        typedef std::map< std::string, IntFunct > IntFunctMap;

        RenderGraphLoader();
        
    private:

        static RgShaderResource getRgShaderResource( Json json );
        static ImageType getImageType( Json json );
        static BufferUsage getBufferUsage( Json json );
        static MemoryUsage getMemoryUsage( Json json );
        static ImageUsageFlags getImageUsage( Json json );
        static DescriptorType getDescriptorType( Json json );
        static ShaderStage getShaderStageFlag( Json json );

        static kege::Filter toFilter( const std::string& name );
        static kege::MipmapMode toMipmapMode( const std::string& name );
        static kege::AddressMode toAddressMode( const std::string& name );

        static int getSwapchainImageCount( kege::RenderGraph& graph, Json json );
        static int getSwapchainExtentHeight( kege::RenderGraph& graph, Json json );
        static int getSwapchainExtentWidth( kege::RenderGraph& graph, Json json );
        static int getFramesInFlight( kege::RenderGraph& graph, Json json );
        static int getInt( kege::RenderGraph& graph, Json json );
        
        static Format getFormat( kege::RenderGraph& graph, Json json );

    private:

        static FunctionMap _parsers;
        static IntFunctMap _int_funct_maps;
    };
}

#endif /* render_graph_loader_hpp */
