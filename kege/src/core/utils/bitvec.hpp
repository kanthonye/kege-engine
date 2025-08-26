//
//  bitvec.hpp
//  kege
//
//  Created by Kenneth Esdaile on 11/26/24.
//

#ifndef bitvec_hpp
#define bitvec_hpp

#include <cstring>
#include <stdlib.h>
#include <stdexcept>

namespace kege{

    class BitVec
    {
    private:

        static uint32_t calculateSize(uint32_t bit_count)
        {
            return (bit_count + 63) / 64; // Round up to the nearest 64-bit boundary
        }

    public:

        class Ref
        {
        public:

            Ref( uint64_t& chunk, uint32_t bit_pos )
            :   chunk(chunk)
            ,   bit_pos(bit_pos)
            {}

            // Assign a value to the bit
            Ref& operator=(bool value)
            {
                if (value)
                {
                    chunk |= (1ULL << bit_pos); // Set the bit
                }
                else
                {
                    chunk &= ~(1ULL << bit_pos); // Clear the bit
                }
                return *this;
            }

            // Convert to bool
            operator bool() const
            {
                return (chunk >> bit_pos) & 1;
            }

        private:

            uint64_t& chunk;
            uint32_t bit_pos;
        };

    public:

        // Copy Assignment Operator
        BitVec& operator=( const BitVec& other )
        {
            if (this != &other)
            {
                delete[] _bits;

                _size = other._size;
                _capacity = other._capacity;
                _bits = new uint64_t[ _size ];
                memcpy(_bits, other._bits, _capacity * sizeof(uint64_t));
            }
            return *this;
        }

        // Move Assignment Operator
        BitVec& operator=(BitVec&& other) noexcept
        {
            if (this != &other)
            {
                delete[] _bits;

                _bits = other._bits;
                _size = other._size;
                _capacity = other._capacity;

                other._bits = nullptr;
                other._capacity = 0;
                other._size = 0;
            }
            return *this;
        }

        // Access a single bit (const version)
        Ref operator[](uint32_t index) const
        {
            if (index >= _size)
            {
                throw std::out_of_range("Index out of bounds");
            }

            uint32_t chunk = index / 64;    // Determine which 64-bit chunk
                                            //uint32_t bit_pos = index % 64; // Determine bit position within the chunk
                                            //return (_bits[ chunk ] >> bit_pos) & 1; // Extract the bit
            return Ref( _bits[ chunk ], index % 64 );
        }

        // Get the total size in bits
        void resize( uint32_t bit_count )
        {
            uint64_t newsize = calculateSize( bit_count );
            uint64_t* newbits = new uint64_t[ newsize ];
            if ( _bits )
            {

                uint64_t minsize = (_size < newsize) ? _size : newsize;
                memcpy(newbits, _bits, minsize * sizeof(uint64_t));
                delete[] _bits;
            }
            _bits = newbits;
            _capacity = bit_count;
            _size = newsize;
        }

        // Get the total size in bits
        uint64_t size()const
        {
            return _size;
        }

        // Set all bits to 0
        void clear()
        {
            std::memset(_bits, 0, _size * sizeof(uint64_t));
        }

        // Set all bits to 1
        void set_all()
        {
            std::memset(_bits, 0xFF, _size * sizeof(uint64_t));
        }

        // Count the number of set bits (Hamming weight)
        uint64_t capacity() const
        {
            return _capacity;
        }

        BitVec( uint32_t bit_count )
        :   _size( calculateSize( bit_count ) )
        ,   _capacity( bit_count )
        {
            _bits = new uint64_t[ _size ]();
        }

        BitVec()
        :   _size( 0 )
        ,   _capacity( 0 )
        ,   _bits( nullptr )
        {
        }

        ~BitVec()
        {
            if( _bits ) delete[] _bits;
            _bits = nullptr;
            _capacity = 0;
            _size = 0;
        }

    private:

        uint64_t _capacity;
        uint64_t _size;
        uint64_t* _bits;
    };

}
#endif /* bitvec_hpp */
