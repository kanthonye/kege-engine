//
//  ui-layout.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-input.hpp"
#include "ui-layout.hpp"

namespace kege::ui{

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

    bool Layout::onNumericInput(const ui::Elem& elem, kege::string* text)
    {
        return _cursor.onInput(Input::INPUT_NUMERIC, elem, _font, text);
    }

    bool Layout::onTextInput(const ui::Elem& elem, kege::string* text)
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

    const kege::dvec2& Layout::pointerPosition() const
    {
        return _input->_last_frame.position;
    }

    const kege::dvec2& Layout::deltaPosition() const
    {
        return _input->_last_frame.delta_position;
    }

    const kege::dvec2& Layout::scrollOffset() const
    {
        return _input->_last_frame.scroll_offset;
    }

    const bool Layout::pointerDragging() const
    {
        return _input->_last_frame.pointer_dragging;
    }

    bool Layout::mouseover( const ui::Elem& elem )const
    {
        return _hot[1].id == _nodes[ _widget_manager.getNodeIndex( elem._handle ) ].id;
    }

    bool Layout::doubleClick( const ui::Elem& elem )const
    {
        return _clicked.clicks == 2 && _clicked.id == _nodes[ _widget_manager.getNodeIndex( elem._handle ) ].id;
    }

    bool Layout::click( const ui::Elem& elem )const
    {
        return _clicked.clicks == 1 && _clicked.id == _nodes[ _widget_manager.getNodeIndex( elem._handle ) ].id;
    }

    bool Layout::hasFocus( const ui::Elem& elem )const
    {
        return _focus.id == _nodes[ _widget_manager.getNodeIndex( elem._handle ) ].id;
    }

    void Layout::setFocus( const ui::Elem& elem )
    {
        _focus.id = _nodes[ _widget_manager.getNodeIndex( elem._handle ) ].id;
    }

    Elem Layout::make( const Widget& widget )
    {
        Handle handle = _widget_manager.make( widget );
        if ( !_widget_manager[ handle ].text.text.empty() )
        {
//            if
//            (
//             _widget_manager[ id ].style->height.type == ui::SizingType::SIZE_FLEXIBLE ||
//             _widget_manager[ id ].style->align_text  == ui::AlignText::Right ||
//             _widget_manager[ id ].style->align_text  == ui::AlignText::Center
//            )
            {
                vec2 extent = computeExtent
                (
                 _widget_manager[ handle ].style->font_size,
                 _widget_manager[ handle ].text.text.c_str()
                );

                _widget_manager[ handle ].text.height = extent.y;
                _widget_manager[ handle ].text.width = extent.x;
            }
        }
        return Elem{ handle, this };
    }

    uint32_t Layout::put( const Elem& elem )
    {
        if ( !elem )
        {
            kege::Log::error << "Invalid Widget Index"<<Log::nl;
            return 0;
        }

        if ( _node_counter + 1 >= _nodes.size() )
        {
            kege::Log::error << "Reached Maximum amount of UI Node";
            return 0;
        }

        int node_index = ( _node_counter += 1 );
        _widget_manager.setNodeIndex( elem._handle, node_index );

        if ( 0 != _parent && node_index == _parent )
        {
            kege::Log::error << "Death cycle detected. Attaching child node to its self will cause the program to looping infinetly and become stuck"<<Log::nl;
            return 0;
        }

        _nodes[ node_index ].widgit = &_widget_manager[ elem._handle ];
        _nodes[ node_index ].parent = _parent;
        _nodes[ node_index ].count = 0;
        _nodes[ node_index ].head = 0;
        _nodes[ node_index ].tail = 0;
        _nodes[ node_index ].next = 0;
        //_nodes[ node_index ].next_free = 0;
        //_nodes[ node_index ].freed = false;
        _nodes[ node_index ].id = node_index;


        // if style width and height is fixed, set the rect width and height of the ui element
        if ( _nodes[ node_index ].widgit->style->height.type == kege::ui::SIZE_FIXED )
        {
            _nodes[ node_index ].widgit->rect.height = _nodes[ node_index ].widgit->style->height.size;
        }
        if ( _nodes[ node_index ].widgit->style->width.type == kege::ui::SIZE_FIXED )
        {
            _nodes[ node_index ].widgit->rect.width = _nodes[ node_index ].widgit->style->width.size;
        }

        // setup the layout tree hierarchy
        if ( 0 < _parent )
        {
            if ( _nodes[ _parent ].head == 0 )
            {
                _nodes[ _parent ].tail = _nodes[ _parent ].head = _node_counter;
            }
            else
            {
                _nodes[ _nodes[ _parent ].tail ].next = _node_counter;
                _nodes[ _parent ].tail = _node_counter;
            }
            _nodes[ _parent ].count++;

            _nodes[ node_index ].depth = 1 + _nodes[ _parent ].depth;
        }

        if ( _root == 0 )
        {
            _root = node_index;
        }
        return node_index;
    }

    uint32_t Layout::push( const Elem& elem )
    {
        _parent = put( elem );
        return _parent;
    }

