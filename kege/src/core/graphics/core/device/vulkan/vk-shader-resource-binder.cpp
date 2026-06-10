//
//  shader-resource-binder.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/28/26.
//

#include "vk-device.hpp"
#include "vk-shader-resource-binder.hpp"

namespace kege::vk{

    void ShaderResourceBinder::insertSemanticBinding(const std::string& name, kege::GlobalSemantic semantic)
    {
        const kege::ShaderResrcIndexPair* pair = getShaderResrcIndexPair( name );
        if ( pair == nullptr ) return;

        _global_binding_map[ semantic ] = *pair;
    }

    void ShaderResourceBinder::setBuffers
    (
        kege::GlobalSemantic semantic,
        uint32_t element_index,
        uint32_t element_count,
        const kege::ref::Buffer* buffers
    )
    {
        const kege::ShaderResrcIndexPair* pair = getShaderResrcIndexPair( semantic );
        if ( pair == nullptr ) return;
        setBinds(pair, element_index, element_count, buffers);
    }

    void ShaderResourceBinder::setImages
    (
        kege::GlobalSemantic semantic,
        uint32_t element_index,
        uint32_t element_count,
        const kege::ref::Image* images
    )
    {
        const kege::ShaderResrcIndexPair* pair = getShaderResrcIndexPair( semantic );
        if ( pair == nullptr ) return;
        setBinds(pair, element_index, element_count, images);
    }

    void ShaderResourceBinder::setSamplers
    (
        kege::GlobalSemantic semantic,
        uint32_t element_index,
        uint32_t element_count,
        const kege::ref::Sampler* samplers
    )
    {
        const kege::ShaderResrcIndexPair* pair = getShaderResrcIndexPair( semantic );
        if ( pair == nullptr ) return;
        setBinds(pair, element_index, element_count, samplers);
    }

    void ShaderResourceBinder::setBufferData
    (
        const std::string& name,
        kege::ArrayIndex array_index,
        size_t offset,
        size_t size,
        const void* data
    )
    {
        const kege::ShaderResrcIndexPair* pair = getShaderResrcIndexPair( name );
        if ( pair == nullptr ) return;

        kege::ShaderResourceSet* set = _sets[ pair->set ].ref();
        kege::Binding* binding = set->getBinding( pair->binding );
        binding->bufferAt( array_index ).buffer->copyFrom(data, size, offset);
    }

    void ShaderResourceBinder::setBuffers
    (
        const std::string& name,
        uint32_t element_index,
        uint32_t element_count,
        const kege::ref::Buffer* buffers
    )
    {
        const kege::ShaderResrcIndexPair* pair = getShaderResrcIndexPair( name );
        if ( pair == nullptr ) return;
        setBinds(pair, element_index, element_count, buffers);
    }

    void ShaderResourceBinder::setImages
    (
        const std::string& name,
        uint32_t element_index,
        uint32_t element_count,
        const kege::ref::Image* images
    )
    {
        const kege::ShaderResrcIndexPair* pair = getShaderResrcIndexPair( name );
        if ( pair == nullptr ) return;
        setBinds(pair, element_index, element_count, images);
    }

    void ShaderResourceBinder::setSamplers
    (
        const std::string& name,
        uint32_t element_index,
        uint32_t element_count,
        const kege::ref::Sampler* samplers
    )
    {
        const kege::ShaderResrcIndexPair* pair = getShaderResrcIndexPair( name );
        if ( pair == nullptr ) return;
        setBinds(pair, element_index, element_count, samplers);
    }

    void ShaderResourceBinder::setBinding
    (
        const std::string& name,
        const kege::Binding& binding
    )
    {
        const kege::ShaderResrcIndexPair* pair = getShaderResrcIndexPair( name );
        if ( pair == nullptr ) return;

        kege::ShaderResourceSet* set = _sets[ pair->set ].ref();
        kege::Binding* dst = set->getBinding( pair->binding );

        if (*dst == binding)
        {
            return;
        }

        *dst = binding;
        _modified_sets.push_back(ModifiedSet{
            .set_index  = pair->set,
            .array_index = 0,
            .binding_count = binding.count(),
            .binding_index = pair->binding,
        });
    }

