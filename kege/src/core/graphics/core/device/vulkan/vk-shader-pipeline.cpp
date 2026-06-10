//
//  shader-pipeline.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/28/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-shader-pipeline.hpp"
#include "vk-shader-pipeline-layout.hpp"

namespace kege::vk{


    std::vector< VkPipelineColorBlendAttachmentState > getColorBlendAttachmentState( ColorBlendState color_blend )
    {
        std::vector< VkPipelineColorBlendAttachmentState > color_blend_attachment_states;
        for ( int i=0; i<color_blend.attachments.size(); i++ )
        {
            VkPipelineColorBlendAttachmentState attachment = {};
            attachment.blendEnable         = (color_blend.attachments[i].enable)? VK_TRUE : VK_FALSE;

            attachment.colorBlendOp        = convertBlendOp( color_blend.attachments[i].color_blend_op );
            attachment.srcColorBlendFactor = convertBlendFactor( color_blend.attachments[i].src_color_blend );
            attachment.dstColorBlendFactor = convertBlendFactor( color_blend.attachments[i].dst_color_blend );

            attachment.alphaBlendOp        = convertBlendOp( color_blend.attachments[i].alpha_blend_op );
            attachment.dstAlphaBlendFactor = convertBlendFactor( color_blend.attachments[i].dst_alpha_blend );
            attachment.srcAlphaBlendFactor = convertBlendFactor( color_blend.attachments[i].src_alpha_blend );

            attachment.colorWriteMask      = convertColorComponentMask( color_blend.attachments[i].color_write_mask );
            color_blend_attachment_states.push_back( attachment );
        }
        return color_blend_attachment_states;
    }

    VkPipelineDepthStencilStateCreateInfo getDepthStencilStateCreateInfo( const DepthStencil& depth_stencil )
    {
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {};
        depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil_info.depthTestEnable       = depth_stencil.depth.enable;
        depth_stencil_info.depthWriteEnable      = depth_stencil.depth.write;
        depth_stencil_info.depthCompareOp        = convertCompareOp( depth_stencil.depth.compare_op );

        depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        depth_stencil_info.minDepthBounds       = 0.0;
        depth_stencil_info.maxDepthBounds       = 1.0;

        if ( depth_stencil.depth.bounds_test )
        {
            depth_stencil_info.stencilTestEnable    = VK_TRUE;
            depth_stencil_info.front.compareMask    = depth_stencil.stencil.front.compare_mask;
            depth_stencil_info.front.compareOp      = convertCompareOp( depth_stencil.stencil.front.compare_op );
            depth_stencil_info.front.depthFailOp    = convertStencilOp( depth_stencil.stencil.front.depth_fail_op );
            depth_stencil_info.front.failOp         = convertStencilOp( depth_stencil.stencil.front.fail_op );
            depth_stencil_info.front.passOp         = convertStencilOp( depth_stencil.stencil.front.pass_op );
            depth_stencil_info.front.reference      = depth_stencil.stencil.reference;
            depth_stencil_info.front.writeMask      = depth_stencil.stencil.front.write_mask;

            depth_stencil_info.back.depthFailOp     = convertStencilOp( depth_stencil.stencil.back.depth_fail_op );
            depth_stencil_info.back.compareOp       = convertCompareOp( depth_stencil.stencil.back.compare_op );
            depth_stencil_info.back.failOp          = convertStencilOp( depth_stencil.stencil.back.fail_op );
            depth_stencil_info.back.passOp          = convertStencilOp( depth_stencil.stencil.back.pass_op );
            depth_stencil_info.back.reference       = depth_stencil.stencil.reference;
            depth_stencil_info.back.writeMask       = depth_stencil.stencil.back.write_mask;
        }
        else
        {
            depth_stencil_info.stencilTestEnable = VK_FALSE;
            depth_stencil_info.front = {};
            depth_stencil_info.back = {};
        }
        return depth_stencil_info;
    }

    VkPipelineRasterizationStateCreateInfo getRasterizationStateCreateInfo( const RasterizerState& rasterization )
    {
        VkPipelineRasterizationStateCreateInfo state = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        state.polygonMode = convertPolygonMode( rasterization.fill_mode );
        state.frontFace = convertFrontFace( rasterization.front_face );
        state.cullMode = convertCullMode( rasterization.cull_mode );
        state.depthBiasConstantFactor = rasterization.depth_bias.constant_factor;
        state.depthBiasSlopeFactor = rasterization.depth_bias.slope_factor;
        state.depthBiasEnable = rasterization.depth_bias.enable;
        state.depthBiasClamp = rasterization.depth_bias.clamp;
        state.depthClampEnable = rasterization.depth_clamp_enable;
        state.lineWidth = rasterization.line_width;
        state.rasterizerDiscardEnable = VK_FALSE;
        return state;
    }

