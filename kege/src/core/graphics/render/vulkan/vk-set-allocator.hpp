//
//  vk-set-allocator.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/15/25.
//

#ifndef kege_vk_set_allocator_hpp
#define kege_vk_set_allocator_hpp

#include "vk-utils.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan descriptor pool resources
     *
     * Encapsulates a VkDescriptorPool for allocating descriptor sets.
     * Could be extended to track allocation status for better management.
     */
    struct DescriptorPool : public kege::RefCounter
    {
        /** @brief Native Vulkan descriptor pool handle */
        VkDescriptorPool handle = VK_NULL_HANDLE;

        /**
         * @brief Number of descriptor sets currently allocated from this pool
         */
        uint32_t allocated_set_count = 0;

        /**
         * @brief Maximum number of descriptor sets that can be allocated from this pool
         */
        uint32_t max_sets = 0;

        /**
         * @brief Index of the next descriptor pool
         */
        int32_t next_pool;

        /**
         * @brief Unique ID of the descriptor pool.
         * This id is used as index to manage the pool within the allocator.
         */
        int32_t id;

        bool linked = false;
    };

    /**
     * @brief Manages a pool of descriptor sets.
     */
    class SetAllocator : public kege::RefCounter
    {
    public:

        vk::DescriptorSet allocate( vk::SetLayout* layout );
        void free( vk::DescriptorSet& set );
        void purge();

        SetAllocator( vk::Device* device, const std::vector< VkDescriptorType >& descriptor_types );
        ~SetAllocator();

    private:

        DescriptorPool* getPool();
        bool createPool();
        
    private:

        struct List
        {
            /**
             * @brief Index of the head of the free list in the pool.
             */
            int32_t head = -1;

            /**
             * @brief Index of the tail of the free list in the pool.
             */
            int32_t tail = -1;
        };

    private:

        /**
         * @brief List of descriptor pools managed by this allocator.
         * Pools are created as needed when allocation requests exceed current capacity.
         */
        std::vector< kege::Ref< DescriptorPool > > _descriptor_pools;
        
        /**
         * @brief Sizes of descriptor pools managed by this allocator.
         */
        std::vector< VkDescriptorType > _descriptor_types;

        /**
         * @brief Linked list managing free descriptor pools.
         */
        List _pool_manager;

        /**
         * @brief Unique ID of the descriptor set allocator.
         */
        int32_t _frames_in_flight;

        vk::Device* _device;
    };

    struct DescriptorTypeVectorHash
    {
        std::size_t operator()(const std::vector<VkDescriptorType>& vec) const noexcept
        {
            std::size_t h = 0;

            // A standard, well-behaved hash combine (boost-like)
            auto hash_combine = [&](std::size_t v) {
                h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
            };

            for (VkDescriptorType t : vec)
            {
                // Casting to size_t is fine; VkDescriptorType is an enum
                hash_combine(std::hash<std::size_t>{}(static_cast<std::size_t>(t)));
            }

            return h;
        }
    };
}

#endif /* kege_vk_set_allocator_hpp */
