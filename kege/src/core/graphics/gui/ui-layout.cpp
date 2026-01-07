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

    uint32_t Layout::computeExtent( int font_size, const char* text, float& width, float& height )
    {
        uint32_t count = 0;
        while (text[count] != 0)
        {
            const Glyph& g = _font->glyphs()[ text[count] ];
            float h = font_size * (g.scaled_height);

            height = kege::max<float>( height, h );
            width += g.advance * font_size;
            count += 1;
        }
        return count;
    }

    bool Layout::onNumericInput(const UID& elem, char* str, size_t& size)
    {
        return _cursor.onInput(Input::INPUT_NUMERIC, elem, _font, str, size);
    }

    bool Layout::onTextInput(const UID& elem, char* str, size_t& size)
    {
        return _cursor.onInput(Input::INPUT_TEXT, elem, _font, str, size);
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

    Text Layout::text( const char* str, int font_size )
    {
        Text text;
        text.length = computeExtent(font_size, str, text.width, text.height);
        text.size = font_size;
        text.ptr = str;
        return text;
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
        return _hit_record.hot == uid.global;
    }

    bool Layout::doubleClick( const UID& uid )const
    {
        return _hit_record.active == uid.global && _hit_record.clicks == 2;
    }

    bool Layout::click( const UID& uid )const
    {
        return _hit_record.active == uid.global && _hit_record.clicks == 1;
    }

    bool Layout::hasFocus( const UID& uid )const
    {
        return _focus == uid.global;
    }

    uint32_t Layout::put( const WidgetDesc& desc )
    {
        if (_widget_count >= _widgets.size()) {
            kege::Log::error << "exceeding maximum ui widget capacity.";
            return 0;
        }

        const uint32_t index = _widget_count;
        Widget* widget = &_widgets[ index ];
        _widget_count += 1;

        // assign uid to widget
        if( desc.uid )
        {
            widget->id.local.version += 1;

            // assign the widget to UID
            desc.uid->local = widget->id.local;
            // assign the UID to widget
            widget->id.global = desc.uid->global;
        }
        else
        {
            widget->id.local.index = index;
        }

        widget->text = desc.text;

        /**
         initalize the widgets parameters
         */

        widget->rect         = desc.rect;
        widget->border       = desc.border;
        widget->style        = desc.style;

        widget->color        = (desc.style)? desc.style->background.color : desc.color;
        widget->texr_info    = desc.texr_info;
        widget->layer        = desc.layer;

        widget->single_click = desc.single_click;
        widget->double_click = desc.double_click;
        widget->enabled      = desc.enabled;
        widget->visible      = desc.visible;

        widget->position     = desc.position;
        widget->padding      = desc.padding;
        widget->alignment    = desc.alignment;

        widget->parent       = 0;
        widget->count        = 0;
        widget->head         = 0;
        widget->tail         = 0;
        widget->next         = 0;
        widget->prev         = 0;

        if (widget->style)
        {
            if (widget->style->width.type == ui::SizingType::Fixed)
            {
                widget->rect.width += widget->style->width.size;
            }
            if (widget->style->height.type == ui::SizingType::Fixed)
            {
                widget->rect.height += widget->style->height.size;
            }
        }
        /**
         assign the widgets io its specified layer
         */

        ui::Layer& layer = _layers[ desc.layer ];
        {
            widget->parent = layer.parent;

            if ( 0 < layer.parent ) // add widget as child of the previous parent if one exist
            {
                Widget& parent = _widgets[ layer.parent ];
                if ( parent.head == 0 )
                {
                    parent.tail = parent.head = index;
                }
                else
                {
                    _widgets[ parent.tail ].next = index;
                    parent.tail = index;
                }
                parent.count++;
            }

            else // otherwise add widget as new root element
            {
                Widget& root = _widgets[ layer.root ];
                if ( root.head == 0 )
                {
                    root.tail = root.head = index;
                }
                else
                {
                    _widgets[ root.tail ].next = index;
                    root.tail = index;
                }
                root.count += 1;
            }
        }

        return index;
    }

    uint32_t Layout::push( const WidgetDesc& desc )
    {
        _layers[ desc.layer ].parent = put( desc );
        return _layers[ desc.layer ].parent;
    }

    uint32_t Layout::pop( int layer )
    {
        int pid = _layers[ layer ].parent;
        if ( 0 != pid )
        {
            _layers[ layer ].parent = _widgets[ pid ].parent;
        }
        return pid;
    }

    const kege::ui::Widget* Layout::elem( const UID& uid ) const
    {
        if ( _widgets.size() <= uid.local.index)
        {
            kege::Log::error << "out of bound UID index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ uid.local.index ].id.local.version != uid.local.version) return nullptr;
        return &_widgets[ uid.local.index ];
    }

    kege::ui::Widget* Layout::elem( const UID& uid )
    {
        if ( _widgets.size() <= uid.local.index)
        {
            kege::Log::error << "out of bound UID index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ uid.local.index ].id.local.version != uid.local.version) return nullptr;
        return &_widgets[ uid.local.index ];
    }

    kege::ui::Widget* Layout::elemParent( const UID& uid )
    {
        if ( _widgets.size() <= uid.local.index)
        {
            kege::Log::error << "out of bound UID index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ uid.local.index ].id.local.version != uid.local.version) return nullptr;
        //int32_t parent = _widgets[ _widgets[ uid.index ].parent ].parent;
        return &_widgets[ _widgets[ uid.local.index ].parent ];
    }

    const Widget* Layout::operator[]( uint32_t node_id )const
    {
        return &_widgets[ node_id ];
    }

    Widget* Layout::operator[]( uint32_t index )
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

    uint32_t Layout::parent( uint32_t index )const
    {
        return _widgets[ index ].parent;
    }
    
    uint32_t Layout::head( uint32_t index )const
    {
        return _widgets[ index ].head;
    }

    uint32_t Layout::tail( uint32_t index )const
    {
        return _widgets[ index ].tail;
    }

    uint32_t Layout::next( uint32_t index )const
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

    uint32_t Layout::count( uint32_t index )const
    {
        return _widgets[ index ].count;
    }

    void Layout::createLayers( uint32_t quantity )
    {
        _layers.resize( quantity );
        for (int i=0; i<quantity; ++i)
        {
            _layers[i].root = i + 1;
            _layers[i].parent = 0;
            _widgets[i].rect.height = _height;
            _widgets[i].rect.width = _width;
        }
    }

    void Layout::resize( uint32_t max_nodes )
    {
        _widgets.resize( max_nodes );
        for (int i=0; i<max_nodes; ++i)
        {
            _widgets[i].id.local.index = i;
            _widgets[i].id.local.version = 0;
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
        for (int i=0; i<_layers.size(); ++i)
        {
            Widget& root = _widgets[ _layers[i].root ];
            root.parent = 0;
            root.count = 0;
            root.head = 0;
            root.tail = 0;
            root.next = 0;
        }

        _widget_count = 1 + uint32_t(_layers.size());
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
            return _widgets[ node_index ].id.global;
        }
        return {};
    }

    Id Layout::getHotElem(bool button)
    {
        Id hot;
        for (int layer=0; layer<_layers.size(); ++layer)
        {
            for( uint32_t root = head( _layers[ layer ].root ); root != 0; root = next( root ))
            {
                hot = getHotElem( root, button );
                if ( hot.id != 0 )
                {
                    break;
                }
            }
        }
        return hot;
    }

    uint32_t Layout::find(uint32_t widget_index, const ui::Id& id)
    {
        if ( id == _widgets[ widget_index ].id.global )
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
        for (int layer=0; layer<_layers.size(); ++layer)
        {
            for( uint32_t root = head( _layers[ layer ].root ); root != 0; root = next( root ))
            {
                uint32_t index = find( root, id );
                if ( index != 0 )
                {
                    return index;;
                }
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
                    _hit = _widgets[ _active_index ].id.global;
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
            for (int layer=0; layer<_layers.size(); ++layer)
            {
                for( uint32_t root = head( _layers[ layer ].root ); root != 0; root = next( root ))
                {
                    Resizer::resize( *this, root );
                    Aligner::align( *this, root );
                }
            }
            handleInputEvents();

            for (int i=0; i<_deferred_op_count; i++)
            {
                char* data = &_state_buffer[_deferred_ops[i].alloc.index];
                _deferred_ops[i].fn(this, *_deferred_ops[i].id, data);
            }
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

    Layout::Layout(uint32_t width, uint32_t height, uint32_t quantity)
    :   _hot{}
    ,   _active{}
    ,   _active_index(0)
    ,   _hot_index(0)
    ,   _widget_count( 1 )
    ,   _button_down( false )
    ,   _height( height )
    ,   _width( width )
    ,   _cursor( this )
    {
        resize( quantity );
    }

}
