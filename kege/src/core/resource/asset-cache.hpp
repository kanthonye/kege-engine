//
//  asset-cache.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/25/25.
//

#ifndef asset_cache_hpp
#define asset_cache_hpp

#include "../memory/ref.hpp"
#include "../utils/log.hpp"

namespace kege{

    /**
     * @brief A list that allows recycling of freed entries to minimize memory allocations.
     *
     * This class maintains a doubly linked list of items, allowing for efficient
     * insertion and removal of entries. Freed entries are recycled for future use,
     * reducing the overhead of dynamic memory allocation.
     *
     * @tparam Item The type of entries stored in the list.
     */
    template< typename Item > class AssetCache
    {
    private:

        struct Entry
        {
            Item     item;
            int32_t  next    = -1;
            int32_t  prev    = -1;
            uint32_t version = 0;
            bool     active  = true;
        };

        struct List
        {
            int32_t head;
            int32_t tail;
            int32_t count;
        };

    public:

        const Item* operator []( uint64_t handle )const;
        Item* operator []( uint64_t handle );
        uint64_t next( uint64_t handle )const;
        uint64_t prev( uint64_t handle )const;

        Item* get( uint64_t handle );
        uint64_t push( Item entry );
        void pop( uint64_t handle );

        uint64_t begin();
        uint64_t end();

        size_t size()const;
        void clear();

        ~AssetCache();
        AssetCache();

    private:

        void decode( uint64_t handle, uint32_t& ver, uint32_t& idx)const
        {
            idx = static_cast< uint32_t >( handle );
            ver = static_cast< uint32_t >( handle >> 32 );
        }
        uint64_t encode( uint32_t idx)const
        {
            return (idx >= _entries.size()) ? 0 : (uint64_t( _entries[idx].version ) << 32) | idx;
        }

        bool invalid(uint32_t ver, uint32_t idx)const
        {
            return (idx >= _entries.size() || _entries[idx].version != ver || !_entries[idx].active );
        }

    private:

        std::vector< Entry > _entries;
        List _active;
        List _freed;
    };

}


namespace kege{

    template< typename Item > const Item* AssetCache< Item >::operator []( uint64_t handle )const
    {
        uint32_t idx,ver;
        decode( handle, ver, idx );
        if (idx >= _entries.size() || _entries[idx].version != ver ) return nullptr;
        return &_entries[ idx ].item;
    }
    template< typename Item > Item* AssetCache< Item >::operator []( uint64_t handle )
    {
        uint32_t idx,ver;
        decode( handle, ver, idx );
        if ( invalid( ver, idx ) ) return nullptr;
        return &_entries[ idx ].item;
    }
    template< typename Item > uint64_t AssetCache< Item >::next( uint64_t handle )const
    {
        uint32_t idx,ver;
        decode( handle, ver, idx );
        if ( invalid( ver, idx ) ) return 0;
        return encode( _entries[ idx ].next );
    }
    template< typename Item > uint64_t AssetCache< Item >::prev( uint64_t handle )const
    {
        uint32_t idx,ver;
        decode( handle, ver, idx );
        if ( invalid( ver, idx ) ) return 0;
        return encode( _entries[ idx ].prev );
    }

    template< typename Item > Item* AssetCache< Item >::get( uint64_t handle )
    {
        uint32_t idx,ver;
        decode( handle, ver, idx );
        if ( invalid( ver, idx ) ) return nullptr;
        return &_entries[ idx ].item;
    }
    template< typename Item > uint64_t AssetCache< Item >::push( Item item )
    {
        int32_t index;
        if ( 0 <= _freed.head )
        {
            // remove from freed list
            index = _freed.head;
            _freed.head = _entries[ index ].next;
            if ( _freed.head < 0 ) _freed.tail = -1;

            _freed.count -= 1;

            _entries[ index ].item = item;
        }
        else
        {
            // create new entry
            index = static_cast< int32_t >( _entries.size() );
            _entries.push_back({});

            _entries[ index ].item = item;
        }

        _entries[ index ].next = -1;
        _entries[ index ].prev = -1;

        // add to active list
        if ( _active.head < 0 )
        {
            _active.head = _active.tail = index;
        }
        else
        {
            _entries[ _active.tail ].next = index;
            _entries[ index ].prev = _active.tail;
            _active.tail = index;
        }

        _entries[ index ].active = true;
        _entries[ index ].version = (_entries[ index ].version == 0) ? 1 : _entries[ index ].version + 1;
        return encode( index );
    }

    template< typename Item > void AssetCache< Item >::pop( uint64_t handle )
    {
        uint32_t idx,ver;
        decode( handle, ver, idx );

        if (idx >= _entries.size()) return;
        if (_entries[idx].version != ver) return;
        if ( !_entries[idx].active ) return;

        // remove from active list
        if ( _active.head == idx )
        {
            _active.head = _entries[ _active.head ].next;
            if ( _active.head < 0 ) _active.tail = -1;
            else _entries[ _active.head ].prev = -1;
        }
        else if ( _active.tail == idx )
        {
            _active.tail = _entries[ _active.tail ].prev;
            if ( _active.tail < 0 ) _active.head = -1;
            else _entries[ _active.tail ].next = -1;
        }
        else
        {
            int n = _entries[ idx ].next;
            int p = _entries[ idx ].prev;
            _entries[ n ].prev = p;
            _entries[ p ].next = n;
        }

        _entries[ idx ].prev = -1;
        _entries[ idx ].next = -1;
        _entries[ idx ].active = false;

        // add to freed list
        if ( _freed.head < 0 )
        {
            _freed.head = _freed.tail = idx;
        }
        else
        {
            _entries[ _freed.tail ].next = idx;
            _entries[ idx ].prev = _freed.tail;
            _freed.tail = idx;
        }
    }

    template< typename Item > uint64_t AssetCache< Item >::begin()
    {
        return encode( _active.head );
    }

    template< typename Item > uint64_t AssetCache< Item >::end()
    {
        return encode( _active.tail );
    }

    template< typename Item > void AssetCache< Item >::clear()
    {
        _entries.clear();
    }

    template< typename Item > size_t AssetCache< Item >::size()const
    {
        return _entries.size();
    }

    template< typename Item > AssetCache< Item >::~AssetCache()
    {
        clear();
    }

    template< typename Item > AssetCache< Item >::AssetCache()
    :   _active({ -1, -1, 0 })
    ,   _freed({ -1, -1, 0 })
    {}
}




#endif /* asset_cache_hpp */
