//
//  vulkan-descriptor-manager.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/4/25.
//

#include "vulkan-device.hpp"
#include "vulkan-instance.hpp"
#include "vulkan-descriptor-manager.hpp"

namespace kege::vk{

    const DescriptorSet* DescriptorManager::getDescriptorSet( const kege::DescriptorSetHandle& handle )const
    {
        return _descriptor_sets.get( handle.id );
    }

    DescriptorSet* DescriptorManager::getDescriptorSet( const kege::DescriptorSetHandle& handle )
    {
        return _descriptor_sets.get( handle.id );
    }

    bool DescriptorManager::updateDescriptorSets( const std::vector< kege::WriteDescriptorSet >& writes )
    {
        if ( _device == nullptr || writes.empty() ) return false;

        std::vector< VkWriteDescriptorSet > descriptor_writes;
        descriptor_writes.reserve( writes.size() );

        int buffer_count = 0;
        int image_count = 0;
        for (const auto& write : writes)
        {
            buffer_count += write.buffer_info.size();
            image_count += write.image_info.size();
        }
        // Need temporary storage for VkDescriptorBufferInfo/ImageInfo because VkWriteDescriptorSet uses pointers
        // These need to stay alive until vkUpdateDescriptorSets returns.
        // Use a larger scope or ensure lifetime. Using vectors here for simplicity.
        std::vector<VkDescriptorBufferInfo> buffer_infos;
        std::vector<VkDescriptorImageInfo> image_infos;
        buffer_infos.reserve( buffer_count ); // Rough estimate
        image_infos.reserve( image_count ); // Rough estimate
        int starting_index;
        for (const auto& write : writes)
        {
            DescriptorSet* descriptor = _descriptor_sets.get( write.set.id );
            if (descriptor == nullptr)
            {
                KEGE_LOG_WARN << "Invalid kege::DescriptorSetHandle in updateDescriptorSets!";
                return false;
            }

            VkWriteDescriptorSet descriptor_write = {};
            descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write.dstSet = descriptor->set;
            descriptor_write.dstBinding = write.binding;
            descriptor_write.dstArrayElement = write.array_element;
            descriptor_write.descriptorType = convertDescriptorType(write.descriptor_type);
            descriptor_write.descriptorCount = 0; // Will be set based on info type

            switch (write.descriptor_type)
            {
                case kege::DescriptorType::UniformBuffer:
                case kege::DescriptorType::StorageBuffer:
                case kege::DescriptorType::UniformBufferDynamic:
                case kege::DescriptorType::StorageBufferDynamic:
                {
                    descriptor_write.descriptorCount = static_cast<uint32_t>( write.buffer_info.size() );
                    starting_index = static_cast<uint32_t>( buffer_infos.size() );
                    for( const auto& info : write.buffer_info )
                    {
                        const Buffer* buffer = _device->getBuffer( info.buffer );
                        if (buffer != nullptr)
                        {
                            buffer_infos.push_back({ buffer->buffer, info.offset, info.range });
                        }
                        else
                        {
                            switch ( write.descriptor_type )
                            {
                                case kege::DescriptorType::UniformBuffer:
                                    KEGE_LOG_ERROR
                                    << "Invalid BufferHandle for DescriptorType::UniformBuffer in updateDescriptorSets!";
                                    break;

                                case kege::DescriptorType::StorageBuffer:
                                    KEGE_LOG_ERROR
                                    << "Invalid BufferHandle for DescriptorType::StorageBuffer in updateDescriptorSets!";
                                    break;

                                case kege::DescriptorType::UniformBufferDynamic:
                                    KEGE_LOG_ERROR
                                    << "Invalid BufferHandle for DescriptorType::UniformBufferDynamic in updateDescriptorSets!";
                                    break;

                                case kege::DescriptorType::StorageBufferDynamic:
                                    KEGE_LOG_ERROR
                                    << "Invalid BufferHandle for DescriptorType::StorageBufferDynamic in updateDescriptorSets!";
                                    break;

                                default:
                                    KEGE_LOG_ERROR
                                    << "Invalid BufferHandle in updateDescriptorSets!";
                                    break;
                            }
                            return false;
                        }
                    }
                    descriptor_write.pBufferInfo = &buffer_infos[ starting_index ]; // Point to start of added elements
                    break;
                }

                case kege::DescriptorType::Sampler:
                {
                    descriptor_write.descriptorCount = static_cast<uint32_t>(write.image_info.size());
                    starting_index = static_cast<uint32_t>( image_infos.size() );
                    for(const auto& info : write.image_info)
                    {
                        const Sampler* sampler = _device->getSampler( info.sampler );
                        if( sampler != nullptr )
                        {
                            image_infos.push_back
                            ({
                                .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                .imageView = VK_NULL_HANDLE,
                                .sampler = sampler->sampler
                            });
                        }
                        else
                        {
                            KEGE_LOG_ERROR
                            << "Invalid SamplerHandle for DescriptorType::Sampler in updateDescriptorSets!";
                            return false;
                        }
                    }
                    descriptor_write.pImageInfo = &image_infos[ starting_index ];
                    break;
                }

                case kege::DescriptorType::SampledImage:
                case kege::DescriptorType::InputAttachment:
                {
                    descriptor_write.descriptorCount = static_cast<uint32_t>(write.image_info.size());
                    starting_index = static_cast<uint32_t>( image_infos.size() );
                    for(const auto& info : write.image_info)
                    {
                        const Sampler* sampler = _device->getSampler( info.sampler );
                        const Image* image = _device->getImage( info.image );
                        if(image != nullptr && sampler != nullptr)
                        {
                            image_infos.push_back
                            ({
                                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                .imageView = image->view,
                                .sampler = VK_NULL_HANDLE
                            });
                        }
                        else
                        {
                            KEGE_LOG_ERROR
                            << "Invalid ImageHandle for DescriptorType::InputAttachment or DescriptorType::SampledImage in updateDescriptorSets!";
                            return false;
                        }
                    }
                    descriptor_write.pImageInfo = &image_infos[ starting_index ];
                    break;
                }

                case kege::DescriptorType::CombinedImageSampler:
                {
                    descriptor_write.descriptorCount = static_cast<uint32_t>(write.image_info.size());
                    starting_index = static_cast<uint32_t>( image_infos.size() );
                    for(const auto& info : write.image_info)
                    {
                        const Sampler* sampler = _device->getSampler( info.sampler );
                        const Image* image = _device->getImage( info.image );
                        if(image != nullptr && sampler != nullptr)
                        {
                            image_infos.push_back
                            ({
                                .imageLayout = vk::convertImageLayout( info.layout ),
                                .imageView = image->view,
                                .sampler = sampler->sampler
                            });
                        }
                        else
                        {
                            if( image == nullptr )
                            {
                                KEGE_LOG_ERROR
                                << "Invalid ImageHandle for DescriptorType::CombinedImageSampler in updateDescriptorSets!";
                            }
                            else
                            {
                                KEGE_LOG_ERROR
                                << "Invalid SamplerHandle for DescriptorType::CombinedImageSampler in updateDescriptorSets!";
                            }
                            return false;
                        }
                    }
                    descriptor_write.pImageInfo = &image_infos[ starting_index ];
                    break;
                }

                case kege::DescriptorType::StorageImage:
                {
                    descriptor_write.descriptorCount = static_cast<uint32_t>(write.image_info.size());
                    starting_index = static_cast<uint32_t>( image_infos.size() );
                    for(const auto& info : write.image_info)
                    {
                        const Image* image = _device->getImage( info.image );
                        if( image != nullptr )
                        {
                            image_infos.push_back
                            ({
                                .imageLayout = VK_IMAGE_LAYOUT_GENERAL, // Often needed for storage images
                                .imageView = image->view,
                                .sampler = nullptr
                            });
                        }
                        else
                        {
                            KEGE_LOG_ERROR
                            << "Invalid ImageHandle for DescriptorType::StorageImage in updateDescriptorSets!";
                            return false;
                        }
                    }
                    descriptor_write.pImageInfo = &image_infos[ starting_index ];
                    break;
                }

                // Add cases for Texel Buffers if needed (requires VkBufferView)

                default:
                    std::cerr << "Unsupported descriptor type in UpdateDescriptorSets!" << std::endl;
                    return false; // Skip this write operation
            }

            if (descriptor_write.descriptorCount > 0)
            {
                descriptor_writes.push_back( descriptor_write );
            }
        }

        if ( !descriptor_writes.empty() )
        {
            _device->updateDescriptorSets( static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr );
        }
        return true;
    }

