//
//  shader-resource-set.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/28/26.
//

#ifndef vk_shader_resource_set_hpp
#define vk_shader_resource_set_hpp

#include  "vk-utils.hpp"
#include "shader-resource-set.hpp"

namespace kege::vk{

    class ShaderResourceSetPool;
    class ShaderResourceSetAllocator;

    class ShaderResourceSet : public kege::ShaderResourceSet
    {
    public:

        void setBufferData
        (
            kege::BindingIndex binding,
            kege::ArrayIndex array_index,
            size_t offset,
            size_t size,
            const void* data
        );

        void setBuffer
        (
            kege::BindingIndex binding_index,
            kege::ArrayIndex array_index,
            kege::BindingIndex count,
            const kege::ref::Buffer* buffers
        );

        void setImage
        (
            kege::BindingIndex binding_index,
            kege::ArrayIndex array_index,
            kege::BindingIndex count,
            const kege::ref::Image* images
        );

        void setSampler
        (
            kege::BindingIndex binding_index,
            kege::ArrayIndex array_index,
            kege::BindingIndex count,
            const kege::ref::Sampler* sampler
        );

        void setBinding
        (
            kege::BindingIndex binding_index,
            const kege::Binding& binding
        );

        void updateBindings
        ( const std::initializer_list< kege::BindingUpdateInfo >& binding_indices );

        virtual const kege::vk::ShaderResourceSet* vk() const { return this; }
        virtual kege::vk::ShaderResourceSet* vk() { return this; }

        const kege::ShaderSetDesc& getShaderSetDesc()const;
        kege::UniformKey getUniformKey()const;
        const VkDescriptorSet& handle()const;

        virtual ~ShaderResourceSet();
        ShaderResourceSet
        (
            kege::vk::ShaderResourceSetAllocator* allocator,
            VkDescriptorSet set,
            int32_t pool_index
        );

    private:

        kege::vk::ShaderResourceSetAllocator* _allocator;
        VkDescriptorSet _set;
        int32_t _pool_index;

    private:

        friend kege::vk::ShaderResourceSetPool;
        friend vk::List< vk::ShaderResourceSet >;
        vk::ShaderResourceSet* next;
        vk::ShaderResourceSet* prev;
    };

}

#endif /* vk_shader_resource_set_hpp */
