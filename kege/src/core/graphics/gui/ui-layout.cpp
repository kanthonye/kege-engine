//
//  ui-layout.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-layout.hpp"

namespace kege::ui{

    float Layout::getClickToCursorOffset( ui::EID& id, const std::string& text, int font_size, int32_t* cursor, const kege::Font& font )
    {
        float length = 0;

        *cursor = 0;
        float char_width;
        float max_length = _input->clickPosition().x - _nodes[ id ].content->x;
        for ( const char* s = text.data(); *s != 0; ++s )
        {
            char_width = font->getCharWidth( font_size, *s );
            if ( ((length + char_width) - max_length) > 0.5 )
            {
                break;
            }
            length += char_width;
            *cursor += 1;
        }

        return length;
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

    bool Layout::mouseover( const ui::EID& id )const
    {
        return _prev_hot.id == _nodes[ _widget_manager.getNodeIndex( id.index ) ].id;
    }

    bool Layout::doubleClick( const ui::EID& id )const
    {
        return _curr_active.clicks == 2 && _curr_active.id == _nodes[ _widget_manager.getNodeIndex( id.index ) ].id;
    }

    bool Layout::click( const ui::EID& id )const
    {
        return _curr_active.clicks == 1 && _curr_active.id == _nodes[ _widget_manager.getNodeIndex( id.index ) ].id;
    }

    bool Layout::hasFocus( const ui::EID& id )const
    {
        return _focus.id == _nodes[ _widget_manager.getNodeIndex( id.index ) ].id;
    }

    void Layout::setFocus( const ui::EID& id )
    {
        _focus.id = _nodes[ _widget_manager.getNodeIndex( id.index ) ].id;
    }

    EID Layout::make( const Widget& widget )
    {
        int id = _widget_manager.make( widget );

        if ( !_widget_manager[ id ].text.text.empty() )
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
                 _widget_manager[ id ].style->font_size,
                 _widget_manager[ id ].text.text.c_str()
                );

                _widget_manager[ id ].text.height = extent.y;
                _widget_manager[ id ].text.width = extent.x;
            }
        }
        return EID
        {
            id,
            this
        };
    }

    uint32_t Layout::put( const EID& id )
    {
        if ( id.index < 0 )
        {
            kege::Log::error << "Invalid Widget Index"<<Log::nl;
            return 0;
        }

        if ( _node_counter + 1 >= _nodes.size() )
        {
            kege::Log::error << "Reached Maximum amount of UI Node";
            return 0;
        }

        if ( 0 != _parent && _widget_manager.getNodeIndex( id.index ) == _parent )
        {
            kege::Log::error << "Death cycle detected. Attaching child node to its self will cause the program to looping infinetly and become stuck"<<Log::nl;
            return 0;
        }

        int node_index = ( _node_counter += 1 );

        _widget_manager.setNodeIndex( id.index, node_index );

        _nodes[ node_index ].content = &_widget_manager[ id.index ];
        _nodes[ node_index ].parent = _parent;
        _nodes[ node_index ].count = 0;
        _nodes[ node_index ].head = 0;
        _nodes[ node_index ].tail = 0;
        _nodes[ node_index ].next = 0;
        //_nodes[ node_index ].next_free = 0;
        //_nodes[ node_index ].freed = false;
        _nodes[ node_index ].id = node_index;


        // if style width and height is fixed, set the rect width and height of the ui element
        if ( _nodes[ node_index ].content->style->height.type == kege::ui::SIZE_FIXED )
        {
            _nodes[ node_index ].content->rect.height = _nodes[ node_index ].content->style->height.size;
        }
        if ( _nodes[ node_index ].content->style->width.type == kege::ui::SIZE_FIXED )
        {
            _nodes[ node_index ].content->rect.width = _nodes[ node_index ].content->style->width.size;
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

        return node_index;
    }

    uint32_t Layout::push( const EID& id )
    {
        _parent = put( id );
        if ( _root == 0 )
        {
            _root = _parent;
        }
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

    const kege::ui::Widget* Layout::operator[]( const EID& eid ) const
    {
        return &_widget_manager[ eid.index ];
    }

    kege::ui::Widget* Layout::operator[]( const EID& eid )
    {
        return &_widget_manager[ eid.index ];
    }

    const Widget* Layout::operator[]( NodeIndex node_id )const
    {
        return _nodes[ node_id ].content;
    }

    Widget* Layout::operator[]( NodeIndex index )
    {
        return _nodes[ index ].content;
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

    void Layout::setFont( const kege::Font& font )
    {
        _font = font;
    }

    const kege::Font& Layout::getFont()const
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
        _curr_hot = {0, 0, 0};

        // First, check if previous hot element is still valid and under mouse
        if (_prev_hot.id != 0)
        {
            if ( testPointVsRect( _input->currentPosition(), _nodes[ _prev_hot.id ].content->rect ) )
            {
                // If previous hot element has children, there is a possibility that the mouse
                // is over its child element. So, we need to account for those child elements.
                if ( _nodes[ _prev_hot.id ].count )
                {
                    findNewHotElement( _prev_hot.id );
                }

                // If the previous hot element has no children then it is still under mouse - keep it hot
                if (_curr_hot.id == 0)
                {
                    _curr_hot = _prev_hot;
                }

            }
        }

        // If we didn't find a persistent hot element, search for a new one
        if (_curr_hot.id == 0)
        {
            findNewHotElement(1);
        }

        // Handle mouse enter/leave events
        if (_curr_hot.id != _prev_hot.id)
        {
            if (_prev_hot.id != 0)
            {
                std::cout  <<"mouse exit: " << _prev_hot.id <<"\n";
            }

            if (_curr_hot.id != 0)
            {
                std::cout  <<"mouse enter: " << _curr_hot.id <<"\n";
            }
        }

        // Handle focus logic (separate from mouseover)
        //handleFocusLogic(root);

        // Store current hot for next frame
        _prev_hot = _curr_hot;
    }


    void Layout::findNewHotElement( uint32_t root )
    {
//        if ( _curr_hot.id != 0) {
//            return;
//        }

        for (uint32_t ui_index = _nodes[ root ].head; ui_index != 0 ; ui_index = _nodes[ ui_index ].next )
        {
            findNewHotElement( ui_index );
        }

        if ( _nodes[ root ].content->mouseover && _nodes[ root ].content->visible )
        {
            if ( testPointVsRect( _input->currentPosition(), _nodes[ root ].content->rect ) )
            {
                if ( _curr_hot.depth < _nodes[ root ].depth  )
                {
                    if ( _curr_hot.id == 0 )
                    {
                        _curr_hot.id    = _nodes[ root ].id;
                        _curr_hot.depth = _nodes[ root ].depth;
                    }
                    else if ( _nodes[ _curr_hot.id ].content->style->zindex <= _nodes[ root ].content->style->zindex )
                    {
                        _curr_hot.id    = _nodes[ root ].id;
                        _curr_hot.depth = _nodes[ root ].depth;
                    }
                }
            }
        }
    }

    void Layout::handleButtonUpEvents()
    {
        if ( _input->buttonDown() ) return;

        if ( _prev_active.id != 0 && _prev_active.id < _nodes.size() )
        {
            if ( _nodes[ _prev_active.id ].content->trigger == ui::ClickTrigger::OnRelease )
            {
                std::cout  <<"release: " << _prev_active.id <<"| " << _prev_hot.id <<"\n";
                if ( _prev_hot.id == _prev_active.id )
                {
                    std::cout  <<"FIRE ON RELEASE\n";
                    _curr_active = _prev_active;
                    _focus = _prev_active;
                }
            }
            _prev_active = {};
        }
    }


    void Layout::handleButtonDownEvents()
    {
        _curr_active = {};
        if ( _input->buttonDown() )
        {
            if ( _prev_active.id != 0 )
            {
                _curr_active = _prev_active;
            }

            if ( _curr_active.id == 0 && _curr_hot.id != 0  )
            {
                if ( _input->doubleClick() )
                {
                    _curr_active = _curr_hot;
                    _curr_active.clicks = 2;
                    std::cout  <<"double-click : " << _curr_active.id <<"\n";

                    if ( _nodes[ _curr_active.id ].content->trigger == ui::ClickTrigger::OnClick )
                    {
                        _curr_active.active = true;
                        _focus = _curr_active;
                    }
                }
                else if ( _input->primaryClick() && _curr_hot.id != 0 )
                {
                    _curr_active = _curr_hot;
                    _curr_active.clicks = 1;
                    std::cout  <<"single-click : " << _curr_active.id <<"\n";

                    if ( _nodes[ _curr_active.id ].content->trigger == ui::ClickTrigger::OnClick )
                    {
                        _curr_active.active = true;
                        _focus = _curr_active;
                    }
                }
                _prev_active = _curr_active;
            }
        }
    }

    void Layout::begin( ui::Input* input )
    {
        _root = 0;
        _parent = 0;
        _input = input;
        _node_counter = 0;

        _widget_manager.refresh();
    }

    void Layout::end()
    {
        /**
         * If the nodes are not empty, we align the layout.
         */
        if ( !_nodes.empty() )
        {
            align( *this, _root );
        }

        _button_down = _input->buttonDown();
        handleMouseOverEvents();
        handleButtonDownEvents();
        handleButtonUpEvents();
    }

    bool Layout::buttonDown()const
    {
        return _button_down;
    }

    ui::Input* Layout::input()
    {
        return _input;
    }

    Layout::Layout()
    :   _parent( 0 )
    ,   _curr_hot{}
    ,   _prev_active{}
    ,   _curr_active{}
    ,   _node_counter( 0 )
    ,   _root( 0 )
    ,   _button_down( false )
    {
    }

}
