//
//  shader-pipeline.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/28/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-shader-pipeline.hpp"
#include "vk-shader-layout.hpp"

namespace kege::vk{

    std::vector< VkPipelineColorBlendAttachmentState > getColorBlendAttachmentState( ColorBlendStateDesc color_blend )
    {
        std::vector< VkPipelineColorBlendAttachmentState > color_blend_attachment_states;
        for ( int i=0; i<color_blend.attachments.size(); i++ )
        {
            VkPipelineColorBlendAttachmentState attachment = {};
            attachment.blendEnable         = (color_blend.attachments[i].blend_enable)? VK_TRUE : VK_FALSE;

            attachment.colorBlendOp        = convertBlendOp( color_blend.attachments[i].color_blend_op );
            attachment.srcColorBlendFactor = convertBlendFactor( color_blend.attachments[i].src_color_blend_factor );
            attachment.dstColorBlendFactor = convertBlendFactor( color_blend.attachments[i].dst_color_blend_factor );

            attachment.alphaBlendOp        = convertBlendOp( color_blend.attachments[i].alpha_blend_op );
            attachment.dstAlphaBlendFactor = convertBlendFactor( color_blend.attachments[i].dst_alpha_blend_factor );
            attachment.srcAlphaBlendFactor = convertBlendFactor( color_blend.attachments[i].src_alpha_blend_factor );

            attachment.colorWriteMask      = convertColorComponentMask( color_blend.attachments[i].color_write_mask );
            color_blend_attachment_states.push_back( attachment );
        }
        return color_blend_attachment_states;
    }

    VkPipelineDepthStencilStateCreateInfo getDepthStencilStateCreateInfo( const DepthStencilStateDesc& depth_stencil )
    {
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {};
        depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil_info.depthTestEnable       = depth_stencil.depth_test_enable;
        depth_stencil_info.depthWriteEnable      = depth_stencil.depth_write_enable;
        depth_stencil_info.depthCompareOp        = convertCompareOp( depth_stencil.depth_compare_op );

        depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        depth_stencil_info.minDepthBounds       = 0.0;
        depth_stencil_info.maxDepthBounds       = 1.0;

        if ( depth_stencil.stencil_test_enable )
        {
            depth_stencil_info.stencilTestEnable    = VK_TRUE;
            depth_stencil_info.front.compareMask    = depth_stencil.front_op.compare_mask;
            depth_stencil_info.front.compareOp      = convertCompareOp( depth_stencil.front_op.compare_op );
            depth_stencil_info.front.depthFailOp    = convertStencilOp( depth_stencil.front_op.depth_fail_op );
            depth_stencil_info.front.failOp         = convertStencilOp( depth_stencil.front_op.fail_op );
            depth_stencil_info.front.passOp         = convertStencilOp( depth_stencil.front_op.pass_op );
            depth_stencil_info.front.reference      = depth_stencil.front_op.reference;
            depth_stencil_info.front.writeMask      = depth_stencil.front_op.write_mask;
            depth_stencil_info.back.depthFailOp     = convertStencilOp( depth_stencil.back_op.depth_fail_op );
            depth_stencil_info.back.compareOp       = convertCompareOp( depth_stencil.back_op.compare_op );
            depth_stencil_info.back.failOp          = convertStencilOp( depth_stencil.back_op.fail_op );
            depth_stencil_info.back.passOp          = convertStencilOp( depth_stencil.back_op.pass_op );
            depth_stencil_info.back.reference       = depth_stencil.back_op.reference;
            depth_stencil_info.back.writeMask       = depth_stencil.back_op.write_mask;
        }
        else
        {
            depth_stencil_info.stencilTestEnable = VK_FALSE;
            depth_stencil_info.front = {};
            depth_stencil_info.back = {};
        }
        return depth_stencil_info;
    }

