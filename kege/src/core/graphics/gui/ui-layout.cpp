//
//  ui-layout.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-layout.hpp"

namespace kege::ui{

    void Layout::onWindowResize(int width, int height)
    {
        _height = height;
        _width = width;
    }


    kege::ui::Widget* Layout::pushRoot( const WidgetDesc& desc )
    {
        if (_parent_stack.size() <= _parent_stack_count)
        {
            _parent_stack.resize(1 + 2 * _parent_stack.size());
        }
        _parent_stack[ _parent_stack_count ] = _current_parent;
        _parent_stack_count += 1;
        _current_parent = 0;

        return push( desc );
    }

    kege::ui::Widget* Layout::putRoot( const WidgetDesc& desc )
    {
        int parent = _current_parent;
        _current_parent = 0;

        kege::ui::Widget* w = put( desc );

        _current_parent = parent;
        return w;
    }

    void Layout::popRoot()
    {
        pop();
        if (0 < _parent_stack_count)
        {
            _current_parent = _parent_stack[ _parent_stack_count - 1 ];
            _parent_stack_count -= 1;
        }
    }

    kege::ui::Widget* Layout::push( const WidgetDesc& desc )
    {
        kege::ui::Widget* widget = widget = put( desc );
        if ( widget != nullptr )
        {
            _current_parent = widget->index;
        }
        return widget;
    }

    uint32_t Layout::pop()
    {
        uint32_t curr_parent = 0;
        if ( _current_parent )
        {
            _current_parent = _widgets[ _current_parent ].parent;
        }
        return curr_parent;
    }

