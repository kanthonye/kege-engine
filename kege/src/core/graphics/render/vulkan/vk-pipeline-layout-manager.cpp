//
//  pipeline-layout-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/11/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-pipeline-layout-manager.hpp"

namespace kege::vk{

    ShaderLayout::ShaderLayout
    (
        VkPipelineLayout handle,
        vk::Device* device, const std::string& name,
        std::vector< ref::ShaderSetBindingPointLayout >& shader_set_binding_layouts,
        std::vector< Ref< ShaderConstantBindingPoint > > push_constant_blocks
    )
    :   kege::ShaderLayout( name, shader_set_binding_layouts, push_constant_blocks )
    ,   _handle( handle )
    ,   _device( device )
    {}

    ShaderLayout::~ShaderLayout()
    {
        if( _device )
        {
            _device->destroyShaderLayout( this );
            _device = nullptr;
        }
    }
    //-------------------------------------------------------------------------
    // Pipeline Layout Lifecycle
    //-------------------------------------------------------------------------

    const vk::PipelineLayout* PipelineLayoutManager::getPipelineLayout( int32_t pipeline_layout_id )const
    {
        return _pipeline_layouts.get( pipeline_layout_id );
    }

    int32_t PipelineLayoutManager::createPipelineLayout
    (
        const char* name,
        const UniformDescriptorSets& layouts,
        const std::vector< PushConstantInfo >& push_constants
    )
    {
        if ( _device == VK_NULL_HANDLE ) return -1;

        int32_t pipeline_layout_id = _pipeline_layouts.gen();
        vk::PipelineLayout* pipeline_layout;
        {
            std::lock_guard<std::mutex> lock(_resource_mutex); // Assuming layouts might be shared/cached
            pipeline_layout = _pipeline_layouts.get( pipeline_layout_id );
            pipeline_layout->name = name;
        }

        //pipeline_layout->descriptor_set_layouts.reserve( desc.descriptor_set_layouts.size() );

        //int binding_index = 0;
        std::vector< VkDescriptorSetLayout > vk_descriptor_set_layouts;
        vk_descriptor_set_layouts.reserve( layouts.size() );
        for (const auto& layout : layouts)
        {
            /**
             * @brief Get the descriptor set layout from the cache.
             * If the descriptor set layout is not found, log an error and return an invalid handle
             */
            int layout_id = createUniformSetLayout( layout.descriptors );
            const vk::DescriptorSetLayout* dsl = getDescriptorSetLayout( layout_id );
            if ( dsl == nullptr )
            {
                kege::Log::error << "Invalid kege::UniformSetLayout provided!";
                 return -1;
            }

            /**
             * @brief Add the descriptor set layout to the list of vk_descriptor_set_layouts.
             * This is necessary for creating the vk pipeline layout handle
             */
            vk_descriptor_set_layouts.push_back( dsl->handle );
        }

        /**
         * @brief Create a vector of VkPushConstantRange from the push constant ranges in the descriptor layout.
         * This is used to specify the push constants that can be used in the pipeline.
         */
        std::vector<VkPushConstantRange> push_constant_ranges;
        push_constant_ranges.reserve( push_constants.size() );
        for ( const auto& range : push_constants )
        {
            VkPushConstantRange constant = {};
            constant.stageFlags = vk::convertShaderStageMask( range.stages );
            constant.offset = range.offset;
            constant.size = range.size;
            push_constant_ranges.push_back( constant );
        }

        /**
         * @brief Create the VkPipelineLayoutCreateInfo structure.
         * This structure is used to create the pipeline layout handle.
         */
        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.setLayoutCount = static_cast<uint32_t>( vk_descriptor_set_layouts.size() );
        info.pSetLayouts = vk_descriptor_set_layouts.data();
        info.pushConstantRangeCount = static_cast<uint32_t>( push_constant_ranges.size() );
        info.pPushConstantRanges = push_constant_ranges.data();

        /**
         * @brief Create the pipeline layout handle.
         * This handle is used to bind descriptor sets to the pipeline.
         */
        VkResult result;
        if (( result = _device->_manager.createPipelineLayout( &info, nullptr, &pipeline_layout->layout ) ) != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return -1;
        }

        if ( _instance->isValidationEnabled() && name != nullptr )
        {
            _device->_manager.debugSetObjectName( (uint64_t)pipeline_layout->layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, name );
        }

        return pipeline_layout_id;
    }