    VkPipelineRasterizationStateCreateInfo getRasterizationStateCreateInfo( const RasterizationStateDesc& rasterization )
    {
        VkPipelineRasterizationStateCreateInfo state = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        state.lineWidth = rasterization.line_width;
        state.rasterizerDiscardEnable = rasterization.rasterizer_disable;
        state.depthClampEnable = rasterization.depth_clamp_enable;
        state.depthBiasEnable = rasterization.depth_bias_enable;
        state.depthBiasClamp = rasterization.depth_bias_clamp;
        state.cullMode = VK_CULL_MODE_NONE;///convertCullMode( desc.rasterization_state.cull_mode );
        state.frontFace = convertFrontFace( rasterization.front_face );
        state.depthBiasConstantFactor = rasterization.depth_bias_constant_factor;
        state.depthBiasSlopeFactor = rasterization.depth_bias_slope_factor;
        state.polygonMode = convertPolygonMode( rasterization.polygon_mode );
        return state;
    }

    VkPipelineMultisampleStateCreateInfo getMultisampleStateCreateInfo( const MultisampleStateDesc& multisample )
    {
        VkPipelineMultisampleStateCreateInfo state = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        state.sampleShadingEnable = multisample.sample_shading_enable;
        state.rasterizationSamples = convertSampleCount( multisample.rasterization_samples );// VK_SAMPLE_COUNT_1_BIT;
        state.minSampleShading = multisample.min_sample_shading;
        state.pSampleMask = 0;
        state.alphaToCoverageEnable = multisample.alpha_to_coverage_enable;
        state.alphaToOneEnable = multisample.alpha_to_one_enable;
        state.flags = 0;
        return state;
    }

    std::vector< VkVertexInputBindingDescription > getVertexInputBindingDescriptions( const VertexBufferLayout& vertex_input )
    {
        std::vector< VkVertexInputBindingDescription > vertex_bindings;
        for (int i=0; i<vertex_input.strides.size(); ++i)
        {
            for (int j=0; j<vertex_input.attributes.size(); ++j)
            {
                if (i == vertex_input.attributes[j].binding)
                {
                    vertex_bindings.push_back
                    ({
                        .inputRate = convertVertexInputRate( vertex_input.attributes[j].input_rate ),
                        .binding   = vertex_input.attributes[j].binding,
                        .stride    = vertex_input.strides[j],
                    });
                    break;
                }
            }
        }
        return vertex_bindings;
    }

    std::vector< VkVertexInputAttributeDescription > getVertexInputAttributeDescriptions( const VertexBufferLayout& vertex_input )
    {
        std::vector< VkVertexInputAttributeDescription > vertex_attributes;
        for (int i=0; i<vertex_input.attributes.size(); ++i)
        {
            vertex_attributes.push_back
            ({
                .location = vertex_input.attributes[i].location,
                .offset   = vertex_input.attributes[i].offset,
                .format   = convertShaderVarTypeToVkFormat( vertex_input.attributes[i].type ),
                .binding  = vertex_input.attributes[i].binding
            });
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


    std::vector< VkFormat > getOutput( const PipelineOutputs& outputs )
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

    VkPipelineRenderingCreateInfoKHR getRenderingCreateInfo( std::vector< VkFormat >& output_formats, const PipelineOutputs& outputs )
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
        const ColorBlendStateDesc& color_blend
    )
    {
        return VkPipelineColorBlendStateCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .attachmentCount = static_cast< uint32_t >( color_blend_attachment_states.size() ),
            .pAttachments = color_blend_attachment_states.data(),
            .logicOpEnable = (color_blend.logic_op_enable) ? VK_TRUE : VK_FALSE,
            .logicOp = vk::convertLogicOp( color_blend.logic_op )
        };
    }

