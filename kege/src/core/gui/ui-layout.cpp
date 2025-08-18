//
//  ui-layout.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-canvas.hpp"
#include "ui-layout.hpp"

namespace kege::ui{

    float Layout::getClickToCursorOffset( ui::EID& id, const std::string& text, int font_size, int32_t* cursor, const kege::Font& font )
    {
        float length = 0;

        *cursor = 0;
        float char_width;
        float max_length = _input->clickPosition().x - _nodes[ id ].elem.x;
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

    bool Layout::testPointVsRect( const kege::dvec2& p, const ui::Rect2D& rect )const
    {
        return
        (
            p.x > rect.x &&
            p.y > rect.y &&
            p.x < rect.x + rect.width &&
            p.y < rect.y + rect.height
        );
    }

    bool Layout::mouseover( uint32_t id )const
    {
        return _mouseover_elem.id == id;
    }

    bool Layout::doubleClick( uint32_t id )const
    {
        return _clicked.id == id && _clicked.clicks == 2;
    }

    bool Layout::click( uint32_t id )const
    {
        return _clicked.id == id && _clicked.clicks == 1;
    }

    bool Layout::hasFocus( uint32_t id )const
    {
        return _focus_id == id;
    }

    void Layout::setFocus( uint32_t id )
    {
        _focus_id = id;
    }

    EID Layout::make( const Info& info )
    {
        uint32_t id = genId();
        if( id <= 0 || id >= _nodes.size() )
        //if ( _count + 1 >= _nodes.size() )
        {
            KEGE_LOG_ERROR << "Reached Maximum amount of UI UIElems -> " << _count + 1 << Log::nl;
            return {};
        }

        //_count += 1;
        //_nodes[ id ] = {};
        _nodes[ id ].elem.style = info.style;
        _nodes[ id ].elem.on_click = info.on_click;
        _nodes[ id ].elem.on_scroll = info.on_scroll;
        _nodes[ id ].elem.on_release = info.on_release;
        _nodes[ id ].elem.on_mouse_exit = info.on_mouse_exit;
        _nodes[ id ].elem.on_mouse_enter = info.on_mouse_enter;
        _nodes[ id ].elem.click_trigger = info.click_trigger;
        _nodes[ id ].elem.text.data = info.text;
        _count++;
        return {id, this};
    }

    UIElem& Layout::put( const EID& id )
    {
        if ( id.index >= _nodes.size() )
        {
            // TODO: LOG_ERROR << "Reached Maximum amount of UI UIElems";
            return _nodes[ 0 ].elem;
        }

        if ( id.index == _parent )
        {
            KEGE_LOG_ERROR << "Death cycle detected. Attaching child node to its self will cause the program to looping infinetly and become stuck"<<Log::nl;
            return _nodes[ 0 ].elem;
        }

        _nodes[ id.index ].parent = _parent;
        _nodes[ id.index ].count = 0;
        _nodes[ id.index ].head = 0;
        _nodes[ id.index ].tail = 0;
        _nodes[ id.index ].next = 0;
        _nodes[ id.index ].index = id.index;
        _nodes[ id.index ].id = id.index;
        _nodes[ id.index ].next_free = 0;
        _nodes[ id.index ].freed = false;

        // if style width and height is fixed, set the rect width and height of the ui element
        if ( _nodes[ id.index ].elem.style.height.type == kege::ui::SIZE_FIXED )
        {
            _nodes[ id.index ].elem.rect.height = _nodes[ id.index ].elem.style.height.size;
        }
        if ( _nodes[ id.index ].elem.style.width.type == kege::ui::SIZE_FIXED )
        {
            _nodes[ id.index ].elem.rect.width = _nodes[ id.index ].elem.style.width.size;
        }

        // setup the layout tree hierarchy
        if ( 0 < _parent )
        {
            if ( _nodes[ _parent ].head == 0 )
            {
                _nodes[ _parent ].tail = _nodes[ _parent ].head = id.index;
            }
            else
            {
                _nodes[ _nodes[ _parent ].tail ].next = id.index;
                _nodes[ _parent ].tail = id.index;
            }
            _nodes[ _parent ].count++;

            _nodes[ id.index ].level = 1 + _nodes[ _parent ].level;
        }

        return _nodes[ id.index ].elem;
    }

    UIElem& Layout::push( const EID& id )
    {
        UIElem& elem = put( id );
        _parent = id.index;
        if ( _root == 0 )
        {
            _root = _parent;
        }
        return elem;
    }

    uint32_t Layout::pop()
    {
        int pid = _parent;
        if ( 0 != _parent )
        {
            _parent = _nodes[ _parent ].parent;
            if ( _aligner.shouldPreCalcFlexibleSize( *this, pid ) )
            /**
             * We only need to compute the lengths for elements with flexible lengths. This
             * is best done when popping a parent element from the stack. for onter lengths
             * types like percentage and extend, these length are computed at the end of
             * building the interface.
             */
            {
                _aligner.computeFlexibleLengths( *this, pid );
            }
        }
        return pid;
    }

    const UIElem& Layout::operator[]( uint32_t node_id )const
    {
        return _nodes[ node_id ].elem;
    }

    UIElem& Layout::operator[]( uint32_t index )
    {
        return _nodes[ index ].elem;
    }

    const kege::ui::Node& Layout::nodes( uint32_t index ) const
    {
        return _nodes[ index ];
    }

    kege::ui::Node& Layout::nodes( uint32_t index )
    {
        return _nodes[ index ];
    }

    void Layout::resize( uint32_t max_nodes )
    {
        _nodes.resize( max_nodes );

        _id_pool.resize( max_nodes );

        // Initialize the free list: each node points to the next.
        for (int32_t i = 0; i < max_nodes - 1; ++i)
        {
            _id_pool[i].prev  = i - 1;
            _id_pool[i].index = i;
            _id_pool[i].next  = i + 1;
            _id_pool[i].duplicates  = 0;
        }
        _id_pool[ max_nodes - 1 ].next = -1;
        _available_id = 1;
    }

    bool Layout::validate( uint32_t node_id )const
    {
        return node_id > 0 && node_id < _nodes.size();
    }

    bool Layout::parent( uint32_t node_id )const
    {
        return validate( _nodes[ node_id ].head );
    }

    void Layout::handleMouseOverEvents( uint32_t root )
    {
        if ( _focus.id == 0 && _focus_id != 0 )
        {
            if ( _focus_id == _nodes[ root ].id )
            {
                _focus.id = _nodes[ root ].id;
                _focus.index = _nodes[ root ].index;
                _focus.level = _nodes[ root ].level;
            }
        }

        for (uint32_t ui_index = _nodes[ root ].head; ui_index != 0 ; ui_index = _nodes[ ui_index ].next )
        {
            handleMouseOverEvents( ui_index );
        }

        if ( _nodes[ root ].elem.style.mouseover )
        {
            if ( testPointVsRect( _input->currentPosition(), _nodes[ root ].elem.rect ) )
            {
                if ( _curr_hot_elem.level < _nodes[ root ].level )
                {
                    _curr_hot_elem.id    = _nodes[ root ].id;
                    _curr_hot_elem.index = _nodes[ root ].index;
                    _curr_hot_elem.level = _nodes[ root ].level;
                }
            }
        }
    }

    void Layout::handleMouseOver()
    {
        handleMouseOverEvents( 1 );
        if ( _curr_hot_elem.id != 0 && _curr_hot_elem.id != _mouseover_elem.id )
        {
            _mouseover_elem = _curr_hot_elem;
            std::cout  <<"hot_elem" << _curr_hot_elem.id <<"\n";
        }
    }

    void Layout::handleButtonDownEvents()
    {
        if ( _input->buttonDown() )
        {
            if ( _input->doubleClick() )
            {
                if ( _prev_active_elem.id != 0 && _prev_active_elem.clicks == 2 )
                {
                    _curr_active_elem.id = _prev_active_elem.id;
                    //std::cout  <<"double-clicking : " << _curr_active_elem.id <<"\n";
                    return;
                }

                else if ( _curr_active_elem.id == 0 && _prev_active_elem.id == 0 )
                {
                    if ( _mouseover_elem.index != 0 && _mouseover_elem.index < _nodes.size() )
                    {
                        _curr_active_elem = _mouseover_elem;
                        _curr_active_elem.clicks = 2;

                        if ( _nodes[ _mouseover_elem.index ].elem.style.click_trigger == ui::ClickTrigger::OnClick )
                        {
                            _clicked = _curr_active_elem;
                            _focus = _curr_active_elem;
                            //std::cout  <<"double-click : " << _curr_active_elem.id <<"\n";
                        }
                        return;
                    }
                }
            }

            else if ( _input->primaryClick() )
            {
                if ( _prev_active_elem.id != 0 && _prev_active_elem.clicks == 1 )
                {
                    _curr_active_elem = _prev_active_elem;
                    //std::cout  <<"single-clicking : " << _prev_active_elem.id <<"\n";
                }

                else if ( _curr_active_elem.id == 0 && _prev_active_elem.id == 0 )
                {
                    if ( _mouseover_elem.index != 0 && _mouseover_elem.index < _nodes.size() )
                    {
                        _curr_active_elem = _mouseover_elem;
                        _curr_active_elem.clicks = 1;

                        if ( _nodes[ _mouseover_elem.index ].elem.style.click_trigger == ui::ClickTrigger::OnClick )
                        {
                            _clicked = _curr_active_elem;
                            _focus = _curr_active_elem;
                            std::cout  <<"single-click : " << _curr_active_elem.id <<"\n";
                        }
                        return;
                    }
                }
            }
        }

        else
        {
            if ( _prev_active_elem.id != 0 && _prev_active_elem.index < _nodes.size() )
            {
                if ( _nodes[ _prev_active_elem.index ].elem.style.click_trigger == ui::ClickTrigger::OnRelease )
                {
                    if ( testPointVsRect( _input->currentPosition(), _nodes[ _prev_active_elem.index ].elem.rect ) )
                    {
                        _clicked = _prev_active_elem;
                        _focus = _prev_active_elem;
                        std::cout  <<"fire on release: " << _prev_active_elem.id <<"\n";
                    }
                }
                _prev_active_elem = {};
            }
        }
    }

    void Layout::begin( ui::Input* input )
    {
        _root = 0;
        _level = 0;
        _parent = 0;
        _input = input;

        /**
         * If the recycled id is not -1, we need to reset the recycled id.
         * This is to ensure that the recycled nodes are available for reuse.
         */
        if ( _recycled_id < 0 )
        {
            _recycled_id = _recycled_node_head;
        }
        /**
         * If the recycled node count is greater than 0, we need to append to the recovered recycled 
         * list of element to the current recycled list. This is to ensure that elements that are 
         * recycled in the current frame is not reused in same frame, because this can lead to 
         * elements being overwritten or addding child node to it self.
         */
        else if ( _recycled_node_count > 0 )
        {
            _id_pool[ _recycled_node_tail ].next = _recycled_id;
            _id_pool[ _recycled_id ].prev = _recycled_node_tail;
            _recycled_id = _recycled_node_head;
        }
        _recycled_node_head = -1;
        _recycled_node_tail = -1;
        _recycled_node_count = 0;
    }

    void Layout::end()
    {
        /**
         * If the nodes are not empty, we align the layout.
         */
        if ( !_nodes.empty() )
        {
            _aligner.align( *this, _root );
        }

        /**
         * If the mouse button is down, we keep the previous active element as the current active element.
         * This is to ensure that the active element remains the same until the button is released.
         */
        if ( _input->buttonDown() )
        {
            if ( _prev_active_elem.id != 0 )
            {
                _curr_active_elem = _prev_active_elem;
            }
            else
            {
                _prev_active_elem = _curr_active_elem;
                _curr_active_elem = {};
            }
        }
        else
        {
            _prev_active_elem = _curr_active_elem;
            _curr_active_elem = {};
        }
        
        _curr_hot_elem = {};
        _clicked = {};

        handleMouseOver();
        handleButtonDownEvents();
    }

    ui::Input* Layout::input()
    {
        return _input;
    }

    void Layout::dupId( uint32_t src_index, uint32_t* dst_index )
    {
        if ( 1 <= src_index )
        {
            *dst_index = src_index;
            _id_pool[ src_index ].duplicates++;
            //std::cout << "duplicated: " << src->index <<"\n";
        }
    }
    void Layout::recycleId( uint32_t index )
    {
        if ( index < 0 || index >= _id_pool.size())
        {
            return; // Invalid ID
        }

        /**
         * Recycle the ID by decrementing its reference count.
         */
        if ( _id_pool[ index ].duplicates > 0 )
        {
            _id_pool[ index ].duplicates--;
            /**
             * If the reference count reaches zero, add the ID to the recycled list.
             */
            if ( _id_pool[ index ].duplicates == 0 )
            {
                _id_pool[ index ].next = -1;
                _id_pool[ index ].prev = -1;
                _id_pool[ index ].duplicates = 0;

                if ( _recycled_node_head < 0 )
                {
                    _recycled_node_head = _recycled_node_tail = index;
                }
                else
                {
                    _id_pool[ index ].prev = _recycled_node_tail;
                    _id_pool[ _recycled_node_tail ].next = index;
                    _recycled_node_tail = index;
                }
                _recycled_node_count += 1;
            }
        }
    }

    uint32_t Layout::genId()
    {
        /**
         * Generate a new ID from the pool.
         * If there are recycled IDs, use the first one.
         * Otherwise, use the next available ID from the pool.
         * If the pool is exhausted, return 0.
         */
        if ( _available_id >= _id_pool.size() || _available_id == -1)
        {
            KEGE_LOG_ERROR << "max available ids exhausted." << Log::nl;
            return 0;
        }

        int index;

        /**
         * If there are recycled IDs, use the first one.
         * Otherwise, use the next available ID from the pool.
         */
        if ( _recycled_id > 0 )
        {
            index = _id_pool[ _recycled_id ].index;
            _recycled_id = _id_pool[ _recycled_id ].next;
        }
        else
        {
            index = _available_id;
            _available_id = _id_pool[ _available_id ].next;
        }

        /**     
         * Initialize the ID structure.
         * Set the next and previous pointers to -1, indicating no links.
         * Set the duplicates count to 1, as this is a new ID.
         */
        _id_pool[ index ].next = -1;
        _id_pool[ index ].prev = -1;
        _id_pool[ index ].duplicates = 1;
        return index;
    }

    Layout::Layout()
    :   _level( 0 )
    ,   _parent( 0 )
    ,   _mouseover_elem{}
    ,   _curr_hot_elem{}
    ,   _prev_active_elem{}
    ,   _curr_active_elem{}
    ,   _clicked{}
    ,   _count( 0 )
    ,   _recycled_node_tail( -1 )
    ,   _recycled_node_head( -1 )
    ,   _recycled_node_count( -1 )
    ,   _root( 0 )
    ,   _recycled_id( -1 )
    ,   _available_id( -1 )
    {
    }

}
