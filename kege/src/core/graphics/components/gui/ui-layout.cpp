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

    void Layout::beginRoot()
    {
        if (_root_stack.size() <= _root_stack_count) _root_stack.resize(1 + 2 * _root_stack.size());
        _root_stack[ _root_stack_count++ ] = _curr_parent;
        _curr_parent = 0;

        _layers[ _curr_layer ].beginRoot();
    }

    void Layout::endRoot()
    {
        _layers[ _curr_layer ].endRoot();
        if (0 < _root_stack_count)
        {
            int index = _root_stack_count - 1;
            _curr_parent = _root_stack[ index ];
            _root_stack_count = index;
        }
    }

    kege::ui::NodeId Layout::push( kege::ui::Node* node )
    {
        _layers[ _curr_layer ].push( node );
        insert( node );
        _curr_parent = node->index;
        return {node->index, node->version};
    }

    uint32_t Layout::pop()
    {
        _layers[ _curr_layer ].pop();
        if ( _curr_parent )
        {
            _curr_parent = _gui->_nodes[ _curr_parent ].parent;
        }
        return _curr_parent;
    }

    kege::ui::NodeId Layout::put( kege::ui::Node* node )
    {
        _layers[ _curr_layer ].put( node );
        insert( node );
        return {node->index, node->version};
    }

    void Layout::insert( kege::ui::Node* node )
    {
        if ( _curr_parent != 0 )
        {
            kege::ui::Node* parent = &_gui->_nodes[ _curr_parent ];
            node->parent = _curr_parent;
            
            if ( parent->head == 0 )
            {
                parent->tail = parent->head = node->index;
            }
            else
            {
                _gui->_nodes[ parent->tail ].next = node->index;
                node->prev = parent->tail;
                parent->tail = node->index;
            }
            parent->count++;
        }
        else // insert as root node
        {
            if ( _head == 0 )
            {
                _tail = _head = node->index;
            }
            else
            {
                _gui->_nodes[ _tail ].next = node->index;
                node->prev = _tail;
                _tail = node->index;
            }
            _count += 1;
        }
    }

    kege::ui::NodeId Layout::text( const kege::ui::Text& text )
    {
        return put( _gui->newNode({
            .wid = _gui->newElem
            ({
                .font_size = text.font_size,
                //.text_align = text.align,
            }),
                .quad = kege::ui::Quad{.width = text.width, .height = static_cast<float>(text.font_size)},
            .text = text
        }) );
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

    const kege::ui::Elem* Layout::elem( const kege::ui::NodeId& id ) const
    {
        return _gui->elem( id );
    }

    kege::ui::Elem* Layout::elem( const kege::ui::NodeId& id )
    {
        return _gui->elem( id );
    }

    const kege::ui::Elem* Layout::elem( const ui::Node* node )const
    {
        return _gui->elem( node );
    }
    
    kege::ui::Elem* Layout::elem( const ui::Node* node )
    {
        return _gui->elem( node );
    }

    const kege::ui::Node* Layout::operator[](uint32_t index) const
    {
        return &_gui->_nodes[ index ];
    }

    kege::ui::Node* Layout::operator[](uint32_t index)
    {
        return &_gui->_nodes[ index ];
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
        _root_stack_count = 0;
        _head = _tail = _count = 0;
        _curr_layer = 0;
        _curr_parent = 0;
        _count = 0;
        for (Layer& layer : _layers)
        {
            layer.reset( dms );
        }
    }

    void Layout::end()
    {
        for (uint32_t root = _head; root != 0; root = _gui->_nodes[ root ].next )
        {
            kege::ui::Resizer::resize( *this, root );
            kege::ui::Aligner::align( *this, root );
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
            layer.initalize( gui );
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
