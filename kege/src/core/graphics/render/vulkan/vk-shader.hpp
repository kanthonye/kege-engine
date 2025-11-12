//
//  vk-shader.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/28/25.
//

#ifndef vk_shader_hpp
#define vk_shader_hpp

#include "vk-utils.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan shader module resources
     *
     * Encapsulates a VkShaderModule and its associated metadata.
     */
    class Shader : public kege::Shader
    {
    public:

        const vk::Shader* vk() const override { return this; };
        vk::Shader* vk() override { return this; };

        const VkShaderStageFlagBits stage()const;
        const VkShaderModule handle()const;
        const std::string& entryPoint()const;

        Shader( vk::Device* d, const ShaderDesc& desc );
        ~Shader();

    public:

        VkShaderStageFlagBits _stage;
        VkShaderModule _handle;

        vk::Device* _device;

        std::string _entry_point;

    public:

        friend List< vk::Shader >;
        vk::Shader* prev;
        vk::Shader* next;
    };

}
#endif /* vk_shader_hpp */
