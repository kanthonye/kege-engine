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
        return _prev_hot_elem.id == _nodes[ _id_pool[ id.index ].node ].id;
    }

    bool Layout::doubleClick( const ui::EID& id )const
    {
        return _clicked.id == _nodes[ _id_pool[ id.index ].node ].id && _clicked.clicks == 2;
    }

    bool Layout::click( const ui::EID& id )const
    {
        return _clicked.id == _nodes[ _id_pool[ id.index ].node ].id && _clicked.clicks == 1;
    }

    bool Layout::hasFocus( const ui::EID& id )const
    {
        return _focus_id == _nodes[ _id_pool[ id.index ].node ].id;
    }

    void Layout::setFocus( const ui::EID& id )
    {
        _focus_id = _nodes[ _id_pool[ id.index ].node ].id;
    }

    EID Layout::make( const Content& content )
    {
        uint32_t id = genId();
        if( id <= 0 || id >= _nodes.size() )
        //if ( _node_counter + 1 >= _nodes.size() )
        {
            KEGE_LOG_ERROR << "Reached Maximum amount of UI Elems -> " << _node_counter + 1 << Log::nl;
            return {};
        }

        _contents[ id ] = content;
        return {id, this};
    }

    uint32_t Layout::put( const EID& id )
    {
        _node_counter += 1;
        if ( _node_counter >= _nodes.size() )
        {
            KEGE_LOG_ERROR << "Reached Maximum amount of UI Node";
            return 0;
        }

        if ( id.index == 0 )
        {
            KEGE_LOG_ERROR << "Invalid UI index"<<Log::nl;
            return 0;
        }

        if ( 0 != _parent && _id_pool[ id.index ].node == _parent )
        {
            KEGE_LOG_ERROR << "Death cycle detected. Attaching child node to its self will cause the program to looping infinetly and become stuck"<<Log::nl;
            return 0;
        }

        _id_pool[ id.index ].node = _node_counter;
        
        _nodes[ _node_counter ].content = &_contents[ id.index ];
        _nodes[ _node_counter ].parent = _parent;
        _nodes[ _node_counter ].count = 0;
        _nodes[ _node_counter ].head = 0;
        _nodes[ _node_counter ].tail = 0;
        _nodes[ _node_counter ].next = 0;
        _nodes[ _node_counter ].next_free = 0;
        _nodes[ _node_counter ].freed = false;
        _nodes[ _node_counter ].index = _node_counter;
        _nodes[ _node_counter ].id = _node_counter;

        // if style width and height is fixed, set the rect width and height of the ui element
        if ( _contents[ id.index ].style->height.type == kege::ui::SIZE_FIXED )
        {
            _nodes[ _node_counter ].content->rect.height = _contents[ id.index ].style->height.size;
        }
        if ( _contents[ id.index ].style->width.type == kege::ui::SIZE_FIXED )
        {
            _nodes[ _node_counter ].content->rect.width = _contents[ id.index ].style->width.size;
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

            _nodes[ _node_counter ].depth = 1 + _nodes[ _parent ].depth;
        }

        return _node_counter;
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

    const Content* Layout::operator[]( NodeIndex node_id )const
    {
        return _nodes[ node_id ].content;
    }

    Content* Layout::operator[]( NodeIndex index )
    {
        return _nodes[ index ].content;
    }

//    const kege::ui::Node& Layout::nodes( uint32_t index ) const
//    {
//        return _nodes[ index ];
//    }
//
//    kege::ui::Node& Layout::nodes( uint32_t index )
//    {
//        return _nodes[ index ];
//    }

    uint32_t Layout::addStyle( const AddStyle& as )
    {
        auto itr = _style_index_map.find( as.name_id );
        if ( itr == _style_index_map.end() )
        {
            _style_index_map[ as.name_id ] = _style_indexer;
            _styles[ _style_indexer ] = as.style;
            uint32_t index = _style_indexer++;
            return  index;
        }
        return itr->second;
    }

    ui::Style* Layout::getStyleByName( const std::string& name_id )
    {
        auto itr = _style_index_map.find( name_id );
        if ( itr == _style_index_map.end() )
        {
            return nullptr;
        }
        return &_styles[ itr->second ];
    }

    ui::Style* Layout::getStyleByID( int index )
    {
        return &_styles[ index ];
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
        _contents.resize( max_nodes );
        _styles.resize( max_nodes );
        _nodes.resize( max_nodes );

        _id_pool.resize( max_nodes );

        // Initialize the free list: each node points to the next.
        for (int32_t i = 0; i < max_nodes - 1; ++i)
        {
            _id_pool[i].node = 0;
            _id_pool[i].prev  = i - 1;
            _id_pool[i].next  = i + 1;
            _id_pool[i].duplicates  = 0;
        }
        _id_pool[ max_nodes - 1 ].next = -1;
        _available_id = 1;
    }

    uint32_t Layout::count()const
    {
        return _node_counter;
    }

    bool Layout::validate( uint32_t node_id )const
    {
        return node_id > 0 && node_id < _nodes.size();
    }

//    bool Layout::parent( uint32_t node_id )const
//    {
//        return validate( _nodes[ node_id ].head );
//    }

    void Layout::handleMouseOverEvents()
    {
        // Reset current hot element at start of frame
        _curr_hot_elem = {0, 0, 0};

        // First, check if previous hot element is still valid and under mouse
        if (_prev_hot_elem.index != 0)
        {
            if ( testPointVsRect( _input->currentPosition(), _nodes[ _prev_hot_elem.index ].content->rect ) )
            {
                // If previous hot element has children, there is a possibility that the mouse
                // is over its child element. So, we need to account for those child elements.
                if ( _nodes[ _prev_hot_elem.index ].count )
                {
                    findNewHotElement( _prev_hot_elem.index );
                }

                // If the previous hot element has no children then it is still under mouse - keep it hot
                if (_curr_hot_elem.id == 0)
                {
                    _curr_hot_elem = _prev_hot_elem;
                }

            }
        }

        // If we didn't find a persistent hot element, search for a new one
        if (_curr_hot_elem.id == 0)
        {
            findNewHotElement(1);
        }

        // Handle mouse enter/leave events
        if (_curr_hot_elem.id != _prev_hot_elem.id)
        {
            if (_prev_hot_elem.id != 0)
            {
                std::cout  <<"mouse exit: " << _prev_hot_elem.index <<"\n";
            }

            if (_curr_hot_elem.id != 0)
            {
                std::cout  <<"mouse enter: " << _curr_hot_elem.index <<"\n";
            }
        }

        // Handle focus logic (separate from mouseover)
        //handleFocusLogic(root);

        // Store current hot for next frame
        _prev_hot_elem = _curr_hot_elem;
    }


    void Layout::findNewHotElement( uint32_t root )
    {
        if ( _curr_hot_elem.id != 0) {
            return;
        }

        for (uint32_t ui_index = _nodes[ root ].head; ui_index != 0 ; ui_index = _nodes[ ui_index ].next )
        {
            findNewHotElement( ui_index );
        }

        if ( _nodes[ root ].content->mouseover && _nodes[ root ].content->visible )
        {
            if ( testPointVsRect( _input->currentPosition(), _nodes[ root ].content->rect ) )
            {
                if ( _curr_hot_elem.depth < _nodes[ root ].depth  )
                {
                    if ( _curr_hot_elem.index == 0 )
                    {
                        _curr_hot_elem.id    = _nodes[ root ].id;
                        _curr_hot_elem.index = _nodes[ root ].index;
                        _curr_hot_elem.depth = _nodes[ root ].depth;
                    }
                    else if ( _nodes[ _curr_hot_elem.index ].content->style->zindex <= _nodes[ root ].content->style->zindex  )
                    {
                        _curr_hot_elem.id    = _nodes[ root ].id;
                        _curr_hot_elem.index = _nodes[ root ].index;
                        _curr_hot_elem.depth = _nodes[ root ].depth;
                    }
//                    _curr_hot_elem.id    = _nodes[ root ].id;
//                    _curr_hot_elem.index = _nodes[ root ].index;
//                    _curr_hot_elem.depth = _nodes[ root ].depth;
                }
            }
        }
    }

    void Layout::handleButtonUpEvents()
    {
        if ( _input->buttonDown() ) return;

        if ( _prev_active_elem.id != 0 && _prev_active_elem.index < _nodes.size() )
        {
            if ( _nodes[ _prev_active_elem.index ].content->trigger == ui::ClickTrigger::OnRelease )
            {
                std::cout  <<"release: " << _prev_active_elem.id <<"| " << _prev_hot_elem.id <<"\n";
                if ( _prev_hot_elem.id == _prev_active_elem.id )
                {
                    std::cout  <<"FIRE ON RELEASE\n";
                    _clicked = _prev_active_elem;
                    _focus = _prev_active_elem;
                }
            }
            _prev_active_elem = {};
            //std::cout  <<"release : " << _prev_active_elem.id <<"\n";
        }
    }


    void Layout::handleButtonDownEvents()
    {
        _curr_active_elem = {};
        if ( _input->buttonDown() )
        {
            if ( _prev_active_elem.id != 0 )
            {
                _curr_active_elem = _prev_active_elem;
                if ( _nodes[ _curr_active_elem.index ].content->trigger == ui::ClickTrigger::OnClick )
                {
                    _clicked = _curr_active_elem;
                }
                //std::cout  <<"clicking : " << _curr_active_elem.id <<"\n";
            }

            if ( _curr_active_elem.id == 0 && _curr_hot_elem.index != 0  )
            {
                if ( _input->doubleClick() )
                {
                    _curr_active_elem = _curr_hot_elem;
                    _curr_active_elem.clicks = 2;
                    std::cout  <<"double-click : " << _curr_active_elem.id <<"\n";

                    if ( _nodes[ _curr_active_elem.index ].content->trigger == ui::ClickTrigger::OnClick )
                    {
                        _clicked = _curr_active_elem;
                        _focus = _curr_active_elem;
                    }
                }
                else if ( _input->primaryClick() && _curr_hot_elem.index != 0 )
                {
                    _curr_active_elem = _curr_hot_elem;
                    _curr_active_elem.clicks = 1;
                    std::cout  <<"single-click : " << _curr_active_elem.id <<"\n";

                    if ( _nodes[ _curr_active_elem.index ].content->trigger == ui::ClickTrigger::OnClick )
                    {
                        _clicked = _curr_active_elem;
                        _focus = _curr_active_elem;
                    }
                }
                _prev_active_elem = _curr_active_elem;
            }
        }
    }

    void Layout::begin( ui::Input* input )
    {
        _node_counter = 0;

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
//        if ( _input->buttonDown() )
//        {
//            if ( _prev_active_elem.id != 0 )
//            {
//                _curr_active_elem = _prev_active_elem;
//            }
//            else
//            {
//                _prev_active_elem = _curr_active_elem;
//                _curr_active_elem = {};
//            }
//        }
//        else
//        {
//            _prev_active_elem = _curr_active_elem;
//            _curr_active_elem = {};
//        }

        _clicked = {};

        handleMouseOverEvents();
        handleButtonDownEvents();
        handleButtonUpEvents();
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
            index = _recycled_id;
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
    ,   _curr_hot_elem{}
    ,   _prev_active_elem{}
    ,   _curr_active_elem{}
    ,   _clicked{}
    ,   _node_counter( 0 )
    ,   _recycled_node_tail( -1 )
    ,   _recycled_node_head( -1 )
    ,   _recycled_node_count( -1 )
    ,   _root( 0 )
    ,   _recycled_id( -1 )
    ,   _available_id( -1 )
    ,   _style_indexer( 0 )
    {
    }

}
