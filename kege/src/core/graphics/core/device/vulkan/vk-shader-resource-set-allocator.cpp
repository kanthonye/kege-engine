//
//  vk-shader-resource-set-allocator.cpp
//  editor
//
//  Created by Kenneth Esdaile on 5/1/26.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-shader-resource-set-allocator.hpp"

namespace kege::vk{

    ShaderResourceSetAllocator::ShaderResourceSetAllocator
    (
        vk::ShaderResourceSetLayout* layout, vk::ShaderResourceSetPool* pool
    )
    :   _layout( layout )
    ,   _pool( pool )
    {
    }

    const kege::ShaderSetDesc& ShaderResourceSetAllocator::getShaderSetDesc()const
    {
        return _layout->getShaderSetDesc();
    }

    kege::Ref< kege::vk::ShaderResourceSet > ShaderResourceSetAllocator::allocate()
    {
        if ( _pool->_descriptor_pools.empty() )
        {
            _pool->createPool();
        }

        // Get the current pool
        kege::vk::DescriptorPool* pool = _pool->getCurrentPool();

        // Allocate from the current pool
        pool->allocated_set_count += 1;

        VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        alloc_info.descriptorPool = pool->handle;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &_layout->handle();

        VkDescriptorSet set;
        VkResult result = _pool->allocateDescriptorSets( &alloc_info, &set );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return {};
        }
        kege::Ref< vk::ShaderResourceSet > resource_set = new vk::ShaderResourceSet
        (
            this, set, pool->id
        );

        return resource_set;
    }

    void ShaderResourceSetAllocator::free( vk::ShaderResourceSet* set )
    {
        _pool->free( set );
    }

    kege::UniformKey ShaderResourceSetAllocator::getUniformKey()const
    {
        return _layout->getUniformKey();
    }

    uint32_t ShaderResourceSetAllocator::getFrameIndex()const
    {
        return _layout->device()->getFrameIndex();
    }

    const VkDevice ShaderResourceSetAllocator::device()const
    {
        return _layout->device()->getVkDevice();
    }

    ShaderResourceSetAllocator::~ShaderResourceSetAllocator()
    {
    }

    ShaderResourceSetAllocator::ShaderResourceSetAllocator()
    :   _pool(nullptr)
    ,   _layout(nullptr)
    {}
}
