//
//  pipeline-layout-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/11/25.
//

#ifndef vk_pipeline_layout_manager_hpp
#define vk_pipeline_layout_manager_hpp

#include <set>
#include "graphics-core.hpp"
#include "resource-recycler.hpp"
#include "vk-utils.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan pipeline layout resources
     *
     * Encapsulates a VkPipelineLayout and the set layouts it references.
     */
    struct PipelineLayout
    {
        /**
         * @brief To automate the process of assigning descriptor sets to the correct
         * slots during binding, each descriptor is given a unique index identifier
         * derived from its descriptor set layout configuration. This index identifier
         * is then used to map the descriptor set to a specific slot index within the
         * pipeline layout.
         *
         * This approach is especially useful for pipelines that share descriptor sets
         * but have different layouts. For example, one pipeline might use set 0 for global
         * resources and set 1 for material resources, while another pipeline uses set 0
         * for global resources but set 2 for material resources. Even though the material
         * resources are bound to different set numbers (set 1 and set 2), they share the
         * same configuration (descriptor type, binding, count) and can therefore be
         * assigned the same resource index.
         *
         * This mapping system allows the pipeline to remap differing set numbers
         * (like set 1 and set 2) to the appropriate slot index when binding descriptor sets.
         * See the setIndex parameter of the vkCmdBindDescriptorSets function for reference.
         */
        std::unordered_map< uint32_t, uint32_t > descriptor_set_index_map;

        /** @brief The pipeline binding point (VK_PIPELINE_BIND_POINT_GRAPHICS, VK_PIPELINE_BIND_POINT_COMPUTE, etc ) */
        VkPipelineBindPoint pipeline_bind_point;

        /** @brief Native Vulkan pipeline layout handle */
        VkPipelineLayout layout = VK_NULL_HANDLE;

        std::string name;
    };

    /**
     * @brief Wrapper for Vulkan descriptor set layout resources
     *
     * Encapsulates a VkDescriptorSetLayout and its associated bindings.
     */
    struct DescriptorSetLayout
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
    struct DescriptorPool
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

    /**
     * @brief Manages a pool of descriptor sets.
     */
    struct DescriptorAllocator
    {
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

        /**
         * @brief Sizes of descriptor pools managed by this allocator.
         */
        std::vector< VkDescriptorType > descriptor_types;

        /**
         * @brief List of descriptor pools managed by this allocator.
         * Pools are created as needed when allocation requests exceed current capacity.
         */
        std::vector< DescriptorPool > descriptor_pools;

        List descriptors;
        List pool;

        /**
         * @brief Unique ID of the descriptor set allocator.
         */
        int32_t id = -1;
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
        int32_t allocator_id;
        int32_t layout_id;
        int32_t pool_id;
        int32_t next;
        bool freed;
    };
    
}




namespace kege::vk{

    class PipelineLayoutManager
    {
    public:

        int32_t createPipelineLayout( const char* name, const UniformDescriptorSets& layouts, const std::vector< PushConstantInfo >& push_constants );

        /**
         * @brief Get a pipeline layout
         *
         * Retrieves the VkPipelineLayout associated with the given handle.
         *
         * @param pipeline_layout_id Handle to the pipeline layout to retrieve
         * @return Pointer to the VkPipelineLayout, or nullptr if not found
         */
        const vk::PipelineLayout* getPipelineLayout( int32_t pipeline_layout_id )const;

        /**
         * @brief Destroy a pipeline layout
         *
         * Releases the VkPipelineLayout.
         *
         * @param pipeline_layout_id Handle to the pipeline layout to destroy
         */
        void destroyPipelineLayout( int32_t pipeline_layout_id );

        //-------------------------------------------------------------------------
        // Descriptor Set Layout Lifecycle
        //-------------------------------------------------------------------------

        std::vector< int > createUniformSetLayouts( const UniformDescriptors& description );

        /**
         * @brief Create a descriptor set layout
         *
         * Creates a VkDescriptorSetLayout based on the provided bindings.
         *
         * @param description The bindings of the descriptor set layout to create
         * @return Handle to the newly created descriptor set layout
         */
        int32_t createUniformSetLayout( const UniformDescriptors& description );

        /**
         * @brief Get a descriptor-set-layout
         *
         * Get index of a descriptor-set-layout based on the provided descriptor set layout bindings.
         *
         * @param description The bindings of the descriptor set layout to get
         * @param create Whether to create the layout if it does not exist
         * @return Handle to the descriptor set layout
         */
        int32_t getDescriptorSetLayoutID( const UniformDescriptors& description, bool create = false );