    VkPipelineMultisampleStateCreateInfo getMultisampleStateCreateInfo( const Multisample& multisample )
    {
        VkPipelineMultisampleStateCreateInfo state = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        state.sampleShadingEnable = multisample.sample_shading;
        state.rasterizationSamples = convertSampleCount(multisample.sample_count);
        state.alphaToCoverageEnable = multisample.alpha_to_coverage;
        state.alphaToOneEnable = multisample.alpha_to_one;
        state.minSampleShading = 1.f;
        state.pSampleMask = 0;
        state.flags = 0;
        return state;
    }

    std::vector< VkVertexInputBindingDescription > getVertexInputBindingDescriptions( const VertexLayout& vertex_input )
    {
        std::vector< VkVertexInputBindingDescription > vertex_bindings;
        for (uint32_t i=0; i<vertex_input.input_rates.size(); ++i)
        {
            vertex_bindings.push_back
            ({
                .binding   = i,
                .stride    = vertex_input.input_rates[i].stride,
                .inputRate = convertVertexInputRate( vertex_input.input_rates[i].input_rate ),
            });
        }
        return vertex_bindings;
    }

    std::vector< VkVertexInputAttributeDescription > getVertexInputAttributeDescriptions( const VertexLayout& vertex_layout )
    {
        std::vector< VkVertexInputAttributeDescription > vertex_attributes;
        uint32_t offset = 0;
        for (int i=0; i<vertex_layout.attributes.size(); ++i)
        {
            vertex_attributes.push_back
            ({
                .location = vertex_layout.attributes[i].location,
                .binding  = vertex_layout.attributes[i].binding,
                .format   = convertShaderVarTypeToVkFormat( vertex_layout.attributes[i].type ),
                .offset   = offset,
            });
            offset += sizeOf(vertex_layout.attributes[i].type);
        }
        return vertex_attributes;
    }

    VkPipelineViewportStateCreateInfo getViewportStateCreateInfo()
    {
        VkPipelineViewportStateCreateInfo state = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        state.viewportCount = 1;
        state.pViewports = nullptr; // Set via dynamic state or provide dummy
        state.scissorCount = 1;
        state.pScissors = nullptr; // Set via dynamic state or provide dummy
        return state;
    }


    std::vector< VkFormat > getOutput( const PipelineRendering& outputs )
    {
        std::vector< VkFormat > color_attachmen_formats;
        for(Format fmt : outputs.color_attachment_formats )
        {
            VkFormat vk_fmt = convertFormat(fmt);
            if (vk_fmt == VK_FORMAT_UNDEFINED)
            {
                kege::Log::error << "Invalid format passed for color attachment!"<<Log::nl;
                return {};
            }
            color_attachmen_formats.push_back( vk_fmt );
        }
        return color_attachmen_formats;
    }

    VkPipelineRenderingCreateInfoKHR getRenderingCreateInfo( std::vector< VkFormat >& output_formats, const PipelineRendering& outputs )
    {
        VkPipelineRenderingCreateInfoKHR state{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR};
        state.stencilAttachmentFormat = convertFormat(outputs.stencil_attachment_format);
        state.depthAttachmentFormat = convertFormat(outputs.depth_attachment_format);
        state.colorAttachmentCount = static_cast< uint32_t >( output_formats.size() );
        state.pColorAttachmentFormats = ( !output_formats.empty() ) ? output_formats.data() : nullptr;
        return state;
    }

    VkPipelineVertexInputStateCreateInfo getVertexInputStateCreateInfo
    (
        std::vector< VkVertexInputAttributeDescription >& attributes,
        std::vector< VkVertexInputBindingDescription >& bindings
    )
    {
        VkPipelineVertexInputStateCreateInfo state = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        state.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
        state.pVertexBindingDescriptions = bindings.empty() ? nullptr : bindings.data();
        state.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
        state.pVertexAttributeDescriptions = attributes.empty() ? nullptr : attributes.data();
        return state;
    }