    kege::ui::Widget* Layout::put( const WidgetDesc& desc )
    {
        if (_widget_count >= _widgets.size()) {
            kege::Log::error << "exceeding maximum ui widget capacity.";
            return nullptr;
        }

        const uint32_t index = _widget_count;
        _widget_count += 1;

        setWidgetParameters( index, desc );
        addToDesignatedLayer( index, desc );

        resolveParentChildRelation( index );

        return &_widgets[ index ];
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

    bool Layout::onNumericInput(const UID& uid, char* str, size_t& size)
    {
        return 0;//_cursor.onInput(Cursor::InputType::Numeric, uid, _font, str, size);
    }

    bool Layout::onTextInput(const UID& uid, char* str, size_t& size)
    {
        return 0;//_cursor.onInput(Cursor::InputType::Any, uid, _font, str, size);
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
        computeExtent(font_size, str, text.width, text.height);
        text.font_size = font_size;
        text.ptr = str;
        return text;
    }


    kege::dvec2 Layout::getClickPosition( MouseButtonCode code ) const
    {
        return _mouse->getClickPosition( code );
    }

    /**
     * Retrieves the current position of the mouse pointer.
     *
     * @return The current position as a 2D vector.
     */
    kege::dvec2 Layout::getPointerPosition() const
    {
        return _mouse->getPosition();
    }

    /**
     * Retrieves the delta (change) in the mouse pointer's position.
     *
     * @return The delta position as a 2D vector.
     */
    kege::dvec2 Layout::getPointerDelta() const
    {
        return _mouse->getDelta();
    }

    /**
     * Retrieves the mouse scroll offset.
     *
     * @return The scroll offset as a 2D vector.
     */
    kege::dvec2 Layout::getScrollOffset() const
    {
        return _mouse->getScrollDelta();
    }

    /**
     * Checks if the mouse pointer is being dragged.
     *
     * @return true if the pointer is being dragged, false otherwise.
     */
    bool Layout::isPointerDragging() const
    {
        return _mouse->isDragging(MouseButtonCode::Left);
    }


    bool Layout::mouseover( const UID& uid )const
    {
        return _hit_record.hot == uid.id;
    }

    bool Layout::doubleClick( const UID& uid )const
    {
        return _hit_record.active == uid.id && _hit_record.clicks == 2;
    }

    bool Layout::click( const UID& uid )const
    {
        return _hit_record.active == uid.id && _hit_record.clicks == 1;
    }

    bool Layout::hasFocus( const UID& uid )const
    {
        return _focus == uid.id;
    }

    const kege::ui::Widget* Layout::elem( const UID& uid ) const
    {
        if ( _widgets.size() <= uid.widget_index)
        {
            kege::Log::error << "out of bound UID index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ uid.widget_index ].id != uid.id) return nullptr;
        return &_widgets[ uid.widget_index ];
    }

    kege::ui::Widget* Layout::elem( const UID& uid )
    {
        if ( _widgets.size() <= uid.widget_index)
        {
            kege::Log::error << "out of bound UID index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ uid.widget_index ].id != uid.id) return nullptr;
        return &_widgets[ uid.widget_index ];
    }

    kege::ui::Widget* Layout::elemParent( const UID& uid )
    {
        if ( _widgets.size() <= uid.widget_index)
        {
            kege::Log::error << "out of bound UID index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ uid.widget_index ].id != uid.id) return nullptr;
        return &_widgets[ _widgets[ uid.widget_index ].parent ];
    }

    const Widget* Layout::operator[]( uint32_t node_id )const
    {
        return &_widgets[ node_id ];
    }

    Widget* Layout::operator[]( uint32_t index )
    {
        return &_widgets[ index ];
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
            _layers[i].head = 0;
            _layers[i].tail = 0;
            _layers[i].count = 0;
        }
    }

    void Layout::resize( uint32_t max_nodes )
    {
        _widgets.resize( max_nodes );
        for (int i=0; i<max_nodes; ++i)
        {
            _widgets[i].index = i;
        }
    }

    uint32_t Layout::count()const
    {
        return _widget_count;
    }

    bool Layout::hasHit()const
    {
        return _hit.num != 0;
    }

    bool Layout::validate( uint32_t node_id )const
    {
        return node_id > 0 && node_id < _widgets.size();
    }

    void Layout::begin( double dms )
    {
        _dms = dms;
        
        _widget_count = 1;
        _root_count = 0;
        _parent_stack_count = 0;
        _current_parent = 0;
        _deferred_operations.reset();

        for (int i=0; i<_layers.size(); ++i)
        {
            _layers[i].head = 0;
            _layers[i].tail = 0;
            _layers[i].count = 0;
        }


        if ( _hit.num != 0)
        {
            _hit_record.active = _hit;
        }
        else
            _hit_record.active.num = 0;

        if ( _focus.num != 0)
        {
            _hit_record.focus = _focus;
        }
        _hit_record.hot = _hot;

        _focus = {};
        _hit = {};
    }

    void Layout::setWidgetParameters(uint32_t index , const WidgetDesc& desc)
    {
        Widget* widget = &_widgets[ index ];

        // assign uid to widget
        if( desc.uid )
        {
            widget->id = desc.uid->id;
            desc.uid->widget_index = index;
        }
        else
        {
            widget->id = {};
        }

        /**
         initalize the widgets parameters
         */
        //setWidgetParameters()

        widget->rect         = desc.rect;
        widget->style        = desc.style;
        widget->text         = desc.text;
        widget->text.color   = (desc.style)? desc.style->text_color : desc.text.color;

        widget->color        = (desc.style)? desc.style->background.color : desc.color;
        widget->texel        = desc.texel;
        widget->texr_info    = desc.texr_info;

        widget->single_click = desc.single_click;
        widget->double_click = desc.double_click;
        widget->inactive     = desc.inactive;
        widget->mouseover    = desc.mouseover;
        widget->visible      = desc.visible;
        widget->clip_overflow= desc.clip_overflow;

        widget->position     = desc.position;
        widget->border       = (desc.style)? desc.style->border : desc.border;
        widget->padding      = (desc.style)? desc.style->padding : desc.padding;
        widget->alignment    = (desc.style)? desc.style->alignment : desc.alignment;
        widget->gap          = (desc.style)? desc.style->gap : desc.gap;

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
    }

    void Layout::resolveParentChildRelation(uint32_t index)
    {
        ui::Widget& widget = _widgets[ index ];
        widget.parent = _current_parent;

        if ( _current_parent != 0 )
        {
            Widget& parent = _widgets[ _current_parent ];
            if ( parent.head == 0 )
            {
                parent.tail = parent.head = index;
            }
            else
            {
                Widget& tail = _widgets[ parent.tail ];

                tail.next   = index;
                widget.prev = parent.tail;
                parent.tail = index;
            }
            parent.count++;
        }
        else
        {
            if (_roots.size() <= _root_count)
            {
                _roots.resize(1 + 2 * _roots.size());
            }
            _roots[ _root_count ] = index;
            _root_count += 1;
        }
    }
    
    void Layout::addToDesignatedLayer(uint32_t index, const WidgetDesc& desc)
    {
        int layer_index = desc.layer;
        if (layer_index < 0)
        {
            if ( _current_parent != 0)
            {
                layer_index = _widgets[ _current_parent ].layer.layer;
            }
            else
            {
                layer_index = 0;
            }
        }

        ui::Layer& layer = _layers[ layer_index ];
        LayerNode& node = _widgets[ index ].layer;
        node.layer = layer_index;

        if ( layer.head == 0 )
        {
            layer.tail = layer.head = index;
            node.next = 0;
            node.prev = 0;
        }
        else
        {
            node.next = 0;
            node.prev = layer.tail;
            _widgets[ layer.tail ].layer.next = index;
            layer.tail = index;
        }

        layer.count++;
    }

    Id Layout::getHotElem(uint32_t node_index, bool button)
    {
        const ui::Widget& widget = _widgets[ node_index ];
        if ( !widget.visible || widget.inactive || widget.rect.width == 0.f || widget.rect.height == 0.f )
            return {};
        //if ( !_widgets[ node_index ].visible || _widgets[ node_index ].inactive ) return {};

        for (int child_index = _widgets[ node_index ].head; child_index != 0; child_index = _widgets[ child_index ].next)
        {
            Id id = getHotElem( child_index, button );
            if ( id.num != 0 )
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

        if ( !_widgets[ node_index ].mouseover )
            return {};

        if ( testPointVsRect( _input_manager->getMouse()->getPosition(), _widgets[ node_index ].rect ) )
        {
            _hot_index = node_index;
            return _widgets[ node_index ].id;
        }
        return {};
    }

    Id Layout::getHotElem(bool button)
    {
        Id hot = {};
        for (int layer_index=0; layer_index<_layers.size(); ++layer_index)
        {
            for( uint32_t root = _layers[ layer_index ].head; root != 0; root = _widgets[ root ].layer.next )
            {
                Id result = getHotElem( root, button );
                if ( result.num != 0 )
                {
                    hot = result;
                    break;
                }
            }
        }
        return hot;
    }

    uint32_t Layout::find(uint32_t widget_index, const ui::Id& id)
    {
        if ( id == _widgets[ widget_index ].id )
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
        for (int layer_index = 0; layer_index < _layers.size(); ++layer_index)
        {
            for( uint32_t root = _layers[ layer_index ].head; root != 0; root = _widgets[ root ].layer.next )
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
        _hit.num = 0;

        // when mouse button not down scan for hot element

        _mouse = _input_manager->getMouse();


        bool button_down = _mouse->isDown(MouseButtonCode::Left);
        if ( !button_down )
        {
            // handleButtonReleaseOnWidget()
            if ( _click_registered )
            {
                _click_registered = false;
                _active_index = find( _active );

                kege::dvec2 position = _mouse->getClickPosition(MouseButtonCode::Left);
                if( testPointVsRect( position, _widgets[ _active_index ].rect ) )
                {
                    if (_widgets[ _active_index ].single_click == ui::ClickTrigger::OnRelease)
                    {
                        _hit.num = _active.num;
                        _focus.num = _hot.num;
                        return;
                    }
                }
            }

            _hot = getHotElem();
            _active_index = 0;
            _active.num = 0;
        }
        else if ( button_down && !_click_registered )
        {
            _click_registered = true;
            _hot = getHotElem(true);
        }

        if ( button_down )
        {
            if ( _active.num == 0 && _hot.num != 0 )
            {
                kege::dvec2 position = _mouse->getClickPosition(MouseButtonCode::Left);
                if( testPointVsRect( position, _widgets[ _hot_index ].rect ) )
                {
                    _hit_record.clicks = (_mouse->isDoubleClick(MouseButtonCode::Left) ? 2 : 1);
                    _active_index = _hot_index;
                    _active.num = _hot.num;
                    _focus.num = _hot.num;

                    if (_widgets[ _hot_index ].single_click == ui::ClickTrigger::Immediate ||
                        _widgets[ _hot_index ].single_click == ui::ClickTrigger::Continuous)
                    {
                        _hit.num = _active.num;
                    }
                }
            }
            else if ( _active.num != 0 )
            {
                _active_index = find( _active );
                if (_widgets[ _active_index ].single_click == ui::ClickTrigger::Continuous ||
                    _widgets[ _active_index ].double_click == ui::ClickTrigger::Continuous)
                {
                    _hit = _widgets[ _active_index ].id;
                }
            }
            else
            {
                _focus.num = 0;
            }
        }
    }

    void Layout::end()
    {
        if ( 0 < _widget_count )
        {
            for (uint32_t root_index = 0; root_index < _root_count; ++root_index)
            {
                Resizer::resize( *this, _roots[ root_index ] );
                Aligner::align( *this, _roots[ root_index ] );
            }
            _deferred_operations.execute(this);
            handleInputEvents();
        }
    }

    bool Layout::buttonDown()const
    {
        return _click_registered;
    }

    const kege::InputManager* Layout::inputManager()const
    {
        return _input_manager;
    }

    Cursor* Layout::cursor()
    {
        return &_cursor;
    }

    Layout::Layout(kege::InputManager* input_manager, uint32_t width, uint32_t height, uint32_t quantity)
    :   _hot{}
    ,   _active{}
    ,   _active_index(0)
    ,   _hot_index(0)
    ,   _widget_count( 1 )
    ,   _height( height )
    ,   _width( width )
    ,   _cursor( this )
    ,   _parent_stack_count(0)
    ,   _root_count(0)
    ,   _input_manager( input_manager )
    ,   _mouse( input_manager->getMouse() )
    {
        resize( quantity );
    }

    Layout::~Layout()
    {
    }

}
