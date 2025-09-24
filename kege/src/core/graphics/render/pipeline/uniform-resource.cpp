//
//  uniform-resource-info.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/11/25.
//

#include "uniform-resource.hpp"

namespace kege{

    std::vector< UniformBinding >::const_iterator UniformSet::begin()const
    {
        return elements.begin();
    }
    std::vector< UniformBinding >::const_iterator UniformSet::end()const
    {
        return elements.end();
    }
    std::vector< UniformSet >::const_iterator UniformSets::begin()const
    {
        return sets.begin();
    }
    std::vector< UniformSet >::const_iterator UniformSets::end()const
    {
        return sets.end();
    }


    

    // --- Helper Function for Combining Hashes ---
    // (Often based on Boost's hash_combine)
    template <class T> inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        // Combine the hash of v with the current seed
        // Uses a prime number and bit shifts for better distribution
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    std::size_t hash(const kege::UniformDesc& binding)
    {
        std::size_t seed = 0;

        // Combine hashes of the relevant members
        kege::hash_combine(seed, binding.binding);
        // Hash the enum value directly (underlying type is usually int)
        kege::hash_combine(seed, static_cast<int>(binding.descriptor_type));
        kege::hash_combine(seed, binding.count);
        // Hash the flags directly (underlying type is usually int/uint)
        kege::hash_combine(seed, static_cast<uint32_t>(binding.stage_flags));

        // ** Important Caveat: pImmutableSamplers **
        // We generally DO NOT hash pImmutableSamplers for several reasons:
        // 1. It's a pointer: Hashing the address is meaningless for value comparison.
        // 2. VkSampler is an opaque handle: We cannot easily hash the "value"
        //    of the sampler itself in a consistent way across runs or different
        //    sampler objects that might be functionally identical.
        // 3. Complexity: Hashing the pointed-to sampler handles would require
        //    knowing the descriptorCount and iterating, adding complexity.
        //
        // If you *absolutely* need to differentiate layouts based *only* on
        // immutable samplers, you'll need a more complex strategy, potentially
        // hashing the VkSampler handles directly IF AND ONLY IF you guarantee
        // those handles are canonical and stable for equivalent layouts.
        // For most caching purposes, ignoring pImmutableSamplers is acceptable.
        // if (binding.pImmutableSamplers != nullptr) {
        //     // Complex: Potentially hash the handles pointed to, requires care
        //     for (uint32_t i = 0; i < binding.descriptorCount; ++i) {
        //         // Be careful: Hashing handle values might not be stable!
        //         hash_combine(seed, reinterpret_cast<size_t>(binding.pImmutableSamplers[i]));
        //     }
        // }

        return seed;
    }

    std::size_t hash( const std::vector< kege::UniformDesc >& bindings )
    {
        std::size_t seed = 0;

        // Combine hash of the size to quickly differentiate vectors of different lengths
        kege::hash_combine(seed, bindings.size());

        // Combine the hash of each element in order
        for (const auto& binding : bindings)
        {
            seed ^= hash( binding ) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            // Note: Using ^= directly here instead of hash_combine(seed, bindingHasher(binding))
            //       is slightly different but still a common pattern for combining element hashes.
            //       Using hash_combine(seed, bindingHasher(binding)) is also perfectly valid.
        }
        return seed;
    }

    std::size_t hash( const std::vector< kege::DescriptorType >& types )
    {
        std::size_t seed = 0;

        // Combine hash of the size to quickly differentiate vectors of different lengths
        kege::hash_combine(seed, types.size());

        // Combine the hash of each element in order
        for (const auto& type : types)
        {
            kege::hash_combine( seed, static_cast< int >( type ) );
        }
        return seed;
    }

}
