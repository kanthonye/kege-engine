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
        const kege::ui::Widget* widget = _gui->at( node_index );
        if ( !widget->visible || widget->inactive )
            return {};

        for (uint32_t child_index = _gui->head( node_index ); child_index != 0; child_index = _gui->next( child_index ))
        {
            kege::ui::Record record = getHotElem( child_index, button );
            if ( record.user_id != 0 )
            {
                return record;
            }
        }

        if ( widget->rect.width == 0.f || widget->rect.height == 0.f )
            return {};

        if (button)
        {
            if (_gui->at( node_index )->single_click == kege::ui::ClickTrigger::Disable &&
                _gui->at( node_index )->double_click == kege::ui::ClickTrigger::Disable)
            {
                return {};
            }
        }

        if ( !_gui->at( node_index )->mouseover )
            return {};

        if ( testPointVsRect( _gui->getPointerPosition(), _gui->at( node_index )->rect ) )
        {
            return kege::ui::Record
            {
                .user_id = _gui->at( node_index )->user_id.value,
                .index = node_index,
            };
        }
        return {};
    }

    kege::ui::Record Layer::getHotElem(bool button)
    {
        kege::ui::Record hot = {};
        for (uint32_t root = _head_root; root != 0; root = _gui->at( root )->layer.next)
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
        if ( user_id == _gui->at( index )->user_id )
        {
            return index;
        }

        uint32_t widget_index = _gui->at( index )->head;
        for (; widget_index != 0; widget_index = _gui->at( widget_index )->next)
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
        for (uint32_t root = _head_root; root != 0; root = _gui->at( root )->layer.next)
        {
            uint32_t index = find( user_id, root );
            if ( index != 0 )
            {
                return index;;
            }
        }
        return 0;
    }

    kege::ui::WidgetId Layer::pushRoot( const kege::ui::WidgetDesc& desc )
    {
        if (_root_stack.size() <= _root_stack_count) _root_stack.resize(1 + 2 * _root_stack.size());
        _root_stack[ _root_stack_count++ ] = _layout->_curr_parent;
        _layout->_curr_parent = 0;

        kege::ui::WidgetId w = put( desc, true );
        if ( w.id != 0 )
        {
            _layout->_curr_parent = w.index;
        }
        return w;
    }

    void Layer::popRoot()
    {
        if (0 < _root_stack_count)
        {
            int id = _root_stack_count - 1;
            _layout->_curr_parent = _root_stack[ id ];
            //_curr_root = _layout->_curr_parent;
            _root_stack_count = id;
        }
        pop();
    }

    kege::ui::WidgetId Layer::push( const kege::ui::WidgetDesc& desc )
    {
        kege::ui::WidgetId widget_id = put( desc, false );
        if ( widget_id.id != 0 )
        {
            _layout->_curr_parent = widget_id.index;
        }
        return widget_id;
    }

    kege::ui::WidgetId Layer::put( const kege::ui::WidgetDesc& desc, bool is_root )
    {
        kege::ui::Widget* widget = _gui->newWidget();
        init( desc, widget );
        if ( _layout->_curr_parent != 0 )
        {
            insertChild( widget );
            if ( is_root )
            {
                insertRoot( widget );
            }
        }
        else
        {
            insertRoot( widget );
        }
        return kege::ui::WidgetId( widget->index, widget->version );
    }

    uint32_t Layer::pop()
    {
        if ( _layout->_curr_parent )
        {
            _layout->_curr_parent = _gui->at( _layout->_curr_parent )->parent;
        }
        return _layout->_curr_parent;
    }

    void Layer::insertChild( kege::ui::Widget* widget )
    {
        kege::ui::Widget* parent = _gui->at( _layout->_curr_parent );
        widget->parent = _layout->_curr_parent;

        if ( parent->head == 0 )
        {
            parent->tail = parent->head = widget->index;
        }
        else
        {
            kege::ui::Widget* tail = _gui->at( parent->tail );

            tail->next   = widget->index;
            widget->prev = parent->tail;
            parent->tail = widget->index;
        }
        parent->count++;
    }

    void Layer::insertRoot( kege::ui::Widget* root )
    {
        if ( _head_root == 0 )
        {
            _tail_root = _head_root = root->index;
        }
        else
        {
            kege::ui::Widget* tail = _gui->at( _tail_root );
            LayerNode& t = tail->layer;
            LayerNode& w = root->layer;

            t.next = root->index;
            w.prev = tail->index;

            _tail_root = root->index;
        }

        _root_count += 1;
    }

    void Layer::init( const kege::ui::WidgetDesc& desc, kege::ui::Widget* widget )
    {
        //_count += 1;
        widget->user_id      = desc.user_id;

        widget->rect         = desc.rect;
        widget->style        = desc.style;
        widget->text         = desc.text;

        widget->width        = desc.width;
        widget->height       = desc.height;

        widget->texel        = desc.texel;
        widget->texr_info    = desc.texr_info;

        widget->single_click = desc.single_click;
        widget->double_click = desc.double_click;
        widget->inactive     = desc.inactive;
        widget->mouseover    = desc.mouseover;
        widget->visible      = desc.visible;
        widget->clip_overflow= desc.clip_overflow;

        widget->position     = desc.position;
        widget->width        = (desc.style)? desc.style->width : desc.width;
        widget->height       = (desc.style)? desc.style->height : desc.height;
        widget->text.color   = (desc.style)? desc.style->text_color : desc.text.color;
        widget->color        = (desc.style)? desc.style->background.color : desc.color;
        widget->border       = (desc.style)? desc.style->border : desc.border;
        widget->padding      = (desc.style)? desc.style->padding : desc.padding;
        widget->alignment    = (desc.style)? desc.style->alignment : desc.alignment;
        widget->gap          = (desc.style)? desc.style->gap : desc.gap;

        if (widget->width.type == kege::ui::SizingType::Fixed)
        {
            widget->rect.width += widget->width.size;
        }
        if (widget->height.type == kege::ui::SizingType::Fixed)
        {
            widget->rect.height += widget->height.size;
        }
    }

    kege::ui::WidgetId Layer::text( const kege::ui::Text& text )
    {
        return put({.rect = kege::ui::Rect{.width = text.width, .height = text.height}, .text = text}, false);
    }

    void Layer::begin( double dms )
    {
        //_count = 0;
        _tail_root = 0;
        _head_root = 0;
        //_curr_root = 0;
        _root_count = 0;
        _root_stack_count = 0;
    }

    void Layer::end()
    {
        for (uint32_t root = _head_root; root != 0; root = _gui->at( root )->layer.next )
        {
            kege::ui::Resizer::resize( *_layout, root );
            kege::ui::Aligner::align( *_layout, root );
        }
    }

    void Layer::initalize( kege::GUI* gui, kege::ui::Layout* layout )
    {
        _layout = layout;
        _gui = gui;
    }

    uint32_t Layer::getHeadRoot()const
    {
        return _head_root;
    }

    uint32_t Layer::getTailRoot()const
    {
        return _tail_root;
    }

    uint32_t Layer::getNextRoot( uint32_t root )const
    {
        return _gui->at( root )->layer.next;
    }

    uint32_t Layer::getPrevRoot( uint32_t root )const
    {
        return _gui->at( root )->layer.prev;
    }

    Layer::Layer()
    :   _layout( nullptr )
    ,   _gui( nullptr )
    ,   _head_root(0)
    ,   _tail_root(0)
    ,   _root_count(0)
    ,   _root_stack_count(0)
    {}
}
