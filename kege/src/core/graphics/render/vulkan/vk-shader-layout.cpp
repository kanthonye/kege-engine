//
//  vk-shader-layout.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/29/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-shader-layout.hpp"

namespace kege::vk{

    ShaderLayout::ShaderLayout
    (
        vk::Device* device,
        const std::string& name,
        const kege::IndexedSetLayouts& indexed_set_layouts,
        const kege::PushBlockDescs& push_blocks
    )
    :   kege::ShaderLayout( name, indexed_set_layouts, push_blocks )
    ,   _handle( VK_NULL_HANDLE )
    ,   _device( device )
    {
        std::vector< VkDescriptorSetLayout > descriptor_set_layouts;
        descriptor_set_layouts.reserve( indexed_set_layouts.size() );
        for (const kege::IndexedSetLayout& layout : indexed_set_layouts )
        {
            descriptor_set_layouts.push_back( layout.set->vk()->handle() );
        }

        std::vector<VkPushConstantRange> push_constant_ranges;
        push_constant_ranges.reserve( push_blocks.size() );
        for ( const kege::PushBlockDesc& desc : push_blocks )
        {
            push_constant_ranges.push_back
            ({
                .stageFlags = vk::convertShaderStageMask( desc.stages ),
                .offset = desc.offset,
                .size = desc.size
            });
        }

        /**
         * @brief Create the VkPipelineLayoutCreateInfo structure.
         * This structure is used to create the pipeline layout handle.
         */
        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.setLayoutCount = static_cast<uint32_t>( descriptor_set_layouts.size() );
        info.pSetLayouts = descriptor_set_layouts.data();
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

        if ( device->instance()->isValidationEnabled() && !name.empty() )
        {
            device->core().debugSetObjectName
            (
                (uint64_t)_handle,
                VK_OBJECT_TYPE_PIPELINE_LAYOUT,
                name.data()
            );
        }
    }

    ShaderLayout::~ShaderLayout()
    {
        if( _device )
        {
            _device->destroyShaderLayout( this );
            _device = nullptr;
        }
    }
}
