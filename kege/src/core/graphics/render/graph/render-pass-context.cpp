//
//  render-pass-context.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/15/25.
//

#include "render-pass.hpp"
#include "render-graph.hpp"
#include "render-pass-context.hpp"

namespace kege{

//    kege::ShaderResource* RenderPassContext::getPhysicalShaderResource( const RgResrcHandle& handle )
//    {
//        return _pass->_graph->getPhysicalShaderResource( handle );
//    }
//    
//    kege::ShaderResource* RenderPassContext::getPhysicalShaderResource( const std::string& name )
//    {
//        return _pass->_graph->getPhysicalShaderResource( name );
//    }
//
//    const std::vector< kege::BufferHandle >* RenderPassContext::getBuffers( const std::string& name )const
//    {
//        auto i = _buffer_defn_map.find( name );
//        return ( i != _buffer_defn_map.end() )
//        ?  &_pass->_graph->getPhysicalBuffers( i->second )
//        :  nullptr;
//    }
//    const std::vector< kege::ImageHandle >* RenderPassContext::getImages( const std::string& name )const
//    {
//        auto i = _image_defn_map.find( name );
//        return ( i != _image_defn_map.end() )
//        ?  &_pass->_graph->getPhysicalImages( i->second )
//        :  nullptr;
//    }
//
//    const std::vector< ShaderPipeline >& RenderPassContext::getShaderPipelines()const
//    {
//        return _pass->getShaderPipelines();
//    }
//
//    kege::BufferHandle RenderPassContext::getBuffer( const std::string& name )
//    {
//        auto i = _buffer_defn_map.find( name );
//        return ( i != _buffer_defn_map.end() )
//        ?  _pass->_graph->getPhysicalBuffer( i->second )
//        :  kege::BufferHandle{ -1 };
//    }
//
//    kege::ImageHandle RenderPassContext::getImage( const std::string& name )
//    {
//        auto i = _image_defn_map.find( name );
//        return ( i != _image_defn_map.end() )
//        ?  _pass->_graph->getPhysicalImage( i->second )
//        :  kege::ImageHandle{ -1 };
//    }
//
//    CommandEncoder* RenderPassContext::getCommandEncoder()
//    {
//        kege::CommandEncoder* encoder = _command_buffer->createCommandEncoder();
//        encoder->setScissor
//        ({
//            0, 0,
//            _render_area.extent.width,
//            _render_area.extent.height
//        });
//        encoder->setViewport
//        ({
//            0, 0,
//            static_cast<float>( _render_area.extent.width ),
//            static_cast<float>( _render_area.extent.height )
//        });
//        return encoder;
//    }
//
//    CommandBuffer* RenderPassContext::getCommandBuffer()
//    {
//        return _command_buffer;
//    }
//
//    Rect2D RenderPassContext::getRenderArea()const
//    {
//        return _render_area;
//    }
//
//    kege::Graphics* RenderPassContext::getGraphics()
//    {
//        return _graphics;
//    }
//
//    bool RenderPassContext::hasFixedPipelines()const
//    {
//        return _pass->hasFixedPipelines();
//    }
//
//    RenderPassType RenderPassContext::getRenderPassType()const
//    {
//        return _pass->_defn.pass;
//    }
//
//    const std::string& RenderPassContext::name()
//    {
//        return _name;
//    }
    
}
