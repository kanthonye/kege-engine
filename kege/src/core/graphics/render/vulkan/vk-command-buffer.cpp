//
//  vulkan-command-recorder.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#include <stdexcept> // For potential exceptions
#include <iostream>  // For errors/warnings
#include <vector>
#include <assert.h>
#include "vk-device.hpp"
#include "vk-command-buffer.hpp"

namespace kege::vk{

    bool CommandBuffer::bind( const kege::BindSet& indexed_set )
    {
        return bind( indexed_set.index, indexed_set.set->vk() );
    }

    bool CommandBuffer::bind( int32_t set_index, const vk::ShaderSet* set )
    {
        if ( set == nullptr )
        {
            Log::error << "INVALID_SHADER_BINDING -> " << Log::nl;
            return false;
        }
        if ( set_index < 0 )
        {
            Log::error << "INVALID_SET_INDEX -> " << Log::nl;
            return false;
        }

        const vk::DescriptorSet& descriptor = set->descriptor();
        int frame_index = _device->getFrameIndex();

        vkCmdBindDescriptorSets
        (
            _handle,
            _current_pipeline_bindpoint,
            _curr_pipeline_layout->handle(),
            set_index,
            1, &descriptor.set[ frame_index % descriptor.frames ],
            0, nullptr
        );
        _sets_bind_state |= (1ULL << set_index);
        return true;
    }

    void CommandBuffer::setPushBlock( ShaderStageFlag stages, uint32_t offset, uint32_t size, const void *data )
    {
        VkShaderStageFlags stage_flags = convertShaderStageMask( stages );
        const vk::ShaderLayout* layout = _curr_bind_pipeline->getShaderLayout()->vk();
        vkCmdPushConstants( _handle, layout->handle(), stage_flags, offset, size, data );
    }

    void CommandBuffer::bindShaderPipeline( const ref::ShaderPipeline& shader_pipeline )
    {
        _curr_bind_pipeline = shader_pipeline->vk();
        if ( !_curr_bind_pipeline )
        {
            _curr_bind_pipeline = nullptr;
            kege::Log::error << "Invalid pipeline handle in bindGraphicsPipeline." <<Log::nl;
             return;
        }

        vkCmdBindPipeline( _handle, VK_PIPELINE_BIND_POINT_GRAPHICS, _curr_bind_pipeline->handle() );

        if ( _curr_bind_pipeline->getType() == kege::PipelineType::Graphics ) {
            _current_pipeline_bindpoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        }
        else if ( _curr_bind_pipeline->getType() == kege::PipelineType::Compute ) {
            _current_pipeline_bindpoint = VK_PIPELINE_BIND_POINT_COMPUTE;
        }
        _curr_pipeline_layout = _curr_bind_pipeline->getShaderLayout()->vk();
    }

