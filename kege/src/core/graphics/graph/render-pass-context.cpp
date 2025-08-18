//
//  render-pass-context.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/15/25.
//

#include "../graph/render-pass.hpp"
#include "../graph/render-graph.hpp"
#include "../graph/render-pass-context.hpp"

namespace kege{

    kege::DescriptorSetHandle RenderPassContext::getPhysicalDescriptorSet( const RgResrcHandle& handle )
    {
        return _pass->_graph->getPhysicalDescriptorSet( handle );
    }
    
    kege::DescriptorSetHandle RenderPassContext::getPhysicalDescriptorSet( const std::string& name )
    {
        return _pass->_graph->getPhysicalDescriptorSet( name );
    }

    const std::vector< kege::BufferHandle >* RenderPassContext::getBuffers( const std::string& name )const
    {
        auto i = _buffer_defn_map.find( name );
        return ( i != _buffer_defn_map.end() )
        ?  &_pass->_graph->getPhysicalBuffers( i->second )
        :  nullptr;
    }
    const std::vector< kege::ImageHandle >* RenderPassContext::getImages( const std::string& name )const
    {
        auto i = _image_defn_map.find( name );
        return ( i != _image_defn_map.end() )
        ?  &_pass->_graph->getPhysicalImages( i->second )
        :  nullptr;
    }

    kege::BufferHandle RenderPassContext::getBuffer( const std::string& name )
    {
        auto i = _buffer_defn_map.find( name );
        return ( i != _buffer_defn_map.end() )
        ?  _pass->_graph->getPhysicalBuffer( i->second )
        :  kege::BufferHandle{ -1 };
    }

    kege::ImageHandle RenderPassContext::getImage( const std::string& name )
    {
        auto i = _image_defn_map.find( name );
        return ( i != _image_defn_map.end() )
        ?  _pass->_graph->getPhysicalImage( i->second )
        :  kege::ImageHandle{ -1 };
    }

    CommandEncoder* RenderPassContext::getCommandEncoder()
    {
        kege::CommandEncoder* encoder = _command_buffer->createCommandEncoder();
        encoder->setScissor
        ({
            0, 0,
            _render_area.extent.width,
            _render_area.extent.height
        });
        encoder->setViewport
        ({
            0, 0,
            static_cast<float>( _render_area.extent.width ),
            static_cast<float>( _render_area.extent.height )
        });
        return encoder;
    }

    CommandBuffer* RenderPassContext::getCommandBuffer()
    {
        return _command_buffer;
    }

    Rect2D RenderPassContext::getRenderArea()const
    {
        return _render_area;
    }

    kege::Graphics* RenderPassContext::getGraphics()
    {
        return _graphics;
    }

    const std::string& RenderPassContext::name()
    {
        return _name;
    }
    
}
