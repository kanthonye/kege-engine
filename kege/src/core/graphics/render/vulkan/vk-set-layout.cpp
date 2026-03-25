//
//  vk-set-layout.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/31/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-shader-set.hpp"
#include "vk-set-layout.hpp"
#include "vk-set-allocator.hpp"

namespace kege::vk{

    SetLayout::SetLayout( vk::Device* device, const BindPointDescs& bindings )
    :   kege::SetLayout( bindings )
    ,   _handle( VK_NULL_HANDLE )
    ,   _allocator( nullptr )
    ,   _device( device )
    {
        /**
         * @brief Create a vector of VkDescriptorSetLayoutBinding from the bindings.
         * This is used to create the VkDescriptorSetLayout handle.
         * Each binding corresponds to a resource in the shader and its properties.
         */
        std::vector< VkDescriptorSetLayoutBinding > descriptor_bindings;
        for ( const kege::BindPointDesc& desc : bindings )
        {
            VkDescriptorSetLayoutBinding dslb = {};
            dslb.binding = desc.index;
            dslb.descriptorCount = desc.count;
            dslb.descriptorType = vk::toDescriptorType( desc.usage );
            dslb.stageFlags = vk::convertShaderStageMask( desc.stages );
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
            std::string name = bindings[0].name;
            for ( int i=1; i<bindings.size(); ++i )
            {
                name = " | " + bindings[i].name;
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

       std::vector< VkDescriptorType > descriptor_types;
       for ( const VkDescriptorSetLayoutBinding& binding : descriptor_bindings )
       {
           descriptor_types.push_back( binding.descriptorType );
       }

        _allocator = _device->getDescriptorAllocator( descriptor_types );
        if ( _allocator == nullptr )
        {
            kege::Log::error << "CREATION_FAILED" <<kege::Log::nl;
            _device->destroySetLayout( this );
            _device = nullptr;
        }
    }

    const VkDescriptorSetLayout& SetLayout::handle()const
    {
        return _handle;
    }

    ref::ShaderSet SetLayout::allocateSet()
    {
        if( !_freed.empty() )
        {
            ref::ShaderSet set = _freed.popFront();
            return set;
        }

        vk::ShaderSet* set = new vk::ShaderSet( this, _allocator->allocate( this ) );
        if( set == nullptr ) return {};
        
        set->_freed = true;
        return set;
    }

    void SetLayout::free(vk::ShaderSet *set)
    {
        if( set )
        {
            if( !set->_freed )
            {
                _freed.insert( set );
                set->_freed = true;
            }
        }
    }

    SetLayout::~SetLayout()
    {
        if ( _device )
        {
            _device->destroySetLayout( this );
            _device = nullptr;
        }
    }
}