    void vk::CommandBuffer::transitionImageLayout
    (
        ref::Image image,
        kege::ImageLayout old_layout,
        kege::ImageLayout new_layout
    )
    {
        VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkAccessFlags srcAccess = 0;
        VkAccessFlags dstAccess = 0;

        // Source
        switch (old_layout)
        {
            case kege::ImageLayout::Undefined:
                srcAccess = 0;
                srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                break;
            case kege::ImageLayout::Color:
                srcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;
            case kege::ImageLayout::ShaderRead:
                srcAccess = VK_ACCESS_SHADER_READ_BIT;
                srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
            case kege::ImageLayout::Present:
                srcAccess = VK_ACCESS_MEMORY_READ_BIT;
                srcStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                break;
            case kege::ImageLayout::Depth:
            case kege::ImageLayout::Stencil:
            case kege::ImageLayout::DepthRead:
            case kege::ImageLayout::StencilRead:
            case kege::ImageLayout::DepthStencil:
            case kege::ImageLayout::DepthStencilRead:
            case kege::ImageLayout::Depth_StencilRead:
            case kege::ImageLayout::DepthRead_Stencil:
                srcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                srcStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;
            case kege::ImageLayout::TransferSrc:
                srcAccess = VK_ACCESS_TRANSFER_READ_BIT;
                srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case kege::ImageLayout::TransferDst:
                srcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case kege::ImageLayout::HostRead:
                srcAccess = VK_ACCESS_HOST_READ_BIT;
                srcStage = VK_PIPELINE_STAGE_HOST_BIT;
                break;
            case kege::ImageLayout::HostWrite:
                srcAccess = VK_ACCESS_HOST_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_HOST_BIT;
                break;
            default:
                break;
        }

        // Destination
        switch (new_layout)
        {
            case kege::ImageLayout::Color:
                dstAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;
            case kege::ImageLayout::ShaderRead:
                dstAccess = VK_ACCESS_SHADER_READ_BIT;
                dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
            case kege::ImageLayout::Present:
                dstAccess = 0;
                dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                break;
            case kege::ImageLayout::Depth:
            case kege::ImageLayout::Stencil:
            case kege::ImageLayout::DepthRead:
            case kege::ImageLayout::StencilRead:
            case kege::ImageLayout::DepthStencil:
            case kege::ImageLayout::DepthStencilRead:
            case kege::ImageLayout::Depth_StencilRead:
            case kege::ImageLayout::DepthRead_Stencil:
                dstAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;
            case kege::ImageLayout::TransferSrc:
                dstAccess = VK_ACCESS_TRANSFER_READ_BIT;
                dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case kege::ImageLayout::TransferDst:
                dstAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case kege::ImageLayout::HostRead:
                dstAccess = VK_ACCESS_HOST_READ_BIT;
                dstStage = VK_PIPELINE_STAGE_HOST_BIT;
                break;
            case kege::ImageLayout::HostWrite:
                dstAccess = VK_ACCESS_HOST_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_HOST_BIT;
                break;
            default:
                break;
        }

//        vk::Image* image = _device->_textures.get(image.id);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = vk::convertImageLayout(old_layout);
        barrier.newLayout = vk::convertImageLayout(new_layout);
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image->vk()->_image;
        barrier.subresourceRange.aspectMask = image->vk()->_aspect;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = srcAccess;
        barrier.dstAccessMask = dstAccess;

        vkCmdPipelineBarrier(
            _handle,
            srcStage,
            dstStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        image->vk()->_current_layout = barrier.newLayout;
    }

    CommandEncoder* vk::CommandBuffer::createCommandEncoder()
    {
        CommandEncoder* encoder = nullptr;
        if ( _encoder_count >= _command_encoders.size() )
        {
            encoder = new CommandEncoder;
            _command_encoders.push_back( encoder );
            encoder->_command_buffer = this;

            VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
            info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
            info.commandPool = _command_pool;
            info.commandBufferCount = 1;
            if ( vkAllocateCommandBuffers( _device->handle(), &info, &encoder->_handle ) != VK_SUCCESS )
            {
                return nullptr;
            }
        }
        else
        {
            encoder = _command_encoders[ _encoder_count ];
        }
        _encoder_count++;

        //VkResult result;
        vkResetCommandBuffer( encoder->_handle, 0 );

        VkCommandBufferInheritanceRenderingInfo inheritance_rendering_info = {};
        inheritance_rendering_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_RENDERING_INFO;
        inheritance_rendering_info.pNext = nullptr;
        inheritance_rendering_info.flags = 0; // Optional (e.g., VK_RENDERING_CONTENTS_SECONDARY_COMMAND_BUFFERS_BIT)
        inheritance_rendering_info.viewMask = 0; // For multiview, otherwise 0
        inheritance_rendering_info.colorAttachmentCount = static_cast< uint32_t >( _color_attachment_formats.size() );
        inheritance_rendering_info.pColorAttachmentFormats = _color_attachment_formats.data();
        inheritance_rendering_info.depthAttachmentFormat = _depth_attachment_format;
        inheritance_rendering_info.stencilAttachmentFormat = _stencil_attachment_format;
        if ( inheritance_rendering_info.colorAttachmentCount != 0 )
        {
            inheritance_rendering_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        }

        VkCommandBufferInheritanceInfo inheritance_info = {};
        inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritance_info.pNext = &inheritance_rendering_info;
        inheritance_info.renderPass = VK_NULL_HANDLE;  // Not used with dynamic rendering
        inheritance_info.subpass = 0;                  // Not used
        inheritance_info.framebuffer = VK_NULL_HANDLE; // Not used
        inheritance_info.occlusionQueryEnable = VK_FALSE;
        inheritance_info.queryFlags = 0;
        inheritance_info.pipelineStatistics = 0;

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.pNext = nullptr;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Required for secondary cmd buffers in render pass
        begin_info.pInheritanceInfo = &inheritance_info;

        vkBeginCommandBuffer( encoder->_handle, &begin_info );
        return encoder;
    }

    bool vk::CommandBuffer::beginCommands( CommandBufferUsage usage )
    {
        if (_is_recording)
        {
            Log::warning << "CommandBuffer::begin called while already recording." <<Log::nl;
            return false;
        }
        if ( _handle == VK_NULL_HANDLE ) return false; // Safety check

        _encoder_count = 0;

        vkResetCommandBuffer( _handle, 0 );
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;
        if ( usage <<= CommandBufferUsage::OneTimeSubmit )
        {
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        }
        else if ( usage <<= CommandBufferUsage::SimultaneousUse )
        {
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        }
        else if ( usage <<= CommandBufferUsage::RenderPassContinue )
        {
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        }

        begin_info.pInheritanceInfo = nullptr; // Not a secondary command buffer

        VkResult result = vkBeginCommandBuffer( _handle, &begin_info );
        if (result != VK_SUCCESS)
        {
            kege::Log::error << "vkBeginCommandBuffer failed! Error code: " << result<<Log::nl;
            return false;
        }

        _is_recording = true;
        _curr_bind_pipeline = nullptr;
        return true;
    }

    bool vk::CommandBuffer::endCommands()
    {
        if (!_is_recording)
        {
            Log::warning << "CommandBuffer::end called while not recording."<<Log::nl;
            return false;
        }
        if (!_handle) return false;

        VkResult result = vkEndCommandBuffer(_handle);
        if (result != VK_SUCCESS)
        {
            kege::Log::error << "vkEndCommandBuffer failed! Error code: " << result <<Log::nl;
            // State might be corrupted, but set recording to false anyway
            _is_recording = false;
            return false;
        }
        _is_recording = false;
        return true;
    }

    // --- Dynamic Rendering ---
    void vk::CommandBuffer::beginRendering(const RenderingInfo& rendering_info)
    {
        if (!_is_recording || !_handle) return;
        // Translate abstract info to Vulkan structs
        std::vector< VkRenderingAttachmentInfo > color_attachments;
        color_attachments.reserve( rendering_info.color_attachments.size() );

        _color_attachment_formats.clear();
        for (const RenderingAttachmentInfo& attach_info : rendering_info.color_attachments)
        {
            if (!attach_info.image)
            {
                kege::Log::error << "Invalid color attachment handle in beginRendering." <<Log::nl;
                 continue; // Skip this attachment
            }

            VkRenderingAttachmentInfo color_attachment = {};
            color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR; // Or core version
            color_attachment.imageView = attach_info.image->vk()->_view;
            // **** CRITICAL: Layout must be correct before this call ****
            // **** Render Graph barrier logic ensures this! ****
            color_attachment.imageLayout = convertImageLayout( attach_info.image_layout );
            // vk_attach_info.resolveImageView = ... // Handle resolve later
            // vk_attach_info.resolveImageLayout = ...
            color_attachment.loadOp = convertAttachmentLoadOp( attach_info.load_op ); // Need TranslateLoadOp helper
            color_attachment.storeOp = convertAttachmentStoreOp( attach_info.store_op ); // Need TranslateStoreOp helper

            color_attachment.clearValue.color.float32[0] = attach_info.clear_value.color[0];
            color_attachment.clearValue.color.float32[1] = attach_info.clear_value.color[1];
            color_attachment.clearValue.color.float32[2] = attach_info.clear_value.color[2];
            color_attachment.clearValue.color.float32[3] = attach_info.clear_value.color[3];

            color_attachments.push_back( color_attachment );
            _color_attachment_formats.push_back( attach_info.image->vk()->_vkformat );
        }

        VkRenderingAttachmentInfo depth_attachment = {};
        depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        const Image* depth_image = nullptr;
        if ( rendering_info.depth_attachment.has_value() )
        {
            const auto& attach_info = rendering_info.depth_attachment.value();
            depth_image = attach_info.image->vk();
            if (depth_image)
            {
                depth_attachment.imageView = depth_image->vk()->_view;
                depth_attachment.imageLayout = convertImageLayout( attach_info.image_layout );// stateToVkLayout(attach_info.initial_layout, depth_image->desc.format);
                depth_attachment.loadOp = convertAttachmentLoadOp(attach_info.load_op);
                depth_attachment.storeOp = convertAttachmentStoreOp(attach_info.store_op);
                depth_attachment.clearValue.depthStencil.depth = attach_info.clear_value.depth_stencil.depth;
            }
            else
            {
                kege::Log::error << "Invalid depth attachment handle in beginRendering." <<Log::nl;
            }
            _depth_attachment_format = depth_image->vk()->_vkformat;
        }

        VkRenderingAttachmentInfo stencil_attachment = {};
        stencil_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        const Image* stencil_image = nullptr;
        if ( rendering_info.stencil_attachment.has_value() )
        {
            const auto& attach_info = rendering_info.stencil_attachment.value();
            stencil_image = attach_info.image->vk();
            if ( stencil_image )
            {
                stencil_attachment.imageView = stencil_image->vk()->_view;
                stencil_attachment.imageLayout = convertImageLayout( attach_info.image_layout );// stateToVkLayout(attach_info.initial_layout, stencil_image->desc.format);
                stencil_attachment.loadOp = convertAttachmentLoadOp(attach_info.load_op);
                stencil_attachment.storeOp = convertAttachmentStoreOp(attach_info.store_op);
                stencil_attachment.clearValue.depthStencil.depth = attach_info.clear_value.depth_stencil.depth;
            }
            else
            {
                kege::Log::error << "Invalid depth attachment handle in beginRendering." <<Log::nl;
            }
            _stencil_attachment_format = stencil_image->vk()->_vkformat;
        }
         // Similar setup for stencil if present and potentially separate from depth...

        VkRenderingInfo vk_rendering_info = {};
        vk_rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR; // Or core version
        vk_rendering_info.renderArea.offset = {rendering_info.render_area.offset.x, rendering_info.render_area.offset.y};
        vk_rendering_info.renderArea.extent = {rendering_info.render_area.extent.width, rendering_info.render_area.extent.height};
        vk_rendering_info.layerCount = rendering_info.layer_count;
        vk_rendering_info.viewMask = 0; // Not handling multiview yet
        vk_rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachments.size());
        vk_rendering_info.pColorAttachments = color_attachments.empty() ? nullptr : color_attachments.data();
        vk_rendering_info.pDepthAttachment = (depth_image) ? &depth_attachment : nullptr;
        vk_rendering_info.pStencilAttachment = (stencil_image) ? &stencil_attachment : nullptr; // Handle separate stencil later
        vk_rendering_info.flags = VK_RENDERING_CONTENTS_SECONDARY_COMMAND_BUFFERS_BIT;

        // Use function pointer for KHR or core version based on device support
        if ( vkCmdBeginRenderingPfn == nullptr )
        {
            vkCmdBeginRenderingPfn = (PFN_vkCmdBeginRendering)vkGetDeviceProcAddr(_device->_device, "vkCmdBeginRendering");
            if (!vkCmdBeginRenderingPfn)
            {
                // Try the KHR version as a fallback
                vkCmdBeginRenderingPfn = (PFN_vkCmdBeginRendering)vkGetDeviceProcAddr(_device->_device, "vkCmdBeginRenderingKHR");
            }
        }
        assert(vkCmdBeginRenderingPfn && "vkCmdBeginRendering and vkCmdBeginRenderingKHR are both NULL!");
        // If it's not null now, use this pointer
        vkCmdBeginRenderingPfn( _handle, &vk_rendering_info );
    }

