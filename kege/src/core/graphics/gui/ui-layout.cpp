//
//  ui-layout.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-input.hpp"
#include "ui-layout.hpp"

namespace kege::ui{


    void* Layout::getParams(AllocParam param)
    {
        if (param.index + param.size > _state_buffer.size()) return nullptr;
        return &_state_buffer[ param.index ];
    }

    AllocParam Layout::alloc(size_t size)
    {
        size_t index = _state_buffer_size;
        if (index + size >= _state_buffer.size())
        {
            _state_buffer.resize(index + size + 1024);
        }
        _state_buffer_size += size;
        return AllocParam{ .index = index, .size = size };
    }

    kege::vec2 Layout::computeExtent( int font_size, const char* text )
    {
        //const int text_index = node.text_id;
        kege::vec2 cursor = {0.f, 0.f};
        for (const char* c = text; 0 < *c; ++c )
        {
            const Glyph& g = _font->glyphs()[ *c ];
            float h = font_size * g.scaled_height;

            cursor.y = kege::max<float>( cursor.y, h );
            cursor.x += g.advance * font_size;
        }
        return cursor;
    }

    bool Layout::onNumericInput(const UID& elem, kege::string* text)
    {
        return _cursor.onInput(Input::INPUT_NUMERIC, elem, _font, text);
    }

    bool Layout::onTextInput(const UID& elem, kege::string* text)
    {
        return _cursor.onInput(Input::INPUT_TEXT, elem, _font, text);
    }

    bool Layout::testPointVsRect( const kege::dvec2& p, const ui::Rect& rect )const
    {
        return
        (
            p.x > rect.x &&
            p.y > rect.y &&
            p.x < rect.x + rect.width &&
            p.y < rect.y + rect.height
        );
    }

//    const kege::dvec2& Layout::pointerPosition() const
//    {
//        return _input->_last_frame.position;
//    }
//
//    const kege::dvec2& Layout::deltaPosition() const
//    {
//        return _input->_last_frame.delta_position;
//    }
//
//    const kege::dvec2& Layout::scrollOffset() const
//    {
//        return _input->_last_frame.scroll_offset;
//    }
//
//    const bool Layout::pointerDragging() const
//    {
//        return _input->_last_frame.pointer_dragging;
//    }

    bool Layout::mouseover( const UID& uid )const
    {
        return _hit_record.hot.id == uid.id.id;
    }

    bool Layout::doubleClick( const UID& uid )const
    {
        return _hit_record.active.id == uid.id.id && _hit_record.clicks == 2;
    }

    bool Layout::click( const UID& uid )const
    {
        return _hit_record.active.id == uid.id.id && _hit_record.clicks == 1;
    }

    bool Layout::hasFocus( const UID& uid )const
    {
        return _focus.id == uid.id.id;
    }

    uint32_t Layout::put( const Desc& desc )
    {
        if (_widget_count >= _widgets.size()) {
            kege::Log::error << "exceeding maximum ui widget capacity.";
            return 0;
        }

        Widget* widget = &_widgets[ _widget_count ];
        uint32_t index = _widget_count;
        _widget_count += 1;

        if( desc.id )
        {
            desc.id->elem_id.version = ++widget->version;
            desc.id->elem_id.index = index;
            widget->id = desc.id->id;
            widget->elem_id = desc.id->elem_id;
        }
        else
        {
            widget->id.index = index;
        }

        if( desc.text ) widget->text.text = desc.text;
        else widget->text.text = "";

        widget->style        = desc.style;
        widget->offset.x     = desc.offset.x;
        widget->offset.y     = desc.offset.y;
        widget->enabled      = desc.enabled;
        widget->single_click = desc.single_click;
        widget->double_click = desc.double_click;
        widget->visible      = desc.visible;

        widget->parent       = _parent;
        widget->count        = 0;
        widget->head         = 0;
        widget->tail         = 0;
        widget->next         = 0;

        widget->rect       = {};

        // setup the layout tree hierarchy
        if ( 0 < _parent )
        {
            if ( _widgets[ _parent ].head == 0 )
            {
                _widgets[ _parent ].tail = _widgets[ _parent ].head = index;
            }
            else
            {
                _widgets[ _widgets[ _parent ].tail ].next = index;
                _widgets[ _parent ].tail = index;
            }
            _widgets[ _parent ].count++;

            //node->depth = 1 + _nodes[ _parent ].depth;
        }
        else
        {
            if (_root_count >= _roots.size())
            {
                _roots.resize((!_roots.empty()? 2 * _roots.size(): 16 ));
            }
            _roots[ _root_count ] = index;
            _root_count += 1;
        }


        vec2 text_extent = {};
        if(!widget->text.text.empty())
        {
            text_extent = computeExtent(widget->style->font_size, widget->text.text.c_str());
        }

        if(widget->style->width.type == kege::ui::SIZE_FIXED)
        {
            widget->rect.width = widget->style->width.size;
        }
        else if(widget->style->width.type == kege::ui::SIZE_FLEXIBLE && !widget->text.text.empty())
        {
            widget->rect.width = text_extent.x + widget->style->padding.left + widget->style->padding.right;
        }

        if(widget->style->height.type == kege::ui::SIZE_FIXED)
        {
            widget->rect.height = widget->style->height.size;
        }
        else if(widget->style->height.type == kege::ui::SIZE_FLEXIBLE && !widget->text.text.empty())
        {
            widget->rect.height = text_extent.y + widget->style->padding.above + widget->style->padding.below;
        }

        return index;
    }