    void ShaderResourceBinder::bind(kege::CommandBuffer* cmd, const std::vector< kege::SetIndex >& set_indices) const
    {
        std::vector< VkDescriptorBufferInfo > buffer_infos;
        std::vector< VkDescriptorImageInfo > image_infos;
        std::vector< VkWriteDescriptorSet > writes;

        std::vector< std::pair<kege::BindType, size_t> > accessor;

        for (uint32_t set_index : set_indices )
        {
            auto set = _sets[ set_index ]->vk();
            const kege::ShaderSetDesc& descs = set->getShaderSetDesc();
            for (int index = 0; index < descs.size(); ++index)
            {
                const auto& desc = descs[ index ];
                const auto* binding = set->getBinding( index );

                switch ( desc.type )
                {
                    case kege::BindType::Buffer:
                    {
                        accessor.push_back({ kege::BindType::Buffer, buffer_infos.size() });
                        buffer_infos.push_back(VkDescriptorBufferInfo
                        {
                            .buffer = binding->bufferAt(0).buffer->vk()->getHandle(),
                            .offset = binding->bufferAt(0).offset,
                            .range = binding->bufferAt(0).offset,
                        });
                        break;
                    }

                    case kege::BindType::Image:
                    {
                        accessor.push_back({ kege::BindType::Image, image_infos.size() });
                        const kege::ImageBindInfo& info = binding->imageAt(0);
                        image_infos.push_back(VkDescriptorImageInfo
                        {
                            .sampler = (info.sampler)? info.sampler->vk()->handle() : VK_NULL_HANDLE,
                            .imageView = (info.image)? info.image->vk()->_view : VK_NULL_HANDLE,
                            .imageLayout = vk::toImageLayout( info.layout ),
                        });
                        break;
                    }

                    default:break;
                }

                VkWriteDescriptorSet write{};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.descriptorType = vk::toDescriptorType( desc.usage );
                write.dstSet = set->handle();
                write.dstBinding = desc.binding_index;
                write.descriptorCount = desc.binding_count;
                write.dstArrayElement = 0;
                writes.push_back( write );
            }
        }

        for (uint32_t set_index : set_indices )
        {
            auto access = accessor[ set_index ];
            auto& write = writes[ set_index ];

            switch ( access.first )
            {
                case kege::BindType::Buffer:
                {
                    write.pBufferInfo = &buffer_infos[ access.second ];
                }

                case kege::BindType::Image:
                {
                    write.pImageInfo = &image_infos[ access.second ];
                    break;
                }

                default:break;
            }
        }

        vkUpdateDescriptorSets
        (
            _device->handle(),
            static_cast<uint32_t>(writes.size()),
            writes.data(),
            0, nullptr
        );

        for (auto& set : _sets)
        {
            cmd->vk()->bind( set->getUniformKey(), set->vk()->handle() );
        }
    }

    void ShaderResourceBinder::bind(kege::CommandBuffer* cmd) const
    {
        if ( !_modified_sets.empty() )
        {
            std::vector< VkDescriptorBufferInfo > buffer_infos;
            std::vector< VkDescriptorImageInfo > image_infos;
            std::vector< VkWriteDescriptorSet > writes;

            std::vector< std::pair<kege::BindType, size_t> > accessor;

            for (int index = 0; index < _modified_sets.size(); ++index)
            {
                const auto& modified = _modified_sets[ index ];
                const kege::vk::ShaderResourceSet* set = _sets[ modified.set_index ]->vk();
                const kege::ShaderSetDesc& desc = set->getShaderSetDesc();
                const Binding* binding = set->getBinding( modified.binding_index );

                switch ( binding->type )
                {
                    case kege::BindType::Buffer:
                    {
                        accessor.push_back({ kege::BindType::Buffer, buffer_infos.size() });
                        for (int i=0; i<modified.binding_count; ++i)
                        {
                            const auto& info = binding->bufferAt(i + modified.array_index);
                            buffer_infos.push_back(VkDescriptorBufferInfo
                            {
                                .buffer = info.buffer->vk()->getHandle(),
                                .offset = info.offset,
                                .range = info.range,
                            });
                        }
                        break;
                    }

                    case kege::BindType::Image:
                    {
                        accessor.push_back({ kege::BindType::Image, image_infos.size() });
                        for (int i=0; i<modified.binding_count; ++i)
                        {
                            const auto& info = binding->imageAt(i + modified.array_index);
                            image_infos.push_back(VkDescriptorImageInfo
                            {
                                .sampler = (info.sampler)? info.sampler->vk()->handle() : VK_NULL_HANDLE,
                                .imageView = (info.image)? info.image->vk()->_view : VK_NULL_HANDLE,
                                .imageLayout = vk::toImageLayout( info.layout ),
                            });
                        }
                        break;
                    }

                    default:break;
                }

                VkWriteDescriptorSet write{};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = set->handle();
                write.dstBinding = modified.binding_index;
                write.descriptorCount = modified.binding_count;
                write.dstArrayElement = modified.array_index;
                write.descriptorType = vk::toDescriptorType( desc[ modified.binding_index ].usage );
                writes.push_back( write );
            }

            for (int index = 0; index < _modified_sets.size(); ++index)
            {
                auto access = accessor[ index ];
                auto& write = writes[ index ];

                switch ( access.first )
                {
                    case kege::BindType::Buffer:
                    {
                        write.pBufferInfo = &buffer_infos[ access.second ];
                        break;
                    }

                    case kege::BindType::Image:
                    {
                        write.pImageInfo = &image_infos[ access.second ];
                        break;
                    }

                    default:break;
                }
            }

            vkUpdateDescriptorSets
            (
                _device->handle(),
                static_cast<uint32_t>(writes.size()),
                writes.data(),
                0, nullptr
            );

            _modified_sets.clear();
        }


        for (auto& set : _sets)
        {
            cmd->vk()->bind( set->getUniformKey(), set->vk()->handle() );
        }
    }