    uint32_t Layout::pop()
    {
        int pid = _parent;
        if ( 0 != _parent )
        {
            _parent = _nodes[ _parent ].parent;
//            if ( _aligner.shouldPreCalcFlexibleSize( *this, pid ) )
//            /**
//             * We only need to compute the lengths for elements with flexible lengths. This
//             * is best done when popping a parent element from the stack. for onter lengths
//             * types like percentage and extend, these length are computed at the end of
//             * building the interface.
//             */
//            {
//                _aligner.computeFlexibleLengths( *this, pid );
//            }
        }
        return pid;
    }

    const kege::ui::Widget* Layout::operator[]( const Elem& elem ) const
    {
        return &_widget_manager[ elem._handle ];
    }

    kege::ui::Widget* Layout::operator[]( const Elem& elem )
    {
        return &_widget_manager[ elem._handle ];
    }

    const Widget* Layout::operator[]( NodeIndex node_id )const
    {
        return _nodes[ node_id ].widgit;
    }

    Widget* Layout::operator[]( NodeIndex index )
    {
        return _nodes[ index ].widgit;
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
        return _nodes[ index ].parent;
    }
    
    NodeIndex Layout::head( NodeIndex index )const
    {
        return _nodes[ index ].head;
    }

    NodeIndex Layout::tail( NodeIndex index )const
    {
        return _nodes[ index ].tail;
    }

    NodeIndex Layout::next( NodeIndex index )const
    {
        return _nodes[ index ].next;
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
        return _nodes[ index ].count;
    }

    void Layout::resize( uint32_t max_nodes )
    {
        _widget_manager.resize( max_nodes );
        _nodes.resize( max_nodes );
    }

    uint32_t Layout::count()const
    {
        return _node_counter;
    }

    bool Layout::validate( uint32_t node_id )const
    {
        return node_id > 0 && node_id < _nodes.size();
    }

    void Layout::handleMouseOverEvents()
    {
        // Reset current hot element at start of frame
        _hot[0] = {};

        // First, check if previous hot element is still valid and under mouse
        if (_hot[1].id != 0)
        {
            if ( testPointVsRect( _input->_curr_frame.position, _nodes[ _hot[1].id ].widgit->rect ) )
            {
                // If previous hot element has children, there is a possibility that the mouse
                // is over its child element. So, we need to account for those child elements.
                if ( _nodes[ _hot[1].id ].count )
                {
                    findNewHotElement( _hot[1].id );
                }

                // If the previous hot element has no children then it is still under mouse - keep it hot
                if (_hot[0].id == 0)
                {
                    _hot[0] = _hot[1];
                }

            }
        }

        // If we didn't find a persistent hot element, search for a new one
        if (_hot[0].id == 0)
        {
            findNewHotElement(1);
        }

        // Handle mouse enter/leave events
        if (_hot[0].id != _hot[1].id)
        {
            if (_hot[1].id != 0)
            {
                std::cout  <<"mouse exit: " << _hot[1].id <<"\n";
            }

            if (_hot[0].id != 0)
            {
                std::cout  <<"mouse enter: " << _hot[0].id <<"\n";
            }
        }

        // Handle focus logic (separate from mouseover)
        //handleFocusLogic(root);

        // Store current hot for next frame
        _hot[1] = _hot[0];
    }


    void Layout::findNewHotElement( uint32_t root )
    {
        for (uint32_t ui_index = _nodes[ root ].head; ui_index != 0 ; ui_index = _nodes[ ui_index ].next )
        {
            findNewHotElement( ui_index );
        }

        if ( _nodes[ root ].widgit->mouseover && _nodes[ root ].widgit->visible )
        {
            if ( testPointVsRect( _input->_curr_frame.position, _nodes[ root ].widgit->rect ) )
            {
                if ( _hot[0].depth < _nodes[ root ].depth  )
                {
                    if ( _hot[0].id == 0 )
                    {
                        _hot[0].id    = _nodes[ root ].id;
                        _hot[0].depth = _nodes[ root ].depth;
                    }
                    else if ( _nodes[ _hot[0].id ].widgit->style->zindex <= _nodes[ root ].widgit->style->zindex )
                    {
                        _hot[0].id    = _nodes[ root ].id;
                        _hot[0].depth = _nodes[ root ].depth;
                    }
                }
            }
        }
    }

    void Layout::handleButtonUpEvents()
    {
        if ( _active[1].id != 0 && _active[1].id < _nodes.size() )
        {
            if
            (
                _nodes[ _active[1].id ].widgit->single_click == ui::ClickTrigger::OnRelease ||
                _nodes[ _active[1].id ].widgit->double_click == ui::ClickTrigger::OnRelease
            )
            {
                std::cout  <<"release: " << _active[1].id <<"| " << _hot[1].id <<"\n";
                if ( _hot[1].id == _active[1].id )
                {
                    std::cout <<"FIRE ON RELEASE\n";
                    _clicked = _active[1];
                }
            }
            _active[1] = {};
        }
    }

