//
//  resource-recycler.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/26/25.
//

#ifndef resource_recycler_hpp
#define resource_recycler_hpp

#include <mutex>
#include <vector>

template< typename T > class  ResourceRecycler
{
public:

    struct Asset
    {
        T    data;
        int  next;
        bool freed;
    };

    typedef std::vector< Asset >::const_iterator const_iterator;
    typedef std::vector< Asset >::iterator iterator;

    inline const T* get( uint32_t index )const
    {
        return (index < _assets.size() )
        ? (( !_assets[ index ].freed ) ? &_assets[ index ].data : nullptr)
        : nullptr;
    }

    T* get( uint32_t index )
    {
        return (index < _assets.size() )
        ? (( !_assets[ index ].freed ) ? &_assets[ index ].data : nullptr)
        : nullptr;
    }

    void free( uint32_t index )
    {
        if ( index < _assets.size() )
        {
            _assets[ index ].freed = true;
            if( _head < 0 )
            {
                _head = _tail = index;
            }
            else
            {
                _assets[ index ].next = _head;
                _head = index;
            }
        }
    }
    
    uint32_t insert( T& data )
    {
        uint32_t id = gen();
        _assets[ id ].data = data;
        _assets[ id ].next = -1;
        return id;
    }

    int32_t gen()
    {
        uint32_t id;
        if ( _head >= 0 )
        {
            id = _head;
            _assets[ _head ].freed = false;
            _head = _assets[ _head ].next;
            if ( _head < 0 )
            {
                _head = _tail = -1;
            }
        }
        else
        {
            id = (uint32_t) _assets.size();
            _assets.push_back({});
        }
        return id;
    }

    const_iterator begin()const
    {
        return _assets.cbegin();
    }

    const_iterator end()const
    {
        return _assets.cend();
    }

    iterator begin()
    {
        return _assets.begin();
    }

    iterator end()
    {
        return _assets.end();
    }

    uint32_t count()const
    {
        return (uint32_t) _assets.size();
    }
    
    void clear()
    {
        _assets.clear();
        _head = -1;
        _tail = -1;
    }

    ResourceRecycler()
    :   _head(-1)
    ,   _tail(-1)
    ,   _assets()
    {}

protected:

    /** @brief Mutex for thread-safe resource management */
    std::mutex _resource_mutex;

    std::vector< Asset > _assets;
    int _head;
    int _tail;
};


#endif /* resource_recycler_hpp */