        /**
         * @brief Get a descriptor-set-layout
         *
         * Retrieves the VkDescriptorSetLayout associated with the given handle.
         *
         * @param handle Handle to the descriptor set layout to retrieve
         * @return Pointer to the VkDescriptorSetLayout, or nullptr if not found
         */
        const vk::DescriptorSetLayout* getDescriptorSetLayout( int32_t handle )const;

        /**
         * @brief Destroy a descriptor set layout
         *
         * Releases the DescriptorSetLayout object.
         *
         * @param handle Handle to the descriptor set layout to destroy
         */
        void destroyDescriptorSetLayout( int32_t handle );

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
        // Shader Resource Set Lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

        int  makeSet( const UniformDescriptorSet& descriptors, const UniformResourceSet& resources );
        bool updateSet( int handle, const UniformResourceSet& resources );
        int  allocateSet( const UniformDescriptorSet& descriptors );
        void freeSet( int set );

        /**
         * @brief Get a descriptor set
         *
         * Retrieves the DescriptorSet object associated with the given handle.
         *
         * @param descriptor_id Handle to the descriptor set to retrieve
         * @return Pointer to the DescriptorSet object, or nullptr if not found
         */
        const vk::DescriptorSet* getSet( int32_t descriptor_id )const;

        /**
         * @brief Allocate multiple descriptor sets from a layout.
         * @param layout The already created descriptor set layout handle.
         * @param quantity Number of descriptor sets to allocate.
         * @param sets Output array to receive allocated descriptor set handles.
         * @return True if allocation succeeded for all sets, false otherwise.
         */
        bool allocateDescriptors( int32_t layout, int32_t quantity, int32_t* sets );

        //-------------------------------------------------------------------------
        // Descriptor Set Lifecycle
        //-------------------------------------------------------------------------

        /**
         * @brief Update multiple descriptor sets with new resource bindings.
         * @param handles Vector of descriptor set handles to update.
         * @param resource_sets Vector of resource sets containing the new bindings.
         * @return True if all updates succeeded, false otherwise.
         */
        //bool updateUniformSets( const std::vector< int >& handles, const UniformSets& resource_sets );

        /**
         * @brief Update a single descriptor set with new resource bindings.
         * @param handle Handle of the descriptor set to update.
         * @param resource_set Resource set containing the new bindings.
         * @return True if the update succeeded, false otherwise.
         */
        //bool updateUniformSet( int handle, const UniformSet& resource_set );

        /**
         * @brief Allocate multiple descriptor sets from layouts.
         * @param description Descriptions of the descriptor set layouts to allocate from.
         * @return Vector of handles to the newly allocated descriptor sets.
         */
        //std::vector< int > allocateUniformSets( const UniformSetsDesc& description );

        /**
         * @brief Allocate a single descriptor set from a layout.
         * @param description Description of the descriptor set layout to allocate from.
         * @return Handle to the newly allocated descriptor set.
         */
        //int allocateUniformSet( const UniformSetDesc& description );

        /**
         * @brief Free a descriptor set
         * @param descriptor_id Handle to the descriptor set to free
         */

        //-------------------------------------------------------------------------
        // SHutdown And Initialization
        //-------------------------------------------------------------------------

        /**
         * @brief Initialize the descriptor manager
         *
         * Sets up internal structures and prepares for descriptor management.
         *
         * @param instance Pointer to the Vulkan instance
         * @param device Pointer to the Vulkan device
         * @return True if initialization succeeded, false otherwise
         */
       bool initialize( vk::Instance* instance, vk::Device* device );

        /**
         * @brief Shutdown the descriptor manager
         *
         * Cleans up all allocated resources and internal structures.
         */
       void shutdown();

        ~PipelineLayoutManager();
        PipelineLayoutManager();

    private:

        /**
         * @brief Generate a unique resource index for a descriptor set layout
         *
         * Computes a hash based on the binding configuration to produce
         * a unique index identifier for the layout.
         *
         * @param description The bindings of the descriptor set layout
         * @return Unique resource index for the layout
         */
//        int generateResourceBindingIndex
//        (
//            const UniformSetDesc& description
//        );

        /**
         * @brief Create a descriptor set allocator
         *
         * Creates a DescriptorAllocator based on the provided bindings.
         *
         * @param description The bindings of the descriptor set layout to create the allocator for
         * @return Index of the newly created descriptor allocator
         */
        int createDescriptorSetAllocator
        (
            const UniformDescriptors& description
        );

        /**
         * @brief Create a descriptor pool for the given allocator
         *
         * Allocates a new VkDescriptorPool based on the allocator's descriptor types.
         *
         * @param allocator The descriptor allocator to create the pool for
         * @return True if the pool was created successfully, false otherwise
         */
        bool createDescriptorPool( vk::DescriptorAllocator* allocator );