    void PipelineLayoutManager::destroyPipelineLayout( int32_t pipeline_layout_id )
    {
        if ( _device == VK_NULL_HANDLE || pipeline_layout_id == 0) return;
        std::lock_guard<std::mutex> lock( _resource_mutex );

        vk::PipelineLayout* layout = _pipeline_layouts.get( pipeline_layout_id );
        if ( layout != nullptr )
        {
            _device->_manager.destroyPipelineLayout( layout->layout, nullptr );
            layout->layout = VK_NULL_HANDLE;
            _pipeline_layouts.free( pipeline_layout_id );
        }
    }

    //-------------------------------------------------------------------------
    // Descriptor Set Layout Lifecycle
    //-------------------------------------------------------------------------

    int32_t PipelineLayoutManager::getDescriptorSetLayoutID( const UniformDescriptors& descriptors, bool create )
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

    int32_t PipelineLayoutManager::createUniformSetLayout( const UniformDescriptors& descriptors )
    {
//        auto i = _descriptor_set_layout_indexmap.find( descriptors );
//        if ( i != _descriptor_set_layout_indexmap.end() )
//        {
//            return i->second;
//        }
//
//        std::string name;
//
//        /**
//         * @brief Create a vector of VkDescriptorSetLayoutBinding from the bindings.
//         * This is used to create the VkDescriptorSetLayout handle.
//         * Each binding corresponds to a resource in the shader and its properties.
//         */
//        std::vector< VkDescriptorSetLayoutBinding > bindings;
//        for ( const kege::UniformDescriptor& desc : descriptors )
//        {
//            VkDescriptorSetLayoutBinding dslb = {};
//            dslb.binding = desc.binding;
//            dslb.descriptorCount = desc.count;
//            dslb.descriptorType = vk::convertDescriptorType( desc.descriptor_type );
//            dslb.stageFlags = VK_SHADER_STAGE_ALL;// vk::convertShaderStageMask( desc.stage_flags );
//            bindings.push_back( dslb );
//
//            name += (name.empty()) ? desc.name : "-" + desc.name;
//        }
//
//        /**
//         * @brief Create the VkDescriptorSetLayoutCreateInfo structure.
//         * This structure is used to create the VkDescriptorSetLayout handle.
//         * It contains the bindings, flags, and other properties of the descriptor set layout.
//         */
//        VkDescriptorSetLayoutCreateInfo create_info = {};
//        create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//        create_info.bindingCount = static_cast<int>( descriptors.size() );
//        create_info.pBindings = bindings.data();
//
//        /**
//         * @brief Create the VkDescriptorSetLayout.
//         * This is the Vulkan handle that represents the descriptor set layout.
//         */
//        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
//        VkResult result = _device->_manager.createDescriptorSetLayout( &create_info, nullptr, &layout );
//        if ( result != VK_SUCCESS )
//        {
//            kege::Log::error << vkResultToString( result );
//            return -1;
//        }
//
//        if ( _instance->isValidationEnabled() && !name.empty() )
//        {
//            _device->_manager.debugSetObjectName( (uint64_t)layout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, name.c_str() );
//        }
//
//        // ... Store handle ...
//
//        vk::DescriptorSetLayout* dsl = new vk::DescriptorSetLayout;
//        
//        dsl->id = _descriptor_set_layouts.gen();
//        *_descriptor_set_layouts[ dsl->id ] = dsl;
//
//        /**
//         * @brief Associate the descriptor set layout binding info to the descriptor set layout id.
//         * This allows us to quickly retrieve the descriptor set layout by its bindings.
//         * The bindings are hashed to create a unique key for the descriptor set layout.
//         * This is useful for caching and reusing descriptor set layouts.
//         */
//        _descriptor_set_layout_indexmap[ descriptors ] = dsl->id;
//
//        dsl->bindings = bindings;
//
//        /**
//         * @brief Assign the descriptor set layout its VkDescriptorSetLayout handle.
//         */
//        dsl->handle = layout;
//
//        /**
//         * @brief Assign the descriptor set layout its assigned debug name.
//         * This is used for debugging purposes and can be set to an empty string if not needed.
//         */
//        dsl->name = name;
//
//        /**
//         * @brief Set the allocator_id to -1, indicating that this descriptor set layout is not yet assigned
//         * a descriptot set allocator.
//         */
//        dsl->allocator_id = -1;
//
//        return dsl->id;
        return 0;
    }

    const vk::DescriptorSetLayout* PipelineLayoutManager::getDescriptorSetLayout( int32_t layout_id )const
    {
        return _descriptor_set_layouts[ layout_id ]->ref();
    }

