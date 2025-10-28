//
//  vk-descriptor-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/15/25.
//

#ifndef vk_descriptor_manager_hpp
#define vk_descriptor_manager_hpp

#include <set>
#include "graphics-core.hpp"
#include "resource-recycler.hpp"
#include "vk-descriptor-allocator.hpp"

namespace kege::vk{

    struct UniformSource
    {
        /** @brief Native Vulkan descriptor set handle */
        DescriptorSet* sources[ MAX_FRAMES_IN_FLIGHT ];
        int32_t frames_in_flight;

        int32_t allocator_id;
        int32_t layout_id;
    };

    class ShaderSetBindingPointLayout : public kege::ShaderSetBindingPointLayout
    {
    public:

        VkDescriptorSetLayout handle = VK_NULL_HANDLE;
    };

    class DescriptorManager
    {
    public:

        vk::ShaderSetBindingPointLayout* createShaderSetBindingLayout
        ( const kege::ShaderSetBindingPoints& shader_set_bindings );

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
         * @param layout_id Handle to the descriptor set layout to retrieve
         * @return Pointer to the VkDescriptorSetLayout, or nullptr if not found
         */
        const vk::DescriptorSetLayout* getDescriptorSetLayout( int32_t layout_id )const;

        /**
         * @brief Destroy a descriptor set layout
         *
         * Releases the DescriptorSetLayout object.
         *
         * @param layout_id Handle to the descriptor set layout to destroy
         */
        void destroyDescriptorSetLayout( int32_t layout_id );

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
        // Shader Resource Set Lifecycle
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

        UniformSource* makeSet( const UniformDescriptorSet& descriptors, const UniformResourceSet& resources );
        UniformSource* allocateSet( const UniformDescriptorSet& descriptors );

        bool updateSet( UniformSource* source, const UniformResourceSet& resources );
        void freeSet( UniformSource* source );

        vk::DescriptorAllocator* getDescriptorAllocator( const UniformDescriptors &descriptors );
        //-------------------------------------------------------------------------
        // Descriptor Set Lifecycle
        //-------------------------------------------------------------------------


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

        ~DescriptorManager();
        DescriptorManager();

    private:


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

//        /**
//         * @brief Map of descriptor set layout binding configurations to a unique index
//         */
//        std::unordered_map
//        <
//            UniformDescriptors,
//            uint32_t
//        >
//        _resource_index_map;

        /** @brief Map of descriptor set layout configurations to their indices */
        std::unordered_map
        <
            UniformDescriptors,
            uint32_t // descriptor set layout index
        >
        _descriptor_set_layout_indexmap;

//        /** @brief Map of descriptor type configurations to their allocator indices */
//        std::unordered_map
//        <
//            UniformDescriptors,
//            uint32_t // descriptor allocator index
//        >
//        _descriptor_allocator_indexmap;

        /** @brief Storage for descriptor set layout objects */
        ResourceRecycler< kege::Ref< vk::DescriptorSetLayout > > _descriptor_set_layouts;

        /** @brief Storage for buffer objects */
        std::vector< kege::Ref< vk::DescriptorAllocator > > _descriptor_allocators;

        /** @brief Storage for descriptor set objects */
        ResourceRecycler< DescriptorSet > _descriptor_sets;

        vk::Instance* _instance;
        vk::Device* _device;
    };

}

#endif /* vk_descriptor_manager_hpp */