        /**
         * @brief Write sampled image info to a descriptor write structure  
         * @param image_info The resource binding containing the sampled image info.
         * @param descriptor_image_infos Vector to store the VkDescriptorImageInfo structures.
         * @param descriptor_write Pointer to the VkWriteDescriptorSet to populate.
         * @return True if the write operation succeeded, false otherwise.
         */
        bool writeSampledImage
        (
            VkDescriptorType descriptor_type,
            const std::vector< kege::ImageInfo >& image_info,
            std::vector< VkDescriptorImageInfo >& descriptor_image_infos,
            VkWriteDescriptorSet* descriptor_write
        );

        /**
         * @brief Write storage image info to a descriptor write structure  
         * @param image_info The resource binding containing the storage image info.
         * @param descriptor_image_infos Vector to store the VkDescriptorImageInfo structures.
         * @param descriptor_write Pointer to the VkWriteDescriptorSet to populate.
         * @return True if the write operation succeeded, false otherwise.
         */
        bool writeStorageImage
        (
            VkDescriptorType descriptor_type,
            const std::vector< kege::ImageInfo >& image_info,
            std::vector< VkDescriptorImageInfo >& descriptor_image_infos,
            VkWriteDescriptorSet* descriptor_write
        );

        /**
         * @brief Write input attachment info to a descriptor write structure  
         * @param image_info The resource binding containing the input attachment info.
         * @param descriptor_image_infos Vector to store the VkDescriptorImageInfo structures.
         * @param descriptor_write Pointer to the VkWriteDescriptorSet to populate.
         * @return True if the write operation succeeded, false otherwise.
         */
        bool writeSampler
        (
            VkDescriptorType descriptor_type,
            const std::vector< kege::ImageInfo >& image_info,
            std::vector< VkDescriptorImageInfo >& descriptor_image_infos,
            VkWriteDescriptorSet* descriptor_write
        );

        /**
         * @brief Write sampler info to a descriptor write structure  
         * @param image_info The resource binding containing the sampler info.
         * @param descriptor_image_infos Vector to store the VkDescriptorImageInfo structures.
         * @param descriptor_write Pointer to the VkWriteDescriptorSet to populate.
         * @return True if the write operation succeeded, false otherwise.
         */
        bool writeCombinedImageSampler
        (
            VkDescriptorType descriptor_type,
            const std::vector< kege::ImageInfo >& image_info,
            std::vector< VkDescriptorImageInfo >& descriptor_image_infos,
            VkWriteDescriptorSet* descriptor_write
        );

        /**
         * @brief Write buffer info to a descriptor write structure  
         * @param buffer_info The resource binding containing the buffer info.
         * @param descriptor_buffer_infos Vector to store the VkDescriptorBufferInfo structures.
         * @param descriptor_write Pointer to the VkWriteDescriptorSet to populate.
         * @return True if the write operation succeeded, false otherwise.
         */
        bool writeBuffer
        (
            VkDescriptorType descriptor_type,
            const std::vector<kege::BufferInfo>& buffer_info,
            std::vector<VkDescriptorBufferInfo>& descriptor_buffer_infos,
            VkWriteDescriptorSet* descriptor_write
        );

        bool writeDescriptor
        (
            const UniformResource& resource,
            VkDescriptorType descriptor_type,
            std::vector< VkDescriptorBufferInfo >& buffer_infos,
            std::vector< VkDescriptorImageInfo >& image_infos,
            VkWriteDescriptorSet* descriptor_write
        );
        
    private:

        /**
         * @brief Map of descriptor set layout binding configurations to a unique index
         */
        std::unordered_map
        <
            UniformDescriptors,
            uint32_t
        >
        _resource_index_map;

        /** @brief Map of descriptor set layout configurations to their indices */
        std::unordered_map
        <
            UniformDescriptors,
            uint32_t // descriptor set layout index
        >
        _descriptor_set_layout_indexmap;

        /** @brief Map of descriptor type configurations to their allocator indices */
        std::unordered_map
        <
            UniformDescriptors,
            uint32_t // descriptor allocator index
        >
        _descriptor_allocator_indexmap;

        /** @brief Storage for descriptor set layout objects */
        ResourceRecycler< DescriptorSetLayout > _descriptor_set_layouts;

        /** @brief Storage for buffer objects */
        ResourceRecycler< DescriptorAllocator > _descriptor_allocators;

        /** @brief Storage for descriptor set objects */
        ResourceRecycler< DescriptorSet > _descriptor_sets;

        /** @brief Storage for pipeline layout objects */
        ResourceRecycler< PipelineLayout > _pipeline_layouts;

        std::mutex _resource_mutex;
        vk::Instance* _instance;
        vk::Device* _device;

        uint32_t _resource_index_counter;
    };

}

#endif /* pipeline_layout_manager_hpp */
