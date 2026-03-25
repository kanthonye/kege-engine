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

    bool ShaderSet::bind(int frame_index, int slot_index, const kege::BufferBindings& buffers)
    {
        BindingSlot& slot = _binding_slots[ frame_index ][ slot_index ];
        slot.modifled = true;
        _modified = true;

        assert(slot.info->type == kege::BindType::Buffer);
        assert(buffers.size() <= slot.info->count);

        for (size_t i = 0; i < buffers.size(); ++i)
        {
            VkDescriptorBufferInfo& info = _flat_buffers[ frame_index ][ slot.offset + i ];
            info.buffer = buffers[i].buffer->vk()->getCurrHandle();
            info.offset = buffers[i].offset;
            info.range  = buffers[i].range;
        }
        return true;
    }
    
    bool ShaderSet::bind(int frame_index, int slot_index, const kege::ImageBindings& images)
    {
        BindingSlot& slot = _binding_slots[ frame_index ][ slot_index ];
        slot.modifled = true;
        _modified = true;

        assert(slot.info->type == kege::BindType::Image);
        assert(images.size() <= slot.info->count);

        for (size_t i = 0; i < images.size(); ++i)
        {
            VkDescriptorImageInfo& info = _flat_images[ frame_index ][slot.offset + i];
            info.imageView   = images[i].image->vk()->view();
            info.sampler     = images[i].sampler ? images[i].sampler->vk()->handle() : VK_NULL_HANDLE;
            info.imageLayout = convertImageLayout(images[i].layout);

            if( info.imageLayout == VK_IMAGE_LAYOUT_UNDEFINED )
            {
                info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
        }
        return true;
    }

    const vk::DescriptorSet& ShaderSet::descriptor()const
    {
        return _descritor;
    }

    void ShaderSet::update()
    {
        if( !_modified ) return;
        _modified = false;

        vk::Device* device = _layout->vk()->_device;
        const int frame = device->getFrameIndex() % _descritor.frames;

        kege::array< BindingSlot >& slots = _binding_slots[ frame ];
        VkDescriptorSet set = _descritor.set[  frame  ];

        std::vector<VkWriteDescriptorSet> writes;
        for (int f=0; f < slots.size(); ++f )
        {
            BindingSlot& slot = slots[f];
            if ( slot.info->count == 0 || !slot.modifled ) continue;

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = set;
            write.dstBinding = slot.info->index;
            write.descriptorCount = slot.info->count;
            write.descriptorType = vk::toDescriptorType( slot.info->usage );

            switch ( slot.info->type )
            {
                case kege::BindType::Buffer:
                    write.pBufferInfo = &_flat_buffers[  frame  ][ slot.offset ];
                    break;

                case kege::BindType::Image:
                    write.pImageInfo = &_flat_images[  frame  ][ slot.offset ];
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

        // Initialize binding slots with offsets
        uint32_t buffer_offset = 0;
        uint32_t image_offset  = 0;
        for (uint32_t f = 0; f < descritor.frames; ++f)
        {
            _binding_slots[f].resize( layout->totalBindingCount() );
            _flat_buffers[f].resize( layout->totalBufferCount() );
            _flat_images[f].resize( layout->totalImageCount() );

            for (uint32_t i = 0; i < _binding_slots[f].size(); ++i)
            {
                auto& slot = _binding_slots[f][i];
                slot.info = layout->getBinding(i);
                slot.modifled = true;

                if (slot.info->type == kege::BindType::Buffer)
                {
                    slot.offset = buffer_offset;
                    buffer_offset += slot.info->count;
                }
                else if (slot.info->type == kege::BindType::Image)
                {
                    slot.offset = image_offset;
                    image_offset += slot.info->count;
                }
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
