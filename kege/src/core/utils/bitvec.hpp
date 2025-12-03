//
//  bitvec.hpp
//  kege
//
//  Created by Kenneth Esdaile on 11/26/24.
//

#ifndef bitvec_hpp
#define bitvec_hpp

#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace kege{

    class BitVec
    {
    public:

        // ========================
        // Nested proxy bit reference
        // ========================
        class Ref
        {
        public:
            Ref(uint64_t& chunk, uint32_t pos)
                : chunk(chunk), bit_pos(pos) {}

            // Assign boolean to bit
            Ref& operator=(bool value)
            {
                if (value)
                    chunk |= (1ULL << bit_pos);
                else
                    chunk &= ~(1ULL << bit_pos);
                return *this;
            }

            // Assign from another Ref
            Ref& operator=(const Ref& other)
            {
                return *this = bool(other);
            }

            // Convert to bool
            operator bool() const
            {
                return (chunk >> bit_pos) & 1ULL;
            }

        private:
            uint64_t& chunk;
            uint32_t bit_pos;
        };

    public:

        // ========================
        // Constructors / destructor
        // ========================
        BitVec() : bit_count(0), chunk_count(0), chunks(nullptr) {}

        BitVec(uint32_t bits)
            : bit_count(bits), chunk_count(calc_chunks(bits))
        {
            chunks = new uint64_t[chunk_count]();
        }

        ~BitVec()
        {
            delete[] chunks;
        }

        // ========================
        // Copy / Move
        // ========================
        BitVec(const BitVec& other)
            : bit_count(other.bit_count),
              chunk_count(other.chunk_count)
        {
            chunks = new uint64_t[chunk_count];
            memcpy(chunks, other.chunks, chunk_count * sizeof(uint64_t));
        }

        BitVec(BitVec&& other) noexcept
            : bit_count(other.bit_count),
              chunk_count(other.chunk_count),
              chunks(other.chunks)
        {
            other.chunks = nullptr;
            other.bit_count = 0;
            other.chunk_count = 0;
        }

        BitVec& operator=(const BitVec& other)
        {
            if (this != &other)
            {
                delete[] chunks;
                bit_count = other.bit_count;
                chunk_count = other.chunk_count;
                chunks = new uint64_t[chunk_count];
                memcpy(chunks, other.chunks, chunk_count * sizeof(uint64_t));
            }
            return *this;
        }

        BitVec& operator=(BitVec&& other) noexcept
        {
            if (this != &other)
            {
                delete[] chunks;
                bit_count = other.bit_count;
                chunk_count = other.chunk_count;
                chunks = other.chunks;

                other.chunks = nullptr;
                other.bit_count = 0;
                other.chunk_count = 0;
            }
            return *this;
        }

        // ========================
        // Access
        // ========================
        Ref operator[](uint32_t index)
        {
            if (index >= bit_count)
                throw std::out_of_range("BitVec index out of range.");

            return Ref(chunks[index >> 6], index & 63);
        }

        bool operator[](uint32_t index) const
        {
            if (index >= bit_count)
                throw std::out_of_range("BitVec index out of range.");

            return (chunks[index >> 6] >> (index & 63)) & 1ULL;
        }

        // ========================
        // Resize
        // ========================
        void resize(uint32_t new_bit_count)
        {
            uint32_t new_chunk_count = calc_chunks(new_bit_count);

            uint64_t* new_chunks = new uint64_t[new_chunk_count]();

            if (chunks)
            {
                uint32_t m = (chunk_count < new_chunk_count ? chunk_count : new_chunk_count);
                memcpy(new_chunks, chunks, m * sizeof(uint64_t));
            }

            delete[] chunks;

            chunks = new_chunks;
            chunk_count = new_chunk_count;
            bit_count = new_bit_count;
        }

        // ========================
        // Utility
        // ========================
        void clear()
        {
            memset(chunks, 0, chunk_count * sizeof(uint64_t));
        }

        void set_all()
        {
            memset(chunks, 0xFF, chunk_count * sizeof(uint64_t));
        }

        uint32_t size() const { return bit_count; }
        uint32_t chunks_used() const { return chunk_count; }

    private:

        static uint32_t calc_chunks(uint32_t bits)
        {
            return (bits + 63) >> 6;
        }

    private:

        uint32_t chunk_count;    // number of uint64 chunks
        uint32_t bit_count;      // total number of bits
        uint64_t* chunks;
    };


}
#endif /* bitvec_hpp */
