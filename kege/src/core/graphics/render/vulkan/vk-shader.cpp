//
//  vk-shader.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/28/25.
//

#include "vk-device.hpp"
#include "vk-shader.hpp"
#include "vk-instance.hpp"

namespace kege::vk{

    Shader::Shader( vk::Device* device, const ShaderDesc& desc )
    {
         if ( _device == VK_NULL_HANDLE || desc.byte_code.empty() ) return;

        VkShaderModuleCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = desc.byte_code.size() * sizeof( desc.byte_code[0] );
        create_info.pCode = desc.byte_code.data();

        VkResult result;
        if ((result = vkCreateShaderModule( device->handle(), &create_info, nullptr, &_handle ) ) != VK_SUCCESS )
        {
            kege::Log::error << "Failed to create shader module!" <<Log::nl;
            return;
        }

        // Set Debug Name (requires VK_EXT_debug_utils)
        if ( device->instance()->isValidationEnabled() && !desc.name.empty() )
        {
            device->core().debugSetObjectName( (uint64_t)_handle, VK_OBJECT_TYPE_SHADER_MODULE, desc.name.data() );
        }

        _device = device;
        _entry_point = desc.entry_point;
        _stage = convertShaderStage( desc.stage );
    }

    const VkShaderStageFlagBits Shader::stage()const
    {
        return _stage;
    }
    
    const VkShaderModule Shader::handle()const
    {
        return _handle;
    }

    const std::string& Shader::entryPoint()const
    {
        return _entry_point;
    }

    Shader::~Shader()
    {
        if ( _device )
        {
            _device->destroyShader( this );
            _device = nullptr;
        }
    }
}
