//
//  vk-shader-binding-set-layout.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/29/25.
//

#ifndef kege_vk_shader_set_hpp
#define kege_vk_shader_set_hpp

#include "vk-utils.hpp"

namespace kege::vk{

    struct BindingSlot
    {
        const kege::LayoutBinding* info = nullptr;
        uint32_t offset = 0;   // offset into flat arrays
        bool modifled = true;
    };

    class ShaderSet final : public kege::ShaderSet
    {
    public:

        bool bind(int frame_index, int binding_index, const kege::BufferBindings& buffers);
        bool bind(int frame_index, int binding_index, const kege::ImageBindings& images);

        const vk::ShaderSet* vk()const { return this; }
        vk::ShaderSet* vk() { return this; }

        const vk::DescriptorSet& descriptor()const;

        void update();

        ShaderSet( vk::SetLayout* layout, const vk::DescriptorSet& descritor );
        ~ShaderSet();

    private:

        // Preallocated flat arrays for all buffers/images
        std::vector< VkDescriptorBufferInfo > _flat_buffers[ MAX_FRAMES_IN_FLIGHT ];
        std::vector< VkDescriptorImageInfo > _flat_images[ MAX_FRAMES_IN_FLIGHT ];
        kege::array< BindingSlot > _binding_slots[ MAX_FRAMES_IN_FLIGHT ];

        vk::DescriptorSet _descritor;

        bool _modified;
        bool _freed;

    private:

        friend vk::List< vk::ShaderSet >;
        vk::ShaderSet* next;
        vk::ShaderSet* prev;

    private:

        friend vk::SetLayout;
        friend vk::Device;
    };

}
#endif /* kege_vk_shader_set_hpp */
