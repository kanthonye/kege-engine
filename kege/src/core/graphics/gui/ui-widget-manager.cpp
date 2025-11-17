//
//  ui-widget-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/27/25.
//

#include "ui-widget-manager.hpp"

namespace kege::ui{

    void WidgetManager::duplicate( const Handle& handle )
    {
        if ( handle.value != 0 )
        {
            _contents[ handle.index ].duplicates++;
            //std::cout << "duplicated: " << src->index <<"\n";
        }
    }

    Handle WidgetManager::make( const Widget& widget )
    {
        /**
         * Generate a new ID from the pool.
         * If there are recycled IDs, use the first one.
         * Otherwise, use the next available ID from the pool.
         * If the pool is exhausted, return 0.
         */
        if ( _available.head >= _contents.size() || _available.head == -1)
        {
            kege::Log::error << "max available ids exhausted." << Log::nl;
            return {};
        }

        uint32_t index = _available.head;
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
        _contents[ index ].version = (_contents[ index ].version == 0) ? 1 : _contents[ index ].version + 1;

        _widgets[ index ] = widget;

        return {index, _contents[ index ].version, 0};
    }

    void WidgetManager::recycle( const Handle& handle )
    {
        if ( handle.index < 0 || handle.index >= _contents.size())
        {
            return; // Invalid ID
        }

        /**
         * Recycle the ID by decrementing its reference count.
         */
        if ( _contents[ handle.index ].duplicates > 0 )
        {
            _contents[ handle.index ].duplicates--;

            /**
             * If the reference count reaches zero, add the ID to the recycled list.
             */
            if ( _contents[ handle.index ].duplicates == 0 )
            {
                _contents[ handle.index ].node_index = 0;

                if ( _recycled.head < 0 )
                {
                    _recycled.head = _recycled.tail = handle.index;
                }
                else
                {
                    _contents[ _recycled.tail ].next = handle.index;
                    _contents[ handle.index ].prev = _recycled.tail;
                    _recycled.tail = handle.index;
                }

                _recycled.count += 1;
            }
        }
    }
    
    void WidgetManager::resize( int32_t max_quantity )
    {
        _widgets.resize( max_quantity );
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

    const Widget& WidgetManager::operator[]( const Handle& handle )const
    {
        return _widgets[ handle.index ];
    }

    Widget& WidgetManager::operator[]( const Handle& handle )
    {
        return _widgets[ handle.index ];
    }

    void WidgetManager::setNodeIndex( const Handle& handle, int32_t nodex_index )
    {
        _contents[ handle.index ].node_index = nodex_index;
    }

    uint32_t WidgetManager::getNodeIndex( const Handle& handle )const
    {
        return _contents[ handle.index ].node_index;
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
