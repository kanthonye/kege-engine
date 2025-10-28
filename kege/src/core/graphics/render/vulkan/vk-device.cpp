//
//  vulkan-device.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/24/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-swapchain.hpp"
#include "vk-command-buffer.hpp"

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wdocumentation"
//#pragma clang diagnostic ignored "-Wnullability-completeness"
//// Define VMA implementation in exactly one cpp file
//#define VMA_IMPLEMENTATION
//#include <vk_mem_alloc.h>  // Include Vulkan Memory Allocator (VMA)
//#pragma clang diagnostic pop

namespace kege::vk{

    bool Device::submit
    (
        const std::vector< kege::SubmitInfo >& submit_infos,
        const ref::Semaphore& image_available,
        const ref::Semaphore& render_complete
    )
    {
        return _queue_manager->submit( submit_infos, image_available->vk(), render_complete->vk(), _frame_index );
    }

    bool  Device::submit( const kege::SubmitInfo& submit_info )
    {
        return _queue_manager->submit( submit_info );
    }

    bool Device::present( kege::Swapchain* swapchain, const ref::Semaphore& wait_sem )
    {
        if( _queue_manager->present( swapchain->vk(), wait_sem->vk() ) )
        {
            _frame_index = (_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
            return true;
        }
        return false;
    }

    int Device::getFrameIndex()const
    {
        return _frame_index;
    }

    bool Device::beginSubmit()
    {
        return _queue_manager->beginSubmit( _frame_index );
    }

    void Device::endSubmit()
    {
        _queue_manager->endSubmit();
    }


    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // ShaderLayout Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::ShaderLayout Device::createShaderLayout( const kege::ShaderLayoutDesc& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {};

        std::vector< VkDescriptorSetLayout > descriptor_set_layouts;
        descriptor_set_layouts.reserve( desc.shader_set_binding_points.size() );

        std::vector< ref::ShaderSetBindingPointLayout > shader_set_binding_layouts;
        shader_set_binding_layouts.resize( desc.shader_set_binding_points.size() );

        for (const auto& [set_index, shader_set_bindings] : desc.shader_set_binding_points )
        {
            vk::ShaderSetBindingPointLayout* shader_set_layout =
            _shader_set_manager.createShaderSetBindingLayout( shader_set_bindings );

            descriptor_set_layouts.push_back( shader_set_layout->handle );
            shader_set_binding_layouts.push_back( shader_set_layout );
        }

        std::vector<VkPushConstantRange> push_constant_ranges;
        push_constant_ranges.reserve( desc.push_constant_blocks.size() );

        for ( const auto& block : desc.push_constant_blocks )
        {
            push_constant_ranges.push_back
            ({
                .stageFlags = vk::convertShaderStageMask( block->stages ),
                .offset = block->offset,
                .size = block->size
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
        VkPipelineLayout pipeline_layout;
        if (( result = _manager.createPipelineLayout( &info, nullptr, &pipeline_layout ) ) != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return {};
        }

        if ( _instance->isValidationEnabled() && !desc.name.empty() )
        {
            _manager.debugSetObjectName
            ( (uint64_t)pipeline_layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, desc.name.data() );
        }

        return _shader_layouts.insert
        (
            new vk::ShaderLayout
            (
                pipeline_layout, this, desc.name,
                shader_set_binding_layouts,
                desc.push_constant_blocks
            )
        );
    }

    void Device::destroyShaderLayout( vk::ShaderLayout* layout )
    {
        if ( layout != nullptr )
        {
            if ( layout->_handle != VK_NULL_HANDLE )
            {
                _manager.destroyPipelineLayout( layout->_handle, nullptr );
                _shader_layouts.remove( layout );
            }
        }
    }

    kege::PipelineLayoutHandle Device::createPipelineLayout( const kege::PipelineLayoutDesc& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {-1};
        return {};// { _pipeline_layout_manager.createPipelineLayout( desc.name.data(), desc.descriptor_set_layouts, desc.push_constant_ranges ) };
    }

    const vk::PipelineLayout* Device::getPipelineLayout( int32_t pipeline_layout_id ) const
    {
        return _pipeline_layout_manager.getPipelineLayout( pipeline_layout_id );
    }

    void Device::destroyPipelineLayout( kege::PipelineLayoutHandle handle )
    {
        return _pipeline_layout_manager.destroyPipelineLayout( handle.id );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Pipeline Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    std::vector< PipelineHandle > Device::createGraphicsPipeline( const CreateShaderPipelineInfo& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {};
        std::vector< PipelineHandle > pipelines;

        for (int i=0; i<desc.pipelines.size(); ++i)
        {
            const kege::PipelineInfo& info = desc.pipelines[i];

            int pipeline_layout_handle = _pipeline_layout_manager.createPipelineLayout
            ( info.name.data(), info.layouts, info.push_constants );

            if ( pipeline_layout_handle < 0 )
            {
                kege::Log::error << "Invalid pipeline-layout in createGraphicsPipeline!"<<Log::nl;
                return {};
            }

            // --- Translate Desc to Vulkan Structures ---
            VkGraphicsPipelineCreateInfo graphics_create = {};
            graphics_create.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // 1. Shader Stages
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

            std::vector< VkPipelineShaderStageCreateInfo > shader_stages;
            for (int k=0; k<info.stages.size(); ++k)
            {
                Shader* shader = _shaders.get( desc.stages[ info.stages[k] ].id );

                shader_stages.push_back
                ({
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
                    convertShaderStage( shader->desc.stage ),
                    shader->shader_module,
                    shader->desc.entry_point.data()
                });
            }
            // Add other stages (Geometry, Tessellation) if present in desc...
            graphics_create.stageCount = static_cast< uint32_t >( shader_stages.size() );
            graphics_create.pStages = shader_stages.data();

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // 2. Vertex Input State
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

            std::vector< VkVertexInputBindingDescription > vertex_bindings;
            std::vector< VkVertexInputAttributeDescription > vertex_attributes;
            for (int i=0; i<info.vertex_input.attributes.size(); ++i)
            {
                vertex_attributes.push_back
                ({
                    .location = info.vertex_input.attributes[i].location,
                    .offset   = info.vertex_input.attributes[i].offset,
                    .format   = convertShaderVarTypeToVkFormat( info.vertex_input.attributes[i].type ),
                    .binding  = info.vertex_input.attributes[i].binding
                });
            }
            for (int i=0; i<info.vertex_input.strides.size(); ++i)
            {
                for (int j=0; j<info.vertex_input.attributes.size(); ++j)
                {
                    if (i == info.vertex_input.attributes[j].binding)
                    {
                        vertex_bindings.push_back
                        ({
                            .inputRate = convertVertexInputRate( info.vertex_input.attributes[j].input_rate ),
                            .binding   = info.vertex_input.attributes[j].binding,
                            .stride    = info.vertex_input.strides[j],
                        });
                        break;
                    }
                }
            }
            VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
            vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_bindings.size());
            vertexInputInfo.pVertexBindingDescriptions = vertex_bindings.empty() ? nullptr : vertex_bindings.data();
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attributes.size());
            vertexInputInfo.pVertexAttributeDescriptions = vertex_attributes.empty() ? nullptr : vertex_attributes.data();
            graphics_create.pVertexInputState = &vertexInputInfo;

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // 3. Input Assembly
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

            VkPipelineInputAssemblyStateCreateInfo input_assembly_info = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
            input_assembly_info.primitiveRestartEnable = info.states.input_assembly.primitive_restart_enable;
            input_assembly_info.topology = convertPrimitiveTopology( info.states.input_assembly.topology );
            graphics_create.pInputAssemblyState = &input_assembly_info;

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // 4. Viewport State (can be dynamic)
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // Assuming non-dynamic for now
            VkPipelineViewportStateCreateInfo viewport_state = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
            viewport_state.viewportCount = 1;
            viewport_state.pViewports = nullptr; // Set via dynamic state or provide dummy
            viewport_state.scissorCount = 1;
            viewport_state.pScissors = nullptr; // Set via dynamic state or provide dummy
            graphics_create.pViewportState = &viewport_state;

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // 5. Rasterization State
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

            VkPipelineRasterizationStateCreateInfo rasterization_info = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
            rasterization_info.lineWidth = info.states.rasterization.line_width;
            rasterization_info.rasterizerDiscardEnable = info.states.rasterization.rasterizer_disable;
            rasterization_info.depthClampEnable = info.states.rasterization.depth_clamp_enable;
            rasterization_info.depthBiasEnable = info.states.rasterization.depth_bias_enable;
            rasterization_info.depthBiasClamp = info.states.rasterization.depth_bias_clamp;
            rasterization_info.cullMode = VK_CULL_MODE_NONE;///convertCullMode( desc.rasterization_state.cull_mode );
            rasterization_info.frontFace = convertFrontFace( info.states.rasterization.front_face );
            rasterization_info.depthBiasConstantFactor = info.states.rasterization.depth_bias_constant_factor;
            rasterization_info.depthBiasSlopeFactor = info.states.rasterization.depth_bias_slope_factor;
            rasterization_info.polygonMode = convertPolygonMode( info.states.rasterization.polygon_mode );
            graphics_create.pRasterizationState = &rasterization_info;

            // 6. Multisample State
            VkPipelineMultisampleStateCreateInfo multisample_info = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
            multisample_info.sampleShadingEnable = info.states.multisample.sample_shading_enable;
            multisample_info.rasterizationSamples = convertSampleCount( info.states.multisample.rasterization_samples );// VK_SAMPLE_COUNT_1_BIT;
            multisample_info.minSampleShading = info.states.multisample.min_sample_shading;
            multisample_info.pSampleMask = 0;
            multisample_info.alphaToCoverageEnable = info.states.multisample.alpha_to_coverage_enable;
            multisample_info.alphaToOneEnable = info.states.multisample.alpha_to_one_enable;
            multisample_info.flags = 0;
            graphics_create.pMultisampleState = &multisample_info;

            // 7. Depth Stencil State
            VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {};
            depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depth_stencil_info.depthTestEnable       = info.states.depth_stencil.depth_test_enable;
            depth_stencil_info.depthWriteEnable      = info.states.depth_stencil.depth_write_enable;
            depth_stencil_info.depthCompareOp        = convertCompareOp( info.states.depth_stencil.depth_compare_op );

            depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
            depth_stencil_info.minDepthBounds       = 0.0;
            depth_stencil_info.maxDepthBounds       = 1.0;

            if ( info.states.depth_stencil.stencil_test_enable )
            {
                depth_stencil_info.stencilTestEnable    = VK_TRUE;
                depth_stencil_info.front.compareMask    = info.states.depth_stencil.front_op.compare_mask;
                depth_stencil_info.front.compareOp      = convertCompareOp( info.states.depth_stencil.front_op.compare_op );
                depth_stencil_info.front.depthFailOp    = convertStencilOp( info.states.depth_stencil.front_op.depth_fail_op );
                depth_stencil_info.front.failOp         = convertStencilOp( info.states.depth_stencil.front_op.fail_op );
                depth_stencil_info.front.passOp         = convertStencilOp( info.states.depth_stencil.front_op.pass_op );
                depth_stencil_info.front.reference      = info.states.depth_stencil.front_op.reference;
                depth_stencil_info.front.writeMask      = info.states.depth_stencil.front_op.write_mask;
                depth_stencil_info.back.depthFailOp     = convertStencilOp( info.states.depth_stencil.back_op.depth_fail_op );
                depth_stencil_info.back.compareOp       = convertCompareOp( info.states.depth_stencil.back_op.compare_op );
                depth_stencil_info.back.failOp          = convertStencilOp( info.states.depth_stencil.back_op.fail_op );
                depth_stencil_info.back.passOp          = convertStencilOp( info.states.depth_stencil.back_op.pass_op );
                depth_stencil_info.back.reference       = info.states.depth_stencil.back_op.reference;
                depth_stencil_info.back.writeMask       = info.states.depth_stencil.back_op.write_mask;
            }
            else
            {
                depth_stencil_info.stencilTestEnable = VK_FALSE;
                depth_stencil_info.front = {};
                depth_stencil_info.back = {};
            }
            graphics_create.pDepthStencilState      = &depth_stencil_info;

            // 8. Color Blend State
            std::vector< VkPipelineColorBlendAttachmentState > color_blend_attachment_states;
            for ( int i=0; i<info.states.color_blend.attachments.size(); i++ )
            {
                VkPipelineColorBlendAttachmentState attachment = {};
                attachment.blendEnable         = (info.states.color_blend.attachments[i].blend_enable)? VK_TRUE : VK_FALSE;

                attachment.colorBlendOp        = convertBlendOp( info.states.color_blend.attachments[i].color_blend_op );
                attachment.srcColorBlendFactor = convertBlendFactor( info. states.color_blend.attachments[i].src_color_blend_factor );
                attachment.dstColorBlendFactor = convertBlendFactor( info.states.color_blend.attachments[i].dst_color_blend_factor );

                attachment.alphaBlendOp        = convertBlendOp( info.states.color_blend.attachments[i].alpha_blend_op );
                attachment.dstAlphaBlendFactor = convertBlendFactor( info.states.color_blend.attachments[i].dst_alpha_blend_factor );
                attachment.srcAlphaBlendFactor = convertBlendFactor( info.states.color_blend.attachments[i].src_alpha_blend_factor );

                attachment.colorWriteMask      = convertColorComponentMask( info.states.color_blend.attachments[i].color_write_mask );
                color_blend_attachment_states.push_back( attachment );
            }
            VkPipelineColorBlendStateCreateInfo color_blend_info = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
            color_blend_info.attachmentCount = static_cast< int >( color_blend_attachment_states.size() );
            color_blend_info.pAttachments = color_blend_attachment_states.data();
            color_blend_info.logicOpEnable = (info.states.color_blend.logic_op_enable) ? VK_TRUE : VK_FALSE;
            color_blend_info.logicOp = vk::convertLogicOp( info.states.color_blend.logic_op );
            graphics_create.pColorBlendState = &color_blend_info;

            // 9. Dynamic State (Optional but common for viewport/scissor)
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
            graphics_create.pDynamicState = &dynamic_state_create_info;

            // 10. Pipeline Layout
            graphics_create.layout = _pipeline_layout_manager.getPipelineLayout( pipeline_layout_handle )->layout;

            // 11. Render Pass / Rendering Info (Crucial!)
            // We'll use Dynamic Rendering (VK_KHR_dynamic_rendering) as it's simpler for a render graph
            // Check if dynamic rendering is supported first!
            std::vector< VkFormat > color_attachmen_formats;
            for(Format fmt : info.outputs.color_attachment_formats )
            {
                VkFormat vk_fmt = convertFormat(fmt);
                if (vk_fmt == VK_FORMAT_UNDEFINED)
                {
                    kege::Log::error << "Invalid format passed for color attachment!"<<Log::nl;
                    _pipeline_layout_manager.destroyPipelineLayout( pipeline_layout_handle );
                    for (int k=0; k<info.stages.size(); ++k)
                    {
                        destroyShader( desc.stages[ info.stages[k] ] );
                    }
                    return {};
                }
                color_attachmen_formats.push_back( vk_fmt );
            }
            VkPipelineRenderingCreateInfoKHR rendering_info{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR};
            rendering_info.stencilAttachmentFormat = convertFormat(info.outputs.stencil_attachment_format);
            rendering_info.depthAttachmentFormat = convertFormat(info.outputs.depth_attachment_format);
            rendering_info.colorAttachmentCount = static_cast< uint32_t >( color_attachmen_formats.size() );
            rendering_info.pColorAttachmentFormats = ( !color_attachmen_formats.empty() ) ? color_attachmen_formats.data() : nullptr;
            // Chain this to pipelineInfo.pNext
            graphics_create.pNext = &rendering_info;
            graphics_create.renderPass = VK_NULL_HANDLE; // Must be NULL for dynamic rendering

            // 12. Base Pipeline (for derivatives, ignore for now)
            graphics_create.basePipelineHandle = VK_NULL_HANDLE;
            graphics_create.basePipelineIndex = -1;

            // --- Create Pipeline ---
            VkPipeline handle;
            VkResult result = vkCreateGraphicsPipelines( _device, VK_NULL_HANDLE, 1, &graphics_create, nullptr, &handle );
            if ( result != VK_SUCCESS )
            {
                kege::Log::error <<vkResultToString( result ) <<Log::nl;
                _pipeline_layout_manager.destroyPipelineLayout( pipeline_layout_handle );
                for (int k=0; k<info.stages.size(); ++k)
                {
                    destroyShader( desc.stages[ info.stages[k] ] );
                }
                return {};
            }

            // Add to runtime map and cache
            int id = _graphics_pipelines.gen();
            {
                vk::GraphicsPipeline* pipeline = _graphics_pipelines.get( id );
                pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
                pipeline->pipeline = handle;
                pipeline->pipeline_layout_id = pipeline_layout_handle;
            }

            if ( _instance->isValidationEnabled() && !info.name.empty() )
            {
                _manager.debugSetObjectName( (uint64_t)handle, VK_OBJECT_TYPE_PIPELINE, info.name.c_str() );
            }
            pipelines.push_back({ id });
        }
        for (int k=0; k<desc.stages.size(); ++k)
        {
            destroyShader({ desc.stages[k] });
        }
        return pipelines;
    }

    void Device::destroyGraphicsPipeline(PipelineHandle handle)
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        if ( _graphics_pipelines.get( handle.id ) != nullptr )
        {
            waitIdle();
            vkDestroyPipeline( _device, _graphics_pipelines.get( handle.id )->pipeline, nullptr );
            _graphics_pipelines.get( handle.id )->pipeline = VK_NULL_HANDLE;
            _graphics_pipelines.free( handle.id );
        }
    }

    kege::PipelineHandle Device::createComputePipeline( const kege::ComputePipelineDesc& desc )
    {
        const vk::PipelineLayout* layout = _pipeline_layout_manager.getPipelineLayout( desc.pipeline_layout.id );
        if ( layout == nullptr )
        {
            kege::Log::error << "Invalid pipeline-layout in createGraphicsPipeline!"<<Log::nl;
            return {-1};
        }

        Shader* shader = _shaders.get( desc.compute_shader.id );

        VkPipelineShaderStageCreateInfo create_shader_info = {};
        create_shader_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        create_shader_info.stage = convertShaderStage( shader->desc.stage );
        create_shader_info.module = shader->shader_module;
        create_shader_info.pName = shader->desc.entry_point.data();

        VkComputePipelineCreateInfo create_pipeline_info{};
        create_pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        create_pipeline_info.layout = layout->layout;
        create_pipeline_info.stage = create_shader_info;

        VkPipeline pipeline;
        VkResult result = vkCreateComputePipelines
        (
            _device,
            VK_NULL_HANDLE,
            1,
            &create_pipeline_info,
            nullptr,
            &pipeline
        );

        if ( result != VK_SUCCESS )
        {
            kege::Log::error << "createComputePipeline" <<Log::nl;
            return {-1};
        }

        int32_t id = _compute_pipelines.gen();
        ComputePipeline* compute = _compute_pipelines.get( id );
        compute->pipeline = pipeline;
        compute->desc = desc;
        return { id };
    }

    void Device::destroyComputePipeline( kege::PipelineHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        if ( _compute_pipelines.get( handle.id ) != nullptr )
        {
            waitIdle();
            vkDestroyPipeline( _device, _compute_pipelines.get( handle.id )->pipeline, nullptr );
            _compute_pipelines.get( handle.id )->pipeline = VK_NULL_HANDLE;
            _compute_pipelines.free( handle.id );
        }
    }


    const vk::DescriptorSetLayout* Device::getDescriptorSetLayout( int32_t layout )
    {
        return _pipeline_layout_manager.getDescriptorSetLayout( layout );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Swapchain Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    kege::Swapchain* Device::createSwapchain( const kege::SwapchainDesc& desc )
    {
        VkResult result;
        vk::Swapchain* swapchain = _swapchains.insert( new vk::Swapchain( this ) );
        if (( result = swapchain->create( desc ) ) != VK_SUCCESS )
        {
            delete swapchain;
            return nullptr;
        }
        return swapchain;
    }

    void Device::destroySwapchain( kege::Swapchain* swapchain )
    {
        if ( swapchain != nullptr )
        {
            _swapchains.remove( swapchain->vk() );
            swapchain->vk()->destroy();
            swapchain->vk()->_device = nullptr;
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // CommandBuffer Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    kege::CommandBuffer* Device::createCommandBuffer( kege::QueueType type )
    {
        vk::CommandBuffer* command_buffer = new vk::CommandBuffer;
        _command_buffers.insert( command_buffer );

        command_buffer->_command_pool = _queue_manager->_command_pools[ type ];
        command_buffer->_device = this;
        command_buffer->_is_recording = false;
        command_buffer->_queue_type = type;

        VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        info.commandPool = _queue_manager->_command_pools[ type ];
        info.commandBufferCount = 1;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        if ( vkAllocateCommandBuffers( _device, &info, &command_buffer->_handle ) != VK_SUCCESS )
        {
            // error
            return nullptr;
        }

        return command_buffer;
    }

    void Device::destroyCommandBuffer( kege::CommandBuffer* cmb )
    {
        if( cmb )
        {
            _command_buffers.remove( cmb->vk() );
            if ( cmb->vk()->handle() != VK_NULL_HANDLE )
            {
                waitIdle();
                vkFreeCommandBuffers( _device, cmb->vk()->_command_pool, 1, &cmb->vk()->handle() );
            }
            delete cmb;
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Uniform Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    int  Device::makeSet( const UniformDescriptorSet& descriptors, const UniformResourceSet& resources )
    {
        return _pipeline_layout_manager.makeSet( descriptors, resources );
    }

    bool Device::updateSet( int handle, const UniformResourceSet& resources )
    {
        return _pipeline_layout_manager.updateSet( handle, resources );
    }

    int  Device::allocateSet( const UniformDescriptorSet& descriptors )
    {
        return _pipeline_layout_manager.allocateSet( descriptors );
    }

    void Device::freeSet( int set )
    {
        return _pipeline_layout_manager.freeSet( set );
    }

    const vk::DescriptorSet* Device::getSet( int32_t descriptor_id ) const
    {
        return _pipeline_layout_manager.getSet( descriptor_id );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // DescriptorSetLayout Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    UniformSetLayout Device::getUniformSetLayout( const UniformDescriptors& desc )
    {
        return { _pipeline_layout_manager.getDescriptorSetLayoutID( desc, false ) };
    }

    UniformSetLayout Device::createUniformSetLayout( const UniformDescriptors& desc )
    {
        return { _pipeline_layout_manager.createUniformSetLayout( desc ) };
    }

    void Device::destroyUniformSetLayout( const UniformSetLayout& layout )
    {
        _pipeline_layout_manager.destroyDescriptorSetLayout( layout.id );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Image Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::Image Device::createImage( const kege::Image::Desc& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {};
        return _images.insert( new vk::Image( this, desc ) );
    }

    void Device::destroyImage( vk::Image* image )
    {
        if ( image == nullptr ) return;

        waitIdle();

        if ( image->_image != VK_NULL_HANDLE && !image->_swapchain_owned )
        {
            _manager.destroyImage( image->_image );
        }
        if ( image->_memory != VK_NULL_HANDLE )
        {
            _manager.destroyMemory( image->_memory );
        }

        if ( image->_view != VK_NULL_HANDLE )
        {
            _manager.destroyImageView( image->_view );
        }

        image->_device = nullptr;
        _images.remove( image );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Sampler Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::Sampler Device::createSampler( const kege::SamplerDesc& desc )
    {
        return _samplers.insert( new vk::Sampler( this, desc ) );
    }

    void Device::destroySampler( vk::Sampler* sampler )
    {
        if ( sampler == nullptr ) return;
        if ( sampler->_sampler != VK_NULL_HANDLE )
        {
            _manager.destroySampler( sampler->_sampler );
            sampler->_device = nullptr;
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Buffer Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::Buffer Device::createBuffer(const kege::BufferDesc& desc)
    {
        vk::Buffer* buffer = new vk::Buffer( this, desc );
        return _buffers.insert( buffer );
    }

    void Device::destroyBuffer( vk::Buffer* buffer )
    {
        if ( buffer != nullptr )
        {
            vk::Buffer* b = buffer->vk();
            for( int i = 0; i < b->_frames; ++i )
            {
                _manager.destroyBuffer( b->_sources[ i ].handle );
                _manager.destroyMemory( b->_sources[ i ].memory );
            }
            _buffers.remove( buffer );
            b->_device = nullptr;
        }
    }

    void Device::copyBuffer( Buffer::Source& dst, Buffer::Source& src, uint64_t size )
    {
       VkBufferCopy copy_region{};
       copy_region.srcOffset = 0; // Optional
       copy_region.dstOffset = 0; // Optional
       copy_region.size = size;

       VkCommandBuffer command_buffer = _queue_manager->beginTransferQueueCommandBuffer();
       vkCmdCopyBuffer( command_buffer, dst.handle, src.handle, 1, &copy_region );
       _queue_manager->endTransferQueueCommandBuffer( command_buffer );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Buffer View Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

//    void Device::destroyBufferView(kege::BufferViewHandle handle)
//    {
//    }

    void Device::copyBufferToImage
    (
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height,
        VkImageAspectFlags aspect_flags,
        VkImageLayout old_layout,
        VkImageLayout new_layout
    )
    {
        VkCommandBuffer command_buffer = _queue_manager->beginTransferQueueCommandBuffer();
        _manager.copyBufferToImage
        (
            command_buffer,
            buffer,
            image,
            width, height,
            aspect_flags,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
        _queue_manager->endTransferQueueCommandBuffer( command_buffer );
    }

    bool Device::copyBufferToImage
    (
        uint64_t size,
        const void* data,
        VkImage image,
        uint32_t width,
        uint32_t height,
        VkImageAspectFlags aspect_flags,
        VkImageLayout old_layout,
        VkImageLayout new_layout
    )
    {
        /**
         * create a seperate shared buffer between the CPU & GPU that contains the data.
         */
        vk::Buffer::Source source = {};
        VkResult result = core().createBuffer
        (
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            size, data, &source.handle, &source.memory
        );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result ) << kege::Log::nl;
            return  false;
        }
        
        copyBufferToImage
        (
            source.handle,
            image, width, height, aspect_flags,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
        core().destroyBuffer( source.handle );
        core().destroyMemory( source.memory );
        return true;
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Shader Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    kege::ShaderHandle Device::createShader( const kege::ShaderDesc& desc )
    {
         if ( _device == VK_NULL_HANDLE || desc.byte_code.empty() ) return {-1};

        kege::ShaderHandle handle = { _shaders.gen() };
        vk::Shader* shader = _shaders.get( handle.id );

        shader->desc = desc; // Store original desc if needed later

        VkShaderModuleCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = desc.byte_code.size() * sizeof( desc.byte_code[0] );
        create_info.pCode = desc.byte_code.data();
        VkResult result;
        if ((result = vkCreateShaderModule( _device, &create_info, nullptr, &shader->shader_module ) ) != VK_SUCCESS )
        {
            kege::Log::error << "Failed to create shader module!" <<Log::nl;
            return {-1};
        }

        // Set Debug Name (requires VK_EXT_debug_utils)
        if ( _instance->isValidationEnabled() && !desc.name.empty() )
        {
            _manager.debugSetObjectName( (uint64_t)shader->shader_module, VK_OBJECT_TYPE_SHADER_MODULE, shader->desc.name.data() );
        }

        return handle;
    }

    void Device::destroyShader( kege::ShaderHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        if ( _shaders.get( handle.id ) != nullptr )
        {
            waitIdle();
            vkDestroyShaderModule(_device, _shaders.get( handle.id )->shader_module, nullptr);
            _shaders.free( handle.id );
        }
    }

    //-------------------------------------------------------------------------
    // Descriptor Set Lifecycle
    //-------------------------------------------------------------------------

    bool Device::updateUniformSets( const std::vector< int >& handles, const UniformSets& resource_sets )
    {
        return 0;//_pipeline_layout_manager.updateUniformSets( handles, resource_sets );
    }

    bool Device::updateUniformSet( int handle, const UniformSet& resource_set )
    {
        return 0;//_pipeline_layout_manager.updateUniformSet( handle, resource_set );
    }

    std::vector< int > Device::allocateUniformSets( const UniformSetsDesc& desc )
    {
        return {};//_pipeline_layout_manager.allocateUniformSets( desc );
    }

    int Device::allocateUniformSet( const UniformSetDesc& desc )
    {
        return 0;//_pipeline_layout_manager.allocateUniformSet( desc );
    }

    // --- Wait Idle ---
    void Device::waitIdle()
    {
        if ( _device == VK_NULL_HANDLE || _device == VK_NULL_HANDLE) return;
        vkDeviceWaitIdle(_device);
    }

    /*
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     * Create Semaphore
     * Creates a VkSemaphore that can be used to synchronize GPU operations.
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     */

    ref::Semaphore Device::createSemaphore()
    {
        return _semaphores.insert( new vk::Semaphore( _manager.createSemaphore(), this ) );
    }

    void Device::destroySemaphore( kege::Semaphore* semaphore )
    {
        if ( semaphore != nullptr )
        {
            vk::Semaphore* sem = static_cast< vk::Semaphore* >( semaphore );
            if ( sem->device != nullptr )
            {
                _manager.destroySemaphore( sem->handle );
                sem->device = nullptr;
            }
        }
    }

    /*
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     * Create Fence
     * Creates a VkFence that can be used to synchronize CPU and GPU operations.
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     */
    
    ref::Fence Device::createFence( bool initially_signaled )
    {
        return _fences.insert( new vk::Fence( _manager.createFence( initially_signaled ), this ) );
    }
    
    void Device::destroyFence( kege::Fence* fence )
    {
        if ( fence != nullptr )
        {
            vk::Fence* f = static_cast< vk::Fence* >( fence );
            if ( f->device != nullptr )
            {
                _manager.destroyFence( f->handle );
                f->device = nullptr;
            }
        }
    }

    const Shader* Device::getShader(ShaderHandle handle) const
    {
        return _shaders.get( handle.id );
    }

    const GraphicsPipeline* Device::getGraphicsPipeline(PipelineHandle handle) const
    {
        return _graphics_pipelines.get( handle.id );
    }

    const ComputePipeline* Device::getComputePipeline(PipelineHandle handle) const
    {
        return _compute_pipelines.get( handle.id );
    }

//    VkSurfaceKHR Device::surface()
//    {
//        return _surface;
//    }
    
    VkDevice Device::handle()
    {
        return _device;
    }

    bool Device::initialize( vk::PhysicalDevice* physical_device )
    {
        _surface = physical_device->getSurface();
        _physical_device = physical_device;
        _instance = physical_device->getInstance();
        if ( physical_device->getHandle() == VK_NULL_HANDLE )
        {
            kege::Log::error << "Invalid PhysicalDevice passed to createLogicalDevice."<<Log::nl;
            return false;
        }

        _queue_family_indices = findQueueFamilies( physical_device->getHandle(), _surface );
        if ( !_queue_family_indices.graphics_family.has_value() ) // graphics is always required
        {
            kege::Log::error << "Failed to find a suitable graphics queue family on the selected device."<<Log::nl;
             return false;
        }

        _device = _manager.createDevice( _physical_device, _queue_family_indices );
        if ( _device == VK_NULL_HANDLE )
        {
            return false;
        }

        _queue_manager = new QueueManager;
        _queue_manager->initialize( this, _queue_family_indices );

        KEGE_LOG_INFO <<"- " << "Vulkan Logical Device created." <<"\n";

        _pipeline_layout_manager.initialize( _instance, this );
        return true;
    }

    void Device::shutdown()
    {
        if ( _device == VK_NULL_HANDLE ) return;

        waitIdle(); // Ensure GPU is idle before destroying anything

        for (vk::Buffer* b = _buffers.head; b != nullptr; b = b->next )
            destroyBuffer( b );
        _buffers.clear();

        for (vk::Image* i = _images.head; i != nullptr; i = i->next )
            destroyImage( i );
        _images.clear();

        for (vk::Sampler* s = _samplers.head; s != nullptr; s = s->next )
            destroySampler( s );
        _samplers.clear();

        for (vk::ShaderLayout* s = _shader_layouts.head; s != nullptr; s = s->next )
            destroyShaderLayout( s );
        _shader_layouts.clear();

        for (vk::Semaphore* s = _semaphores.head; s != nullptr; s = s->next )
            destroySemaphore( s );
        _semaphores.clear();

        for (vk::Fence* f = _fences.head; f != nullptr; f = f->next )
            destroyFence( f );
        _fences.clear();

        for ( int32_t i = 0; i < _shaders.count(); ++i )
        {
            if ( _shaders.get( i ) != nullptr )
            {
                destroyShader({ i });
            };
        }

        for ( int32_t i = 0; i < _graphics_pipelines.count(); ++i )
        {
            if ( _graphics_pipelines.get( i ) != nullptr )
            {
                destroyGraphicsPipeline({ i });
            };
        }
        for ( int32_t i = 0; i < _compute_pipelines.count(); ++i )
        {
            if ( _compute_pipelines.get( i ) != nullptr )
            {
                destroyComputePipeline({ i });
            };
        }
        // Destroy user-created resources first
        _pipeline_layout_manager.shutdown();

        for (Swapchain* sc = _swapchains.head; sc != nullptr; sc = sc->next )
            destroySwapchain( sc );
        _swapchains.clear();

        if ( _queue_manager )
        {
            _queue_manager->shutdown();
            delete _queue_manager;
            _queue_manager = nullptr;
        }

        // Destroy Device
        if (_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(_device, nullptr);
            _device = VK_NULL_HANDLE;
        }
        
        _device = VK_NULL_HANDLE;
        _physical_device = nullptr;
        _instance = nullptr;
        KEGE_LOG_INFO << "Device Shutdown Complete."<<Log::nl;
    }

    vk::Manager& Device::core()
    {
        return _manager;
    }

    Device::~Device()
    {
        if ( _device != VK_NULL_HANDLE )
        {
            shutdown(); // Ensure shutdown is called if not explicitly done
        }
    }

    Device::Device()
    :   vkSetDebugUtilsObjectName( nullptr )
    ,   _frame_index( 0 )
    {
    }

}
