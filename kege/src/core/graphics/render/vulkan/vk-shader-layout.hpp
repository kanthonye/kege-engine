//
//  vk-shader-layout.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/29/25.
//

#ifndef vk_shader_layout_hpp
#define vk_shader_layout_hpp

#include "vk-utils.hpp"

namespace kege::vk{

    class ShaderLayout final : public kege::ShaderLayout
    {
    public:

        const vk::ShaderLayout* vk()const { return this; }
        vk::ShaderLayout* vk() { return this; }

        const VkPipelineLayout handle() const { return _handle; }

        ShaderLayout
        (
            vk::Device* device,
            const std::string& name,
            const kege::IndexedSetLayouts& indexed_set_layouts,
            const kege::PushBlockLayout& push_block_layout
        );

        ~ShaderLayout();

    private:

        /** @brief Native Vulkan pipeline layout handle */
        VkPipelineLayout _handle;
        vk::Device* _device;

    private:

        friend List< vk::ShaderLayout >;
        vk::ShaderLayout* prev;
        vk::ShaderLayout* next;

    private:

        friend vk::Device;
    };

}
#endif /* vk_shader_layout_hpp */