    VkPipelineInputAssemblyStateCreateInfo getInputAssemblyStateCreateInfo( const InputAssemblyStateDesc& input_assembly )
    {
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        input_assembly_info.primitiveRestartEnable = VK_TRUE;
        input_assembly_info.topology = convertPrimitiveTopology( input_assembly.topology );
        return input_assembly_info;
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
                    .pName = shader->entryPoint().data(),
                    .stage = shader->stage(),
                    .module = shader->handle(),
                    .pNext = nullptr,
                    .flags = 0,
                }
            );
        }
        return shader_stages;
    }

    void ShaderPipeline::createGraphicsPipeline( const kege::PipelineCreateInfo& info, const ref::ShaderLayout& shader_layout )
    {
        // --- Translate Desc to Vulkan Structures ---
        VkGraphicsPipelineCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        create_info.layout = shader_layout->vk()->handle();

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 1. Shader Stages
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        std::vector< VkPipelineShaderStageCreateInfo > shader_stages = getShaderStageCreateInfo( info.shaders );
        create_info.stageCount = static_cast< uint32_t >( shader_stages.size() );
        create_info.pStages = shader_stages.data();

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 2. Vertex Input State
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        std::vector< VkVertexInputAttributeDescription > attributes = getVertexInputAttributeDescriptions( info.vertex_input );
        std::vector< VkVertexInputBindingDescription > bindings = getVertexInputBindingDescriptions( info.vertex_input );
        VkPipelineVertexInputStateCreateInfo vertex_input_info = getVertexInputStateCreateInfo(attributes, bindings);
        create_info.pVertexInputState = &vertex_input_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 3. Input Assembly
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        VkPipelineInputAssemblyStateCreateInfo input_assembly_info = getInputAssemblyStateCreateInfo( info.input_assembly );
        create_info.pInputAssemblyState = &input_assembly_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 4. Viewport State (can be dynamic) // Assuming non-dynamic for now
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        VkPipelineViewportStateCreateInfo viewport_state = getViewportStateCreateInfo();
        create_info.pViewportState = &viewport_state;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 5. Rasterization State
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        VkPipelineRasterizationStateCreateInfo rasterization_info = getRasterizationStateCreateInfo( info.rasterization );
        create_info.pRasterizationState = &rasterization_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 6. Multisample State
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        VkPipelineMultisampleStateCreateInfo multisample_info = getMultisampleStateCreateInfo( info.multisample );
        create_info.pMultisampleState = &multisample_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 7. Depth Stencil State
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info = getDepthStencilStateCreateInfo( info.depth_stencil );
        create_info.pDepthStencilState = &depth_stencil_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 8. Color Blend State
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        std::vector< VkPipelineColorBlendAttachmentState > color_blend_attachment_states = getColorBlendAttachmentState( info.color_blend );
        VkPipelineColorBlendStateCreateInfo color_blend_info = getColorBlendStateCreateInfo( color_blend_attachment_states, info.color_blend );
        create_info.pColorBlendState = &color_blend_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 9. Dynamic State (Optional but common for viewport/scissor)
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        std::vector< VkDynamicState > dynamic_states = std::vector< VkDynamicState >
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            //VK_DYNAMIC_STATE_LINE_WIDTH
        };
        VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
        dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state_create_info.dynamicStateCount = static_cast< uint32_t >( dynamic_states.size() );
        dynamic_state_create_info.pDynamicStates = dynamic_states.data();
        create_info.pDynamicState = &dynamic_state_create_info;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // 10. Render Pass / Rendering Info (Crucial!)
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        std::vector< VkFormat > color_attachmen_formats = getOutput( info.outputs );
        VkPipelineRenderingCreateInfoKHR rendering_info = getRenderingCreateInfo(color_attachmen_formats, info.outputs);

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

    void ShaderPipeline::createComputePipeline( const kege::PipelineCreateInfo& info, const ref::ShaderLayout& shader_layout )
    {
        std::vector< VkPipelineShaderStageCreateInfo > shader_stages = getShaderStageCreateInfo( info.shaders );

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
        const kege::PipelineCreateInfo& info,
        const ref::ShaderLayout& shader_layout
    )
    :   kege::ShaderPipeline( info, shader_layout )
    ,   _device( device )
    {
        if ( info.pipeline_type == PipelineType::Graphics )
        {
            createGraphicsPipeline( info, shader_layout );
        }
        else if ( info.pipeline_type == PipelineType::Compute )
        {
            createGraphicsPipeline( info, shader_layout );
        }

        if ( _device->_instance->isValidationEnabled() && !info.name.empty() )
        {
            _device->core().debugSetObjectName( (uint64_t)_handle, VK_OBJECT_TYPE_PIPELINE, info.name.c_str() );
        }
    }
}