    uint32_t Layout::push( const Desc& desc )
    {
        _parent = put( desc );
        return _parent;
    }

//    uint32_t Layout::push( const Elem& elem )
//    {
//        _parent = put( elem );
//        return _parent;
//    }

    uint32_t Layout::pop()
    {
        int pid = _parent;
        if ( 0 != _parent )
        {
            _parent = _widgets[ _parent ].parent;
        }
        //if ( _widgets[pid].style->width.type == SizingType::SIZE_FLEXIBLE || _widgets[pid].style->height.type == SizingType::SIZE_FLEXIBLE )
        //{
        //    Resizer::calcFlexSize( *this, pid );
        //}
        return pid;
    }

    const kege::ui::Widget* Layout::elem( const UID& uid ) const
    {
        if ( _widgets.size() <= uid.elem_id.index)
        {
            kege::Log::error << "out of bound UID index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ uid.elem_id.index ].version != uid.elem_id.version) return nullptr;
        return &_widgets[ uid.elem_id.index ];
    }

    kege::ui::Widget* Layout::elem( const UID& uid )
    {
        if ( _widgets.size() <= uid.elem_id.index)
        {
            kege::Log::error << "out of bound UID index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ uid.elem_id.index ].version != uid.elem_id.version) return nullptr;
        return &_widgets[ uid.elem_id.index ];
    }

    kege::ui::Widget* Layout::elemParent( const UID& uid )
    {
        if ( _widgets.size() <= uid.elem_id.index)
        {
            kege::Log::error << "out of bound UID index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ uid.elem_id.index ].version != uid.elem_id.version) return nullptr;
        //int32_t parent = _widgets[ _widgets[ uid.index ].parent ].parent;
        return &_widgets[ _widgets[ uid.elem_id.index ].parent ];
    }

    const Widget* Layout::operator[]( NodeIndex node_id )const
    {
        return &_widgets[ node_id ];
    }

    Widget* Layout::operator[]( NodeIndex index )
    {
        return &_widgets[ index ];
    }

    uint32_t Layout::addStyle( const AddStyle& as )
    {
        return _style_manager.addStyle( as );
    }

    ui::Style* Layout::getStyleByName( const std::string& name_id )
    {
        return _style_manager.getStyleByName( name_id );
    }

    ui::Style* Layout::getStyleByID( int index )
    {
        return _style_manager.getStyleByID( index );
    }

    bool Layout::loadStyles( const std::string& filename )
    {
        return _style_manager.load( filename );
    }

    void Layout::setFont( const ref::Font& font )
    {
        _font = font;
    }

    const ref::Font& Layout::getFont()const
    {
        return _font;
    }

    NodeIndex Layout::parent( NodeIndex index )const
    {
        return _widgets[ index ].parent;
    }
    
    NodeIndex Layout::head( NodeIndex index )const
    {
        return _widgets[ index ].head;
    }

    NodeIndex Layout::tail( NodeIndex index )const
    {
        return _widgets[ index ].tail;
    }

    NodeIndex Layout::next( NodeIndex index )const
    {
        return _widgets[ index ].next;
    }

    uint32_t Layout::getHeight()const
    {
        return _height;
    }

    uint32_t Layout::getWidth()const
    {
        return _width;
    }

    uint32_t Layout::count( NodeIndex index )const
    {
        return _widgets[ index ].count;
    }

    void Layout::resize( uint32_t max_nodes )
    {
        _widgets.resize( max_nodes );
        for (int i=0; i<max_nodes; ++i)
        {
            _widgets[i].id.index = i;
            _widgets[i].id.version = 0;
        }
    }

    uint32_t Layout::count()const
    {
        return _widget_count;
    }

    bool Layout::validate( uint32_t node_id )const
    {
        return node_id > 0 && node_id < _widgets.size();
    }

    void Layout::begin( double dms, ui::Input* input )
    {
        _parent = 0;
        _root_count = 0;
        _widget_count = 1;
        _state_buffer_size = 0;
        _deferred_op_count = 0;

        _input = input;
        _cursor.update( dms, input );

        if ( _hit.id != 0)
        {
            _hit_record.active = _hit;
        }
        else
            _hit_record.active.id = 0;

        if ( _focus.id != 0)
        {
            _hit_record.focus = _focus;
        }
        _hit_record.hot = _hot;
    }

    Id Layout::getHotElem(uint32_t node_index, bool button)
    {
        if ( !_widgets[ node_index ].visible || !_widgets[ node_index ].enabled ) return {};

        for (int child_index = _widgets[ node_index ].head; child_index != 0; child_index = _widgets[ child_index ].next)
        {
            Id id = getHotElem( child_index, button );
            if ( id.id != 0 )
            {
                return id;
            }
        }

        if (button)
        {
            if (_widgets[ node_index ].single_click == ui::ClickTrigger::Disable &&
                _widgets[ node_index ].double_click == ui::ClickTrigger::Disable)
            {
                return {};
            }
        }
        if ( testPointVsRect( _input->_current_position, _widgets[ node_index ].rect ) )
        {
            _hot_index = node_index;
            return _widgets[ node_index ].id;
        }
        return {};
    }

    Id Layout::getHotElem(bool button)
    {
        Id hot;
        for (int i=0; i<_root_count; ++i)
        {
            hot = getHotElem( _roots[i], button );
            if ( hot.id != 0 )
            {
                break;
            }
        }
        return hot;
    }

    uint32_t Layout::find(uint32_t widget_index, const ui::Id& id)
    {
        if ( id.id == _widgets[ widget_index ].id.id )
        {
            return widget_index;
        }

        for (int child_index = _widgets[ widget_index ].head; child_index != 0; child_index = _widgets[ child_index ].next)
        {
            uint32_t index = find( child_index, id );
            if ( index != 0 )
            {
                return index;
            }
        }
        return 0;
    }

    uint32_t Layout::find(const ui::Id& id)
    {
        for (int i=0; i<_root_count; ++i)
        {
            uint32_t index = find( _roots[i], id );
            if ( index != 0 )
            {
                return index;;
            }
        }
        return 0;
    }

    void Layout::handleInputEvents()
    {
        _hit.id = 0;

        // when mouse button not down scan for hot element
        _button_down = _input->buttonDown();
        if ( !_button_down )
        {
            if ( _button_active )
            {
                _button_active = false;
                _active_index = find( _active );

                const Input::Click& click = _input->getClick(0);
                if( testPointVsRect( click.position, _widgets[ _active_index ].rect ) )
                {
                    if (_widgets[ _active_index ].single_click == ui::ClickTrigger::OnRelease)
                    {
                        std::cout <<"release: "<< _active.id <<"\n";
                        _hit.id = _active.id;
                        _focus.id = _hot.id;
                        return;
                    }
                }
            }

            _hot = getHotElem();
            _active_index = 0;
            _active.id = 0;
        }
        else if ( _button_down && !_button_active )
        {
            _button_active = true;
            _hot = getHotElem(true);
        }

        if ( _button_down )
        {
            if ( _active.id == 0 && _hot.id != 0 )
            {
                const Input::Click& click = _input->getClick(0);
                if ( click.down )
                {
                    if( testPointVsRect( click.position, _widgets[ _hot_index ].rect ) )
                    {
                        _hit_record.clicks = click.clicks;
                        _active_index = _hot_index;
                        _active.id = _hot.id;
                        _focus.id = _hot.id;

                        if (_widgets[ _hot_index ].single_click == ui::ClickTrigger::Immediate ||
                            _widgets[ _hot_index ].single_click == ui::ClickTrigger::Continuous)
                        {
                            _hit.id = _active.id;
                        }
                        //std::cout <<"clicked: "<< _active.id <<"\n";
                    }
                }
            }
            else if ( _active.id != 0 )
            {
                _active_index = find( _active );
                if (_widgets[ _active_index ].single_click == ui::ClickTrigger::Continuous ||
                    _widgets[ _active_index ].double_click == ui::ClickTrigger::Continuous)
                {
                    _hit.id = _widgets[ _active_index ].id.id;
                }
            }
            else
            {
                _focus.id = 0;
            }
        }
    }

    void Layout::end()
    {
        if ( 0 < _widget_count )
        {
            Resizer::resize( *this );
            for (int i=0; i<_deferred_op_count; i++)
            {
                char* data = &_state_buffer[_deferred_ops[i].alloc.index];
                _deferred_ops[i].fn(this, *_deferred_ops[i].id, data);
            }
            Aligner::align( *this );

            handleInputEvents();
        }
    }

    bool Layout::buttonDown()const
    {
        return _button_down;
    }

    const ui::Input* Layout::input()const
    {
        return _input;
    }

    Layout::Layout(uint32_t width, uint32_t height)
    :   _parent( 0 )
    ,   _hot{}
    ,   _active{}
    ,   _active_index(0)
    ,   _hot_index(0)
    ,   _widget_count( 1 )
    ,   _button_down( false )
    ,   _height( height )
    ,   _width( width )
    ,   _cursor( this )
//    ,   _head(-1)
//    ,   _tail(-1)
    {
    }

}