    void vk::CommandBuffer::endRendering()
    {
        if (!_is_recording || !_handle) return;

        // --- End Secondary Commands Before Ending The Dynamic Rendering Process ---

        if ( 0 < _encoder_count )
        {
            VkCommandBuffer secondary_commands[ _encoder_count ];
            for (int i=0; i<_encoder_count; ++i)
            {
                secondary_commands[ i ] = _command_encoders[ i ]->_handle;
                VkResult result = vkEndCommandBuffer( secondary_commands[ i ] );
                if (result != VK_SUCCESS)
                {
                    kege::Log::error << "vkEndCommandBuffer failed! Error code: " << result <<Log::nl;
                    _is_recording = false;
                    return;
                }
            }
            vkCmdExecuteCommands( _handle, _encoder_count, secondary_commands );
        }

        // --- End The Dynamic Rendering Process ---

        if ( vkCmdEndRenderingPfn == nullptr )
        {
            vkCmdEndRenderingPfn = (PFN_vkCmdEndRendering)vkGetDeviceProcAddr(_device->_device, "vkCmdEndRendering");
            if ( !vkCmdEndRenderingPfn )
            {
                // Try the KHR version as a fallback
                vkCmdEndRenderingPfn = (PFN_vkCmdEndRendering)vkGetDeviceProcAddr(_device->_device, "vkCmdEndRenderingKHR");
            }
        }
        assert( vkCmdEndRenderingPfn && "vkCmdEndRendering and vkCmdEndRenderingKHR are both NULL!");
        vkCmdEndRenderingPfn( _handle ); // Assuming core 1.3 or KHR loaded
    }

