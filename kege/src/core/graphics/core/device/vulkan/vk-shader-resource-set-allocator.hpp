//
//  vk-shader-resource-set-allocator.hpp
//  editor
//
//  Created by Kenneth Esdaile on 5/1/26.
//

#ifndef vk_shader_resource_set_allocator_hpp
#define vk_shader_resource_set_allocator_hpp

#include  "vk-utils.hpp"
#include "vk-shader-resource-set.hpp"
#include "vk-shader-resource-set-pool.hpp"
#include "vk-shader-resource-set-layout.hpp"

namespace kege::vk{

    class ShaderResourceSetAllocator : public kege::RefCounter
    {
    public:

        const kege::ShaderSetDesc& getShaderSetDesc()const;
        kege::Ref< kege::vk::ShaderResourceSet > allocate();
        void free(kege::vk::ShaderResourceSet* set);

        kege::UniformKey getUniformKey()const;
        uint32_t getFrameIndex()const;

        const VkDevice device()const;

        ShaderResourceSetAllocator( vk::ShaderResourceSetLayout* layout, vk::ShaderResourceSetPool* pool );
        ~ShaderResourceSetAllocator();
        ShaderResourceSetAllocator();

    protected:

        kege::vk::ShaderResourceSetPool* _pool;
        kege::vk::ShaderResourceSetLayout* _layout;
    };

}

#endif /* vk_shader_resource_set_allocator_hpp */
