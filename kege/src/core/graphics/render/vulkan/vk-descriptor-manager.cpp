//
//  vk-descriptor-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/15/25.
//

#include "vk-descriptor-manager.hpp"
#include "vk-device.hpp"
#include "vk-instance.hpp"

namespace kege::vk{

    vk::ShaderSetBindingPointLayout* DescriptorManager::createShaderSetBindingLayout
    (
        const kege::ShaderSetBindingPoints& shader_set_bindings
    )
    {
        return nullptr;
    }

    //-------------------------------------------------------------------------
    // Descriptor Set Layout Lifecycle
    //-------------------------------------------------------------------------

    int32_t DescriptorManager::getDescriptorSetLayoutID( const UniformDescriptors& descriptors, bool create )
    {
        auto i = _descriptor_set_layout_indexmap.find( descriptors );
        if ( i != _descriptor_set_layout_indexmap.end() )
        {
            return i->second;
        }

        if ( create )
        {
            return createUniformSetLayout( descriptors );
        }
        return -1;
    }

    int32_t DescriptorManager::createUniformSetLayout( const UniformDescriptors& descriptors )
    {
        auto i = _descriptor_set_layout_indexmap.find( descriptors );
        if ( i != _descriptor_set_layout_indexmap.end() )
        {
            return i->second;
        }

        std::string name;

        /**
         * @brief Create a vector of VkDescriptorSetLayoutBinding from the bindings.
         * This is used to create the VkDescriptorSetLayout handle.
         * Each binding corresponds to a resource in the shader and its properties.
         */
        std::vector< VkDescriptorSetLayoutBinding > bindings;
        for ( const kege::UniformDescriptor& desc : descriptors )
        {
            VkDescriptorSetLayoutBinding dslb = {};
            dslb.binding = desc.binding;
            dslb.descriptorCount = desc.count;
            dslb.descriptorType = vk::convertDescriptorType( desc.descriptor_type );
            dslb.stageFlags = VK_SHADER_STAGE_ALL;// vk::convertShaderStageMask( desc.stage_flags );
            bindings.push_back( dslb );

            name += (name.empty()) ? desc.name : "-" + desc.name;
        }

        /**
         * @brief Create the VkDescriptorSetLayoutCreateInfo structure.
         * This structure is used to create the VkDescriptorSetLayout handle.
         * It contains the bindings, flags, and other properties of the descriptor set layout.
         */
        VkDescriptorSetLayoutCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        create_info.bindingCount = static_cast<int>( descriptors.size() );
        create_info.pBindings = bindings.data();

        /**
         * @brief Create the VkDescriptorSetLayout.
         * This is the Vulkan handle that represents the descriptor set layout.
         */
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        VkResult result = _device->core().createDescriptorSetLayout( &create_info, nullptr, &layout );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return -1;
        }

        if ( _instance->isValidationEnabled() && !name.empty() )
        {
            _device->core().debugSetObjectName( (uint64_t)layout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, name.c_str() );
        }

        // ... Store handle ...

        vk::DescriptorSetLayout* dsl = new vk::DescriptorSetLayout;

        dsl->id = _descriptor_set_layouts.gen();
        *_descriptor_set_layouts[ dsl->id ] = dsl;

        /**
         * @brief Associate the descriptor set layout binding info to the descriptor set layout id.
         * This allows us to quickly retrieve the descriptor set layout by its bindings.
         * The bindings are hashed to create a unique key for the descriptor set layout.
         * This is useful for caching and reusing descriptor set layouts.
         */
        _descriptor_set_layout_indexmap[ descriptors ] = dsl->id;

        dsl->bindings = bindings;

        /**
         * @brief Assign the descriptor set layout its VkDescriptorSetLayout handle.
         */
        dsl->handle = layout;

        /**
         * @brief Assign the descriptor set layout its assigned debug name.
         * This is used for debugging purposes and can be set to an empty string if not needed.
         */
        dsl->name = name;

        /**
         * @brief Set the allocator_id to -1, indicating that this descriptor set layout is not yet assigned
         * a descriptot set allocator.
         */
        dsl->allocator_id = -1;

