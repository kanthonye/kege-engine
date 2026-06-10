//
//  shader-resource-set.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/28/26.
//

#ifndef shader_resource_set_hpp
#define shader_resource_set_hpp

#include "shader-common.hpp"

namespace kege
{
    class ShaderResourceSet;
}

namespace kege::vk
{
    class ShaderResourceSet;
}

namespace kege::ref
{
    typedef kege::Ref< kege::ShaderResourceSet > ShaderResourceSet;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::ShaderResourceSet > ShaderResourceSet;
}

namespace kege{

    struct ShaderResrcIndexPair
    {
        uint32_t set, binding;
    };



    struct BindingUpdateInfo
    {
        uint32_t binding_index;
        uint32_t element_index;
        uint32_t element_count;
    };

    /**
     * An abstract class representing a set of shader resources, such as uniform buffers, storage buffers, images, and samplers. 
     */
    class ShaderResourceSet : public kege::RefCounter
    {
    public:

        virtual void updateBindings
        ( const std::initializer_list< BindingUpdateInfo >& binding_indices ) = 0;

        virtual void setBufferData
        (
            kege::BindingIndex binding,
            kege::ArrayIndex array_index,
            size_t offset,
            size_t size,
            const void* data
        )
        = 0;

        virtual void setBuffer
        (
            kege::BindingIndex binding_index,
            kege::ArrayIndex array_index,
            kege::BindingIndex count,
            const kege::ref::Buffer* buffers
        )
        = 0;

        virtual void setImage
        (
            kege::BindingIndex binding_index,
            kege::ArrayIndex array_index,
            kege::BindingIndex count,
            const kege::ref::Image* images
        )
        = 0;

        virtual void setSampler
        (
            kege::BindingIndex binding_index,
            kege::ArrayIndex array_index,
            kege::BindingIndex count,
            const kege::ref::Sampler* sampler
        )
        = 0;

        virtual void setBinding
        (
            kege::BindingIndex binding_index,
            const kege::Binding& binding
        )
        = 0;

        const kege::Binding* getBinding( uint32_t binding_index )const;
        kege::Binding* getBinding( uint32_t binding_index );

        virtual const kege::ShaderSetDesc& getShaderSetDesc() const = 0;
        virtual kege::UniformKey getUniformKey() const = 0;

        virtual const kege::vk::ShaderResourceSet* vk() const { return nullptr; }
        virtual kege::vk::ShaderResourceSet* vk() { return nullptr; }

        virtual ~ShaderResourceSet(){}

    protected:

        ShaderResourceSet(){}

        kege::array< kege::Binding > _bindings;
    };

}
#endif /* shader_resource_set_hpp */
