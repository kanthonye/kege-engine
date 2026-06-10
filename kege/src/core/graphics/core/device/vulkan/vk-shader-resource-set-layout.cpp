//
//  vk-set-layout.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/31/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-shader-resource-set-layout.hpp"

#include <functional>
#include <vector>
#include <string>
#include <cstdint>

namespace kege::vk{

    ShaderResourceSetLayout::ShaderResourceSetLayout
    (
        vk::Device* device,
        kege::UniformKey key,
        uint32_t size,
        const kege::ShaderSetBindingDesc* descs
    )
    :   kege::ShaderResourceSetLayout( key, size, descs )
    ,   _handle( VK_NULL_HANDLE )
    ,   _device( device )
    {
        /**
         * @brief Create a vector of VkDescriptorSetLayoutBinding from the bindings.
         * This is used to create the VkDescriptorSetLayout handle.
         * Each binding corresponds to a resource in the shader and its properties.
         */
        std::vector< VkDescriptorSetLayoutBinding > descriptor_bindings;
        for ( uint32_t i = 0; i < size; ++i )
        {
            VkDescriptorSetLayoutBinding dslb = {};
            dslb.binding = descs[i].binding_index;
            dslb.descriptorCount = descs[i].binding_count;
            dslb.descriptorType = vk::toDescriptorType( descs[i].usage );
            dslb.stageFlags = vk::convertShaderStageMask( descs[i].stages );
            descriptor_bindings.push_back( dslb );
        }

        /**
         * @brief Create the VkDescriptorSetLayoutCreateInfo structure.
         * This structure is used to create the VkDescriptorSetLayout handle.
         * It contains the bindings, flags, and other properties of the descriptor set layout.
         */
        VkDescriptorSetLayoutCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        create_info.bindingCount = static_cast<int>( descriptor_bindings.size() );
        create_info.pBindings = descriptor_bindings.data();

        /**
         * @brief Create the VkDescriptorSetLayout.
         * This is the Vulkan handle that represents the descriptor set layout.
         */
        VkResult result = _device->core().createDescriptorSetLayout( &create_info, nullptr, &_handle );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return;
        }

        if ( _device->instance()->isValidationEnabled() )
        {
            std::string name = descs[0].name;
            for ( int i = 1; i < size; ++i )
            {
                name = " | " + descs[i].name;
            }
            if (!name.empty())
            {
                _device->core().debugSetObjectName
                (
                    (uint64_t)_handle,
                    VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
                    name.c_str()
                );
            }
        }
    }

    kege::UniformKey ShaderResourceSetLayout::getUniformKey()const
    {
        return _global_uniform_key;
    }

    const kege::ShaderSetDesc& ShaderResourceSetLayout::getShaderSetDesc()const
    {
        return _bindings;
    }

    const VkDescriptorSetLayout& ShaderResourceSetLayout::handle()const
    {
        return _handle;
    }

    const kege::vk::Device* ShaderResourceSetLayout::device()const
    {
        return _device;
    }

    ShaderResourceSetLayout::~ShaderResourceSetLayout()
    {
        if ( _device )
        {
            _device->destroyShaderResourceSetLayout( this );
            _device = nullptr;
        }
    }
}

