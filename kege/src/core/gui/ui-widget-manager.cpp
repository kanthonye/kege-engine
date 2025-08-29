//
//  ui-widget-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/27/25.
//

#include "ui-widget-manager.hpp"

namespace kege::ui{

    void WidgetManager::duplicate( uint32_t src_index, uint32_t* dst_index )
    {
        if ( 1 <= src_index )
        {
            *dst_index = src_index;
            _contents[ src_index ].duplicates++;
            //std::cout << "duplicated: " << src->index <<"\n";
        }
    }

    uint32_t WidgetManager::make( const Widget& widget )
    {
        uint32_t id = generate();
        if( id <= 0 || id >= _contents.size() )
        {
            KEGE_LOG_ERROR << "Reached maximum amount of UI Widget -> " << _contents.size() + 1 << Log::nl;
            return {};
        }
        _contents[ id ].widget = widget;
        return id;
    }

    void WidgetManager::recycle( uint32_t index )
    {
        if ( index < 0 || index >= _contents.size())
        {
            return; // Invalid ID
        }

        /**
         * Recycle the ID by decrementing its reference count.
         */
        if ( _contents[ index ].duplicates > 0 )
        {
            _contents[ index ].duplicates--;
            /**
             * If the reference count reaches zero, add the ID to the recycled list.
             */
            if ( _contents[ index ].duplicates == 0 )
            {
                _contents[ index ].next = -1;
                _contents[ index ].prev = -1;
                _contents[ index ].duplicates = 0;

                if ( _recycled_head < 0 )
                {
                    _recycled_head = _recycled_tail = index;
                }
                else
                {
                    _contents[ index ].prev = _recycled_tail;
                    _contents[ _recycled_tail ].next = index;
                    _recycled_tail = index;
                }
                _recycled_count += 1;
            }
        }
    }
    void WidgetManager::resize(uint32_t max_quantity)
    {
        _contents.resize( max_quantity );
        // Initialize the free list: each node points to the next.
        for (int32_t i = 0; i < max_quantity; ++i)
        {
            _contents[i].node_index = 0;
            _contents[i].prev  = i - 1;
            _contents[i].next  = i + 1;
            _contents[i].duplicates  = 0;
        }
        _contents[ max_quantity - 1 ].next = -1;
        _available_id = 1;
    }

    uint32_t WidgetManager::generate()
    {
        /**
         * Generate a new ID from the pool.
         * If there are recycled IDs, use the first one.
         * Otherwise, use the next available ID from the pool.
         * If the pool is exhausted, return 0.
         */
        if ( _available_id >= _contents.size() || _available_id == -1)
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
            _recycled_id = _contents[ _recycled_id ].next;
        }
        else
        {
            index = _available_id;
            _available_id = _contents[ _available_id ].next;
        }

        /**
         * Initialize the ID structure.
         * Set the next and previous pointers to -1, indicating no links.
         * Set the duplicates count to 1, as this is a new ID.
         */
        _contents[ index ].next = -1;
        _contents[ index ].prev = -1;
        _contents[ index ].duplicates = 1;
        return index;
    }

    const Widget& WidgetManager::operator[]( uint32_t index )const
    {
        return _contents[ index ].widget;
    }

    Widget& WidgetManager::operator[]( uint32_t index )
    {
        return _contents[ index ].widget;
    }

    void WidgetManager::setNodeIndex( uint32_t index, uint32_t nodex_index )
    {
        _contents[ index ].node_index = nodex_index;
    }

    uint32_t WidgetManager::getNodeIndex( uint32_t index )const
    {
        return _contents[ index ].node_index;
    }

    void WidgetManager::refresh()
    {
        /**
         * If the recycled id is not -1, we need to reset the recycled id.
         * This is to ensure that the recycled nodes are available for reuse.
         */
        if ( _recycled_id < 0 )
        {
            _recycled_id = _recycled_head;
        }
        /**
         * If the recycled node count is greater than 0, we need to append to the recovered recycled
         * list of element to the current recycled list. This is to ensure that elements that are
         * recycled in the current frame is not reused in same frame, because this can lead to
         * elements being overwritten or addding child node to it self.
         */
        else if ( _recycled_count > 0 )
        {
            _contents[ _recycled_tail ].next = _recycled_id;
            _contents[ _recycled_id ].prev = _recycled_tail;
            _recycled_id = _recycled_head;
        }
        _recycled_head = -1;
        _recycled_tail = -1;
        _recycled_count = 0;
    }


    WidgetManager::WidgetManager()
    :   _recycled_tail( -1 )
    ,   _recycled_head( -1 )
    ,   _recycled_count( 0 )
    ,   _recycled_id( -1 )
    ,   _available_id( -1 )
    {
    }



}
