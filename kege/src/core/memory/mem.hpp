//
//  mem.hpp
//  kege
//
//  Created by Kenneth Esdaile on 11/8/24.
//

#ifndef mem_hpp
#define mem_hpp

#include <stdio.h>

namespace kege
{

    class Memory
    {
    public:

        virtual void purge() = 0;
        virtual ~Memory(){}
    };

}

namespace kege
{

    template< typename T > class MemT : public Memory
    {
    public:

        class Elem
        {
            int32_t  next;
            uint16_t storage;
            bool     freed;
            friend   MemT;
        };

        class Storage
        {
        public:

            T* alloc()
            {
                uint16_t index = 0;
                if ( head < 0 )
                {
                    index = uint16_t(head - (&cells[ 0 ]));
                    head = cells[ head ]->next;
                    if ( head < 0 )
                    {
                        tail = -1;
                    }
                }
                else if( size < cells.size() )
                {
                    index = size;
                    size++;
                }
                count++;
                cells[ index ].storage = id;
                cells[ index ].freed = false;
                cells[ index ].next = -1;
                return &cells[ index ];
            }

            bool free( Elem* mem )
            {
                uint16_t index = (mem - &cells[ 0 ]);
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
                            cells[ index ].next = -1;
                            tail = head = index;
                        }
                        else
                        {
                            cells[ tail ].next = index;
                            tail = index;
                        }
                        // mark as freed to indicate this cell is already in the list
                        cells[ index ].freed = true;
                        count--;
                        return true;
                    }
                }
                return false;
            }

            bool full()const
            {
                return size >= cells.size() && head == nullptr;
            }

            Storage( uint32_t size )
            :   head( 0 )
            ,   tail( 0 )
            ,   next( 0 )
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
            :   head( 0 )
            ,   tail( 0 )
            ,   next( 0 )
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

            int32_t head;
            int32_t tail;
            int32_t next;

            uint32_t count;
            uint32_t size;
            uint32_t id;

            bool listed;
        };

    private:

        void setGrowSize(uint16_t exponent, uint16_t max_exponent, uint16_t base )
        {
            _max_exponent = max_exponent;
            _exponent = exponent;
            _base = base;
        }

        uint32_t newStorage()
        {
            Storage* s = new Storage( getsize() );
            s->id = static_cast< uint32_t >( _storages.size() );
            _storages.push_back( s );
            return s->id;
        }

        uint32_t getsize()
        {
            uint32_t size = pow( _base, _exponent );
            size = ( size <= 0 ) ? _base : size;

            _exponent = ( _exponent < _max_exponent ) ? _exponent : _max_exponent;
            _exponent++;
            return size;
        }

    public:

        bool free( Elem* mem )
        {
            // return the cell back to storage which it came from.
            Storage* storage = _storages[ mem->storage ];
            if( storage->free( mem ) )
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
                        _storages[ _tail ]->next = storage->id;
                        _tail = storage->id;
                    }
                    storage->listed = true;
                }
                return true;
            }
            return false;
        }

        T* alloc()
        {
            if( _head < 0 )
            {
                _storages[ 0 ] = newStorage();
                _storages[ 0 ]->listed = true;
                _head = _tail = 0;
            }
            if ( _storages[ _head ]->full() )
            {
                _storages[ _head ]->listed = false;
                _head = -1;
                return alloc();
            }
            return _storages[ _head ]->alloc();
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

        MemT(uint16_t exponent, uint16_t max_exponent, uint16_t base )
        {
            setGrowSize(exponent, max_exponent, base);
        }

        ~MemT()
        {
            purge();
        }

        MemT()
        :   _head( -1 )
        ,   _tail( -1 )
        ,   _max_exponent( 8 )
        ,   _exponent( 0 )
        ,   _base( 4 )
        {}

    private:

        std::vector< Storage* > _storages;
        int32_t _head;
        int32_t _tail;

        uint32_t _max_exponent;
        uint32_t _exponent;
        uint32_t _base;
    };

}
#endif /* mem_hpp */

//struct test : MemT< test >::Elem
//{
//    static void* operator new(size_t size){ return mem.alloc(); }
//    static void operator delete(void* p){ mem.free((test*)p); }
//    static MemT< test > mem;
//    uint32_t _base;
//};
//
//void mainly()
//{
//    test* m = new test;
//    m->freed;
//}