    void vk::CommandBuffer::bindVertexBuffers
    (
        uint32_t first_binding,
        const std::vector< ref::Buffer >& buffers,
        const std::vector< uint64_t >& offsets
    )
    {
         if (buffers.empty() || buffers.size() != offsets.size()) return;

         std::vector< VkBuffer > vk_buffers;
         std::vector< VkDeviceSize > vk_offsets; // Vulkan uses VkDeviceSize for offsets
         vk_buffers.reserve( buffers.size() );
         vk_offsets.reserve( buffers.size() );

        for(size_t i = 0; i < buffers.size(); ++i)
        {
            int frame = _device->getFrameIndex() & buffers[i]->frames();
            const VkBuffer buffer_handle = buffers[i]->vk()->getSource( frame ).handle;

            if ( buffer_handle != VK_NULL_HANDLE )
            {
                vk_buffers.push_back( buffer_handle );
                vk_offsets.push_back(static_cast<VkDeviceSize>(offsets[i]));
            }
            else
            {
                kege::Log::error  << "Invalid buffer handle in bindVertexBuffers at index " << int(i) <<Log::nl;
                // Skip this buffer or return error? For now, continue processing others if possible.
                // To be safe, probably should return early if any handle is invalid.
                return;
            }
        }

        vkCmdBindVertexBuffers( _handle, first_binding, static_cast<uint32_t>(vk_buffers.size()), vk_buffers.data(), vk_offsets.data());
    }

    void CommandBuffer::bindIndexBuffer(const ref::Buffer& buffer, uint64_t offset, bool use_uint16)
    {
        const vk::Buffer* b = buffer->vk();
        int frame = _device->getFrameIndex() & b->frames();

        VkIndexType index_type = use_uint16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
        vkCmdBindIndexBuffer( _handle, b->getSource( frame ).handle, static_cast<VkDeviceSize>(offset), index_type);
    }

