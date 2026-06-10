//
//  shader-resource-set.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/28/26.
//

#include "vk-device.hpp"
#include "vk-shader-resource-set.hpp"
#include "vk-shader-resource-set-allocator.hpp"

namespace kege::vk{

    void ShaderResourceSet::setBufferData
    (
        kege::BindingIndex binding_index,
        kege::ArrayIndex array_index,
        size_t offset,
        size_t size,
        const void* data
    )
    {
        kege::Binding& binding = _bindings[ binding_index ];
        binding.bufferAt( array_index ).buffer->copyFrom( data, size, offset );
    }

    void ShaderResourceSet::setBuffer
    (
        kege::BindingIndex binding_index,
        kege::ArrayIndex array_index,
        kege::BindingIndex count,
        const kege::ref::Buffer* buffers
    )
    {
        kege::Binding& binding = _bindings[ binding_index ];
        for (int i = 0; i < count; ++i)
        {
            binding.bufferAt(i + array_index).buffer = buffers[i];
        }
    }

    void ShaderResourceSet::setImage
    (
        kege::BindingIndex binding_index,
        kege::ArrayIndex array_index,
        kege::BindingIndex count,
        const kege::ref::Image* images
    )
    {
        kege::Binding& binding = _bindings[ binding_index ];
        for (int i = 0; i < count; ++i)
        {
            binding.imageAt(i + array_index).image = images[i];
        }
    }

    void ShaderResourceSet::setSampler
    (
        kege::BindingIndex binding_index,
        kege::ArrayIndex array_index,
        kege::BindingIndex count,
        const kege::ref::Sampler* samplers
    )
    {
        kege::Binding& binding = _bindings[ binding_index ];
        for (int i = 0; i < count; ++i)
        {
            binding.imageAt(i + array_index).sampler = samplers[i];
        }
    }

    void ShaderResourceSet::setBinding
    (
        kege::BindingIndex binding_index,
        const kege::Binding& binding
    )
    {
        _bindings[ binding_index ] = binding;
    }

    void ShaderResourceSet::updateBindings( const std::initializer_list< kege::BindingUpdateInfo >& updates )
    {
        const kege::ShaderSetDesc& descs = getShaderSetDesc();
        std::vector< VkDescriptorBufferInfo > buffer_infos;
        std::vector< VkDescriptorImageInfo > image_infos;
        std::vector< VkWriteDescriptorSet > writes;
        std::vector< std::pair< kege::BindType, size_t > > update_entry;

        for (auto info = updates.begin(); info != updates.end(); ++info)
        {
            const kege::ShaderSetBindingDesc& desc = descs[ info->binding_index ];
            kege::Binding& binding = _bindings[info->binding_index];

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = _set;
            write.dstBinding = info->binding_index;
            write.descriptorCount = info->element_count;
            write.dstArrayElement = info->element_index;
            write.descriptorType = vk::toDescriptorType( desc.usage );

            switch ( desc.type )
            {
                case kege::BindType::Buffer:
                {
                    update_entry.push_back({ kege::BindType::Buffer, buffer_infos.size() });
                    for (int i=0; i<info->element_count; ++i)
                    {
                        const auto& b = binding.bufferAt(i + info->element_index);
                        buffer_infos.push_back(VkDescriptorBufferInfo
                        {
                            .buffer = b.buffer->vk()->getHandle(),
                            .offset = b.offset,
                            .range = b.range,
                        });
                    }
                    break;
                }

                case kege::BindType::Image:
                {
                    update_entry.push_back({ kege::BindType::Image, image_infos.size() });
                    for (int i=0; i<info->element_count; ++i)
                    {
                        const auto& a = binding.imageAt(i + info->element_index);
                        image_infos.push_back(VkDescriptorImageInfo
                        {
                            .sampler = (a.sampler)? a.sampler->vk()->handle() : VK_NULL_HANDLE,
                            .imageView = (a.image)? a.image->vk()->_view : VK_NULL_HANDLE,
                            .imageLayout = vk::toImageLayout( a.layout ),
                        });
                    }
                    break;
                }

                default:break;
            }
            writes.push_back( write );
        }

        for (int i = 0; i < writes.size(); ++i)
        {
            VkWriteDescriptorSet& write  = writes[ i ];
            kege::BindType type = update_entry[ i ].first;
            size_t index = update_entry[ i ].second;

            switch ( type )
            {
                case kege::BindType::Buffer: write.pBufferInfo = &buffer_infos[ index ]; break;
                case kege::BindType::Image: write.pImageInfo = &image_infos[ index ]; break;
                default:break;
            }
        }
        
        vkUpdateDescriptorSets
        (
             _allocator->device(),
            static_cast<uint32_t>(writes.size()),
            writes.data(),
            0, nullptr
        );
    }

    const kege::ShaderSetDesc& ShaderResourceSet::getShaderSetDesc()const
    {
        return _allocator->getShaderSetDesc();
    }

    kege::UniformKey ShaderResourceSet::getUniformKey()const
    {
        return _allocator->getUniformKey();
    }

    const VkDescriptorSet& ShaderResourceSet::handle()const
    {
        //int frame_index = _allocator->getFrameIndex();
        return _set;
    }

    ShaderResourceSet:: ~ShaderResourceSet()
    {
        if ( _allocator )
        {
            _allocator->free( this );
            _allocator = nullptr;
        }
    }

    ShaderResourceSet::ShaderResourceSet
    (
        kege::vk::ShaderResourceSetAllocator* allocator,
        VkDescriptorSet set,
        int32_t pool_index
    )
    :   _allocator( allocator )
    ,   _set( set )
    ,   _pool_index( pool_index )
    {
        _bindings.resize( allocator->getShaderSetDesc().size() );
        for (auto& a : allocator->getShaderSetDesc())
        {
            _bindings[ a.binding_index ] = kege::Binding(a.type, a.binding_count);
        }
    }

}