    kege::DescriptorSetHandle DescriptorManager::allocateDescriptorSet( const std::vector< kege::DescriptorSetLayoutBinding >& bindings )
    {
        kege::DescriptorSetLayoutHandle layout_handle = getDescriptorSetLayout( bindings, true );
        return allocateDescriptorSet( layout_handle );
    }

    kege::DescriptorSetHandle DescriptorManager::allocateDescriptorSet( kege::DescriptorSetLayoutHandle layout_handle )
    {
        if ( layout_handle.id < 0 )
        {
            KEGE_LOG_ERROR << "Invalid DescriptorSetLayoutHandle parameter" << Log::nl;
            return {};
        }

        std::lock_guard<std::mutex> lock(_resource_mutex);
        vk::DescriptorSetLayout* dsl = getDescriptorSetLayout( layout_handle );
        /**
         1. Check if the descriptor set layout already contained a pointer to a descriptor allocation.
            If it doesn't create a new allocator and assign it to the descriptor set layout
         */
        if ( dsl->allocator_id < 0 )
        {
            std::vector< kege::DescriptorType > descriptor_types;
            for ( const kege::DescriptorSetLayoutBinding& binding : dsl->bindings )
            {
                descriptor_types.push_back( binding.descriptor_type );
            }
            dsl->allocator_id = createDescriptorAllocator( descriptor_types );
        }
        /**
         2. Get allocator that is assocaited with the descriptor set layout.
         */
        vk::DescriptorAllocator* allocator = _descriptor_allocator.get( dsl->allocator_id );
        vk::DescriptorPool* pool = nullptr;
        /**
         3. Check if the allocator has a descriptor pool, if not create and assign it to the allocator.
         */
        if ( allocator->curr < 0 )
        {
            pool = resolveDescriptorAllocator( allocator );
        }
        /**
         4. Otherwise if the allocator already have a descriptor pool, check if it has available descriptot sets.
            If not, allocate a new descriptor pool and assign it to the allocator
         */
        else
        {
            pool = _descriptor_pools.get( allocator->curr );
            if ( pool->max_sets <= pool->allocated_sets )
            {
                pool = resolveDescriptorAllocator( allocator );
            }
        }
        /**
         5. Note: For each descriptot set we allocate we need a descriptor_set_layout handle.
         */
        std::vector< VkDescriptorSetLayout > descriptor_set_layouts( MAX_FRAMES_IN_FLIGHT, dsl->layout );
        /**
         6. Allocate a descriptot sets from the current descriptor pool.
         */
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorSetCount = (uint32_t)descriptor_set_layouts.size();
        alloc_info.pSetLayouts = descriptor_set_layouts.data();
        alloc_info.descriptorPool = pool->handle;
        VkResult result;
        VkDescriptorSet descriptor_set;
        if (( result = _device->allocateDescriptorSets( &alloc_info, &descriptor_set ) ) != VK_SUCCESS )
        {
            logVkError( result, "vulkan-device.hpp", __FUNCTION__ );
            return {};
        }
        
        /**
         7. With the descriptor set allocated. store its info into a new descriptor set object.
         */
        int32_t id = _descriptor_sets.gen();
        vk::DescriptorSet* set = _descriptor_sets.get( id );
        set->set = descriptor_set;
        set->layout = layout_handle;
        set->pool_id = pool->id;
        return {id};
    }