    void vk::CommandBuffer::setViewport(const Viewport& viewport)
    {
        if ( !_is_recording || !_handle ) return;
        VkViewport vk_viewport = {};
        vk_viewport.x = viewport.x;
        // Vulkan viewport Y is often inverted compared to other APIs (origin top-left)
        // Adjust if necessary based on your coordinate system conventions vs Vulkan's NDC
        vk_viewport.y = viewport.y; // Or viewport.height - viewport.y if flipping
        vk_viewport.width = viewport.width;
        vk_viewport.height = viewport.height; // Or -viewport.height if flipping
        vk_viewport.minDepth = viewport.min_depth;
        vk_viewport.maxDepth = viewport.max_depth;
        vkCmdSetViewport( _handle, 0, 1, &vk_viewport );
    }

    void vk::CommandBuffer::setScissor(const Rect2D& rect)
    {
        if (!_is_recording || !_handle) return;
        VkRect2D vk_scissor = {};
        vk_scissor.offset = { rect.offset.x, rect.offset.y };
        vk_scissor.extent = { rect.extent.width, rect.extent.height };
        vkCmdSetScissor( _handle, 0, 1, &vk_scissor );
    }

    void vk::CommandBuffer::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
    {
        if (!_is_recording || !_handle) return;
        vkCmdDraw( _handle, vertex_count, instance_count, first_vertex, first_instance );
    }