    void PipelineLayoutManager::destroyDescriptorSetLayout( int32_t layout_id )
    {
        if ( _descriptor_set_layouts[ layout_id ] )
        {
            vk::DescriptorSetLayout* dsl = _descriptor_set_layouts[ layout_id ]->ref();
            _device->_manager.destroyUniformSetLayout( dsl->handle, nullptr );
            dsl->handle = VK_NULL_HANDLE;
            dsl->bindings = {};
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    // Shader Resource Set Lifecycle
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    int  PipelineLayoutManager::makeSet( const UniformDescriptorSet& descriptors, const UniformResourceSet& resources )
    {
        int set = allocateSet( descriptors );
        if ( set < 0 )
        {
            kege::Log::error << "Failed to allocate a descriptor-set"<<kege::Log::nl;
            return -1;
        }

        if ( !updateSet( set, resources ) )
        {
            kege::Log::error << "Failed to update descriptor-set" <<kege::Log::nl;
            freeSet( set );
            set = -1;
        }
        return set;
    }

    bool PipelineLayoutManager::updateSet( int handle, const UniformResourceSet& resources )
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

    int  PipelineLayoutManager::allocateSet( const UniformDescriptorSet& descriptors )
    {
//        vk::DescriptorAllocator* allocator = getDescriptorAllocator( descriptors.descriptors );
//        if ( allocator == nullptr )
//        {
//            kege::Log::error << "There are no existing descriptot-set-layout that matchs the falloowing layout \n";
//            for ( int i=0; i<descriptors.descriptors.size(); ++i)
//            {
//                kege::Log::error <<"name: " <<descriptors.descriptors[i].name << "\n";
//                kege::Log::error <<"descriptor_type: " << descriptorTypeCString( descriptors.descriptors[i].descriptor_type ) << "\n";
//                kege::Log::error <<"binding: " <<descriptors.descriptors[i].binding << "\n";
//                kege::Log::error <<"count: " <<descriptors.descriptors[i].count << "\n";
//            }
//            kege::Log::error <<kege::Log::nl;
//            return -1;
//        }
//
//        UniformSource* source;
//        for ( int i=0; i<descriptors.frames_in_flight; ++i)
//        {
//            source->sources[i] = allocator->allocate();
//        }
//        return set;
        return true;
    }

    void PipelineLayoutManager::freeSet( int set )
    {
        if ( _device == VK_NULL_HANDLE || set == 0) return;
        std::lock_guard<std::mutex> lock(_resource_mutex);

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
    
    const vk::DescriptorSet* PipelineLayoutManager::getSet( int32_t set )const
    {
        return _descriptor_sets.get( set );
    }

    vk::DescriptorAllocator* PipelineLayoutManager::getDescriptorAllocator( const UniformDescriptors& descriptors )
    {
        int32_t layout_index = getDescriptorSetLayoutID( descriptors );
        if ( layout_index < 0 )
        {}

        vk::DescriptorSetLayout* layout = _descriptor_set_layouts[ layout_index ]->ref();
        if ( layout->allocator_id < 0 )
        {
            layout->allocator_id = static_cast< int >( _descriptor_allocators.size() );
            //_descriptor_allocator_indexmap[ description ] = layout->allocator_id;

            DescriptorAllocator* allocator = new DescriptorAllocator( &_device->_manager, layout );
            _descriptor_allocators[ layout->allocator_id ] = allocator;

            return allocator;
        }
        return _descriptor_allocators[ layout_index ].ref();
    }

    bool PipelineLayoutManager::writeSampledImage
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
            if(info.image != nullptr )
            {
                descriptor_image_infos.push_back
                ({
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    .imageView = info.image->vk()->view(),
                    .sampler = VK_NULL_HANDLE
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

    bool PipelineLayoutManager::writeStorageImage
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

    bool PipelineLayoutManager::writeSampler
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

    bool PipelineLayoutManager::writeCombinedImageSampler
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

    bool PipelineLayoutManager::writeBuffer
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

    bool PipelineLayoutManager::writeDescriptor
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

    bool PipelineLayoutManager::initialize( vk::Instance* instance, vk::Device* device )
    {
        _instance = instance;
        _device = device;
        return true;
    }

    void PipelineLayoutManager::shutdown()
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
        _descriptor_allocator_indexmap.clear();
        _descriptor_set_layout_indexmap.clear();
        _descriptor_set_layouts.clear();
        _descriptor_allocators.clear();
        _descriptor_sets.clear();
        _pipeline_layouts.clear();
        _instance = nullptr;
        _device = nullptr;
    }

    PipelineLayoutManager::~PipelineLayoutManager()
    {
        shutdown();
    }

    PipelineLayoutManager::PipelineLayoutManager()
    :   _instance( nullptr )
    ,   _device( nullptr )
    ,   _resource_index_counter( 0 )
    {}

}
