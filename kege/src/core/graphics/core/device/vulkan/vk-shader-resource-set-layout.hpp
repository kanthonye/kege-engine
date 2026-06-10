//
//  vk-set-layout.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/31/25.
//

#ifndef vk_set_layout_hpp
#define vk_set_layout_hpp

#include  "vk-utils.hpp"
#include "vk-shader-resource-set.hpp"

namespace kege::vk{

    class ShaderResourceSetLayout : public kege::ShaderResourceSetLayout
    {
    public:

        const kege::ShaderSetDesc& getShaderSetDesc()const;
        kege::Ref< kege::vk::ShaderResourceSet > allocate();

        const kege::vk::ShaderResourceSetLayout* vk() const { return this; }
        kege::vk::ShaderResourceSetLayout* vk() { return this; }

        kege::UniformKey getUniformKey()const;
        const VkDescriptorSetLayout& handle()const;

        const kege::vk::Device* device()const;

        ShaderResourceSetLayout
        (
            vk::Device* device,
            kege::UniformKey key,
            uint32_t size,
            const kege::ShaderSetBindingDesc* descs
        );
        ~ShaderResourceSetLayout();

    private:

        /** @brief Native Vulkan descriptor set layout handle */
        VkDescriptorSetLayout _handle;
        kege::vk::Device* _device;

        vk::ShaderResourceSetLayout* next;
        vk::ShaderResourceSetLayout* prev;
        
    private:

        friend vk::List< vk::ShaderResourceSetLayout >;
        friend kege::vk::Device;
    };

}

#endif /* vk_set_layout_hpp */
