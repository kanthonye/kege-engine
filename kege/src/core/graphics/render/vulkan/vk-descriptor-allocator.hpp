//
//  vk-descriptor-allocator.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/15/25.
//

#ifndef vk_descriptor_allocator_hpp
#define vk_descriptor_allocator_hpp

#include "vk-utils.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan descriptor set layout resources
     *
     * Encapsulates a VkDescriptorSetLayout and its associated bindings.
     */
    struct DescriptorSetLayout : public RefLink< vk::DescriptorSetLayout >
    {
        /** @brief Original layout creation parameters for reference/recreation */
        std::vector< VkDescriptorSetLayoutBinding > bindings;

        /** @brief Native Vulkan descriptor set layout handle */
        VkDescriptorSetLayout handle = VK_NULL_HANDLE;

        std::string name;

        /**
         * @brief Each descriptor set layout is assigned a resource index based on its
         * binding configuration. This index maps the descriptor set to the correct
         * slot index within a pipeline layout, enabling automatic binding. This
         * allows pipelines with different set numbers but identical layouts
         * (e.g., one pipeline uses set 1 for materials, another uses set 2) to
         * share the same resource index. At bind time, the system remaps the
         * differing set numbers to the proper slot index.
         */
        //int32_t resource_index;

        int32_t allocator_id;
        int32_t id;
    };


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

        /** @brief Index of the next descriptor pool */
        int32_t next_pool;

        /**
         * @brief Unique ID of the descriptor pool.
         * This id is used as index to manage the pool within the allocator.
         */
        int32_t id;

        bool linked = false;
    };

    struct DescriptorSet : public RefLink< DescriptorSet >
    {
        DescriptorSet( VkDescriptorSet s, DescriptorAllocator* a, int32_t i )
        :   set( s )
        ,   allocator( a )
        ,   pool_index( i )
        ,   frames_in_flight( 1 )
        {}

        /** @brief Native Vulkan descriptor set handle */
        VkDescriptorSet set[ MAX_FRAMES_IN_FLIGHT ];

        DescriptorAllocator* allocator;
        int32_t pool_index;
        int32_t frames_in_flight;
    };


    /**
     * @brief Manages a pool of descriptor sets.
     */
    class DescriptorAllocator : public kege::RefCounter
    {
    public:

        void free( DescriptorSet* set );
        DescriptorSet* allocate();

        bool createPool();
        void purge();

        DescriptorAllocator( vk::Manager* manager, const vk::DescriptorSetLayout* layout );
        ~DescriptorAllocator();

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


        vk::List< vk::DescriptorSet > _allocs;
        vk::List< vk::DescriptorSet > _freed;
        
        const vk::DescriptorSetLayout* _layout;

        List _pools;

        /**
         * @brief Unique ID of the descriptor set allocator.
         */
        int32_t _id = -1;

        vk::Manager* _manager;
    };

}

#endif /* vk_descriptor_allocator_hpp */