    void Layout::handleButtonDownEvents()
    {
        _active[0] = {};

        // Persist active state while holding
        if (_active[1].id != 0)
        {
            _active[0] = _active[1];

            // OnClick fires continuous
            if
            (
                _nodes[_active[0].id].widgit->single_click == ui::ClickTrigger::Continuous ||
                _nodes[_active[0].id].widgit->double_click == ui::ClickTrigger::Continuous
            )
            {
                std::cout <<"DOUBLE CLICK: FIRE CONTINUOUSLY\n";
                _clicked = _active[0];
                // Fire callback here
            }
        }
        // Initiate new click
        else if (_hot[0].id != 0)
        {
            if (_input->_curr_frame.double_click )
            {
                _active[0] = _hot[0];
                _active[0].clicks = 2;
                _focus = _active[0];

                // OnClick fires immediately
                if
                (
                    _nodes[_active[0].id].widgit->double_click == ui::ClickTrigger::Immediate ||
                    _nodes[_active[0].id].widgit->double_click == ui::ClickTrigger::Continuous
                )
                {
                    std::cout <<"DOUBLE CLICK: FIRE IMMEDIATELY\n";
                    _clicked = _active[0];
                    // Fire callback here
                }
            }
            else if ( _input->_curr_frame.single_click )
            {
                _active[0] = _hot[0];
                _active[0].clicks = 1;
                _focus = _active[0];

                // OnClick fires immediately
                if
                (
                    _nodes[_active[0].id].widgit->single_click == ui::ClickTrigger::Immediate ||
                    _nodes[_active[0].id].widgit->single_click == ui::ClickTrigger::Continuous
                )
                {
                    std::cout <<"SINGLE CLICK: FIRE IMMEDIATELY\n";
                    _clicked = _active[0];
                    // Fire callback here
                }
            }
        }
        _active[1] = _active[0];  // Store for next frame

        // If button not down, _active[0] stays empty
        // but _active[1] retains value for release detection
    }
    HitRecord Layout::scanEvents(uint32_t root)
    {
        HitRecord rec{};
        const auto* inp = &_input->_curr_frame;
        const bool  leftDown  = inp->button_down && inp->single_click;
        const bool  leftPressed  = inp->single_click;
        const bool  leftReleased = !leftDown && _button_down;
        const bool  doubleClick  = inp->double_click;

        for (uint32_t i = root; i != 0; i = next(i))
        {
            const Node& n = _nodes[i];
            if (!n.widgit->visible) continue;

            bool inside = testPointVsRect(inp->position, n.widgit->rect);
            if (inside) {
                rec.hotId = n.id;
                if (leftPressed || doubleClick) {
                    rec.activeId = n.id;
                    rec.focusId  = n.id;
                    rec.clicks   = doubleClick ? 2 : 1;
                }
            }
            // depth-first recursion
            if (n.count) {
                auto childRec = scanEvents(n.id + 1);  // first child
                if (childRec.hotId && !rec.hotId) rec = childRec;
            }
        }
        rec.release = leftReleased && rec.activeId;
        return rec;
    }
    
    void Layout::begin( double dms, ui::Input* input )
    {
        _root = 0;
        _parent = 0;
        _input = input;
        _node_counter = 0;

        _widget_manager.refresh();

        _cursor.update( dms, input );
    }

    void Layout::end()
//    {
//        if (_node_counter == 0) return;
//
//        align( *this, _root );                    // one layout pass
//        HitRecord hit = scanEvents(_root);
//
//        _hot[CURR] = {hit.hotId, 0, 0, 0, false};
//        if (hit.activeId) {
//            _active[CURR] = {hit.activeId, 0, 0, hit.clicks, false};
//            _focus = {hit.focusId, 0, 0, 0, false};
//        }
//        _clicked = (hit.release && hit.activeId == hit.hotId) ? _active[CURR] : State{};
//
//        _hot[PREV]    = _hot[CURR];
//        _active[PREV] = _active[CURR];
//        _button_down  = _input->buttonDown();
//    }
    {
        /**
         * If the nodes are not empty, we align the layout.
         */
        if ( 0 < _node_counter )
        {
            if ( 0 < _root )
            {
                align( *this, _root );
            }

            handleMouseOverEvents();

            _clicked = {};
            _button_down = _input->buttonDown();
            if ( _button_down )
            {
                handleButtonDownEvents();
            }
            else
            {
                handleButtonUpEvents();
            }
        }
//        if ( _active[0].id )
//        {
//            _cursor = _nodes[ _active[0].id ].content->rect;
//            _cursor.width = 4;
//        }
    }

    bool Layout::buttonDown()const
    {
        return _input->_last_frame.button_down;
    }

    ui::Input* Layout::input()
    {
        return _input;
    }

    Layout::Layout(uint32_t width, uint32_t height)
    :   _parent( 0 )
    ,   _hot{}
    ,   _active{}
    ,   _node_counter( 0 )
    ,   _root( 0 )
    ,   _button_down( false )
    ,   _height( height )
    ,   _width( width )
    ,   _cursor( this )
    {
    }

}