    kege::DescriptorSetLayoutHandle DescriptorManager::createDescriptorSetLayout( const std::vector< kege::DescriptorSetLayoutBinding >& bindings )
    {
        auto i = _descriptor_set_layout_cache.find( bindings );
        if ( i != _descriptor_set_layout_cache.end() )
        {
            return {int( i->second )};
        }

        std::string debug_name;
        
        /**
         * @brief Create a vector of VkDescriptorSetLayoutBinding from the bindings.
         * This is used to create the VkDescriptorSetLayout handle.
         * Each binding corresponds to a resource in the shader and its properties.
         */
        std::vector< VkDescriptorSetLayoutBinding > vk_bindings;
        for ( const kege::DescriptorSetLayoutBinding& binding : bindings )
        {
            VkDescriptorSetLayoutBinding dslb = {};
            dslb.binding = binding.binding;
            dslb.descriptorCount = binding.count;
            dslb.descriptorType = convertDescriptorType( binding.descriptor_type );
            dslb.stageFlags = convertShaderStageMask( binding.stage_flags );
            vk_bindings.push_back( dslb );

            debug_name += (debug_name.empty()) ? binding.name : "-" + binding.name;
        }

        /**
         * @brief Create the VkDescriptorSetLayoutCreateInfo structure.
         * This structure is used to create the VkDescriptorSetLayout handle.
         * It contains the bindings, flags, and other properties of the descriptor set layout.
         */
        VkDescriptorSetLayoutCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        create_info.bindingCount = static_cast<int>( bindings.size() );
        create_info.pBindings = vk_bindings.data();

        /**
         * @brief Create the VkDescriptorSetLayout.
         * This is the Vulkan handle that represents the descriptor set layout.
         */
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        VkResult result = _device->createDescriptorSetLayout( &create_info, nullptr, &layout );

        if ( result != VK_SUCCESS )
        {
            logVkError( result, "vulkan-device.hpp", "createDescriptorSetLayout" );
            return { -1 };
        }

        if ( _instance->isValidationEnabled() && !debug_name.empty() )
        {
            _device->debugSetObjectName( (uint64_t)layout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, debug_name.c_str() );
        }

        // ... Store handle ...
        int32_t id = _descriptor_set_layouts.gen();
        vk::DescriptorSetLayout* dsl = _descriptor_set_layouts.get( id );
        if ( dsl == nullptr )
        {
            KEGE_LOG_ERROR << "Failed to create DescriptorSetLayoutHandle!";
            return {-1};
        }
        /** 
         * @brief Assign a binding location to the descriptor set layout.
         * The binding location is an index associated with the descriptor set layout's name.
         */
        dsl->binding_location = getBindingKey( dsl->name );

        /**
         * @brief Set the allocator_id to -1, indicating that this descriptor set layout is not yet assigned
         * a descriptot set allocator.
         */
        dsl->allocator_id = -1;

        /**
         * @brief Set the descriptor set layout binding information.
         * This includes the bindings, layout, name, and id.
         */
        dsl->bindings = bindings;

        /**
         * @brief Assign the descriptor set layout its VkDescriptorSetLayout handle.
         */
        dsl->layout = layout;

        /**
         * @brief Assign the descriptor set layout its assigned debug name.
         * This is used for debugging purposes and can be set to an empty string if not needed.
         */
        dsl->name = debug_name;

        /**
         * @brief Assign the descriptor set layout its id.
         * This id is used to uniquely identify the descriptor set layout in the descriptor set layout cache.
         */
        dsl->id = id;

        /**
         * @brief Associate the descriptor set layout binding info to the descriptor set layout id.
         * This allows us to quickly retrieve the descriptor set layout by its bindings.
         * The bindings are hashed to create a unique key for the descriptor set layout.
         * This is useful for caching and reusing descriptor set layouts.
         */
        _descriptor_set_layout_cache[ bindings ] = id;
        return { id };
    }

