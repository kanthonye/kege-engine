//
//  vk-set-layout.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/31/25.
//

#ifndef vk_set_layout_hpp
#define vk_set_layout_hpp

#include "vk-utils.hpp"

namespace kege::vk{

    class SetLayout final : public kege::SetLayout
    {
    public:

        SetLayout( vk::Device* device, const LayoutBindings& bindings );
        ~SetLayout();

        virtual const vk::SetLayout* vk() const override { return this; }
        virtual vk::SetLayout* vk() override { return this; }

        const VkDescriptorSetLayout& handle()const;

        ref::ShaderSet allocateSet() override;
        void free( vk::ShaderSet* set );

    private:

        vk::SetAllocator* _allocator;
        vk::Device* _device;

        vk::List< vk::ShaderSet > _freed;

        /** @brief Native Vulkan descriptor set layout handle */
        VkDescriptorSetLayout _handle;

    private:

        friend vk::List< vk::SetLayout >;
        vk::SetLayout* next;
        vk::SetLayout* prev;

    private:

        friend vk::ShaderSet;
        friend vk::Device;
    };

    struct DescriptorSetLayoutVectorHash
    {
        std::size_t operator()( const std::vector<VkDescriptorSetLayout>& vec ) const
        {
            std::size_t seed = 0xcbf29ce484222325ull; // FNV offset basis

            for (auto ptr : vec)
            {
                std::uintptr_t value = reinterpret_cast<std::uintptr_t>(ptr);
                // Mix bits of pointer
                value ^= (value >> 33);
                value *= 0xff51afd7ed558ccdull;
                value ^= (value >> 33);
                value *= 0xc4ceb9fe1a85ec53ull;
                value ^= (value >> 33);

                // Combine into seed
                seed ^= value + 0x9e3779b97f4a7c15ull + (seed << 6) + (seed >> 2);
            }

            return seed;
        }
    };
}

#endif /* vk_set_layout_hpp */