    void vk::CommandBuffer::drawIndexed
    (
        uint32_t index_count,
        uint32_t instance_count,
        uint32_t first_index,
        int32_t vertex_offset,
        uint32_t first_instance
    )
    {
        if (!_is_recording || !_handle) return;
        vkCmdDrawIndexed(_handle, index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    void vk::CommandBuffer::dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
    {
        if (!_is_recording || !_handle) return;
        vkCmdDispatch(_handle, group_count_x, group_count_y, group_count_z);
    }

    void vk::CommandBuffer::pipelineBarrier
    (
        std::vector< ImageMemoryBarrier >& image_barriers,
        std::vector< BufferMemoryBarrier >& buffer_barriers
    )
    {
        if ( !_is_recording || !_handle ) return;

        std::vector< VkImageMemoryBarrier > image_memory_barriers;
        std::vector< VkBufferMemoryBarrier > buffer_memory_barriers;
        VkPipelineStageFlags src_pipeline_stage = 0;
        VkPipelineStageFlags dst_pipeline_stage = 0;

        for ( ImageMemoryBarrier& barrier : image_barriers )
        {
            // Translate abstract stage/access to Vulkan stage/access using helpers
            // These helpers likely live in Device or VulkanTypeConversion
            src_pipeline_stage |= convertPipelineStageFlag( barrier.src_stage );
            dst_pipeline_stage |= convertPipelineStageFlag( barrier.dst_stage );

            //Image* barrier.image = _device->_textures.get( barrier.image.id );
            Format format = barrier.image->getFormat(); // Get format for layout/aspect

            VkImageMemoryBarrier img_barrier = {};
            img_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            img_barrier.srcAccessMask = convertAccessFlag( barrier.src_access );
            img_barrier.dstAccessMask = convertAccessFlag( barrier.dst_access );
            img_barrier.oldLayout = convertImageLayout( barrier.old_layout );
            img_barrier.newLayout = convertImageLayout( barrier.new_layout );
            img_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            img_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            img_barrier.image = barrier.image->vk()->_image;
            img_barrier.subresourceRange.aspectMask = getImageAspectFlags( format ); // Need helper
            img_barrier.subresourceRange.baseMipLevel = 0; // TODO: Support subresource ranges
            img_barrier.subresourceRange.levelCount = barrier.image->getMipLevels();
            img_barrier.subresourceRange.baseArrayLayer = 0;
            img_barrier.subresourceRange.layerCount = barrier.image->getExtent().depth;
            image_memory_barriers.push_back( img_barrier );

            barrier.image->vk()->_current_layout = img_barrier.newLayout;
        }

        for ( BufferMemoryBarrier& barrier : buffer_barriers )
        {
            // Translate abstract stage/access to Vulkan stage/access using helpers
            // These helpers likely live in Device or VulkanTypeConversion
            src_pipeline_stage |= convertPipelineStageFlag( barrier.src_stage );
            dst_pipeline_stage |= convertPipelineStageFlag( barrier.dst_stage );
            VkAccessFlags vk_src_access = convertAccessFlag( barrier.src_access );
            VkAccessFlags vk_dst_access = convertAccessFlag( barrier.dst_access );

            int frame = _device->getFrameIndex() & barrier.buffer->frames();
            const VkBuffer buffer = barrier.buffer->vk()->getSource( frame ).handle;

            VkBufferMemoryBarrier memory_barrier = {};
            memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            memory_barrier.srcAccessMask = vk_src_access;
            memory_barrier.dstAccessMask = vk_dst_access;
            memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memory_barrier.buffer = buffer;
            memory_barrier.offset = barrier.offset; // TODO: Support offset/size
            memory_barrier.size = barrier.size;
            buffer_memory_barriers.push_back( memory_barrier );
        }

        // Ensure valid stages if none were accumulated
        if (src_pipeline_stage == 0) src_pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        if (dst_pipeline_stage == 0) dst_pipeline_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        // Issue the vkCmdPipelineBarrier
        vkCmdPipelineBarrier
        (
            _handle,
            src_pipeline_stage,
            dst_pipeline_stage,
            0, // Dependency flags
            0, nullptr, // Global Memory Barriers
            static_cast<uint32_t>( buffer_memory_barriers.size() ), buffer_memory_barriers.data(),
            static_cast<uint32_t>( image_memory_barriers.size() ), image_memory_barriers.data()
        );
    }

    void vk::CommandBuffer::copyBuffer
    (
        const ref::Buffer& src, ref::Buffer& dst,
        const std::vector<BufferCopyRegion>& regions
    )
    {
        if (!_is_recording || !_handle || regions.empty()) return;
        const int frame = _device->getFrameIndex();

        std::vector< VkBufferCopy > vk_regions;
        vk_regions.reserve( regions.size() );

        for (const auto& region : regions)
        {
            vk_regions.push_back
            ({
                static_cast<VkDeviceSize>(region.src_offset),
                static_cast<VkDeviceSize>(region.dst_offset),
                static_cast<VkDeviceSize>(region.size)
            });
        }

        vkCmdCopyBuffer
        (
            _handle,
            src->vk()->getSource( frame & src->frames() ).handle,
            dst->vk()->getSource( frame & dst->frames() ).handle,
            static_cast<uint32_t>(vk_regions.size()),
            vk_regions.data()
        );
    }

    void vk::CommandBuffer::copyImage
    (
        const ref::Image& src, ref::Image& dst,
        const std::vector<ImageCopyRegion>& regions
    )
    {
        if ( !_is_recording || !_handle || regions.empty() ) return;
        if ( !src || !dst ) { /* Error */ return; }

        std::vector<VkImageCopy> vk_regions;
        vk_regions.reserve(regions.size());
        for (const auto& region : regions)
        {
            VkImageCopy vk_region = {};
            // --- Translate src subresource ---
            vk_region.srcSubresource.aspectMask = getImageAspectFlags( src->getFormat() );
            vk_region.srcSubresource.baseArrayLayer = region.src_subresource.base_array_layer;
            vk_region.srcSubresource.layerCount = region.src_subresource.layer_count;
            vk_region.srcSubresource.mipLevel = region.src_subresource.mip_level;
            vk_region.srcOffset =
            {
                region.src_offset.x,
                region.src_offset.y,
                region.src_offset.z
            };

            // --- Translate dst subresource ---
            vk_region.dstSubresource.aspectMask = getImageAspectFlags( dst->getFormat() );
            vk_region.dstSubresource.mipLevel = region.dst_subresource.mip_level;
            vk_region.dstSubresource.baseArrayLayer = region.dst_subresource.base_array_layer;
            vk_region.dstSubresource.layerCount = region.dst_subresource.layer_count;
            vk_region.dstOffset =
            {
                region.dst_offset.x,
                region.dst_offset.y,
                region.dst_offset.z
            };

            // --- Extent ---
            vk_region.extent =
            {
                region.extent.width,
                region.extent.height,
                region.extent.depth
            };
            vk_regions.push_back(vk_region);
        }

        // --- IMPORTANT: Layouts must be SRC_OPTIMAL and DST_OPTIMAL respectively! ---
        vkCmdCopyImage
        (
            _handle,
            src->vk()->_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dst->vk()->_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            static_cast<uint32_t>(vk_regions.size()), vk_regions.data()
        );
    }

    void vk::CommandBuffer::clearDepthStencilTexture
    (
        ref::Image& image,
        float depth,
        uint32_t stencil,
        const std::vector< ImageSubresourceRange >& sub_resource_ranges
    )
    {
        if (!_is_recording || !_handle || sub_resource_ranges.empty() )
        {
            if ( sub_resource_ranges.empty() )
            {
                Log::warning << "Warning: clearDepthStencilTexture called with empty ranges." <<Log::nl;
            }
            return;
        }

        // Resolve texture handle and validate format
        if ( !image )
        {
            kege::Log::error<< "Invalid texture handle in clearDepthStencilTexture." <<Log::nl;
            return;
        }

        if ( !isDepthStencilFormat( image->getFormat() ) )
        {
            kege::Log::error<< "clearDepthStencilTexture called on non-depth/stencil format texture." <<Log::nl;
            return;
        }

        // Define the clear value
        VkClearDepthStencilValue clear_value = {};
        clear_value.depth = depth;
        clear_value.stencil = stencil;

        // Translate abstract ranges to Vulkan ranges
        std::vector< VkImageSubresourceRange > vk_image_subresource_ranges;
        vk_image_subresource_ranges.reserve( sub_resource_ranges.size() );

        for ( const auto& abstract_range : sub_resource_ranges )
        {
            VkImageSubresourceRange vk_range = {};

            // Translate aspect mask - Ensure only Depth/Stencil aspects are included
            vk_range.aspectMask = toVkImageAspect( abstract_range.aspect_mask );
            if ((vk_range.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) == 0)
            {
                Log::warning
                << "clearDepthStencilTexture range specifies no depth or stencil aspect. Inferring from format." <<Log::nl;

                // Infer aspect from format if not specified correctly in the range
                vk_range.aspectMask = getImageAspectFlags( image->getFormat() );
                if ((vk_range.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) == 0)
                {
                    kege::Log::error
                    << "ERROR: Cannot infer depth/stencil aspect for clear operation on format "
                    << static_cast<int>(image->getFormat()) <<Log::nl;

                    continue; // Skip this invalid range
                }
            }
            else
            {
                // Mask out any potentially incorrect Color bit if user supplied it
                vk_range.aspectMask &= (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
            }

            vk_range.baseMipLevel = abstract_range.base_mip_level;

            // Handle VK_REMAINING_MIP_LEVELS if needed, or assume caller specifies exact counts
            vk_range.levelCount = ( abstract_range.level_count == VK_REMAINING_MIP_LEVELS )
            ? (image->getMipLevels() - abstract_range.base_mip_level)
            : abstract_range.level_count;

            vk_range.baseArrayLayer = abstract_range.base_array_layer;

            // Handle VK_REMAINING_ARRAY_LAYERS
            vk_range.layerCount = ( abstract_range.layer_count == VK_REMAINING_ARRAY_LAYERS )
            ? (image->getExtent().depth - abstract_range.base_array_layer)
            : abstract_range.layer_count;

            // Basic validation on counts
            if (vk_range.levelCount == 0 || vk_range.layerCount == 0)
            {
                kege::Log::error
                << "Warning: Skipping zero-sized subresource range in clearDepthStencilTexture." <<Log::nl;

                continue;
            }

            vk_image_subresource_ranges.push_back(vk_range);
        }

        if ( vk_image_subresource_ranges.empty() )
        {
            kege::Log::error
            << "Warning: No valid ranges remaining after translation for clearDepthStencilTexture." <<Log::nl;

            return; // No valid ranges processed
        }
        
        // --- IMPORTANT: Layout Assumption ---
        // This command requires the image layout to be VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        // or VK_IMAGE_LAYOUT_GENERAL. The Render Graph MUST have transitioned the image
        // to one of these layouts *before* this command buffer executes this clear.
        VkImageLayout required_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        // You *could* check texture_internals->currentLayout here if tracking layout,
        // but relying on the RG is generally the intended design.
        // Record the Vulkan command
        vkCmdClearDepthStencilImage
        (
            _handle,
            image->vk()->_image,
            required_layout, // Layout MUST be correct here
            &clear_value,
            static_cast< uint32_t >( vk_image_subresource_ranges.size() ),
            vk_image_subresource_ranges.data()
        );
    }

    void vk::CommandBuffer::clearColorTexture
    (
        ref::Image& image,
        const float color[4],
        const std::vector<ImageSubresourceRange>& sub_resource_ranges
    )
    {
         // Basic validation
        if (!_is_recording || !_handle || sub_resource_ranges.empty() )
        {
            if ( sub_resource_ranges.empty() )
            {
                Log::warning << "Warning: clearColorTexture called with empty ranges.";
            }
            return;
        }

        // Resolve texture handle and validate format
        if ( !image )
        {
            kege::Log::error<< "Invalid texture handle in clearColorTexture." <<Log::nl;
            return;
        }

        if ( isDepthStencilFormat( image->getFormat() ) )
        {
            kege::Log::error<< "clearColorTexture called on depth/stencil format texture." <<Log::nl;
            return;
        }

        // Define the clear value
        VkClearColorValue clear_color;
        memcpy(clear_color.float32, color, sizeof(float) * 4); // Assuming float format for now
        // TODO: Handle integer formats using clear_color.uint32 or clear_color.int32

        // Translate abstract ranges to Vulkan ranges
        std::vector<VkImageSubresourceRange> vk_image_subresource_ranges;
        vk_image_subresource_ranges.reserve( sub_resource_ranges.size() );

        for ( const auto& abstract_range : sub_resource_ranges )
        {
            VkImageSubresourceRange vk_range = {};
            // Ensure aspect is COLOR or NONE (default to COLOR if NONE)
            vk_range.aspectMask = toVkImageAspect( abstract_range.aspect_mask );
            if (( vk_range.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT ) == 0 )
            {
                // Allow AspectFlags::None, default it to COLOR
                vk_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            }
            // Mask out Depth/Stencil just in case
            vk_range.aspectMask &= VK_IMAGE_ASPECT_COLOR_BIT;

            // Translate levels/layers (handle REMAINING consts)
            vk_range.baseMipLevel = abstract_range.base_mip_level;

            vk_range.levelCount = (abstract_range.level_count == VK_REMAINING_MIP_LEVELS)
            ? (image->getMipLevels() - abstract_range.base_mip_level)
            : abstract_range.level_count;

            vk_range.baseArrayLayer = abstract_range.base_array_layer;

            vk_range.layerCount = (abstract_range.layer_count == VK_REMAINING_ARRAY_LAYERS)
            ? (image->getExtent().depth - abstract_range.base_array_layer)
            : abstract_range.layer_count;

            if (vk_range.levelCount == 0 || vk_range.layerCount == 0) continue; // Skip empty ranges
            vk_image_subresource_ranges.push_back( vk_range );
        }


        if ( vk_image_subresource_ranges.empty() )
        {
            kege::Log::error
            << "Warning: No valid ranges remaining after translation for clearColorTexture." <<Log::nl;

            return; // No valid ranges processed
        }

        // --- IMPORTANT: Layout Assumption ---
        VkImageLayout required_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        // Record the Vulkan command
        vkCmdClearColorImage
        (
            _handle,
            image->vk()->_image,
            required_layout, // Layout MUST be correct here
            &clear_color,
            static_cast<uint32_t>(vk_image_subresource_ranges.size()),
            vk_image_subresource_ranges.data()
        );
    }


    void vk::CommandBuffer::clearAttachments
    (
        const std::vector< kege::ClearAttachment >& clear_attachments,
        const std::vector< kege::ClearRect >& clear_rects
    )
    {
        VkClearAttachment attachments[ clear_attachments.size() ];
        VkClearRect rects[ clear_rects.size() ];
        for (int i=0; i<clear_attachments.size(); ++i)
        {
            attachments[i].aspectMask = toVkImageAspect( clear_attachments[i].aspect_mask );
            attachments[i].colorAttachment = clear_attachments[i].attachment_index;
            memcpy(attachments[i].clearValue.color.float32, clear_attachments[i].clear_value.color, 4*sizeof( float ));
        }
        for (int i=0; i<clear_rects.size(); ++i)
        {
            rects[i].baseArrayLayer = clear_rects[i].base_array_layer;
            rects[i].layerCount = clear_rects[i].layer_count;
            rects[i].rect.offset.x = clear_rects[i].rect.offset.x;
            rects[i].rect.offset.y = clear_rects[i].rect.offset.y;
            rects[i].rect.extent.width = clear_rects[i].rect.extent.width;
            rects[i].rect.extent.height = clear_rects[i].rect.extent.height;
        }

        vkCmdClearAttachments
        (
            _handle,
            static_cast<uint32_t>( clear_attachments.size() ), attachments,
            static_cast<uint32_t>( clear_rects.size() ), rects
        );
    }

//
//    void vk::CommandBuffer::copyBuffer(BufferHandle src_buffer, BufferHandle dst_buffer, const std::vector<BufferCopyRegion>& regions)
//    {}
//
//    void vk::CommandBuffer::copyImage(ref::Image src, ref::Image dst, const std::vector<ImageCopyRegion>& regions)
//    {}

    void vk::CommandBuffer::copyBufferToImage
    (
        const ref::Buffer& src, ref::Image& dst,
        const std::vector<BufferTextureCopyRegion>& regions
    )
    {}

    void vk::CommandBuffer::copyTextureToBuffer
    (
        const ref::Image& src, ref::Buffer& dst,
        const std::vector<BufferTextureCopyRegion>& regions
    )
    {}

    vk::CommandBuffer::CommandBuffer(Device* device, VkCommandPool command_pool, VkCommandBuffer command_buffer)
    :   _device( device )
    ,   _command_pool( command_pool )
    ,   _handle( command_buffer )
    ,   _is_recording( false )
    ,   _encoder_count( 0 )
    ,   _sets_bind_state( 0 )
    {
        if (!_device || _command_pool == VK_NULL_HANDLE || _handle == VK_NULL_HANDLE)
        {
            // Or use exceptions for fatal errors
            kege::Log::error << "Invalid arguments passed to vk::CommandBuffer constructor!"<<Log::nl;
        }
    }

    vk::CommandBuffer::~CommandBuffer()
    {
        for (int i=0; i<_command_encoders.size(); i++)
        {
            delete _command_encoders[i];
            _command_encoders[i] = nullptr;
        }
        _command_encoders.clear();
    }

    vk::CommandBuffer::CommandBuffer()
    :   _device( nullptr )
    ,   _command_pool( VK_NULL_HANDLE )
    ,   _handle( VK_NULL_HANDLE )
    ,   _is_recording( false )
    ,   _sets_bind_state( 0 )
    {}

    PFN_vkCmdBeginRendering vk::CommandBuffer::vkCmdBeginRenderingPfn = nullptr;
    PFN_vkCmdEndRendering vk::CommandBuffer::vkCmdEndRenderingPfn = nullptr;

}
