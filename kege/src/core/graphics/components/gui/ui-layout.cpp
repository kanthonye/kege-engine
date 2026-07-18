//
//  ui-layout.cpp
//  ui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-gui.hpp"

namespace kege::ui{

    const kege::ui::Layer& Layout::getLayer( uint32_t i )const
    {
        return _layers[ i ];
    }

    uint32_t Layout::getLayerCount()const
    {
        return (uint32_t)_layers.size();
    }

    kege::ui::Record Layout::getHotElem( bool button )
    {
        for (int layer_index = int(_layers.size() - 1); 0 <= layer_index; --layer_index)
        {
            kege::ui::Record result = _layers[ layer_index ].getHotElem( button );
            if ( result.user_id != 0 )
            {
                return result;
            }
        }
        return {};
    }

    uint32_t Layout::find(uint64_t user_id)
    {
        for (int layer_index = int(_layers.size() - 1); 0 <= layer_index; --layer_index)
        {
            uint32_t index = _layers[ layer_index ].find( user_id );
            if ( index != 0 )
            {
                return index;
            }
        }
        return 0;
    }

    kege::ui::WidgetId Layout::pushRoot( const kege::ui::WidgetDesc& desc )
    {
        return _layers[ _curr_layer ].pushRoot( desc );
    }

    kege::ui::WidgetId Layout::putRoot( const kege::ui::WidgetDesc& desc )
    {
        return _layers[ _curr_layer ].put( desc, true );
    }

    void Layout::popRoot()
    {
        return _layers[ _curr_layer ].popRoot();
    }

    kege::ui::WidgetId Layout::push( const kege::ui::WidgetDesc& desc )
    {
        return _layers[ _curr_layer ].push( desc );
    }

    uint32_t Layout::pop()
    {
        return _layers[ _curr_layer ].pop();
    }

    kege::ui::WidgetId Layout::put( const kege::ui::WidgetDesc& desc )
    {
        return _layers[ _curr_layer ].put( desc, false );
    }

    kege::ui::WidgetId Layout::text( const kege::ui::Text& text )
    {
        return _layers[ _curr_layer ].text( text );
    }

    void Layout::pushLayer( uint32_t index )
    {
        _layer_stack.push( _curr_layer );
        _curr_layer = index;
    }

    bool Layout::popLayer()
    {
        if ( !_layer_stack.empty() )
        {
            _curr_layer = _layer_stack.top();
            _layer_stack.pop();
            return true;
        }
        return false;
    }

    void Layout::onWindowResize(int width, int height)
    {
        _rect.height = height;
        _rect.width = width;
    }

    const kege::ui::Widget* Layout::operator[](uint32_t index) const
    {
        return _gui->at( index );
    }

    kege::ui::Widget* Layout::operator[](uint32_t index)
    {
        return _gui->at( index );
    }

    uint32_t Layout::parent( uint32_t index )const
    {
        return _gui->parent( index );
    }

    uint32_t Layout::head( uint32_t index )const
    {
        return _gui->head( index );
    }

    uint32_t Layout::tail( uint32_t index )const
    {
        return _gui->tail( index );
    }

    uint32_t Layout::next( uint32_t index )const
    {
        return _gui->next( index );
    }

    uint32_t Layout::prev( uint32_t index )const
    {
        return _gui->prev( index );
    }

    const kege::ui::Rect& Layout::getRect()const
    {
        return _rect;
    }

    const kege::mat44& Layout::getTransform()const
    {
        return _transform;
    }

    void Layout::begin( double dms )
    {
        _curr_layer = 0;
        _curr_parent = 0;
        for (Layer& layer : _layers)
        {
            layer.begin( dms );
        }
    }

    void Layout::end()
    {
        for (Layer& layer : _layers)
        {
            layer.end();
        }
    }

    void Layout::initialize
    (
        kege::GUI* gui,
        const kege::mat44& transform,
        const kege::ui::Rect& rect,
        uint32_t max_layers
    )
    {
        _layers.resize( max_layers );
        _transform = transform;
        _rect = rect;
        _gui = gui;
        for(auto& layer : _layers )
        {
            layer.initalize( gui, this );
        }
    }

    void Layout::shutdown()
    {}

    Layout::Layout()
    :   _curr_layer(0)
    ,   _curr_parent(0)
    {}
    
    Layout::~Layout()
    {
        shutdown();
    }

}