    vk::DescriptorPool* DescriptorManager::resolveDescriptorAllocator( vk::DescriptorAllocator* allocator )
    {
        vk::DescriptorPool* pool = nullptr;
        if ( 0 < allocator->head )
        {
            allocator->curr = allocator->head;
            pool = _descriptor_pools.get( allocator->head );
            pool->linked = false;
            pool->next = -1;

            allocator->head = pool->next;
            if ( allocator->head < 0 )
            {
                allocator->tail = -1;
            }
        }
        else
        {
            allocator->pool_count += 1;
            allocator->curr = createDescriptorPool( allocator->pool_count, allocator->descriptor_types );

            pool = _descriptor_pools.get( allocator->curr );
            pool->owner = allocator->id;
        }
        return pool;
    }

    kege::PipelineLayoutHandle DescriptorManager::createPipelineLayout( const kege::PipelineLayoutDesc& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {-1};

        kege::PipelineLayoutHandle handle = { _pipeline_layouts.gen() };
        vk::PipelineLayout* pipeline_layout;
        {
            std::lock_guard<std::mutex> lock(_resource_mutex); // Assuming layouts might be shared/cached
            pipeline_layout = _pipeline_layouts.get( handle.id );
        }

        pipeline_layout->desc = desc;
        pipeline_layout->descriptor_set_layouts.reserve( desc.descriptor_set_layouts.size() );

        int binding_index = 0;

        std::vector< VkDescriptorSetLayout > vk_descriptor_set_layouts;
        for (const auto& descriptor_set_layout : desc.descriptor_set_layouts)
        {
            /**
             * @brief Get the descriptor set layout from the cache.
             * If the descriptor set layout is not found, log an error and return an invalid handle
             */
            vk::DescriptorSetLayout* dsl = _descriptor_set_layouts.get( descriptor_set_layout.id );
            if ( dsl == nullptr )
            {
                KEGE_LOG_ERROR << "Invalid kege::DescriptorSetLayoutHandle provided!";
                 return {-1};
            }

            /**
             * @brief Add the descriptor set layout to the pipeline layout list.
             */
            pipeline_layout->descriptor_set_layouts.push_back( dsl );

            /**
             * @brief Add the descriptor set layout to the list of vk_descriptor_set_layouts.
             * This is necessary for creating the vk pipeline layout handle
             */
            vk_descriptor_set_layouts.push_back( dsl->layout );

            /**
             * @brief Assign a binding location to the descriptor set layout if it doesn't already have one.
             *
             * The binding location is an index associated with the descriptor set layout's name.
             * This index is used to map descriptor set layouts to their binding points in the pipeline.
             * When binding descriptor sets using vkCmdBindDescriptorSets(), the 'firstSet' parameter
             * corresponds to this binding index. Any descriptor set allocated from a descriptor set layout
             * will use its associated binding location to determine where it should be bound in the pipeline.
             * This ensures consistent mapping between descriptor set layouts and their binding indices.
             */
            if ( dsl->binding_location < 0 )
            {
                dsl->binding_location = getBindingKey( dsl->name );
            }

            /**
             * @brief If the descriptor set layout already has a binding location.
             * assign a binding index to that location.
             */
            pipeline_layout->binding_locations[ dsl->binding_location ] = binding_index;
            binding_index += 1;
        }

        /**
         * @brief Create a vector of VkPushConstantRange from the push constant ranges in the descriptor layout.
         * This is used to specify the push constants that can be used in the pipeline.
         */
        std::vector<VkPushConstantRange> push_constant_ranges;
        push_constant_ranges.reserve(desc.push_constant_ranges.size());
        for (const auto& range : desc.push_constant_ranges)
        {
            VkPushConstantRange vkRange = {};
            vkRange.stageFlags = convertShaderStageMask(range.stage_flags);
            vkRange.offset = range.offset;
            vkRange.size = range.size;
            push_constant_ranges.push_back(vkRange);
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
        if (( result = _device->createPipelineLayout( &info, nullptr, &pipeline_layout->layout ) ) != VK_SUCCESS )
        {
            logVkError( result, "vulkan-device.hpp", "createPipelineLayout" );
            return {-1};
        }

        if ( _instance->isValidationEnabled() && !desc.debug_name.empty() )
        {
            _device->debugSetObjectName( (uint64_t)pipeline_layout->layout, VK_OBJECT_TYPE_IMAGE, desc.debug_name.c_str() );
        }

        /**
         * @brief Store a reference to the pipeline layout in each descriptor set layout.
         *
         * This allows descriptor sets to be associated with all pipeline layouts that use them.
         * By keeping track of these relationships, you can bind a descriptor set globally to any pipeline
         * it is linked with, even before binding a specific pipeline. This enables flexible and efficient
         * descriptor set management and retrieval across multiple pipelines.
         */
        for (const auto& descriptor_set_layout : pipeline_layout->descriptor_set_layouts )
        {
            descriptor_set_layout->pipeline_layout_sets.insert( pipeline_layout );
        }
        return handle;
    }

    const PipelineLayout* DescriptorManager::getPipelineLayout( const kege::PipelineLayoutHandle& handle )const
    {
        return _pipeline_layouts.get( handle.id );
    }

    vk::PipelineLayout* DescriptorManager::getPipelineLayout( const kege::PipelineLayoutHandle& handle )
    {
        return _pipeline_layouts.get( handle.id );
    }
    
    VkPipelineLayout DescriptorManager::getVkPipelineLayout( const kege::PipelineLayoutHandle& handle )
    {
        if ( _pipeline_layouts.get( handle.id ) != nullptr )
        {
            return _pipeline_layouts.get( handle.id )->layout;
        }
        return VK_NULL_HANDLE;
    }

    const vk::DescriptorSetLayout* DescriptorManager::getDescriptorSetLayout( const kege::DescriptorSetLayoutHandle& handle )const
    {
        return _descriptor_set_layouts.get( handle.id );
    }

    vk::DescriptorSetLayout* DescriptorManager::getDescriptorSetLayout( const kege::DescriptorSetLayoutHandle& handle )
    {
        return _descriptor_set_layouts.get( handle.id );
    }

    kege::DescriptorSetLayoutHandle DescriptorManager::getDescriptorSetLayout( const std::vector< kege::DescriptorSetLayoutBinding >& bindings, bool create )
    {
        auto i = _descriptor_set_layout_cache.find( bindings );
        if ( i != _descriptor_set_layout_cache.end() )
        {
            //vk::DescriptorSetLayout* dsl = _descriptor_set_layouts.get( i->second );
            return {int( i->second )};
        }

        if ( create )
        {
            return createDescriptorSetLayout( bindings );
        }
        return {-1};
    }

    uint32_t DescriptorManager::createDescriptorAllocator( const std::vector< kege::DescriptorType >& descriptor_types )
    {
        auto m = _descriptor_allocator_cache.find( descriptor_types );
        if ( m == _descriptor_allocator_cache.end() )
        {
            uint32_t id = _descriptor_allocator.gen();
            // record the descriptor allocator id, for lookup by descriptor types
            _descriptor_allocator_cache[ descriptor_types ] = id;

            // setup the descriptor allocator
            DescriptorAllocator* allocator = _descriptor_allocator.get( id );
            allocator->descriptor_types = descriptor_types;
            allocator->quantity = 1;
            allocator->head = -1;
            allocator->tail = -1;
            allocator->id = id;
            allocator->pool_count = 0;
            return id;
        }
        return m->second;
    }

    VkDescriptorPool DescriptorManager::createDescriptorPoolHandle( uint32_t maxsets, const std::vector< VkDescriptorPoolSize >& pool_sizes )
    {
        VkDescriptorPoolCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        create_info.poolSizeCount = static_cast< uint32_t>( pool_sizes.size() );
        create_info.pPoolSizes = pool_sizes.data();
        create_info.maxSets = maxsets;

        VkResult result;
        VkDescriptorPool handle;
        if((result = _device->createDescriptorPool( &create_info, nullptr, &handle )) != VK_SUCCESS)
        {
            logVkError( result, "vulkan-device.hpp", "createDescriptorPoolHandle" );
            return VK_NULL_HANDLE;
        }
        return handle;
    }

    uint32_t DescriptorManager::createDescriptorPool( uint32_t pool_count, const std::vector< kege::DescriptorType >& descriptor_types )
    {
        /**
         the new pool index is necessary for, computing the next pool size, and updating the list array of descriptor-pools
         */
        uint32_t exponent = min<uint32_t>( 5, pool_count );
        uint32_t maxsize = pow(8, exponent);
        uint32_t maxsets = maxsize * MAX_FRAMES_IN_FLIGHT;

        /**
         setup VkDescriptorPoolSizes
         */
        std::vector< VkDescriptorPoolSize > pool_sizes( descriptor_types.size() );
        for (int i=0; i<descriptor_types.size(); ++i)
        {
            pool_sizes[i].type = convertDescriptorType( descriptor_types[i] );
            pool_sizes[i].descriptorCount = maxsets;
        }

        uint32_t id = _descriptor_pools.gen();
        vk::DescriptorPool* pool = _descriptor_pools.get( id );
        pool->handle = createDescriptorPoolHandle( maxsize, pool_sizes );
        pool->individual_free_supported = true;
        pool->allocated_sets = 0;
        pool->max_sets = maxsets;
        pool->owner = -1;
        pool->id = id;
        pool->next = -1;

        return id;
    }

    void DescriptorManager::destroyPipelineLayout(PipelineLayoutHandle handle)
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0) return;
        std::lock_guard<std::mutex> lock( _resource_mutex );

