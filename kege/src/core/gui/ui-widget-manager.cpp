//
//  ui-widget-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/27/25.
//

#include "ui-widget-manager.hpp"

namespace kege::ui{

    void WidgetManager::duplicate( int32_t src_index, int32_t* dst_index )
    {
        if ( 1 <= src_index )
        {
            *dst_index = src_index;
            _contents[ src_index ].duplicates++;
            //std::cout << "duplicated: " << src->index <<"\n";
        }
    }

    int32_t WidgetManager::make( const Widget& widget )
    {
        uint32_t id = generate();
        if( id < 0 || id >= _contents.size() )
        {
            KEGE_LOG_ERROR << "Reached maximum amount of UI Widget -> " << _contents.size() + 1 << Log::nl;
            return {};
        }
        _contents[ id ].widget = widget;
        return id;
    }

    void WidgetManager::recycle( int32_t index )
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
                _contents[ index ].node_index = 0;

                if ( _recycled.head < 0 )
                {
                    _recycled.head = _recycled.tail = index;
                }
                else
                {
                    _contents[ _recycled.tail ].next = index;
                    _contents[ index ].prev = _recycled.tail;
                    _recycled.tail = index;
                }

                _recycled.count += 1;
            }
        }
    }
    void WidgetManager::resize( int32_t max_quantity )
    {
        _contents.resize( max_quantity );

        _available.head = 0;
        _available.tail = max_quantity - 1;
        _available.count = max_quantity;

        // Initialize the free list: each node points to the next.
        for (int32_t i = 0; i < max_quantity; ++i)
        {
            _contents[i].prev  = i - 1;
            _contents[i].next  = i + 1;
            _contents[i].node_index = 0;
            _contents[i].duplicates  = 0;
        }

        _contents[ _available.tail ].next = -1;
    }

    int32_t WidgetManager::generate()
    {
        /**
         * Generate a new ID from the pool.
         * If there are recycled IDs, use the first one.
         * Otherwise, use the next available ID from the pool.
         * If the pool is exhausted, return 0.
         */
        if ( _available.head >= _contents.size() || _available.head == -1)
        {
            KEGE_LOG_ERROR << "max available ids exhausted." << Log::nl;
            return 0;
        }

        int index = _available.head;
        _available.head = _contents[ _available.head ].next;

        if ( _available.head < 0 )
        {
            _available.tail = -1;
        }
        else
        {
            _contents[ _available.head ].prev = -1;
        }
        _available.count -= 1;

        /**
         * Initialize the ID structure.
         * Set the next and previous pointers to -1, indicating no links.
         * Set the duplicates count to 1, as this is a new ID.
         */
        _contents[ index ].next = -1;
        _contents[ index ].prev = -1;
        _contents[ index ].node_index = 0;
        _contents[ index ].duplicates = 1;

        return index;
    }

    const Widget& WidgetManager::operator[]( int32_t index )const
    {
        return _contents[ index ].widget;
    }

    Widget& WidgetManager::operator[]( int32_t index )
    {
        return _contents[ index ].widget;
    }

    void WidgetManager::setNodeIndex( int32_t index, int32_t nodex_index )
    {
        _contents[ index ].node_index = nodex_index;
    }

    uint32_t WidgetManager::getNodeIndex( int32_t index )const
    {
        return _contents[ index ].node_index;
    }

    void WidgetManager::refresh()
    {
        /**
         * If the recycled node count is greater than 0, we need to append to the recovered recycled
         * list of element to the current recycled list. This is to ensure that elements that are
         * recycled in the current frame is not reused in same frame, because this can lead to
         * elements being overwritten or addding child node to it self.
         */
        if ( _recycled.count > 0 )
        {
            _contents[ _recycled.tail ].next = _available.head;
            _contents[ _available.head ].prev = _recycled.tail;
            _available.head = _recycled.head;

            _available.count += _recycled.count;
        }

        _recycled.head = -1;
        _recycled.tail = -1;
        _recycled.count = 0;
    }


    WidgetManager::WidgetManager()
    {
        _recycled.head = -1;
        _recycled.tail = -1;
        _recycled.count = 0;
    }



}
