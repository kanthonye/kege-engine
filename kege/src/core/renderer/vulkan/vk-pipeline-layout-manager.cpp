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

    //-------------------------------------------------------------------------
    // Pipeline Layout Lifecycle
    //-------------------------------------------------------------------------

    const vk::PipelineLayout* PipelineLayoutManager::getPipelineLayout( int32_t pipeline_layout_id )const
    {
        return _pipeline_layouts.get( pipeline_layout_id );
    }

    int32_t PipelineLayoutManager::createPipelineLayout( const char* name, const UniformSetsDesc& layouts, const std::vector< PushConstantInfo >& push_constants )
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

        int binding_index = 0;
        std::vector< VkDescriptorSetLayout > vk_descriptor_set_layouts;
        vk_descriptor_set_layouts.reserve( layouts.size() );
        for (const auto& layout : layouts)
        {
            /**
             * @brief Get the descriptor set layout from the cache.
             * If the descriptor set layout is not found, log an error and return an invalid handle
             */
            int layout_id = createUniformSetLayout( layout );
            const vk::DescriptorSetLayout* dsl = getDescriptorSetLayout( layout_id );
            if ( dsl == nullptr )
            {
                KEGE_LOG_ERROR << "Invalid kege::UniformSetLayout provided!";
                 return -1;
            }

            /**
             * @brief Add the descriptor set layout to the list of vk_descriptor_set_layouts.
             * This is necessary for creating the vk pipeline layout handle
             */
            vk_descriptor_set_layouts.push_back( dsl->handle );

            /**
             * @brief If the descriptor set layout already has a binding location.
             * assign a binding index to that location.
             */
            pipeline_layout->descriptor_set_index_map[ dsl->resource_index ] = binding_index;
            Log::info << dsl->name << " [binding_location: " <<dsl->resource_index <<"] [ binding_index: " << binding_index <<"]" << Log::nl;
            binding_index += 1;
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
        if (( result = _device->createPipelineLayout( &info, nullptr, &pipeline_layout->layout ) ) != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return -1;
        }

        if ( _instance->isValidationEnabled() && name != nullptr )
        {
            _device->debugSetObjectName( (uint64_t)pipeline_layout->layout, VK_OBJECT_TYPE_IMAGE, name );
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
            _device->destroyPipelineLayout( layout->layout, nullptr );
            layout->layout = VK_NULL_HANDLE;
            _pipeline_layouts.free( pipeline_layout_id );
        }
    }

    //-------------------------------------------------------------------------
    // Descriptor Set Layout Lifecycle
    //-------------------------------------------------------------------------

    int32_t PipelineLayoutManager::getDescriptorSetLayoutID( const UniformSetDesc& descriptors, bool create )
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

    //TODO: createUniformSetLayouts( const UniformSetsDesc& descriptors )
    std::vector< int > PipelineLayoutManager::createUniformSetLayouts( const UniformSetsDesc& descriptors )
    {
        return {};
    }
    
    int32_t PipelineLayoutManager::createUniformSetLayout( const UniformSetDesc& descriptors )
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
        for ( const kege::UniformDesc& desc : descriptors )
        {
            VkDescriptorSetLayoutBinding dslb = {};
            dslb.binding = desc.binding;
            dslb.descriptorCount = desc.count;
            dslb.descriptorType = vk::convertDescriptorType( desc.descriptor_type );
            dslb.stageFlags = vk::convertShaderStageMask( desc.stage_flags );
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
        VkResult result = _device->createDescriptorSetLayout( &create_info, nullptr, &layout );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return -1;
        }

        if ( _instance->isValidationEnabled() && !name.empty() )
        {
            _device->debugSetObjectName( (uint64_t)layout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, name.c_str() );
        }

        // ... Store handle ...
        int32_t id = _descriptor_set_layouts.gen();
        vk::DescriptorSetLayout* dsl = _descriptor_set_layouts.get( id );
        if ( dsl == nullptr )
        {
            KEGE_LOG_ERROR << "Failed to create UniformSetLayout!";
            return -1;
        }

        /**
         * @brief Associate the descriptor set layout binding info to the descriptor set layout id.
         * This allows us to quickly retrieve the descriptor set layout by its bindings.
         * The bindings are hashed to create a unique key for the descriptor set layout.
         * This is useful for caching and reusing descriptor set layouts.
         */
        _descriptor_set_layout_indexmap[ descriptors ] = id;

        dsl->bindings = bindings;

        /**
         * @brief Assign a binding location to the descriptor set layout.
         * The binding location is an index associated with the descriptor set layout's name.
         */
        dsl->resource_index = generateResourceBindingIndex( descriptors );

        /**
         * @brief Set the allocator_id to -1, indicating that this descriptor set layout is not yet assigned
         * a descriptot set allocator.
         */
        dsl->allocator_id = createDescriptorSetAllocator( descriptors );

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
         * @brief Assign the descriptor set layout its id.
         * This id is used to uniquely identify the descriptor set layout in the descriptor set layout cache.
         */
        dsl->id = id;
        return id;
    }

    const vk::DescriptorSetLayout* PipelineLayoutManager::getDescriptorSetLayout( int32_t handle )const
    {
        return _descriptor_set_layouts.get( handle );
    }

    void PipelineLayoutManager::destroyDescriptorSetLayout( int32_t handle )
    {
        if ( _descriptor_set_layouts.get( handle ) != nullptr )
        {
            vk::DescriptorSetLayout* dsl = _descriptor_set_layouts.get( handle );
            _device->destroyUniformSetLayout( dsl->handle, nullptr );
            dsl->handle = VK_NULL_HANDLE;
            dsl->bindings = {};
            _descriptor_set_layouts.free( handle );
        }
    }

    //-------------------------------------------------------------------------
    // Descriptor Set Lifecycle
    //-------------------------------------------------------------------------

    bool PipelineLayoutManager::updateUniformSets( const std::vector< int >& handles, const UniformSets& resource_sets )
    {
        int buffer_count = 0, image_count = 0, write_count = 0;;
        for (const auto& resource_set : resource_sets )
        {
            write_count += resource_set.size();
            for (const auto& set : resource_set )
            {
                switch ( set.uniform.type )
                {
                    case Uniform::BUFFER:
                    case Uniform::BUFFER_VIEW:
                    {
                        buffer_count += set.uniform.count();
                        break;
                    }
                    case Uniform::IMAGE:
                    {
                        image_count += set.uniform.count();
                        break;
                    }
                    case Uniform::INVALID:
                    {
                        kege::Log::error << "INVALID Uniform!" << kege::Log::nl;
                        return false;
                    }
                }
            }
        }

        std::vector< VkDescriptorBufferInfo > buffer_infos;
        buffer_infos.reserve( buffer_count ); // Rough estimate
        
        std::vector< VkDescriptorImageInfo > image_infos;
        image_infos.reserve( image_count ); // Rough estimate

        std::vector< VkWriteDescriptorSet > descriptor_writes;
        descriptor_writes.reserve( write_count );

        for ( int set_index=0; set_index<resource_sets.size(); ++set_index )
        {
            DescriptorSet* descriptor = _descriptor_sets.get( handles[ set_index ] );
            if (descriptor == nullptr)
            {
                KEGE_LOG_WARN << "Invalid kege::ShaderResource in updateDescriptorSets!";
                return false;
            }
            DescriptorSetLayout* dsl = _descriptor_set_layouts.get( descriptor->layout_id );

            const UniformSet& resource_set = resource_sets[ set_index ];
            for ( int i=0; i<resource_set.size(); ++i )
            {
                VkDescriptorType descriptor_type = dsl->bindings[ resource_set[i].binding ].descriptorType;

                VkWriteDescriptorSet descriptor_write = {};
                descriptor_write.dstSet = descriptor->set;

                bool state = writeDescriptor
                (
                    resource_set[i],
                    descriptor_type,
                    buffer_infos,
                    image_infos,
                    &descriptor_write
                );
                if( !state || descriptor_write.descriptorCount <= 0 )
                {
                    return false;
                }

                descriptor_writes.push_back( descriptor_write );
            }
        }

        if ( !descriptor_writes.empty() )
        {
            write_count = static_cast< uint32_t >( descriptor_writes.size() );
            _device->updateDescriptorSets( write_count, descriptor_writes.data(), 0, nullptr );
        }

        return true;
    }

    std::vector< int > PipelineLayoutManager::allocateUniformSets( const UniformSetsDesc& desc )
    {
        std::vector< int > sets( desc.size() );
        for( int i = 0; i < desc.size(); ++i )
        {
            sets[ i ] = allocateUniformSet( desc[i] );
        }
        return sets;
    }

    bool PipelineLayoutManager::updateUniformSet( int handle, const UniformSet& resource_set )
    {
        DescriptorSet* descriptor = _descriptor_sets.get( handle );
        if (descriptor == nullptr)
        {
            KEGE_LOG_WARN << "Invalid kege::ShaderResource in updateDescriptorSets!";
            return false;
        }
        DescriptorSetLayout* dsl = _descriptor_set_layouts.get( descriptor->layout_id );

        int buffer_count = 0, image_count = 0;
        for (const auto& set : resource_set )
        {
            switch ( set.uniform.type )
            {
                case Uniform::BUFFER:
                case Uniform::BUFFER_VIEW:
                {
                    buffer_count += set.uniform.count();
                    break;
                }
                case Uniform::IMAGE:
                {
                    image_count += set.uniform.count();
                    break;
                }
                case Uniform::INVALID:
                {
                    kege::Log::error << "INVALID Uniform!" << kege::Log::nl;
                    return false;
                }
            }
        }

        std::vector< VkDescriptorBufferInfo > buffer_infos;
        std::vector< VkDescriptorImageInfo > image_infos;
        buffer_infos.reserve( buffer_count ); // Rough estimate
        image_infos.reserve( image_count ); // Rough estimate

        std::vector< VkWriteDescriptorSet > descriptor_writes;
        descriptor_writes.reserve( resource_set.size() );

        //int starting_index;
        for ( int i=0; i<resource_set.size(); ++i )
        {
            VkDescriptorType descriptor_type = dsl->bindings[ resource_set[i].binding ].descriptorType;

            VkWriteDescriptorSet descriptor_write = {};
            descriptor_write.dstSet = descriptor->set;

            bool state = writeDescriptor
            (
                resource_set[i],
                descriptor_type,
                buffer_infos,
                image_infos,
                &descriptor_write
            );
            if( !state || descriptor_write.descriptorCount <= 0 )
            {
                return false;
            }

            descriptor_writes.push_back( descriptor_write );
        }

        if ( !descriptor_writes.empty() )
        {
            _device->updateDescriptorSets( static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr );
        }
        return true;
    }

    int PipelineLayoutManager::allocateUniformSet( const UniformSetDesc& desc )
    {
        int32_t handle = -1;
        int layout_id = getDescriptorSetLayoutID( desc );
        if ( !allocateDescriptors( layout_id, 1, &handle ) )
        {
            kege::Log::error << "fail to create shader resource -> " <<desc[0].name <<kege::Log::nl;
            freeUniformSet( handle );
            return -1;
        }
        return handle;
    }


    const vk::DescriptorSet* PipelineLayoutManager::getDescriptorSet( int32_t set )const
    {
        return _descriptor_sets.get( set );
    }

    bool PipelineLayoutManager::allocateDescriptors
    (
        int32_t layout_id,
        int32_t quantity,
        int32_t* sets
    )
    {
        if ( layout_id < 0 )
        {
            KEGE_LOG_ERROR << "Invalid UniformSetLayout parameter" << Log::nl;
            return {};
        }

        vk::DescriptorSetLayout* layout = _descriptor_set_layouts.get( layout_id );
        vk::DescriptorAllocator* allocator = _descriptor_allocators.get( layout->allocator_id );

        if ( allocator->descriptor_pools.empty() )
        {
            createDescriptorPool( allocator );
        }
        else if
        (
            allocator->descriptor_pools[ allocator->pool.head ].allocated_set_count >=
            allocator->descriptor_pools[ allocator->pool.head ].max_sets
        )
        {
            allocator->descriptor_pools[ allocator->pool.head ].linked = false;
            allocator->pool.head = allocator->descriptor_pools[ allocator->pool.head ].next_pool;

            if ( allocator->pool.head < 0 )
            {
                allocator->pool.tail = -1;
                if ( !createDescriptorPool( allocator ) )
                {
                    return false;
                }
            }
        }

        int count=0;
        if ( 0 <= allocator->descriptors.head )
        {
            for (; count<quantity; ++count )
            {
                vk::DescriptorSet* descriptor = _descriptor_sets.get( allocator->descriptors.head );
                descriptor->freed = false;
                descriptor->next = -1;

                sets[count] = allocator->descriptors.head;
                allocator->descriptors.head = descriptor->next;
                if ( allocator->descriptors.head < 0 )allocator->descriptors.tail = -1;
            }

            if ( count == quantity )
            {
                return true;
            }
            quantity -= count;
        }

        /**
         5. Note: For each descriptot set we allocate we need a descriptor_set_layout handle.
         */
        std::vector< VkDescriptorSetLayout > descriptor_set_layouts( quantity, layout->handle );

        /**
         6. Allocate a descriptot sets from the current descriptor pool.
         */
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = allocator->descriptor_pools[ allocator->pool.head ].handle;
        alloc_info.descriptorSetCount = (uint32_t) descriptor_set_layouts.size();
        alloc_info.pSetLayouts = descriptor_set_layouts.data();

        VkResult result;
        VkDescriptorSet descriptor_set[ quantity ];
        if (( result = _device->allocateDescriptorSets( &alloc_info, descriptor_set ) ) != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return false;
        }

        allocator->descriptor_pools[ allocator->pool.head ].allocated_set_count += 1;

        /**
         7. With the descriptor set allocated. store its info into a new descriptor set object.
         */
        for (int i=count; i<quantity; ++i )
        {
            sets[i] = _descriptor_sets.gen();
            vk::DescriptorSet* descriptor = _descriptor_sets.get( sets[i] );
            descriptor->pool_id = allocator->pool.head;
            descriptor->allocator_id = allocator->id;
            descriptor->layout_id = layout_id;
            descriptor->set = descriptor_set[i];
            descriptor->freed = false;
            descriptor->next = -1;
        }

        return true;
    }

    void PipelineLayoutManager::freeUniformSet( int32_t descriptor_id )
    {
        if ( _device == VK_NULL_HANDLE || descriptor_id == 0) return;
        std::lock_guard<std::mutex> lock(_resource_mutex);

        if ( _descriptor_sets.get( descriptor_id ) != nullptr )
        {
            vk::DescriptorSet* set = _descriptor_sets.get( descriptor_id );
            if ( 0 <= set->allocator_id && 0 <= set->pool_id && !set->freed )
            {
                vk::DescriptorAllocator* allocator = _descriptor_allocators.get( set->allocator_id );
                vk::DescriptorPool& pool = allocator->descriptor_pools[ set->pool_id ];

                // Assumes pool was created with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
                //_device->freeDescriptorSets( pool.handle, 1, &set->set );
;
                if ( !pool.linked )
                {
                    if ( allocator->pool.head < 0 )
                    {
                        allocator->pool.head = allocator->pool.tail = set->pool_id;
                    }
                    else
                    {
                        allocator->descriptor_pools[ allocator->pool.tail ].next_pool = set->pool_id;
                        allocator->pool.head = set->pool_id;
                    }
                    pool.linked = true;
                }

                if ( allocator->descriptors.head < 0 )
                {
                    allocator->descriptors.head = allocator->descriptors.tail = descriptor_id;
                }
                else
                {
                    set->next = allocator->pool.tail;
                    allocator->pool.head = descriptor_id;
                }

                pool.allocated_set_count--;
                set->freed = true;
            }
            //_descriptor_sets.free( handle.id );
        }
    }

    //-------------------------------------------------------------------------
    // Helper Functions
    //-------------------------------------------------------------------------

    int PipelineLayoutManager::generateResourceBindingIndex( const UniformSetDesc& description )
    {
        auto m = _resource_index_map.find( description );
        if ( m == _resource_index_map.end() )
        {
            _resource_index_map[ description ] = _resource_index_counter;
            return _resource_index_counter++;
        }
        return m->second;
    }

    int PipelineLayoutManager::createDescriptorSetAllocator( const UniformSetDesc& description )
    {
        auto m = _descriptor_allocator_indexmap.find( description );
        if ( m == _descriptor_allocator_indexmap.end() )
        {
            int32_t index = _descriptor_allocators.gen();
            _descriptor_allocator_indexmap[ description ] = index;
            {
                DescriptorAllocator* allocator = _descriptor_allocators.get( index );
                allocator->descriptor_types.resize( description.size() );
                for ( int i=0; i<description.size(); ++i )
                {
                    allocator->descriptor_types[i] = vk::convertDescriptorType( description[i].descriptor_type );
                }
                allocator->descriptors.head = -1;
                allocator->descriptors.tail = -1;
                allocator->pool.head = -1;
                allocator->pool.tail = -1;
                allocator->id = index;
            }
            return index;
        }
        return m->second;
    }

    bool PipelineLayoutManager::createDescriptorPool( vk::DescriptorAllocator* allocator )
    {
        uint32_t pool_count = static_cast< int >( allocator->descriptor_pools.size() );

        /**
         the new pool index is necessary for, computing the next pool size, and updating the list array of descriptor-pools
         */
        uint32_t exponent = min<uint32_t>( 5, pool_count );
        uint32_t maxsize = pow(8, exponent);
        uint32_t maxsets = maxsize * MAX_FRAMES_IN_FLIGHT;

        /**
         setup VkDescriptorPoolSizes
         */
        std::vector< VkDescriptorPoolSize > pool_sizes( allocator->descriptor_types.size() );
        for (int i=0; i<allocator->descriptor_types.size(); ++i)
        {
            pool_sizes[i].type = allocator->descriptor_types[i];
            pool_sizes[i].descriptorCount = maxsets;
        }

        allocator->descriptor_pools.push_back({});

        DescriptorPool& pool = allocator->descriptor_pools[ pool_count ];
        pool.allocated_set_count = 0;
        pool.max_sets = maxsets;
        pool.id = pool_count;
        pool.next_pool = -1;
        pool.linked = true;
        {
            VkDescriptorPoolCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            create_info.poolSizeCount = static_cast< uint32_t>( pool_sizes.size() );
            create_info.pPoolSizes = pool_sizes.data();
            create_info.maxSets = maxsets;

            if( _device->createDescriptorPool( &create_info, nullptr, &pool.handle ) != VK_SUCCESS )
            {
                kege::Log::error << "createDescriptorPool";
                return false;
            }
        }

        if ( allocator->pool.head < 0 )
        {
            allocator->pool.tail = allocator->pool.head = pool_count;
        }
        else
        {
            allocator->descriptor_pools[ allocator->pool.tail ].next_pool = pool_count;
            allocator->pool.tail = pool_count;
        }
        return true;
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
            const vk::Sampler* sampler = _device->getSampler( info.sampler );
            const vk::Image* image = _device->getImage( info.image );
            if(image != nullptr && sampler != nullptr)
            {
                descriptor_image_infos.push_back
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
            const vk::Image* image = _device->getImage( info.image );
            if( image != nullptr )
            {
                descriptor_image_infos.push_back
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
            const vk::Sampler* sampler = _device->getSampler( info.sampler );
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
            const vk::Sampler* sampler = _device->getSampler( info.sampler );
            const vk::Image* image = _device->getImage( info.image );
            if(image != nullptr && sampler != nullptr)
            {
                descriptor_image_infos.push_back
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
            const vk::Buffer* buffer = _device->getBuffer( info.buffer );
            if (buffer != nullptr)
            {
                descriptor_buffer_infos.push_back({ buffer->buffer, info.offset, info.range });
            }
            else
            {
                switch ( descriptor_type )
                {
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                        KEGE_LOG_ERROR
                        << "Invalid BufferHandle for DescriptorType::UniformBuffer in updateDescriptorSets!";
                        break;

                    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                        KEGE_LOG_ERROR
                        << "Invalid BufferHandle for DescriptorType::StorageBuffer in updateDescriptorSets!";
                        break;

                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                        KEGE_LOG_ERROR
                        << "Invalid BufferHandle for DescriptorType::UniformBufferDynamic in updateDescriptorSets!";
                        break;

                    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
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

        descriptor_write->descriptorCount = static_cast< uint32_t >( buffer_info.size() );
        descriptor_write->pBufferInfo = &descriptor_buffer_infos[ starting_index ];
        return true;
    }

    bool PipelineLayoutManager::writeDescriptor
    (
        const UniformBinding& element,
        VkDescriptorType descriptor_type,
        std::vector< VkDescriptorBufferInfo >& buffer_infos,
        std::vector< VkDescriptorImageInfo >& image_infos,
        VkWriteDescriptorSet* descriptor_write
    )
    {
        descriptor_write->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write->descriptorType = descriptor_type;
        descriptor_write->dstBinding = element.binding;
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
                    element.uniform.buffers,
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
                    element.uniform.images,
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
                    element.uniform.images,
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
                    element.uniform.images,
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
                    element.uniform.images,
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
                _device->destroyUniformSetLayout( _descriptor_set_layouts.get( i )->handle, nullptr );
            };
        }
        for ( uint32_t i = 0; i < _descriptor_allocators.count(); ++i )
        {
            if ( _descriptor_allocators.get( i ) != nullptr )
            {
                for ( uint32_t j = 0; j < _descriptor_allocators.get(i)->descriptor_pools.size(); ++j )
                {
                    _device->destroyDescriptorPool( _descriptor_allocators.get(i)->descriptor_pools[j].handle, nullptr );
                }
            };
        }
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
