//
//  shader-resource-set-layout.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/31/25.
//

#ifndef shader_resource_set_layout_hpp
#define shader_resource_set_layout_hpp

#include "shader-common.hpp"

namespace kege
{
    class ShaderResourceSetLayout;
}

namespace kege::vk
{
    class ShaderResourceSetLayout;
}

namespace kege::ref
{
    typedef kege::Ref< kege::ShaderResourceSetLayout > ShaderResourceSetLayout;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::ShaderResourceSetLayout > ShaderResourceSetLayout;
}

namespace kege{

    class ShaderResourceSetLayout : public kege::RefCounter
    {
    public:

        const kege::ShaderSetDesc& getDesc() const;
        virtual kege::UniformKey getUniformKey()const = 0;

        virtual const kege::vk::ShaderResourceSetLayout* vk() const { return nullptr; }
        virtual kege::vk::ShaderResourceSetLayout* vk() { return nullptr; }

        const std::string& getName()const;
        uint32_t getId()const;
        
        ShaderResourceSetLayout
        (
            kege::UniformKey key,
            uint32_t size,
            const kege::ShaderSetBindingDesc* descs
        );
        virtual ~ShaderResourceSetLayout();

    protected:

        kege::ShaderSetDesc _bindings;
        kege::UniformKey _global_uniform_key;
        std::string _name;
        uint32_t _id;
    };

}

#endif /* shader_resource_set_layout_hpp */