    const kege::ShaderResrcIndexPair* ShaderResourceBinder::getShaderResrcIndexPair
    (
        kege::GlobalSemantic semantic
    )   const
    {
        auto i = _global_binding_map.find( semantic );
        if ( i == _global_binding_map.end())
        {
            kege::Log::error << "ShaderResourceBinder -> " << (uint64_t) this <<" does not have ";
            kege::Log::error << "GlobalSemantic -> " << toString( semantic ) << kege::Log::nl;
            return nullptr;
        }
        return &i->second;
    }


    const kege::ShaderResrcIndexPair* ShaderResourceBinder::getShaderResrcIndexPair
    (
        const std::string& name
    )   const
    {
        auto i = _binding_map.find( name );
        if ( i == _binding_map.end())
        {
            kege::Log::error << "ShaderResourceBinder -> " << (uint64_t) this <<" does not contain ";
            kege::Log::error << "binding -> " << name << kege::Log::nl;
            return nullptr;
        }
        return &i->second;
    }

    void ShaderResourceBinder::setBinds
    (
        const kege::ShaderResrcIndexPair* pair,
        uint32_t element_index,
        uint32_t element_count,
        const kege::ref::Buffer* buffers
    )
    {
        kege::ref::ShaderResourceSet& set = _sets[ pair->set ];
        kege::Binding* binding = set->getBinding( pair->binding );

        for (uint32_t i = 0; i < element_count; ++i)
        {
            binding->bufferAt( i + element_index ).buffer = buffers[ i ];
        }

        _modified_sets.push_back(ModifiedSet{
            .set_index  = pair->set,
            .array_index = element_index,
            .binding_count = element_count,
            .binding_index = pair->binding,
        });
    }

    void ShaderResourceBinder::setBinds
    (
        const kege::ShaderResrcIndexPair* pair,
        uint32_t element_index,
        uint32_t element_count,
        const kege::ref::Image* images
    )
    {
        kege::ref::ShaderResourceSet& set = _sets[ pair->set ];
        kege::Binding* binding = set->getBinding( pair->binding );

        for (uint32_t i = 0; i < element_count; ++i)
        {
            binding->imageAt( i + element_index ).image = images[ i ];
        }

        _modified_sets.push_back(ModifiedSet{
            .set_index  = pair->set,
            .array_index = element_index,
            .binding_count = element_count,
            .binding_index = pair->binding,
        });
    }

    void ShaderResourceBinder::setBinds
    (
        const kege::ShaderResrcIndexPair* pair,
        uint32_t element_index,
        uint32_t element_count,
        const kege::ref::Sampler* samplers
    )
    {
        kege::ref::ShaderResourceSet& set = _sets[ pair->set ];
        kege::Binding* binding = set->getBinding( pair->binding );

        for (uint32_t i = 0; i < element_count; ++i)
        {
            binding->imageAt( i + element_index ).sampler = samplers[ i ];
        }

        _modified_sets.push_back(ModifiedSet{
            .set_index  = pair->set,
            .array_index = element_index,
            .binding_count = element_count,
            .binding_index = pair->binding,
        });
    }

    ShaderResourceBinder::ShaderResourceBinder
    (
        vk::Device* device,
        const std::vector< kege::ref::ShaderResourceSet >& sets
    )
    :   _device( device )
    {
        _sets.resize( sets.size() );
        for (int set_index = 0; set_index < sets.size(); ++set_index )
        {
            _sets[ set_index ] = sets[ set_index];
            const kege::ShaderSetDesc& descs = sets[ set_index ]->getShaderSetDesc();

            // map name to set binding pair
            for (int j = 0; j < descs.size(); ++j )
            {
                const kege::ShaderSetBindingDesc& desc = descs[j];

                kege::ShaderResrcIndexPair& index = _binding_map[ desc.name ];
                index.binding = desc.binding_index;
                index.set = set_index;
            }
        }
    }

    ShaderResourceBinder::~ShaderResourceBinder()
    {
        _global_binding_map.clear();
        _binding_map.clear();
        _modified_sets.clear();
        _sets.clear();

        if ( _device )
        {
            _device->destroyShaderResourceBinder( this );
            _sets.clear();
        }
    }
}
