//
//  vulkan-descriptor-manager.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/4/25.
//

#ifndef vulkan_descriptor_manager_hpp
#define vulkan_descriptor_manager_hpp

#include <mutex>
#include <set>
#include <vector>
#include "vulkan-resources.hpp"
#include "../../core/resource-recycler.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan descriptor pool resources
     *
     * Encapsulates a VkDescriptorPool for allocating descriptor sets.
     * Could be extended to track allocation status for better management.
     */
    struct DescriptorPool
    {
        /** @brief Native Vulkan descriptor pool handle */
        VkDescriptorPool handle = VK_NULL_HANDLE;

        /** @brief Maximum number of descriptor sets this pool can allocate */
        uint32_t max_sets = 0;

        /** @brief Number of descriptor sets currently allocated from this pool */
        uint32_t allocated_sets = 0;

        /** @brief Tracks whether this pool supports freeing individual sets */
        bool individual_free_supported = false;

        int32_t owner;
        int32_t next;
        int32_t id;

        bool linked = false;
    };

    /**
     * @brief Wrapper for Vulkan descriptor set layout resources
     *
     * Encapsulates a VkDescriptorSetLayout and its associated bindings.
     */
    struct DescriptorSetLayout
    {
        /** @brief Native Vulkan descriptor set layout handle */
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;

        /** @brief Original layout creation parameters for reference/recreation */
        std::vector< DescriptorSetLayoutBinding > bindings;

        std::set< PipelineLayoutHandle > pipeline_layout_sets;
        std::string name;
        int32_t binding_location = -1;
        int32_t allocator_id = -1;
        int32_t id = -1;
    };

    /**
     * @brief Wrapper for Vulkan pipeline layout resources
     *
     * Encapsulates a VkPipelineLayout and the set layouts it references.
     */
    struct PipelineLayout
    {
        /** @brief Native Vulkan pipeline layout handle */
        VkPipelineLayout layout = VK_NULL_HANDLE;

        /** @brief Original layout creation parameters for reference/recreation */
        kege::PipelineLayoutDesc desc;

        /** @brief Cached native descriptor set layouts for pipeline creation */
        std::vector< VkDescriptorSetLayout > descriptor_set_layouts;

        std::unordered_map< uint32_t, uint32_t > binding_locations;
        VkPipelineBindPoint pipeline_bind_point;
    };

    /**
     * @brief Wrapper for Vulkan descriptor set resources
     *
     * Encapsulates a VkDescriptorSet and its associated metadata.
     */
    struct DescriptorSet
    {
        /** @brief Native Vulkan descriptor set handle */
        VkDescriptorSet set = VK_NULL_HANDLE;

        /** @brief Layout handle this set was allocated with */
        //DescriptorSetLayoutHandle layout_handle;

        /** @brief Pointer to the pool this set was allocated from */
        //DescriptorPool* pool_owner = nullptr;

        /** @brief Whether this set has been updated since allocation */
        bool is_updated = false;

        DescriptorSetLayoutHandle layout;
        int32_t pool_id;
    };

    /**
     * @brief Manages a pool of descriptor sets.
     */
    struct DescriptorAllocator
    {
        /**
         * @brief Sizes of descriptor pools managed by this allocator.
         */
        std::vector< DescriptorType > descriptor_types;

        /**
         * @brief The number of descriptors to allocate for each allocation.
         */
        uint32_t quantity = 1;

        /**
         * @brief Index of the head of the free list in the pool.
         */
        int32_t head = -1;

        /**
         * @brief Index of the tail of the free list in the pool.
         */
        int32_t tail = -1;

        /**
         * @brief Index of the current pool
         */
        int32_t curr = -1;

        /**
         * @brief Unique ID of the descriptor set allocator.
         */
        int32_t id = -1;

        /**
         * @brief Name of the descriptor set allocator.
         */
        std::string name;

        /**
         * @brief Track the number of pools associated with this descriptor allocator.
         */
        int32_t pool_count = 0;
    };



    class DescriptorManager
    {
    public:

        enum{ MAX_FRAMES_IN_FLIGHT = 2 };

        const vk::DescriptorSet* getDescriptorSet( const kege::DescriptorSetHandle& handle )const;
        vk::DescriptorSet* getDescriptorSet( const kege::DescriptorSetHandle& handle );

        /**
         * @brief Update descriptor sets with resource bindings
         *
         * Updates one or more descriptor sets with buffer, texture, or sampler bindings.
         *
         * @param writes Vector of descriptor write operations to perform
         */
        bool updateDescriptorSets( const std::vector< kege::WriteDescriptorSet >& writes );

        kege::DescriptorSetHandle allocateDescriptorSet( const std::vector< kege::DescriptorSetLayoutBinding >& bindings );

        /**
         * @brief Allocate a descriptor set
         *
         * Allocates a VkDescriptorSet based on the provided descriptor set layout.
         *
         * @param layout_handle Handle to the descriptor set layout to use
         * @return Handle to the newly allocated descriptor set
         */
        kege::DescriptorSetHandle allocateDescriptorSet( kege::DescriptorSetLayoutHandle layout_handle );

        /**
         * @brief Create a descriptor set layout
         *
         * Creates a VkDescriptorSetLayout based on the provided binding descriptions.
         *
         * @param bindings Description of the descriptor set layout to create
         * @return Index to the newly created descriptor set layout
         */
        kege::DescriptorSetLayoutHandle createDescriptorSetLayout( const std::vector< kege::DescriptorSetLayoutBinding >& bindings );

        /**
         * @brief Resolve a DescriptorAllocator
         *
         * Resolve full capacity or initialize a DescriptorPool with in a DescriptorAllocator
         *
         * @param allocator A point to the DescriptorAllocator to resolve
         * @return A pointer to the current DescriptorPool
         */
        vk::DescriptorPool* resolveDescriptorAllocator( vk::DescriptorAllocator* allocator );

        /**
         * @brief Create a pipeline layout
         *
         * Creates a VkPipelineLayout based on the provided descriptor set layouts
         * and push constant ranges.
         *
         * @param desc Description of the pipeline layout to create
         * @return Handle to the newly created pipeline layout
         */
        kege::PipelineLayoutHandle createPipelineLayout(const kege::PipelineLayoutDesc& desc);

        /**
         * @brief Get a pipeline-layout
         *
         * Get pipeline-layout object associated with PipelineLayoutHandle.
         *
         * @param handle The handle of the descriptor set layout to get
         * @return Pointer to the pipeline-layout object
         */
        const vk::PipelineLayout* getPipelineLayout( const kege::PipelineLayoutHandle& handle )const;
        vk::PipelineLayout* getPipelineLayout( const kege::PipelineLayoutHandle& handle );
        VkPipelineLayout getVkPipelineLayout( const kege::PipelineLayoutHandle& handle );

        /**
         * @brief Get a descriptor-set-layout
         *
         * Get index of a descriptor-set-layout based on the provided descriptor set layout bindings.
         *
         * @param handle The handle of the descriptor set layout to get
         * @return Pointer to the descriptor set layout
         */
        const vk::DescriptorSetLayout* getDescriptorSetLayout( const kege::DescriptorSetLayoutHandle& handle )const;
        vk::DescriptorSetLayout* getDescriptorSetLayout( const kege::DescriptorSetLayoutHandle& handle );

        /**
         * @brief Get a descriptor-set-layout
         *
         * Get index of a descriptor-set-layout based on the provided descriptor set layout bindings.
         *
         * @param bindings Description of the descriptor set layout to create
         * @param create If true, and the descriptor set layout does not exist, create it using the binding info
         * @return Index to the descriptor set layout
         */
        kege::DescriptorSetLayoutHandle getDescriptorSetLayout( const std::vector< kege::DescriptorSetLayoutBinding >& bindings, bool create );

        /**
         * @brief Destroy a pipeline layout
         *
         * Releases the VkPipelineLayout.
         *
         * @param handle Handle to the pipeline layout to destroy
         */
        void destroyPipelineLayout( kege::PipelineLayoutHandle handle );

        /**
         * @brief Destroy a descriptor set layout
         *
         * Releases the VkDescriptorSetLayout.
         *
         * @param handle Handle to the descriptor set layout to destroy
         */
        void destroyDescriptorSetLayout( kege::DescriptorSetLayoutHandle handle );

        /**
         * @brief Free a descriptor set
         *
         * Returns the VkDescriptorSet to the descriptor pool.
         *
         * @param handle Handle to the descriptor set to free
         */
        void freeDescriptorSet( kege::DescriptorSetHandle handle );

        bool initialize( Instance* instance, Device* device );
        void shutdown();
        
        DescriptorManager();

    private:

        /**
         * @brief Allocate a descriptor set
         *
         * Allocates a VkDescriptorSet from the main descriptor pool based on the
         * provided descriptor set layout.
         *
         * @param dsl The descriptor set layout to use
         * @return Index to the descriptor set
         */
        uint32_t allocateDescriptor( vk::DescriptorSetLayout* dsl );

        VkDescriptorPool createDescriptorPoolHandle( uint32_t maxsets, const std::vector< VkDescriptorPoolSize >& pool_sizes );
        uint32_t createDescriptorPool( uint32_t size, const std::vector< kege::DescriptorType >& pool_sizes );

        uint32_t createDescriptorAllocator( const std::vector< kege::DescriptorType >& descriptor_type );


        int getBindingKey( const std::string& name );
        
    private:

        std::unordered_map
        <
            std::vector< kege::DescriptorSetLayoutBinding >,
            uint32_t // descriptor set layout index
        >
        _descriptor_set_layout_cache;

        std::unordered_map
        <
            std::vector< kege::DescriptorType >,
            uint32_t // descriptor allocator index
        >
        _descriptor_allocator_cache;

        /** @brief Storage for descriptor set layout objects */
        ResourceRecycler< vk::DescriptorSetLayout > _descriptor_set_layouts;

        /** @brief Storage for descriptor set objects */
        ResourceRecycler< vk::DescriptorSet > _descriptor_sets;

        /** @brief Storage for descriptor pool objects */
        ResourceRecycler< vk::DescriptorPool > _descriptor_pools;

        /** @brief Storage for buffer objects */
        ResourceRecycler< vk::DescriptorAllocator > _descriptor_allocator;

        /** @brief Storage for pipeline layout objects */
        ResourceRecycler< vk::PipelineLayout > _pipeline_layouts;


        std::unordered_map< std::string, uint32_t > _descriptor_set_binding_keys;
        uint32_t _descriptor_set_binding_key_counter;

        vk::Instance* _instance;
        vk::Device* _device;

        std::mutex _resource_mutex;
    };
}

#endif /* vulkan_descriptor_manager_hpp */
