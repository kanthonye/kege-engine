//
//  ui-layer.cpp
//  editor
//
//  Created by Kenneth Esdaile on 5/12/26.
//

#include "ui-gui.hpp"

namespace kege::ui{

    kege::ui::Record Layer::getHotElem(uint32_t node_index, bool button)
    {
        const kege::ui::Node* node = &_gui->_nodes[ node_index ];
        if ( !node->visible || node->inactive )
            return {};

        for (uint32_t child_index = _gui->head( node_index ); child_index != 0; child_index = _gui->next( child_index ))
        {
            kege::ui::Record record = getHotElem( child_index, button );
            if ( record.user_id != 0 )
            {
                return record;
            }
        }

        //kege::ui::Elem* elem = _gui->elem( node->wid );
        if ( node->quad.width == 0.f || node->quad.height == 0.f )
            return {};

        if (button)
        {
            if (node->single_click == kege::ui::ClickTrigger::Disable &&
                node->double_click == kege::ui::ClickTrigger::Disable)
            {
                return {};
            }
        }

        if ( !node->mouseover )
            return {};

        if ( testPointVsRect( _gui->getPointerPosition(), node->quad ) )
        {
            return kege::ui::Record
            {
                .user_id = node->user_id.value,
                .index = node_index,
            };
        }
        return {};
    }

    kege::ui::Record Layer::getHotElem(bool button)
    {
        kege::ui::Record hot = {};
        for (uint32_t root = _head; root != 0; root = _gui->_nodes[ root ].layer.next)
        {
            kege::ui::Record result = getHotElem( root, button );
            if ( result.user_id != 0 )
            {
                hot = result;
                break;
            }
        }
        return hot;
    }

    uint32_t Layer::find(uint64_t user_id, uint32_t index)
    {
        if ( user_id == _gui->_nodes[ index ].user_id )
        {
            return index;
        }

        uint32_t widget_index = _gui->_nodes[ index ].head;
        for (; widget_index != 0; widget_index = _gui->_nodes[ widget_index ].next)
        {
            uint32_t index = find( user_id, widget_index );
            if ( index != 0 )
            {
                return index;
            }
        }
        return 0;
    }

    uint32_t Layer::find(uint64_t user_id)
    {
        for (uint32_t root = _head; root != 0; root = _gui->_nodes[ root ].layer.next)
        {
            uint32_t index = find( user_id, root );
            if ( index != 0 )
            {
                return index;;
            }
        }
        return 0;
    }

    void Layer::beginRoot()
    {
        if (_root_stack.size() <= _root_stack_count) _root_stack.resize(1 + 2 * _root_stack.size());
        _root_stack[ _root_stack_count++ ] = _parent;
        _parent = 0;
    }

    void Layer::endRoot()
    {
        if (0 < _root_stack_count)
        {
            int id = _root_stack_count - 1;
            _parent = _root_stack[ id ];
            _root_stack_count = id;
        }
    }

    void Layer::push( kege::ui::Node* node )
    {
        put( node );
        _parent = node->index;
    }

    void Layer::put( kege::ui::Node* node )
    {
        if ( _parent != 0 )
        {
            kege::ui::Node* parent = &_gui->_nodes[ _parent ];
            node->layer.parent = _parent;

            if ( parent->layer.head == 0 )
            {
                parent->layer.tail = parent->layer.head = node->index;
            }
            else
            {
                _gui->_nodes[ parent->tail ].layer.next = node->index;
                node->layer.prev = parent->layer.tail;
                parent->layer.tail = node->index;
            }
            //parent->layer.count++;
        }
        else // insert as root node
        {
            if ( _head == 0 )
            {
                _tail = _head = node->index;
            }
            else
            {
                _gui->_nodes[ _tail ].layer.next = node->index;
                node->layer.prev = _tail;
                _tail = node->index;
            }
            _count += 1;
        }
    }

    void Layer::pop()
    {
        if ( _parent )
        {
            _parent = _gui->_nodes[ _parent ].layer.parent;
        }
    }

    void Layer::reset( double dms )
    {
        _tail = 0;
        _head = 0;
        _count = 0;
        _parent = 0;
        _root_stack_count = 0;
    }

    void Layer::initalize( kege::GUI* gui )
    {
        _gui = gui;
    }

    uint32_t Layer::getHeadRoot()const
    {
        return _head;
    }

    uint32_t Layer::getTailRoot()const
    {
        return _tail;
    }

    uint32_t Layer::getNextRoot( uint32_t root )const
    {
        return _gui->_nodes[ root ].layer.next;
    }

    uint32_t Layer::getPrevRoot( uint32_t root )const
    {
        return _gui->_nodes[ root ].layer.prev;
    }

    Layer::Layer()
    :   _gui( nullptr )
    ,   _head(0)
    ,   _tail(0)
    ,   _count(0)
    ,   _root_stack_count(0)
    {}

}
