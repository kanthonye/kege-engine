//
//  vk-shader-set.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/29/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-shader-set.hpp"
#include "vk-set-layout.hpp"

namespace kege::vk{

    bool ShaderSet::bind(int slot_index, const kege::BufferBindings& buffers)
    {
        BindingSlot& slot = _binding_slots[ slot_index ];
        slot.modifled = true;

        assert(slot.info->type == kege::BindingType::Buffer);
        assert(buffers.size() <= slot.info->count);

        for (size_t i = 0; i < buffers.size(); ++i)
        {
            VkDescriptorBufferInfo& info = _flat_buffers[ slot.offset + i ];
            //info.buffer = buffers[i].buffer->vk()->_sources[0].handle;
            info.offset = buffers[i].offset;
            info.range  = buffers[i].range;
        }
        return true;
    }
    
    bool ShaderSet::bind(int slot_index, const kege::ImageBindings& images)
    {
        BindingSlot& slot = _binding_slots[ slot_index ];
        slot.modifled = true;

        assert(slot.info->type == kege::BindingType::Image);
        assert(images.size() <= slot.info->count);

        for (size_t i = 0; i < images.size(); ++i)
        {
            VkDescriptorImageInfo& info = _flat_images[slot.offset + i];
            info.imageView   = images[i].image->vk()->view();
            info.sampler     = images[i].sampler ? images[i].sampler->vk()->handle() : VK_NULL_HANDLE;
            info.imageLayout = static_cast<VkImageLayout>(images[i].layout);
        }
        return true;
    }

    const VkDescriptorSet& ShaderSet::handle()const
    {
        return _descritor.set[0];
    }

    void ShaderSet::update()
    {
        std::vector<VkWriteDescriptorSet> writes;
        
        for (BindingSlot& slot : _binding_slots)
        {
            if ( slot.info->count == 0 || !slot.modifled ) continue;

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = _descritor.set[0];
            write.dstBinding = slot.info->binding;
            write.descriptorCount = slot.info->count;
            write.descriptorType = vk::toDescriptorType( slot.info->usage );

            switch ( slot.info->type )
            {
                case kege::BindingType::Buffer:
                    write.pBufferInfo = &_flat_buffers[ slot.offset ];
                    break;

                case kege::BindingType::Image:
                    write.pImageInfo = &_flat_images[ slot.offset ];
                    break;

                default: break;
            }
            writes.push_back( write );
            slot.modifled = false;
        }

        vkUpdateDescriptorSets
        (
            _layout->vk()->_device->handle(),
            static_cast<uint32_t>(writes.size()),
            writes.data(),
            0, nullptr
        );
    }

    ShaderSet::ShaderSet( vk::SetLayout* layout, const vk::DescriptorSet& descritor )
    :   kege::ShaderSet( layout )
    ,   _descritor( descritor )
    {
        // Reserve flat arrays based on config metadata
        _flat_buffers.resize( layout->totalBufferCount() );
        _flat_images.resize( layout->totalImageCount() );

        // Initialize binding slots with offsets
        _binding_slots.resize( layout->totalBindingCount() );
        uint32_t buffer_offset = 0;
        uint32_t image_offset  = 0;
        for (uint32_t i = 0; i < _binding_slots.size(); ++i)
        {
            auto& slot = _binding_slots[i];
            slot.info = layout->getBinding( i );
            slot.modifled = true;

            if (slot.info->type == kege::BindingType::Buffer)
            {
                slot.offset = buffer_offset;
                buffer_offset += slot.info->count;
            }
            else if (slot.info->type == kege::BindingType::Image)
            {
                slot.offset = image_offset;
                image_offset += slot.info->count;
            }
        }
    }

    ShaderSet::~ShaderSet()
    {
        if ( _layout )
        {
            _layout->vk()->free( this );
            _layout = nullptr;
        }
    }

}