        return dsl->id;
    }

    const vk::DescriptorSetLayout* DescriptorManager::getDescriptorSetLayout( int32_t layout_id )const
    {
        return _descriptor_set_layouts[ layout_id ]->ref();
    }

    void DescriptorManager::destroyDescriptorSetLayout( int32_t layout_id )
    {
        if ( _descriptor_set_layouts[ layout_id ] )
        {
            vk::DescriptorSetLayout* dsl = _descriptor_set_layouts[ layout_id ]->ref();
            _device->core().destroyUniformSetLayout( dsl->handle, nullptr );
            dsl->handle = VK_NULL_HANDLE;
            dsl->bindings = {};
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    // Shader Resource Set Lifecycle
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    UniformSource* DescriptorManager::makeSet( const UniformDescriptorSet& descriptors, const UniformResourceSet& resources )
    {
        UniformSource* set = allocateSet( descriptors );
        if ( set == nullptr )
        {
            kege::Log::error << "Failed to allocate a descriptor-set"<<kege::Log::nl;
            return set;
        }

        if ( !updateSet( set, resources ) )
        {
            kege::Log::error << "Failed to update descriptor-set" <<kege::Log::nl;
            freeSet( set );
            set = nullptr;
        }
        return set;
    }

    UniformSource* DescriptorManager::allocateSet( const UniformDescriptorSet& descriptors )
    {
        vk::DescriptorAllocator* allocator = getDescriptorAllocator( descriptors.descriptors );
        if ( allocator == nullptr )
        {
            kege::Log::error << "There are no existing descriptot-set-layout that matchs the falloowing layout \n";
//            for ( int i=0; i<descriptors.descriptors.size(); ++i)
//            {
//                kege::Log::error <<"name: " <<descriptors.descriptors[i].name << "\n";
//                kege::Log::error <<"descriptor_type: " << descriptorTypeCString( descriptors.descriptors[i].descriptor_type ) << "\n";
//                kege::Log::error <<"binding: " <<descriptors.descriptors[i].binding << "\n";
//                kege::Log::error <<"count: " <<descriptors.descriptors[i].count << "\n";
//            }
            kege::Log::error <<kege::Log::nl;
            return nullptr;
        }

        UniformSource* source;
        for ( int i=0; i<descriptors.frames_in_flight; ++i)
        {
            source->sources[i] = allocator->allocate();
        }
        return source;
    }

    bool DescriptorManager::updateSet( UniformSource* handle, const UniformResourceSet& resources )
    {
//        DescriptorSet* descriptor = _descriptor_sets.get( handle );
//        if (descriptor == nullptr)
//        {
//            KEGE_LOG_WARN << "Invalid ShaderResource handle in updateSet!"<<kege::Log::nl;
//            return false;
//        }
//        DescriptorSetLayout* dsl = getDescriptorSetLayout( descriptor->layout_id );
//
//        int buffer_count = 0, image_count = 0;
//        for (const auto& set : resources )
//        {
//            switch ( set.uniform.type )
//            {
//                case Uniform::BUFFER:
//                case Uniform::BUFFER_VIEW:
//                {
//                    buffer_count += set.uniform.count();
//                    break;
//                }
//                case Uniform::IMAGE:
//                {
//                    image_count += set.uniform.count();
//                    break;
//                }
//                case Uniform::INVALID:
//                {
//                    kege::Log::error << "INVALID Uniform!" << kege::Log::nl;
//                    return false;
//                }
//            }
//        }
//        std::vector< VkDescriptorBufferInfo > buffer_infos;
//        buffer_infos.reserve( buffer_count ); // Rough estimate
//
//        std::vector< VkDescriptorImageInfo > image_infos;
//        image_infos.reserve( image_count ); // Rough estimate
//
//        std::vector< VkWriteDescriptorSet > descriptor_writes;
//        descriptor_writes.reserve( resources.size() );
//
//        //int starting_index;
//        for ( int i=0; i<resources.size(); ++i )
//        {
//            VkDescriptorType descriptor_type = dsl->bindings[ resources[i].binding ].descriptorType;
//
//            VkWriteDescriptorSet descriptor_write = {};
//            descriptor_write.dstSet = descriptor->set;
//
//            bool state = writeDescriptor
//            (
//                resources[i],
//                descriptor_type,
//                buffer_infos,
//                image_infos,
//                &descriptor_write
//            );
//            if( !state || descriptor_write.descriptorCount <= 0 )
//            {
//                return false;
//            }
//
//            descriptor_writes.push_back( descriptor_write );
//        }
//
//        if ( !descriptor_writes.empty() )
//        {
//            _device->_manager.updateDescriptorSets( static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr );
//        }
        return true;
    }

    void DescriptorManager::freeSet( UniformSource* source )
    {
//        if ( _device == VK_NULL_HANDLE || set == 0) return;
//        std::lock_guard<std::mutex> lock(_resource_mutex);
//
//        if ( _descriptor_sets.get( set ) != nullptr )
//        {
//            vk::DescriptorSet* descripor = _descriptor_sets.get( set );
//            if ( 0 <= descripor->allocator_id && 0 <= descripor->pool_id && !descripor->freed )
//            {
//                vk::DescriptorAllocator* allocator = _descriptor_allocators.get( descripor->allocator_id );
//                vk::DescriptorPool& pool = allocator->descriptor_pools[ descripor->pool_id ];
//
//                if ( !pool.linked )
//                {
//                    if ( allocator->pool.head < 0 )
//                    {
//                        allocator->pool.head = allocator->pool.tail = descripor->pool_id;
//                    }
//                    else
//                    {
//                        allocator->descriptor_pools[ allocator->pool.tail ].next_pool = descripor->pool_id;
//                        allocator->pool.head = descripor->pool_id;
//                    }
//                    pool.linked = true;
//                }
//
//                if ( allocator->descriptors.head < 0 )
//                {
//                    allocator->descriptors.head = allocator->descriptors.tail = set;
//                }
//                else
//                {
//                    descripor->next = allocator->pool.tail;
//                    allocator->pool.head = set;
//                }
//
//                pool.allocated_set_count--;
//                descripor->freed = true;
//            }
//            //_descriptor_sets.free( handle.id );
//        }
    }

//    const vk::DescriptorSet* DescriptorManager::getSet( int32_t set )const
//    {
//        return 0;//_descriptor_sets.get( set );
//    }

    vk::DescriptorAllocator* DescriptorManager::getDescriptorAllocator( const UniformDescriptors& descriptors )
    {
        int32_t layout_index = getDescriptorSetLayoutID( descriptors );
        if ( layout_index < 0 )
        {}

        vk::DescriptorSetLayout* layout = _descriptor_set_layouts[ layout_index ]->ref();
        if ( layout->allocator_id < 0 )
        {
            layout->allocator_id = static_cast< int >( _descriptor_allocators.size() );
            //_descriptor_allocator_indexmap[ description ] = layout->allocator_id;

            DescriptorAllocator* allocator = 0;//TODO: new DescriptorAllocator( &_device->_manager, layout );
            _descriptor_allocators[ layout->allocator_id ] = allocator;

            return allocator;
        }
        return _descriptor_allocators[ layout_index ].ref();
    }

    bool DescriptorManager::writeSampledImage
    (
        VkDescriptorType descriptor_type,
        const std::vector< kege::ImageInfo >& image_info,
        std::vector< VkDescriptorImageInfo >& descriptor_image_infos,
        VkWriteDescriptorSet* descriptor_write
    )
    {
        int starting_index = static_cast< uint32_t >( descriptor_image_infos.size() );
        for(const auto& info : image_info)
        {
            if(info.image != nullptr && info.sampler != nullptr)
            {
                descriptor_image_infos.push_back
                ({
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    .imageView = info.image->vk()->view(),
                    .sampler = info.sampler->vk()->handle()
                });
            }
            else
            {
                kege::Log::error
                << "Invalid ref::Image for DescriptorType::InputAttachment or DescriptorType::SampledImage in updateDescriptorSets!";
                return false;
            }
        }

        descriptor_write->descriptorCount = static_cast< uint32_t >( image_info.size() );
        descriptor_write->pImageInfo = &descriptor_image_infos[ starting_index ];
        return true;
    }

    bool DescriptorManager::writeStorageImage
    (
        VkDescriptorType descriptor_type,
        const std::vector< kege::ImageInfo >& image_info,
        std::vector< VkDescriptorImageInfo >& descriptor_image_infos,
        VkWriteDescriptorSet* descriptor_write
    )
    {
        int starting_index = static_cast< uint32_t >( descriptor_image_infos.size() );
        for(const auto& info : image_info)
        {
            if( info.image != nullptr )
            {
                descriptor_image_infos.push_back
                ({
                    .imageLayout = VK_IMAGE_LAYOUT_GENERAL, // Often needed for storage images
                    .imageView = info.image->vk()->view(),
                    .sampler = nullptr
                });
            }
            else
            {
                kege::Log::error
                << "Invalid ref::Image for DescriptorType::StorageImage in updateDescriptorSets!";
                return false;
            }
        }

        descriptor_write->descriptorCount = static_cast< uint32_t >( image_info.size() );
        descriptor_write->pImageInfo = &descriptor_image_infos[ starting_index ];
        return true;
    }

    bool DescriptorManager::writeSampler
    (
        VkDescriptorType descriptor_type,
        const std::vector< kege::ImageInfo >& image_info,
        std::vector< VkDescriptorImageInfo >& image_infos,
        VkWriteDescriptorSet* descriptor_write
    )
    {
        int starting_index = static_cast< uint32_t >( image_infos.size() );
        for(const auto& info : image_info)
        {
            if( info.sampler != nullptr )
            {
                image_infos.push_back
                ({
                    .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .imageView = VK_NULL_HANDLE,
                    .sampler = info.sampler->vk()->handle()
                });
            }
            else
            {
                kege::Log::error
                << "Invalid ref::Sampler for DescriptorType::Sampler in updateDescriptorSets!";
                return false;
            }
        }

        descriptor_write->descriptorCount = static_cast< uint32_t >( image_info.size() );
        descriptor_write->pImageInfo = &image_infos[ starting_index ];
        return true;
    }

    bool DescriptorManager::writeCombinedImageSampler
    (
        VkDescriptorType descriptor_type,
        const std::vector< kege::ImageInfo >& image_info,
        std::vector< VkDescriptorImageInfo >& descriptor_image_infos,
        VkWriteDescriptorSet* descriptor_write
    )
    {
        int starting_index = static_cast< uint32_t >( descriptor_image_infos.size() );
        for(const auto& info : image_info)
        {
            if(info.image != nullptr && info.sampler != nullptr)
            {
                descriptor_image_infos.push_back
                ({
                    .imageLayout = vk::convertImageLayout( info.layout ),
                    .imageView = info.image->vk()->view(),
                    .sampler = info.sampler->vk()->handle()
                });
            }
            else
            {
                if( info.image == nullptr )
                {
                    kege::Log::error
                    << "Invalid ref::Image for DescriptorType::CombinedImageSampler in updateDescriptorSets!";
                }
                else
                {
                    kege::Log::error
                    << "Invalid ref::Sampler for DescriptorType::CombinedImageSampler in updateDescriptorSets!";
                }
                return false;
            }
        }

        descriptor_write->descriptorCount = static_cast< uint32_t >( image_info.size() );
        descriptor_write->pImageInfo = &descriptor_image_infos[ starting_index ];
        return true;
    }

    bool DescriptorManager::writeBuffer
    (
        VkDescriptorType descriptor_type,
        const std::vector< kege::BufferInfo >& buffer_info,
        std::vector< VkDescriptorBufferInfo >& descriptor_buffer_infos,
        VkWriteDescriptorSet* descriptor_write
    )
    {
        int starting_index = static_cast< uint32_t >( descriptor_buffer_infos.size() );
        for( const auto& info : buffer_info )
        {
            const vk::Buffer* buffer = info.buffer->vk();
            int frame = _device->getFrameIndex() % buffer->frames();

            if (buffer != nullptr)
            {
                descriptor_buffer_infos.push_back({ buffer->getSource( frame ).handle, info.offset, info.range });
            }
            else
            {
                switch ( descriptor_type )
                {
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                        kege::Log::error
                        << "Invalid BufferHandle for DescriptorType::UniformBuffer in updateDescriptorSets!";
                        break;

                    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                        kege::Log::error
                        << "Invalid BufferHandle for DescriptorType::StorageBuffer in updateDescriptorSets!";
                        break;

                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                        kege::Log::error
                        << "Invalid BufferHandle for DescriptorType::UniformBufferDynamic in updateDescriptorSets!";
                        break;

                    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                        kege::Log::error
                        << "Invalid BufferHandle for DescriptorType::StorageBufferDynamic in updateDescriptorSets!";
                        break;

                    default:
                        kege::Log::error
                        << "Invalid BufferHandle in updateDescriptorSets!";
                        break;
                }
                return false;
            }
        }

        descriptor_write->descriptorCount = static_cast< uint32_t >( buffer_info.size() );
        descriptor_write->pBufferInfo = &descriptor_buffer_infos[ starting_index ];
        return true;
    }

    bool DescriptorManager::writeDescriptor
    (
        const UniformResource& resource,
        VkDescriptorType descriptor_type,
        std::vector< VkDescriptorBufferInfo >& buffer_infos,
        std::vector< VkDescriptorImageInfo >& image_infos,
        VkWriteDescriptorSet* descriptor_write
    )
    {
        descriptor_write->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write->descriptorType = descriptor_type;
        descriptor_write->dstBinding = resource.binding;
        descriptor_write->dstArrayElement = 0;//resource_binding.array_element;
        descriptor_write->descriptorCount = 0; // Will be set based on info type
        //descriptor_write.dstSet = descriptor->set;

        switch ( descriptor_type )
        {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            {
                bool success = writeBuffer
                (
                    descriptor_type,
                    resource.uniform.buffers,
                    buffer_infos,
                    descriptor_write
                );
                if ( !success )
                {
                    return false;
                }
                break;
            }

            case VK_DESCRIPTOR_TYPE_SAMPLER:
            {
                bool success = writeSampler
                (
                    descriptor_type,
                    resource.uniform.images,
                    image_infos,
                    descriptor_write
                );
                if ( !success )
                {
                    return false;
                }
                break;
            }

            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            {
                bool success = writeSampledImage
                (
                    descriptor_type,
                    resource.uniform.images,
                    image_infos,
                    descriptor_write
                );
                if ( !success )
                {
                    return false;
                }
                break;
            }

            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            {
                bool success = writeCombinedImageSampler
                (
                    descriptor_type,
                    resource.uniform.images,
                    image_infos,
                    descriptor_write
                );
                if ( !success )
                {
                    return false;
                }
                break;
            }

            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            {
                bool success = writeStorageImage
                (
                    descriptor_type,
                    resource.uniform.images,
                    image_infos,
                    descriptor_write
                );
                if ( !success )
                {
                    return false;
                }
                break;
            }

            // Add cases for Texel Buffers if needed (requires VkBufferView)

            default:
                kege::Log::error << "Unsupported descriptor type in UpdateDescriptorSets!" << kege::Log::nl;
                return false; // Skip this write operation
        }
        return true;
    }

    bool DescriptorManager::initialize( vk::Instance* instance, vk::Device* device )
    {
        _instance = instance;
        _device = device;
        return true;
    }

    void DescriptorManager::shutdown()
    {
//        for ( uint32_t i = 0; i < _pipeline_layouts.count(); ++i )
//        {
//            if ( _pipeline_layouts.get( i ) != nullptr )
//            {
//                _device->_manager.destroyPipelineLayout( _pipeline_layouts.get( i )->layout, nullptr );
//            };
//        }
//        for ( uint32_t i = 0; i < _descriptor_set_layouts.count(); ++i )
//        {
//            if ( _descriptor_set_layouts.get( i ) != nullptr )
//            {
//                _device->_manager.destroyUniformSetLayout( _descriptor_set_layouts.get( i )->handle, nullptr );
//            };
//        }
//        for ( uint32_t i = 0; i < _descriptor_allocators.count(); ++i )
//        {
//            if ( _descriptor_allocators.get( i ) != nullptr )
//            {
//                for ( uint32_t j = 0; j < _descriptor_allocators.get(i)->descriptor_pools.size(); ++j )
//                {
//                    _device->_manager.destroyDescriptorPool( _descriptor_allocators.get(i)->descriptor_pools[j].handle, nullptr );
//                }
//            };
//        }
//        _descriptor_allocator_indexmap.clear();
        _descriptor_set_layout_indexmap.clear();
        _descriptor_set_layouts.clear();
        _descriptor_allocators.clear();
        _descriptor_sets.clear();
//        _pipeline_layouts.clear();
        _instance = nullptr;
        _device = nullptr;
    }

    DescriptorManager::~DescriptorManager()
    {
        shutdown();
    }

    DescriptorManager::DescriptorManager()
    :   _instance( nullptr )
    ,   _device( nullptr )
//    ,   _resource_index_counter( 0 )
    {}

}
