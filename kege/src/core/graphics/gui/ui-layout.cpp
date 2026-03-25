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


    WidgetId Layout::pushRoot( const WidgetDesc& desc )
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

    WidgetId Layout::putRoot( const WidgetDesc& desc )
    {
        int parent = _current_parent;
        _current_parent = 0;

        WidgetId widget_id = put( desc );

        _current_parent = parent;
        return widget_id;
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

    WidgetId Layout::push( const WidgetDesc& desc )
    {
        WidgetId widget_id = put( desc );
        if ( widget_id.id != 0 )
        {
            _current_parent = widget_id.index;
        }
        return widget_id;
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

    WidgetId Layout::put( const WidgetDesc& desc )
    {
        if (_widget_count >= _widgets.size()) {
            kege::Log::error << "exceeding maximum ui widget capacity.";
            return {};
        }

        const uint32_t widget_index = _widget_count;
        _widget_count += 1;

        setWidgetParameters( widget_index, desc );
        addToDesignatedLayer( widget_index, desc );

        resolveParentChildRelation( widget_index );

        return WidgetId( _widgets[ widget_index ].index,_widgets[ widget_index ].version );
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

    bool Layout::onNumericInput(const WidgetId& widget_id, char* str, size_t& size)
    {
        return 0;//_cursor.onInput(Cursor::InputType::Numeric, uid, _font, str, size);
    }

    bool Layout::onTextInput(const WidgetId& widget_id, char* str, size_t& size)
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

    bool Layout::mouseover( const UserId& uid )const
    {
        return _curr.hot.user_id == uid && _curr.hot.user_id != 0;
    }

    bool Layout::doubleClick( const UserId& uid )const
    {
        return _curr.hit.user_id == uid && _curr.clicks == 2 && _curr.hit.user_id != 0;
    }

    bool Layout::click( const UserId& uid )const
    {
        return _curr.hit.user_id == uid && _curr.clicks == 1 && _curr.hit.user_id != 0;
    }

    bool Layout::hasFocus( const UserId& uid )const
    {
        return _curr.focus.user_id == uid && _curr.focus.user_id != 0;
    }

    const kege::ui::Widget* Layout::elem( const WidgetId& widget_id ) const
    {
        if ( _widgets.size() <= widget_id.index)
        {
            kege::Log::error << "out of bound WidgetId index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ widget_id.index ].user_id != widget_id.id) return nullptr;
        return &_widgets[ widget_id.index ];
    }

    kege::ui::Widget* Layout::elem( const WidgetId& widget_id )
    {
        if ( _widgets.size() <= widget_id.index)
        {
            kege::Log::error << "out of bound WidgetId index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ widget_id.index ].user_id != widget_id.id) return nullptr;
        return &_widgets[ widget_id.index ];
    }

    kege::ui::Widget* Layout::elemParent( const WidgetId& widget_id )
    {
        if ( _widgets.size() <= widget_id.index)
        {
            kege::Log::error << "out of bound WidgetId index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ widget_id.index ].user_id != widget_id.id) return nullptr;
        return &_widgets[ _widgets[ widget_id.index ].parent ];
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
        return _curr.hit.user_id != 0;
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


        // update current hit
        if ( _next.hit.user_id != 0 )
        {
            _curr.hit = _next.hit;
        }
        else
        {
            _curr.hit.user_id = 0;
        }

        // update current focus
        _curr.focus = _next.focus;

        // update current mouse over
        _curr.hot = _next.hot;

        //_focus = {};
        //_hit = {};

        _left_click_down = _left_click_state;
        //_left_click_state = false;
    }

    void Layout::setWidgetParameters(uint32_t index, const WidgetDesc& desc)
    {
        Widget* widget = &_widgets[ index ];

        // assign uid to widget
        widget->user_id = desc.user_id;
        widget->version = widget->version + 1;
        //widget->index = index;

        /**
         initalize the widgets parameters
         */
        //setWidgetParameters()

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

        widget->parent       = 0;
        widget->count        = 0;
        widget->head         = 0;
        widget->tail         = 0;
        widget->next         = 0;
        widget->prev         = 0;

        if (widget->width.type == ui::SizingType::Fixed)
        {
            widget->rect.width += widget->width.size;
        }
        if (widget->height.type == ui::SizingType::Fixed)
        {
            widget->rect.height += widget->height.size;
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

    Record Layout::getHotElem(uint32_t node_index, bool button)
    {
        const ui::Widget& widget = _widgets[ node_index ];
        if ( !widget.visible || widget.inactive || widget.rect.width == 0.f || widget.rect.height == 0.f )
            return {};
        
        for (int child_index = _widgets[ node_index ].head; child_index != 0; child_index = _widgets[ child_index ].next)
        {
            Record record = getHotElem( child_index, button );
            if ( record.user_id != 0 )
            {
                return record;
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
            return Record
            {
                .index = node_index,
                .user_id = _widgets[ node_index ].user_id
            };
        }
        return {};
    }

    Record Layout::getHotElem(bool button)
    {
        Record hot = {};
        for (int layer_index=0; layer_index<_layers.size(); ++layer_index)
        {
            for( uint32_t root = _layers[ layer_index ].head; root != 0; root = _widgets[ root ].layer.next )
            {
                Record result = getHotElem( root, button );
                if ( result.user_id != 0 )
                {
                    hot = result;
                    break;
                }
            }
        }
        return hot;
    }

    uint32_t Layout::find(uint64_t user_id, uint32_t index)
    {
        if ( user_id == _widgets[ index ].user_id )
        {
            return index;
        }

        uint32_t widget_index = _widgets[ index ].head;
        for (; widget_index != 0; widget_index = _widgets[ widget_index ].next)
        {
            uint32_t index = find( user_id, widget_index );
            if ( index != 0 )
            {
                return index;
            }
        }
        return 0;
    }

    uint32_t Layout::find(uint64_t user_id)
    {
        for (int layer_index = 0; layer_index < _layers.size(); ++layer_index)
        {
            uint32_t widget_index = _layers[ layer_index ].head;
            for( ; widget_index != 0; widget_index = _widgets[ widget_index ].layer.next )
            {
                uint32_t index = find( user_id, widget_index );
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
        _next.hit.user_id = 0;

        // when mouse button not down scan for hot element

        _mouse = _input_manager->getMouse();


        _left_click_state = _mouse->isDown(MouseButtonCode::Left);
        if ( !_left_click_state )
        {
            if ( _click_registered )
            {
                _click_registered = false;
                //_active_index = find( _active );

                if (_next.hot.index != 0 && _next.hot.index < _widgets.size())
                {
                    kege::dvec2 position = _mouse->getClickPosition(MouseButtonCode::Left);
                    if( testPointVsRect( position, _widgets[ _next.pressing.index ].rect ) )
                    {
                        if (_widgets[ _next.pressing.index ].single_click == ui::ClickTrigger::OnRelease)
                        {
                            _next.focus = _next.hit = _next.pressing;
                            return;
                        }
                    }
                }
            }

            _next.hot = getHotElem();
            _next.pressing = {};
            std::cout <<"INDEX: "<< _next.hot.index << " : " << _next.hot.user_id <<"\n";
        }
        else if ( _left_click_state && !_click_registered )
        {
            //_left_click_state = true;
            _click_registered = true;
            _next.hot = getHotElem(true);
        }

        if ( _left_click_state )
        {
            if ( _next.pressing.user_id == 0 && _next.hot.user_id != 0 )
            {
                kege::dvec2 position = _mouse->getClickPosition(MouseButtonCode::Left);
                if( testPointVsRect( position, _widgets[ _next.hot.index ].rect ) )
                {
                    _next.clicks = (_mouse->isDoubleClick(MouseButtonCode::Left) ? 2 : 1);
                    _next.pressing = _next.hot;
                    _next.focus = _next.hot;

                    if (_widgets[ _next.hot.index ].single_click == ui::ClickTrigger::Immediate ||
                        _widgets[ _next.hot.index ].single_click == ui::ClickTrigger::Continuous)
                    {
                        _next.hit = _next.hot;
                    }
                }
            }
            else if ( _next.pressing.user_id != 0 )
            {
                //_active_index = find( _active );
                if (_widgets[ _next.pressing.index ].single_click == ui::ClickTrigger::Continuous ||
                    _widgets[ _next.pressing.index ].double_click == ui::ClickTrigger::Continuous)
                {
                    _next.hit = _next.pressing;
                }
            }
            else
            {
                _next.focus.user_id = 0;
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

    bool Layout::leftClickDown()const
    {
        return _left_click_down;
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
    :   _curr{}
    ,   _next{}
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
