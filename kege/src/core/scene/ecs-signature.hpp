//
//  signature.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 11/25/25.
//

#ifndef signature_hpp
#define signature_hpp

#include <stdlib.h>

namespace kege::ecs{

    struct Signature
    {
        // 4 * 64-bit = 256 possible component slots
        uint64_t bits[4];
        uint64_t hash; // cached hash of the bits

        Signature() noexcept {
            bits[0] = bits[1] = bits[2] = bits[3] = 0;
            hash = 1469598103934665603ULL; // some non-zero default (optional)
        }

        // set a single bit (component id < 256)
        inline void set(uint32_t comp) noexcept {
            const uint32_t w = comp >> 6;               // divide by 64
            const uint32_t b = comp & 63;               // bit index
            bits[w] |= (1ull << b);
            recompute_hash();
        }

        inline void clear(uint32_t comp) noexcept {
            const uint32_t w = comp >> 6;
            const uint32_t b = comp & 63;
            bits[w] &= ~(1ull << b);
            recompute_hash();
        }

        inline bool test(uint32_t comp) const noexcept {
            const uint32_t w = comp >> 6;
            const uint32_t b = comp & 63;
            return (bits[w] >> b) & 1ull;
        }

        inline void reset() noexcept {
            bits[0] = bits[1] = bits[2] = bits[3] = 0;
            recompute_hash();
        }

        inline bool empty() noexcept {
            return bits[0] == 0 && bits[1] == 0 && bits[2] == 0 && bits[3] == 0;
        }

        // bitwise AND operator (useful for subset checks)
        inline Signature operator&(const Signature &o) const noexcept {
            Signature r;
            r.bits[0] = bits[0] & o.bits[0];
            r.bits[1] = bits[1] & o.bits[1];
            r.bits[2] = bits[2] & o.bits[2];
            r.bits[3] = bits[3] & o.bits[3];
            r.recompute_hash();
            return r;
        }

        // equality check (compare words)
        inline bool operator==(const Signature &o) const noexcept {
            return bits[0] == o.bits[0]
                && bits[1] == o.bits[1]
                && bits[2] == o.bits[2]
                && bits[3] == o.bits[3];
        }

        // subset test: (A ⊆ B) <=> (A & B) == A
        inline bool isSubsetOf(const Signature &superset) const noexcept {
            return ((bits[0] & superset.bits[0]) == bits[0]) &&
                   ((bits[1] & superset.bits[1]) == bits[1]) &&
                   ((bits[2] & superset.bits[2]) == bits[2]) &&
                   ((bits[3] & superset.bits[3]) == bits[3]);
        }

        // number of set bits (popcount) across the signature
        inline int popcount() const noexcept {
            return __builtin_popcountll(bits[0]) +
                   __builtin_popcountll(bits[1]) +
                   __builtin_popcountll(bits[2]) +
                   __builtin_popcountll(bits[3]);
        }

        // recompute the cached hash. Use a fast, portable mix function.
        inline void recompute_hash() noexcept {
            // simple 64-bit mix based on splitmix64
            uint64_t x = 0x9E3779B97F4A7C15ULL;
            auto mix = [&](uint64_t v) {
                x += v + 0x9e3779b97f4a7c15ULL;
                uint64_t z = (v ^ (v >> 30)) * 0xbf58476d1ce4e5b9ULL;
                z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
                z ^= (z >> 31);
                return z;
            };
            uint64_t h0 = mix(bits[0]);
            uint64_t h1 = mix(bits[1] + 0x9e3779b97f4a7c15ULL);
            uint64_t h2 = mix(bits[2] + 0x3c6ef372fe94f82aULL);
            uint64_t h3 = mix(bits[3] + 0xdaa66d2c7d5a5e3dULL);
            hash = h0 ^ (h1 << 1) ^ (h2 << 2) ^ (h3 << 3);
            if (hash == 0) hash = 1; // avoid zero if you prefer
        }
    };

    // Hash functor suitable for unordered_map
    struct SignatureHash {
        size_t operator()(const Signature &s) const noexcept {
            // use the cached 64-bit hash; size_t may be 64-bit on your platform
            return static_cast<size_t>(s.hash);
        }
    };

    // optional equality functor (redundant if operator== is available)
    struct SignatureEq {
        bool operator()(const Signature &a, const Signature &b) const noexcept {
            return a == b;
        }
    };
}
#endif /* signature_hpp */
