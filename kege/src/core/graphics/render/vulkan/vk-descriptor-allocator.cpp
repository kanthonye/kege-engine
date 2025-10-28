//
//  vk-descriptor-allocator.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/15/25.
//

#include "vk-device.hpp"
#include "vk-descriptor-allocator.hpp"

namespace kege::vk{

    void DescriptorAllocator::free( DescriptorSet* set )
    {
        if ( set != nullptr && _layout != nullptr )
        {
            if ( set->allocator == this )
            {
                _allocs.remove( set );
                _freed.insert( set );
            }
        }
    }
    
    DescriptorSet* DescriptorAllocator::allocate()
    {
        if ( _descriptor_pools.empty() )
        {
            createPool();
        }

        if ( _freed.head != nullptr )
        {
            return _freed.popFront();
        }

        DescriptorPool* p = _descriptor_pools[ _pools.head ].ref();
        if ( p->allocated_set_count >= p->max_sets )
        {
            p->linked = false;
            _pools.head = p->next_pool;

            if ( _pools.head < 0 )
            {
                _pools.tail = -1;
                if ( !createPool() )
                {
                    return nullptr;
                }
            }
        }
        p->allocated_set_count += 1;

        VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        alloc_info.descriptorPool = p->handle;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &_layout->handle;

        VkDescriptorSet descriptor;
        VkResult result = _manager->allocateDescriptorSets( &alloc_info, &descriptor );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return nullptr;
        }

        return _allocs.insert( new DescriptorSet{ descriptor, this, _pools.head } );
    }

    bool DescriptorAllocator::createPool()
    {
        uint32_t pool_count = static_cast< int >( _descriptor_pools.size() );

        /**
         the new pool index is necessary for, computing the next pool size, and updating the list array of descriptor-pools
         */
        uint32_t exponent = min<uint32_t>( 5, pool_count );
        uint32_t maxsize = pow(8, exponent);
        uint32_t maxsets = maxsize * MAX_FRAMES_IN_FLIGHT;

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

            if( _manager->createDescriptorPool( &create_info, nullptr, &pool->handle ) != VK_SUCCESS )
            {
                kege::Log::error << "createDescriptorPool";
                return false;
            }
        }

        if ( _pools.head < 0 )
        {
            _pools.tail = _pools.head = pool_count;
        }
        else
        {
            pool->next_pool = pool_count;
            _pools.tail = pool_count;
        }
        return true;
    }


    DescriptorAllocator::DescriptorAllocator( vk::Manager* manager, const vk::DescriptorSetLayout* layout )
    :   _manager( manager )
    ,   _layout( layout )
    ,   _pools({ -1, -1 })
    ,   _id( layout->allocator_id )
    {
        _descriptor_types.resize( layout->bindings.size() );
        for ( int i=0; i<layout->bindings.size(); ++i )
        {
            _descriptor_types[i] = layout->bindings[i].descriptorType;
        }
    }

    DescriptorAllocator::~DescriptorAllocator()
    {
        _layout = nullptr;
        _descriptor_types.clear();
        _allocs.clear();
        _freed.clear();

        for ( kege::Ref< DescriptorPool >& pool : _descriptor_pools )
        {
            _manager->destroyDescriptorPool( pool->handle, nullptr );
        }
        _descriptor_pools.clear();
    }
    
}
