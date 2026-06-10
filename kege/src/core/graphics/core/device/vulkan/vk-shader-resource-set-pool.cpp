//
//  kege_vk_shader_resource_set_pool_hpp.cpp
//  KEGE
//
//  Created by Kenneth Esdaile on 10/15/25.
//

#include "vk-device.hpp"
#include "vk-shader-resource-set.hpp"
#include "vk-shader-resource-set-pool.hpp"

namespace kege::vk{

    VkResult ShaderResourceSetPool::allocateDescriptorSets( const VkDescriptorSetAllocateInfo* info, VkDescriptorSet* descriptors )
    {
        return _device->core().allocateDescriptorSets( info, descriptors );
    }

//    kege::Ref< vk::ShaderResourceSet > ShaderResourceSetPool::allocate( vk::ShaderResourceSetLayout* layout )
//    {
//        if ( _descriptor_pools.empty() )
//        {
//            createPool();
//        }
//
//        // Get the current pool
//        DescriptorPool* pool = getPool();
//
//        // Allocate from the current pool
//        pool->allocated_set_count += _frames_in_flight;
//
//        VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
//        alloc_info.descriptorPool = pool->handle;
//        alloc_info.descriptorSetCount = 1; // TODO: should be _frames_in_flight
//        alloc_info.pSetLayouts = &layout->handle();
//
//        VkDescriptorSet set;
//        VkResult result = _device->core().allocateDescriptorSets( &alloc_info, set );
//        if ( result != VK_SUCCESS )
//        {
//            kege::Log::error << vkResultToString( result );
//            return {};
//        }
//        kege::Ref< vk::ShaderResourceSet > resource_set = new vk::ShaderResourceSet
//        (
//         this, set, pool->id, _frames_in_flight
//        );
//        //set->_pool = this;
//        //set->_layout = layout;
//        //set->_frames = _frames_in_flight;
//        //set->_pool_index = pool->id;
//        return resource_set;
//    }

    void ShaderResourceSetPool::free( vk::ShaderResourceSet* set )
    {
        if ( set )
        {
            DescriptorPool* pool = _descriptor_pools[ _pool_manager.head ].ref();
            vkFreeDescriptorSets( _device->handle(), pool->handle, 1, &set->_set );

            if ( !pool->linked )
            {
                pool->linked = true;
                _descriptor_pools[ _pool_manager.tail ]->next_pool = pool->id;
                _pool_manager.tail = pool->id;
            }
        }
    }

    DescriptorPool* ShaderResourceSetPool::getCurrentPool()
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

    bool ShaderResourceSetPool::createPool()
    {
        int frames_in_flight = 1;
        uint32_t pool_count = static_cast< int >( _descriptor_pools.size() );

        /**
         the new pool index is necessary for, computing the next pool size, and updating the list array of descriptor-pools
         */
        uint32_t exponent = min< uint32_t >( 5, pool_count );
        uint32_t maxsize = pow(8, (exponent == 0)? 1: exponent);
        uint32_t maxsets = maxsize * frames_in_flight;

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


    ShaderResourceSetPool::ShaderResourceSetPool( vk::Device* device, const std::vector< VkDescriptorType >& descriptor_types )
    :   _device( device )
    ,   _descriptor_types( descriptor_types )
    ,   _pool_manager({ -1, -1 })
    {
    }

    ShaderResourceSetPool::~ShaderResourceSetPool()
    {
        for ( kege::Ref< DescriptorPool >& pool : _descriptor_pools )
        {
            _device->core().destroyDescriptorPool( pool->handle, nullptr );
        }
        _descriptor_pools.clear();
        _descriptor_types.clear();
    }
    
}
