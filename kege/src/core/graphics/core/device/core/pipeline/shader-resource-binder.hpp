//
//  shader-resource-binder.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/28/26.
//

#ifndef shader_resource_binder_hpp
#define shader_resource_binder_hpp

#include "../command-buffer.hpp"
#include "shader-resource-set.hpp"

namespace kege
{
    class ShaderResourceBinder;
}

namespace kege::vk
{
    class ShaderResourceBinder;
}

namespace kege::ref
{
    typedef kege::Ref< kege::ShaderResourceBinder > ShaderResourceBinder;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::ShaderResourceBinder > ShaderResourceBinder;
}

namespace kege{

    struct SetUpdateInfo
    {
        uint32_t set_index;
        std::vector<std::pair<uint32_t, kege::Binding>> bindings;
    };
    
    /** A class for binding shader resources to a graphics pipeline. */
    class ShaderResourceBinder : public kege::RefCounter
    {
    public:

        virtual void insertSemanticBinding(const std::string& name, kege::GlobalSemantic semantic) = 0;
        virtual void setBuffers
        (
            kege::GlobalSemantic semantic,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Buffer* buffers
        )
        = 0;

        virtual void setImages
        (
            kege::GlobalSemantic semantic,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Image* images
        )
        = 0;

        virtual void setSamplers
        (
            kege::GlobalSemantic semantic,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Sampler* samplers
        )
        = 0;

        virtual void setBufferData
        (
            const std::string& name,
            kege::ArrayIndex array_index,
            size_t offset, size_t size,
            const void* data
        )
        = 0;

        /* Sets the buffer bindings for a specific set, binding, and array index. */
        virtual void setBuffers
        (
            const std::string& name,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Buffer* buffers
        )
        = 0;

        /* Sets the image bindings for a specific set, binding, and array index. */
        virtual void setImages
        (
            const std::string& name,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Image* images
        )
        = 0;

        virtual void setSamplers
        (
            const std::string& name,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Sampler* samplers
        )
        = 0;

        virtual void setBinding
        (
            const std::string& binding_name,
            const kege::Binding& binding
        )
        = 0;

        /* Binds the shader resources to the command buffer for the specified set indices. */
        virtual void bind
        (
            kege::CommandBuffer* cmd,
            const std::vector< kege::SetIndex >& set_indices
        )
        const = 0;

        /* Binds the shader resources to the command buffer for all sets. */
        virtual void bind(kege::CommandBuffer* cmd) const = 0;

        virtual ~ShaderResourceBinder(){}
    };

}
#endif /* shader_resource_binder_hpp */