    VkPipelineColorBlendStateCreateInfo getColorBlendStateCreateInfo
    (
        const std::vector< VkPipelineColorBlendAttachmentState >& color_blend_attachment_states,
        const ColorBlendState& color_blend
    )
    {
        return VkPipelineColorBlendStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = (color_blend.logic_op_enable) ? VK_TRUE : VK_FALSE,
            .logicOp = vk::convertLogicOp( color_blend.logic_op ),
            .attachmentCount = static_cast< uint32_t >( color_blend_attachment_states.size() ),
            .pAttachments = color_blend_attachment_states.data(),
        };
    }

    VkPipelineInputAssemblyStateCreateInfo getInputAssemblyStateCreateInfo( const kege::InputAssembly& input_assembly )
    {
        return VkPipelineInputAssemblyStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = convertPrimitiveTopology( input_assembly.topology ),
            .primitiveRestartEnable = input_assembly.primitive_restart,
        };
    }

    std::vector< VkPipelineShaderStageCreateInfo > getShaderStageCreateInfo( const std::vector< ref::Shader >& stages )
    {
        std::vector< VkPipelineShaderStageCreateInfo > shader_stages;
        for (int k=0; k<stages.size(); ++k)
        {
            const vk::Shader* shader = stages[ k ]->vk();
            shader_stages.push_back
            (
                VkPipelineShaderStageCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = shader->stage(),
                    .module = shader->handle(),
                    .pName = shader->entryPoint().data(),
                }
            );
        }
        return shader_stages;
    }

    VkDynamicState toVkDynamicState(const kege::DynamicState& state)
    {
        switch( state )
        {
            default:
            case kege::DynamicState::Viewport: return VK_DYNAMIC_STATE_VIEWPORT;
            case kege::DynamicState::Scissor: return VK_DYNAMIC_STATE_SCISSOR; 
            case kege::DynamicState::LineWidth: return VK_DYNAMIC_STATE_LINE_WIDTH;
            case kege::DynamicState::DepthBias: return VK_DYNAMIC_STATE_DEPTH_BIAS;
            case kege::DynamicState::BlendConstants: return VK_DYNAMIC_STATE_BLEND_CONSTANTS;
            case kege::DynamicState::BlendBounds: return VK_DYNAMIC_STATE_DEPTH_BOUNDS;
            case kege::DynamicState::StencilCompareMask: return VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
            case kege::DynamicState::StencilWriteMask: return VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
            case kege::DynamicState::StencilReference: return VK_DYNAMIC_STATE_STENCIL_REFERENCE;
            case kege::DynamicState::CullMode: return VK_DYNAMIC_STATE_CULL_MODE;
            case kege::DynamicState::FrontFace: return VK_DYNAMIC_STATE_FRONT_FACE;
            case kege::DynamicState::PrimitiveTopology: return VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY;
            case kege::DynamicState::DepthTestEnabled: return VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE;
            case kege::DynamicState::PolygonMode: return VK_DYNAMIC_STATE_POLYGON_MODE_EXT;
            case kege::DynamicState::PrimitiveRestartEnable: return VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE;
            case kege::DynamicState::DepthWriteEnable: return VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE;
            case kege::DynamicState::DepthCompareOpP: return VK_DYNAMIC_STATE_DEPTH_COMPARE_OP;
            case kege::DynamicState::DepthBoundTestEnable: return VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE;
            case kege::DynamicState::StencilTestEnable: return VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE;
            case kege::DynamicState::RasterizerDiscardEbale: return VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE;
        }

    }
    std::vector< VkDynamicState > getDynamicState(const kege::DynamicStates& states)
    {
        std::vector< VkDynamicState > dynamic_states;
        for (const kege::DynamicState& state: states )
        {
            dynamic_states.push_back( toVkDynamicState( state ) );
        }
        return dynamic_states;
    }
    VkPipelineDynamicStateCreateInfo getDynamicStateCreateInfo(const std::vector< VkDynamicState >& dynamic_states)
    {
        return VkPipelineDynamicStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast< uint32_t >( dynamic_states.size() ),
            .pDynamicStates = dynamic_states.data()
        };
    }

    void ShaderPipeline::createGraphicsPipeline( const kege::ShaderPipelineDesc& desc, const ref::ShaderPipelineLayout& shader_layout )
    {
        // --- Translate Desc to Vulkan Structures ---
        VkGraphicsPipelineCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        create_info.layout = shader_layout->vk()->handle();

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 1. Shader Stages
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        std::vector< VkPipelineShaderStageCreateInfo > shader_stages = getShaderStageCreateInfo( desc.shaders );
        create_info.stageCount = static_cast< uint32_t >( shader_stages.size() );
        create_info.pStages = shader_stages.data();

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 2. Vertex Input State
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        std::vector< VkVertexInputAttributeDescription > attributes = getVertexInputAttributeDescriptions( desc.vertex_layout );
        std::vector< VkVertexInputBindingDescription > bindings = getVertexInputBindingDescriptions( desc.vertex_layout );
        VkPipelineVertexInputStateCreateInfo vertex_input_info = getVertexInputStateCreateInfo(attributes, bindings);
        create_info.pVertexInputState = &vertex_input_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 3. Input Assembly
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        VkPipelineInputAssemblyStateCreateInfo input_assembly_info = getInputAssemblyStateCreateInfo( desc.input_assembly );
        create_info.pInputAssemblyState = &input_assembly_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 4. Viewport State (can be dynamic) // Assuming non-dynamic for now
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        VkPipelineViewportStateCreateInfo viewport_state = getViewportStateCreateInfo();
        create_info.pViewportState = &viewport_state;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 5. Rasterization State
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        VkPipelineRasterizationStateCreateInfo rasterization_info = getRasterizationStateCreateInfo( desc.rasterizer );
        create_info.pRasterizationState = &rasterization_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 7. Depth Stencil State
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info = getDepthStencilStateCreateInfo( desc.depth_stencil );
        create_info.pDepthStencilState = &depth_stencil_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 8. Color Blend State
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        std::vector< VkPipelineColorBlendAttachmentState > color_blend_attachment_states = getColorBlendAttachmentState( desc.color_blend );
        VkPipelineColorBlendStateCreateInfo color_blend_info = getColorBlendStateCreateInfo( color_blend_attachment_states, desc.color_blend );
        create_info.pColorBlendState = &color_blend_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 6. Multisample State
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        VkPipelineMultisampleStateCreateInfo multisample_info = getMultisampleStateCreateInfo( desc.multisample );
        create_info.pMultisampleState = &multisample_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 9. Dynamic State (Optional but common for viewport/scissor)
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        std::vector< VkDynamicState > dynamic_states = getDynamicState(desc.dynamic_states);
        VkPipelineDynamicStateCreateInfo dynamic_state_create_info = getDynamicStateCreateInfo( dynamic_states );
        create_info.pDynamicState = &dynamic_state_create_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 10. Render Pass / Rendering Info (Crucial!)
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        std::vector< VkFormat > color_attachmen_formats = getOutput( desc.pipeline_rendering );
        VkPipelineRenderingCreateInfoKHR rendering_info = getRenderingCreateInfo(color_attachmen_formats, desc.pipeline_rendering);

        // Chain this to pipelineInfo.pNext
        create_info.pNext = &rendering_info;
        create_info.renderPass = VK_NULL_HANDLE; // Must be NULL for dynamic rendering

        // 12. Base Pipeline (for derivatives, ignore for now)
        create_info.basePipelineHandle = VK_NULL_HANDLE;
        create_info.basePipelineIndex = -1;

        // --- Create Pipeline ---
        VkResult result = vkCreateGraphicsPipelines
        (
            _device->handle(), VK_NULL_HANDLE, 1,
            &create_info, nullptr, &_handle
        );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error <<vkResultToString( result ) <<Log::nl;
            return;
        }
    }

    void ShaderPipeline::createComputePipeline( const kege::ShaderPipelineDesc& desc, const ref::ShaderPipelineLayout& shader_layout )
    {
        std::vector< VkPipelineShaderStageCreateInfo > shader_stages = getShaderStageCreateInfo( desc.shaders );

        VkComputePipelineCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        create_info.layout = shader_layout->vk()->handle();
        create_info.stage = *shader_stages.data();

        VkResult result = vkCreateComputePipelines
        (
            _device->handle(), VK_NULL_HANDLE, 1,
            &create_info, nullptr, &_handle
        );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << "createComputePipeline" <<Log::nl;
            return;
        }
    }

    ShaderPipeline::ShaderPipeline
    (
        vk::Device* device,
        const kege::ShaderPipelineDesc& desc,
        const ref::ShaderPipelineLayout& shader_layout
    )
    :   kege::ShaderPipeline( desc, shader_layout )
    ,   _device( device )
    {
        if ( desc.pipeline_type == PipelineType::Graphics )
        {
            createGraphicsPipeline( desc, shader_layout );
        }
        else if ( desc.pipeline_type == PipelineType::Compute )
        {
            createGraphicsPipeline( desc, shader_layout );
        }

        if ( _device->_instance->isValidationEnabled() && !desc.name.empty() )
        {
            _device->core().debugSetObjectName( (uint64_t)_handle, VK_OBJECT_TYPE_PIPELINE, desc.name.c_str() );
        }
    }
}
