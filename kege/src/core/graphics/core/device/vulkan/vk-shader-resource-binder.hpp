//
//  shader-resource-binder.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/28/26.
//

#ifndef vk_shader_resource_binder_hpp
#define vk_shader_resource_binder_hpp

#include "vk-utils.hpp"

namespace kege::vk{
    
    class ShaderResourceBinder final : public kege::ShaderResourceBinder
    {
    public:

        void insertSemanticBinding(const std::string& name, kege::GlobalSemantic semantic);

        void setBuffers
        (
            kege::GlobalSemantic semantic,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Buffer* buffers
        );

        void setImages
        (
            kege::GlobalSemantic semantic,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Image* images
        );

        void setSamplers
        (
            kege::GlobalSemantic semantic,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Sampler* samplers
        );

        void setBinding
        (
            const std::string& name,
            const kege::Binding& binding
        );

        void setBufferData
        (
            const std::string& name,
            kege::ArrayIndex array_index,
            size_t offset, size_t size,
            const void* data
        );

        void setBuffers
        (
            const std::string& name,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Buffer* buffers
        );

        void setImages
        (
            const std::string& name,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Image* images
        );

        void setSamplers
        (
            const std::string& name,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Sampler* samplers
        );

        void bind
        (
            kege::CommandBuffer* cmd,
            const std::vector< kege::SetIndex >& set_indices
        )
        const;

        void bind(kege::CommandBuffer* cmd) const;

        ShaderResourceBinder
        (
            vk::Device* device,
            const std::vector< kege::ref::ShaderResourceSet >& sets
        );

        ~ShaderResourceBinder();

    private:

        struct ModifiedSet
        {
            uint32_t set_index;
            uint32_t array_index;
            uint32_t binding_count;
            uint32_t binding_index;
        };

        const kege::ShaderResrcIndexPair* getShaderResrcIndexPair( kege::GlobalSemantic semantic )const;
        const kege::ShaderResrcIndexPair* getShaderResrcIndexPair( const std::string& name )const;

        void setBinds
        (
            const kege::ShaderResrcIndexPair* pair,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Buffer* buffers
        );

        void setBinds
        (
            const kege::ShaderResrcIndexPair* pair,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Image* images
        );

        void setBinds
        (
            const kege::ShaderResrcIndexPair* pair,
            uint32_t element_index,
            uint32_t element_count,
            const kege::ref::Sampler* samplers
        );

    private:

        std::map< kege::GlobalSemantic, kege::ShaderResrcIndexPair > _global_binding_map;
        std::map< std::string, kege::ShaderResrcIndexPair > _binding_map;

        mutable std::vector< ModifiedSet > _modified_sets;

        std::vector< kege::ref::ShaderResourceSet > _sets;
        vk::Device* _device;

    private:

        vk::ShaderResourceBinder* prev;
        vk::ShaderResourceBinder* next;

        friend List< vk::ShaderResourceBinder >;
        friend vk::Device;

    };

}

#endif /* vk_shader_resource_binder_hpp */
