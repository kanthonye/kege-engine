//
//  vk-shader-layout.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/29/25.
//

#ifndef vk_shader_layout_hpp
#define vk_shader_layout_hpp

#include "vk-utils.hpp"
#include "vk-shader-resource-set-layout.hpp"

namespace kege::vk{

    class ShaderPipelineLayout final : public kege::ShaderPipelineLayout
    {
    public:

        const vk::ShaderPipelineLayout* vk()const { return this; }
        vk::ShaderPipelineLayout* vk() { return this; }

        const VkPipelineLayout handle() const { return _handle; }

        ShaderPipelineLayout
        (
            vk::Device* device, const std::string& name,
            const kege::PushBlockDescs& push_block_desc,
            std::vector< kege::ref::ShaderResourceSetLayout >& layouts
        );

        ~ShaderPipelineLayout();

    private:

        /** @brief Native Vulkan pipeline layout handle */
        VkPipelineLayout _handle;
        
        vk::Device* _device;

    private:

        friend List< vk::ShaderPipelineLayout >;
        vk::ShaderPipelineLayout* prev;
        vk::ShaderPipelineLayout* next;
        friend vk::Device;
    };

}
#endif /* vk_shader_layout_hpp */
