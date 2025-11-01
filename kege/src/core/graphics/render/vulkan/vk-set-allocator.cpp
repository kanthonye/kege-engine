//
//  vk-set-allocator.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/15/25.
//

#include "vk-device.hpp"
#include "vk-set-layout.hpp"
#include "vk-set-allocator.hpp"

namespace kege::vk{

    vk::DescriptorSet SetAllocator::allocate( vk::SetLayout* layout )
    {
        if ( _descriptor_pools.empty() )
        {
            createPool();
        }

        // Get the current pool
        DescriptorPool* pool = getPool();

        // Allocate from the current pool
        pool->allocated_set_count += _frames_in_flight;

        VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        alloc_info.descriptorPool = pool->handle;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &layout->handle();

        DescriptorSet descriptor;
        VkResult result = _device->core().allocateDescriptorSets( &alloc_info, descriptor.set );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return {};
        }
        descriptor.frames = _frames_in_flight;
        descriptor.pool = pool->id;
        return descriptor;
    }

    void SetAllocator::free( DescriptorSet& set )
    {
        if ( set.frames )
        {
            DescriptorPool* pool = _descriptor_pools[ _pool_manager.head ].ref();
            vkFreeDescriptorSets( _device->handle(), pool->handle, set.frames, set.set );
            set.frames = 0;

            if ( !pool->linked )
            {
                pool->linked = true;
                _descriptor_pools[ _pool_manager.tail ]->next_pool = pool->id;
                _pool_manager.tail = pool->id;
            }
        }
    }

    DescriptorPool* SetAllocator::getPool()
    {
        DescriptorPool* pool = _descriptor_pools[ _pool_manager.head ].ref();

        // Check if the current pool is full
        if ( pool->allocated_set_count >= pool->max_sets )
        {
            // unlink the pull pool from list and
            // Move to the next pool in the list
            pool->linked = false;
            _pool_manager.head = pool->next_pool;

            // If there is no next pool, create a new one
            if ( _pool_manager.head < 0 )
            {
                _pool_manager.tail = -1;
                if ( !createPool() )
                {
                    return {};
                }
            }
            pool = _descriptor_pools[ _pool_manager.head ].ref();
        }
        return pool;
    }

    bool SetAllocator::createPool()
    {
        uint32_t pool_count = static_cast< int >( _descriptor_pools.size() );

        /**
         the new pool index is necessary for, computing the next pool size, and updating the list array of descriptor-pools
         */
        uint32_t exponent = min< uint32_t >( 5, pool_count );
        uint32_t maxsize = pow(8, exponent);
        uint32_t maxsets = maxsize * _frames_in_flight;

        /**
         setup VkDescriptorPoolSizes
         */
        std::vector< VkDescriptorPoolSize > pool_sizes( _descriptor_types.size() );
        for (int i=0; i<_descriptor_types.size(); ++i)
        {
            pool_sizes[i].type = _descriptor_types[i];
            pool_sizes[i].descriptorCount = maxsets;
        }

        _descriptor_pools.push_back( new DescriptorPool{} );

        kege::Ref< DescriptorPool >& pool = _descriptor_pools[ pool_count ];
        pool->allocated_set_count = 0;
        pool->max_sets = maxsets;
        pool->id = pool_count;
        pool->next_pool = -1;
        pool->linked = true;
        {
            VkDescriptorPoolCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            create_info.poolSizeCount = static_cast< uint32_t>( pool_sizes.size() );
            create_info.pPoolSizes = pool_sizes.data();
            create_info.maxSets = maxsets;

            if( _device->core().createDescriptorPool( &create_info, nullptr, &pool->handle ) != VK_SUCCESS )
            {
                kege::Log::error << "createDescriptorPool";
                return false;
            }
        }

        if ( _pool_manager.head < 0 )
        {
            _pool_manager.tail = _pool_manager.head = pool_count;
        }
        else
        {
            pool->next_pool = pool_count;
            _pool_manager.tail = pool_count;
        }
        return true;
    }


    SetAllocator::SetAllocator( vk::Device* device, const std::vector< VkDescriptorType >& descriptor_types )
    :   _device( device )
    ,   _descriptor_types( descriptor_types )
    ,   _pool_manager({ -1, -1 })
    ,   _frames_in_flight( 1 )
    {
    }

    SetAllocator::~SetAllocator()
    {
        _descriptor_types.clear();
        for ( kege::Ref< DescriptorPool >& pool : _descriptor_pools )
        {
            _device->core().destroyDescriptorPool( pool->handle, nullptr );
        }
        _descriptor_pools.clear();
    }
    
}
