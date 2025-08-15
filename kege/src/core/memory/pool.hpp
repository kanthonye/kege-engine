//
//  pool.hpp
//  kege
//
//  Created by Kenneth Esdaile on 8/31/24.
//

#ifndef kege_pool_hpp
#define kege_pool_hpp

#include <stdlib.h>
#include <vector>

class Pool
{
public:

    virtual bool valid( uint16_t storage_index, uint16_t cell_index )const = 0;
    virtual void* data( uint16_t storage_index, uint16_t cell_index ) = 0;
    virtual bool free( uint16_t storage_index, uint16_t cell_index ) = 0;
    virtual void create( uint16_t& storage_index, uint16_t& cell_index ) = 0;
    virtual void purge() = 0;
    virtual ~Pool(){}
};


template< typename T > class PoolT : public Pool
{
public:

    class Storage
    {
    public:

        uint16_t alloc()
        {
            uint16_t index = 0;
            if ( head )
            {
                index = uint16_t(head - (&cells[ 0 ]));
                head = head->next;
                if ( !head )
                {
                    tail = nullptr;
                }
            }
            else if( size < cells.size() )
            {
                index = size;
                size++;
            }
            count++;
            cells[ index ].freed = false;
            return index + 1;
        }

        bool free( uint16_t cell_index )
        {
            uint16_t index = cell_index - 1;
            // make sure that the index is with in range of the array
            if (index <= cells.size())
            {
                /* 
                 make sure that the cell at the given index isn't already freed, if freed
                 previously its already in the list, and should not be insert more than once.
                 */
                if ( !cells[ index ].freed )
                {
                    if ( !head )
                    {
                        tail = head = &cells[ cell_index - 1 ];
                    }
                    else
                    {
                        tail->next = &cells[ cell_index - 1 ];
                        tail = &cells[ cell_index - 1 ];
                    }
                    // mark as freed to indicate this cell is already in the list
                    cells[ index ].freed = true;
                    count--;
                    return true;
                }
            }
            return false;
        }

        T& data(  uint16_t cell_index )
        {
            return cells[ cell_index - 1 ].data;
        }

        bool full()const
        {
            return size >= cells.size() && head == nullptr;
        }

        Storage( uint32_t size )
        :   head( nullptr )
        ,   tail( nullptr )
        ,   next( nullptr )
        ,   count( 0 )
        ,   size( 0 )
        ,   id( 0 )
        ,   listed( false )
        {
            cells.resize( size );
        }

        ~Storage()
        {
            cells.clear();
        }

        Storage()
        :   head( nullptr )
        ,   tail( nullptr )
        ,   next( nullptr )
        ,   count( 0 )
        ,   size( 0 )
        ,   id( 0 )
        ,   listed( false )
        {}

        struct Cell
        {
            T data;
            Cell* next;
            bool freed;
        };

        std::vector< Cell > cells;

        Storage::Cell* head;
        Storage::Cell* tail;
        Storage* next;

        uint32_t count;
        uint32_t size;
        uint32_t id;

        bool listed;
    };

private:

    uint32_t getsize()
    {
        uint32_t size = pow( _base, _exponent );
        size = ( size <= 0 ) ? _base : size;

        _exponent = ( _exponent < _max_exponent ) ? _exponent : _max_exponent;
        _exponent++;
        return size;
    }

public:

    void decode( uint32_t component_id, uint16_t& storage, uint16_t& cell )const
    {
        storage = (uint16_t)(component_id >> 16);
        cell = (uint16_t)(component_id & 0xFFFF);
    }

    void encode( uint16_t storage, uint16_t cell, uint32_t& component_id )const
    {
        component_id = ((uint32_t) storage << 16) | (uint32_t) cell;
    }

    bool free( uint16_t storage_index, uint16_t cell_index )
    {
        // return the cell back to storage which it came from.
        Storage* storage = _storages[ storage_index ];
        if( storage->free( cell_index ) )
        {
            // insert the storage in the list of storage with available cells
            if ( !storage->listed )
            {
                if ( !_head )
                {
                    _head = _tail = storage;
                }
                else
                {
                    _tail->next = storage;
                    _tail = storage;
                }
                storage->listed = true;
            }
            return true;
        }
        return false;
    }

    const T* dataT( uint16_t storage_index, uint16_t cell_index )const
    {
        return &_storages[ storage_index ]->cells[ cell_index - 1 ].data;
    }

    T* dataT( uint16_t storage_index, uint16_t cell_index )
    {
        return &_storages[ storage_index ]->cells[ cell_index - 1 ].data;
    }

    const void* data( uint16_t storage_index, uint16_t cell_index )const
    {
        return dataT( storage_index, cell_index );
    }

    void* data( uint16_t storage_index, uint16_t cell_index )
    {
        return dataT( storage_index, cell_index );
    }

    void create( uint16_t& storage_id, uint16_t& cell_index )
    {
        if( !_head )
        {
            _head = _tail = newStorage();
            _head->listed = true;
        }
        if ( _head->full() )
        {
            _head->listed = false;
            _head = _head->next;
            if ( !_head )
            {
                _head = _tail = newStorage();
                _head->listed = true;
            }
        }
        cell_index = _head->alloc();
        storage_id = _head->id;
    }

    bool valid( uint16_t storage_index, uint16_t cell_index )const
    {
        return ( storage_index < _storages.size() )
        ? (cell_index - 1) < _storages[ storage_index ]->cells.size()
        : false;
    }

    void setGrowSize(uint16_t exponent, uint16_t max_exponent, uint16_t base )
    {
        _max_exponent = max_exponent;
        _exponent = exponent;
        _base = base;
    }

    Storage* newStorage()
    {
        Storage* s = new Storage( getsize() );
        s->id = static_cast< uint32_t >( _storages.size() );
        _storages.push_back( s );
        return s;
    }

    void purge()
    {
        for ( int i=0; i < _storages.size(); i++ )
        {
            _storages[ i ]->cells.clear();
            delete _storages[ i ];
            _storages[ i ] = nullptr;
        }
        _storages.clear();
    }

    uint32_t poolSize( uint16_t storage_index )const
    {
        return (int) _storages[ storage_index ]->cells.size();
    }

    size_t size()const
    {
        return _storages.size();
    }

    PoolT(uint16_t exponent, uint16_t max_exponent, uint16_t base )
    {
        setGrowSize(exponent, max_exponent, base);
    }

    ~PoolT()
    {
        purge();
    }

    PoolT()
    :   _head( nullptr )
    ,   _tail( nullptr )
    ,   _max_exponent( 8 )
    ,   _exponent( 1 )
    ,   _base( 4 )
    {}

private:

    std::vector< Storage* > _storages;
    Storage* _head;
    Storage* _tail;

    uint32_t _max_exponent;
    uint32_t _exponent;
    uint32_t _base;
};



#endif /* kege_pool_hpp */