        vk::PipelineLayout* layout = _pipeline_layouts.get( handle.id );
        if ( layout != nullptr )
        {
            _device->destroyPipelineLayout( layout->layout, nullptr );
            layout->descriptor_set_layouts = {};
            layout->layout = VK_NULL_HANDLE;
            layout->desc = {};
            _pipeline_layouts.free( handle.id );
        }
    }

    void DescriptorManager::destroyDescriptorSetLayout(kege::DescriptorSetLayoutHandle handle)
    {
        if ( _descriptor_set_layouts.get( handle.id ) != nullptr )
        {
            vk::DescriptorSetLayout* dsl = _descriptor_set_layouts.get( handle.id );
            _device->destroyDescriptorSetLayout( dsl->layout, nullptr );
            dsl->layout = VK_NULL_HANDLE;
            dsl->bindings = {};
            _descriptor_set_layouts.free( handle.id );
        }
    }

    void DescriptorManager::freeDescriptorSet(kege::DescriptorSetHandle handle)
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0) return;
        std::lock_guard<std::mutex> lock(_resource_mutex);

        if ( _descriptor_sets.get( handle.id ) != nullptr )
        {
            vk::DescriptorSet* set = _descriptor_sets.get( handle.id );
            if ( 0 <= set->pool_id )
            {
                vk::DescriptorPool* pool = _descriptor_pools.get( set->pool_id );

                // Assumes pool was created with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
                _device->freeDescriptorSets( pool->handle, 1, &set->set );
                pool->allocated_sets--;

                vk::DescriptorAllocator* allocator = _descriptor_allocator.get( pool->owner );
                if ( !pool->linked )
                {
                    if ( allocator->head < 0 )
                    {
                        allocator->head = allocator->tail = pool->id;
                    }
                    else
                    {
                        _descriptor_pools.get( pool->id )->next = allocator->head;
                        allocator->head = pool->id;
                    }
                    pool->linked = true;
                }
            }
            _descriptor_sets.free( handle.id );
        }
    }

    int DescriptorManager::getBindingKey( const std::string& name )
    {
        auto i = _descriptor_set_binding_keys.find( name );
        if ( i == _descriptor_set_binding_keys.end() )
        {
            uint32_t bindkey = _descriptor_set_binding_key_counter++;
            _descriptor_set_binding_keys[ name ] = bindkey;
            return bindkey;
        }
        return i->second;
    }

    bool DescriptorManager::initialize( Instance* instance, Device* device )
    {
        if ( _instance != nullptr && _device != nullptr )
        {
            KEGE_LOG_ERROR
            << "calling initialize on an already initialized object in DescriptorManager::initialize()" << Log::nl;
            return true;
        }
        _instance = instance;
        _device = device;
        return true;
    }

    void DescriptorManager::shutdown()
    {
        for ( uint32_t i = 0; i < _pipeline_layouts.count(); ++i )
        {
            if ( _pipeline_layouts.get( i ) != nullptr )
            {
                _device->destroyPipelineLayout( _pipeline_layouts.get( i )->layout, nullptr );
            };
        }
        for ( uint32_t i = 0; i < _descriptor_set_layouts.count(); ++i )
        {
            if ( _descriptor_set_layouts.get( i ) != nullptr )
            {
                _device->destroyDescriptorSetLayout( _descriptor_set_layouts.get( i )->layout, nullptr );
            };
        }
        for ( uint32_t i = 0; i < _descriptor_pools.count(); ++i )
        {
            if ( _descriptor_pools.get( i ) != nullptr )
            {
                _device->destroyDescriptorPool( _descriptor_pools.get( i )->handle, nullptr );
            };
        }
        _descriptor_set_binding_keys.clear();
        _descriptor_allocator_cache.clear();
        _descriptor_set_layout_cache.clear();
        _descriptor_allocator.clear();
        _descriptor_sets.clear();
        _pipeline_layouts.clear();
        _descriptor_set_layouts.clear();
        _descriptor_pools.clear();
        _instance = nullptr;
        _device = nullptr;
    }

    DescriptorManager::DescriptorManager()
    :   _descriptor_set_binding_key_counter( 0 )
    ,   _instance( nullptr )
    ,   _device( nullptr )
    {}

}
