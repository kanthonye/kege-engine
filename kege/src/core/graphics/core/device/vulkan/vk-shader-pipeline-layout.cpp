//
//  vk-shader-layout.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/29/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-shader-pipeline-layout.hpp"

namespace kege::vk{

    ShaderPipelineLayout::ShaderPipelineLayout
    (
        vk::Device* device, const std::string& name,
        const kege::PushBlockDescs& push_block_desc,
        std::vector< kege::ref::ShaderResourceSetLayout >& shader_resource_layouts
    )
    :   kege::ShaderPipelineLayout( name, push_block_desc, shader_resource_layouts )
    ,   _handle( VK_NULL_HANDLE )
    ,   _device( device )
    {
        std::vector< VkPushConstantRange > push_constant_ranges;
        
        push_constant_ranges.reserve( push_block_desc.size() );
        for ( const kege::PushBlockDesc& desc : push_block_desc )
        {
            push_constant_ranges.push_back
            ({
                .stageFlags = vk::convertShaderStageMask( desc.stages ),
                .offset = desc.offset,
                .size = desc.size
            });
        }

        size_t size = shader_resource_layouts.size();
        std::vector< VkDescriptorSetLayout > descriptor_set_layout_handles( size );
        for (uint32_t i=0; i<size; ++i )
        {
            descriptor_set_layout_handles[i] = shader_resource_layouts[i]->vk()->handle();
        }

        /**
         * @brief Create the VkPipelineLayoutCreateInfo structure.
         * This structure is used to create the pipeline layout handle.
         */
        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.setLayoutCount = static_cast<uint32_t>( descriptor_set_layout_handles.size() );
        info.pSetLayouts = descriptor_set_layout_handles.data();
        info.pushConstantRangeCount = static_cast<uint32_t>( push_constant_ranges.size() );
        info.pPushConstantRanges = push_constant_ranges.data();

        /**
         * @brief Create the pipeline layout handle.
         * This handle is used to bind descriptor sets to the pipeline.
         */
        VkResult result;
        if (( result = device->core().createPipelineLayout( &info, nullptr, &_handle ) ) != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return;
        }

        if ( _device->instance()->isValidationEnabled() && !name.empty() )
        {
            _device->core().debugSetObjectName
            (
                (uint64_t)_handle,
                VK_OBJECT_TYPE_PIPELINE_LAYOUT,
                name.data()
            );
        }
    }

    ShaderPipelineLayout::~ShaderPipelineLayout()
    {
        if( _device )
        {
            _device->destroyShaderPipelineLayout( this );
            _device = nullptr;
        }
    }
}
