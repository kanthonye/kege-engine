//
//  vulkan-command-recorder.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#include "vulkan-device.hpp"
#include "vulkan-command-buffer.hpp"
#include <stdexcept> // For potential exceptions
#include <iostream>  // For errors/warnings
#include <vector>
#include <assert.h>

namespace kege::vk{

    PFN_vkCmdBeginRendering vk::CommandBuffer::vkCmdBeginRenderingPfn = nullptr;
    PFN_vkCmdEndRendering vk::CommandBuffer::vkCmdEndRenderingPfn = nullptr;

    vk::CommandBuffer::CommandBuffer(Device* device, VkCommandPool command_pool, VkCommandBuffer command_buffer)
    :   _device( device )
    ,   _command_pool( command_pool )
    ,   _handle( command_buffer )
    ,   _is_recording( false )
    ,   _encoder_count( 0 )
    {
        if (!_device || _command_pool == VK_NULL_HANDLE || _handle == VK_NULL_HANDLE)
        {
            // Or use exceptions for fatal errors
            KEGE_LOG_ERROR << "Invalid arguments passed to vk::CommandBuffer constructor!"<<Log::nl;
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
    {}

    CommandEncoder* vk::CommandBuffer::createCommandEncoder()
    {
        CommandEncoder* encoder = nullptr;
        if ( _encoder_count >= _command_encoders.size() )
        {
            encoder = new CommandEncoder;
            _command_encoders.push_back( encoder );
            encoder->_command_buffer = this;

            VkCommandBufferAllocateInfo info = {};
            info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
            info.commandPool = _command_pool;
            info.commandBufferCount = 1;
            if ( _device->allocateCommandBuffers( &info, &encoder->_handle ) != VK_SUCCESS )
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

    bool vk::CommandBuffer::beginCommands()
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
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo = nullptr; // Not a secondary command buffer

        VkResult result = vkBeginCommandBuffer( _handle, &begin_info );
        if (result != VK_SUCCESS)
        {
            KEGE_LOG_ERROR << "vkBeginCommandBuffer failed! Error code: " << result<<Log::nl;
            return false;
        }

        _is_recording = true;
        _current_pipeline_layout = nullptr;
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
            KEGE_LOG_ERROR << "vkEndCommandBuffer failed! Error code: " << result <<Log::nl;
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
            const Image* image = _device->getImage( attach_info.image_view_handle );
            if (!image)
            {
                KEGE_LOG_ERROR << "Invalid color attachment handle in beginRendering." <<Log::nl;
                 continue; // Skip this attachment
            }

            VkRenderingAttachmentInfo color_attachment = {};
            color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR; // Or core version
            color_attachment.imageView = image->view;
            // **** CRITICAL: Layout must be correct before this call ****
            // **** Render Graph barrier logic ensures this! ****
            color_attachment.imageLayout = convertImageLayout( attach_info.image_layout );// stateToVkLayout( attach_info.initial_layout, tex_internals->desc.format );
            // vk_attach_info.resolveImageView = ... // Handle resolve later
            // vk_attach_info.resolveImageLayout = ...
            color_attachment.loadOp = convertAttachmentLoadOp( attach_info.load_op ); // Need TranslateLoadOp helper
            color_attachment.storeOp = convertAttachmentStoreOp( attach_info.store_op ); // Need TranslateStoreOp helper

            color_attachment.clearValue.color.float32[0] = attach_info.clear_value.color[0];
            color_attachment.clearValue.color.float32[1] = attach_info.clear_value.color[1];
            color_attachment.clearValue.color.float32[2] = attach_info.clear_value.color[2];
            color_attachment.clearValue.color.float32[3] = attach_info.clear_value.color[3];

            color_attachments.push_back( color_attachment );
            _color_attachment_formats.push_back( image->format );
        }

        VkRenderingAttachmentInfo depth_attachment = {};
        depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        const Image* depth_image = nullptr;
        if ( rendering_info.depth_attachment.has_value() )
        {
            const auto& attach_info = rendering_info.depth_attachment.value();
            depth_image = _device->getImage( attach_info.image_view_handle );
            if (depth_image)
            {
                depth_attachment.imageView = depth_image->view;
                depth_attachment.imageLayout = convertImageLayout( attach_info.image_layout );// stateToVkLayout(attach_info.initial_layout, depth_image->desc.format);
                depth_attachment.loadOp = convertAttachmentLoadOp(attach_info.load_op);
                depth_attachment.storeOp = convertAttachmentStoreOp(attach_info.store_op);
                depth_attachment.clearValue.depthStencil.depth = attach_info.clear_value.depth_stencil.depth;
            }
            else
            {
                KEGE_LOG_ERROR << "Invalid depth attachment handle in beginRendering." <<Log::nl;
            }
            _depth_attachment_format = depth_image->format;
        }

        VkRenderingAttachmentInfo stencil_attachment = {};
        stencil_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        const Image* stencil_image = nullptr;
        if ( rendering_info.stencil_attachment.has_value() )
        {
            const auto& attach_info = rendering_info.stencil_attachment.value();
            stencil_image = _device->getImage( attach_info.image_view_handle );
            if ( stencil_image )
            {
                stencil_attachment.imageView = stencil_image->view;
                stencil_attachment.imageLayout = convertImageLayout( attach_info.image_layout );// stateToVkLayout(attach_info.initial_layout, stencil_image->desc.format);
                stencil_attachment.loadOp = convertAttachmentLoadOp(attach_info.load_op);
                stencil_attachment.storeOp = convertAttachmentStoreOp(attach_info.store_op);
                stencil_attachment.clearValue.depthStencil.depth = attach_info.clear_value.depth_stencil.depth;
            }
            else
            {
                KEGE_LOG_ERROR << "Invalid depth attachment handle in beginRendering." <<Log::nl;
            }
            _stencil_attachment_format = stencil_image->format;
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
                    KEGE_LOG_ERROR << "vkEndCommandBuffer failed! Error code: " << result <<Log::nl;
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

    bool vk::CommandBuffer::bindDescriptorSets( DescriptorSetHandle handle, bool globally )
    {
        const DescriptorSet* set = _device->getDescriptorSet( handle );
        const DescriptorSetLayout* dsl = _device->getDescriptorSetLayout( set->layout );
        const VkDescriptorSet sets[] = { set->set };

        if( dsl->binding_location < 0 )
        {
            KEGE_LOG_ERROR << "Invalid binding location "<< dsl->binding_location << " for descriptor set -> " << dsl->name << "" <<Log::nl;
            return false;
        }

        if ( globally )
        {
            for (const PipelineLayoutHandle& layout : dsl->pipeline_layout_sets )
            {
                const PipelineLayout* vkpl = _device->getPipelineLayout( layout );
                auto i = vkpl->binding_locations.find( dsl->binding_location );
                vkCmdBindDescriptorSets( _handle, vkpl->pipeline_bind_point, vkpl->layout, i->second, 1, sets, 0, nullptr );
            }
        }
        else if( _current_pipeline_layout != nullptr )
        {
            auto i = _current_pipeline_layout->binding_locations.find( dsl->binding_location );
            vkCmdBindDescriptorSets
            (
                _handle,
                _current_pipeline_bindpoint,
                _current_pipeline_layout->layout,
                i->second, 1, sets, 0, nullptr
            );
        }
        return true;
    }

    // --- State Binding ---
    void vk::CommandBuffer::bindGraphicsPipeline( PipelineHandle pipeline_handle )
    {
        if (!_is_recording || !_handle) return;

        const GraphicsPipeline* pipeline = _device->getGraphicsPipeline( pipeline_handle );
        if ( !pipeline )
        {
            KEGE_LOG_ERROR << "Invalid pipeline handle in bindGraphicsPipeline." <<Log::nl;
             return;
        }

        vkCmdBindPipeline( _handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline );

        // Store layout for subsequent binds/push constants
        _current_pipeline_layout = _device->getPipelineLayout( pipeline->desc.pipeline_layout );
        _current_pipeline_bindpoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    }

    void vk::CommandBuffer::bindComputePipeline(PipelineHandle pipeline_handle)
    {
         if (!_is_recording || !_handle) return;
        const ComputePipeline* pipeline = _device->getComputePipeline(pipeline_handle);
        if (!pipeline)
        {
            KEGE_LOG_ERROR << "Invalid pipeline handle in bindComputePipeline." <<Log::nl;
             return;
        }

        vkCmdBindPipeline(_handle, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->pipeline);

        _current_pipeline_layout = _device->getPipelineLayout( pipeline->desc.pipeline_layout );
        _current_pipeline_bindpoint = VK_PIPELINE_BIND_POINT_COMPUTE;
    }

    void vk::CommandBuffer::bindVertexBuffers
    (
        uint32_t first_binding,
        const std::vector<BufferHandle>& buffer_handles,
        const std::vector<uint64_t>& offsets
    )
    {
         if (!_is_recording || !_handle || buffer_handles.empty() || buffer_handles.size() != offsets.size()) return;

         std::vector<VkBuffer> vk_buffers;
         std::vector<VkDeviceSize> vk_offsets; // Vulkan uses VkDeviceSize for offsets
         vk_buffers.reserve( buffer_handles.size() );
         vk_offsets.reserve( buffer_handles.size() );

        for(size_t i = 0; i < buffer_handles.size(); ++i)
        {
            const Buffer* buffer_internals = _device->getBuffer(buffer_handles[i]);
            if (buffer_internals)
            {
                vk_buffers.push_back(buffer_internals->buffer);
                vk_offsets.push_back(static_cast<VkDeviceSize>(offsets[i]));
            }
            else
            {
                KEGE_LOG_ERROR  << "Invalid buffer handle in bindVertexBuffers at index " << int(i) <<Log::nl;
                // Skip this buffer or return error? For now, continue processing others if possible.
                // To be safe, probably should return early if any handle is invalid.
                return;
            }
        }
        vkCmdBindVertexBuffers
        (_handle, first_binding, static_cast<uint32_t>(vk_buffers.size()), vk_buffers.data(), vk_offsets.data());
    }

    void vk::CommandBuffer::bindIndexBuffer(BufferHandle buffer_handle, uint64_t offset, bool use_uint16)
    {
        if (!_is_recording || !_handle) return;
        const Buffer* buffer_internals = _device->getBuffer( buffer_handle );
        if (!buffer_internals)
        {
            KEGE_LOG_ERROR << "ERROR: Invalid buffer handle in bindIndexBuffer." <<Log::nl;
            return;
        }

        VkIndexType index_type = use_uint16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
        vkCmdBindIndexBuffer(_handle, buffer_internals->buffer, static_cast<VkDeviceSize>(offset), index_type);
    }

    void vk::CommandBuffer::setPushConstants( ShaderStage stages, uint32_t offset, uint32_t size, const void *data )
    {
        VkShaderStageFlags stage_flags = convertShaderStageMask( stages );
        vkCmdPushConstants( _handle, _current_pipeline_layout->layout, stage_flags, offset, size, data );
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

    void vk::CommandBuffer::pipelineBarrierBatch
    (
        const std::vector< AbstractResourceBarrier >& abstract_barriers,
        const ResourceRegistry& registry
    )
    {
        if (!_is_recording || !_handle || abstract_barriers.empty()) return;

        std::vector< VkImageMemoryBarrier > image_barriers;
        std::vector< VkBufferMemoryBarrier > buffer_barriers;
        VkPipelineStageFlags src_stage_mask = 0;
        VkPipelineStageFlags dst_stage_mask = 0;

        for ( const AbstractResourceBarrier& abstract_barrier : abstract_barriers )
        {
            // Translate abstract stage/access to Vulkan stage/access using helpers
            // These helpers likely live in Device or VulkanTypeConversion
            VkPipelineStageFlags vk_src_stage = convertAccessFlag( abstract_barrier.src_stage_mask );
            VkPipelineStageFlags vk_dst_stage = convertAccessFlag( abstract_barrier.dst_stage_mask );
            VkAccessFlags vk_src_access = convertAccessFlag( abstract_barrier.src_access_mask );
            VkAccessFlags vk_dst_access = convertAccessFlag( abstract_barrier.dst_access_mask );

            src_stage_mask |= vk_src_stage;
            dst_stage_mask |= vk_dst_stage;

            // Resolve logical handle to physical Vulkan handle using the registry
            // The registry lambda returns a void* which we need to cast based on isTexture
            const void* resource_ptr = registry( abstract_barrier.resource_handle, abstract_barrier.is_texture );
            if (!resource_ptr)
            {
                KEGE_LOG_ERROR  << "Failed to resolve logical resource ID "
                << abstract_barrier.resource_handle << " in pipelineBarrierBatch." <<Log::nl;
                continue;
            }

            if ( abstract_barrier.is_texture )
            {
                const Image* tex_internals = static_cast<const Image*>(resource_ptr);
                Format format = tex_internals->desc.format; // Get format for layout/aspect

                VkImageMemoryBarrier vk_barrier = {};
                vk_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                vk_barrier.srcAccessMask = vk_src_access;
                vk_barrier.dstAccessMask = vk_dst_access;
                vk_barrier.oldLayout = stateToVkLayout( abstract_barrier.previous_state, format);
                vk_barrier.newLayout = stateToVkLayout( abstract_barrier.new_state, format);
                vk_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                vk_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                vk_barrier.image = tex_internals->image;
                vk_barrier.subresourceRange.aspectMask = getImageAspectFlags( format ); // Need helper
                vk_barrier.subresourceRange.baseMipLevel = 0; // TODO: Support subresource ranges
                vk_barrier.subresourceRange.levelCount = tex_internals->desc.mip_levels;
                vk_barrier.subresourceRange.baseArrayLayer = 0;
                vk_barrier.subresourceRange.layerCount = tex_internals->desc.depth;
                image_barriers.push_back(vk_barrier);
            }
            else
            { // Buffer
                const Buffer* buf_internals = static_cast<const Buffer*>(resource_ptr);

                VkBufferMemoryBarrier vk_barrier = {};
                vk_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                vk_barrier.srcAccessMask = vk_src_access;
                vk_barrier.dstAccessMask = vk_dst_access;
                vk_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                vk_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                vk_barrier.buffer = buf_internals->buffer;
                vk_barrier.offset = 0; // TODO: Support offset/size
                vk_barrier.size = buf_internals->desc.size;
                buffer_barriers.push_back(vk_barrier);
            }
        }

        // Ensure valid stages if none were accumulated
        if (src_stage_mask == 0) src_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        if (dst_stage_mask == 0) dst_stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        // Issue the vkCmdPipelineBarrier
        vkCmdPipelineBarrier
        (
            _handle,
            src_stage_mask,
            dst_stage_mask,
            0, // Dependency flags
            0, nullptr, // Global Memory Barriers
            static_cast<uint32_t>(buffer_barriers.size()), buffer_barriers.data(),
            static_cast<uint32_t>(image_barriers.size()), image_barriers.data()
        );
    }

    void vk::CommandBuffer::pipelineBarrier
    (
        PipelineStageFlag src_stage_mask,
        PipelineStageFlag dst_stage_mask,
        const std::vector< ImageMemoryBarrier >& image_barriers,
        const std::vector< BufferMemoryBarrier >& buffer_barriers
    )
    {
        if ( !_is_recording || !_handle ) return;

        std::vector< VkImageMemoryBarrier > image_memory_barriers;
        std::vector< VkBufferMemoryBarrier > buffer_memory_barriers;
        VkPipelineStageFlags src_pipeline_stage = 0;
        VkPipelineStageFlags dst_pipeline_stage = 0;

        for ( const ImageMemoryBarrier& barrier : image_barriers )
        {
            // Translate abstract stage/access to Vulkan stage/access using helpers
            // These helpers likely live in Device or VulkanTypeConversion
            src_pipeline_stage |= convertPipelineStageFlag( barrier.src_stage );
            dst_pipeline_stage |= convertPipelineStageFlag( barrier.dst_stage );
            VkAccessFlags vk_src_access = convertAccessFlag( barrier.src_access );
            VkAccessFlags vk_dst_access = convertAccessFlag( barrier.dst_access );

            const Image* tex_internals = _device->getImage( barrier.image );
            Format format = tex_internals->desc.format; // Get format for layout/aspect

            VkImageMemoryBarrier img_barrier = {};
            img_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            img_barrier.srcAccessMask = vk_src_access;
            img_barrier.dstAccessMask = vk_dst_access;
            img_barrier.oldLayout = convertImageLayout( barrier.old_layout );
            img_barrier.newLayout = convertImageLayout( barrier.new_layout );
            img_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            img_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            img_barrier.image = tex_internals->image;
            img_barrier.subresourceRange.aspectMask = getImageAspectFlags( format ); // Need helper
            img_barrier.subresourceRange.baseMipLevel = 0; // TODO: Support subresource ranges
            img_barrier.subresourceRange.levelCount = tex_internals->desc.mip_levels;
            img_barrier.subresourceRange.baseArrayLayer = 0;
            img_barrier.subresourceRange.layerCount = tex_internals->desc.depth;
            image_memory_barriers.push_back( img_barrier );
        }

        for ( const BufferMemoryBarrier& barrier : buffer_barriers )
        {
            // Translate abstract stage/access to Vulkan stage/access using helpers
            // These helpers likely live in Device or VulkanTypeConversion
            src_pipeline_stage |= convertPipelineStageFlag( barrier.src_stage );
            dst_pipeline_stage |= convertPipelineStageFlag( barrier.dst_stage );
            VkAccessFlags vk_src_access = convertAccessFlag( barrier.src_access );
            VkAccessFlags vk_dst_access = convertAccessFlag( barrier.dst_access );

            const Buffer* buffer = _device->getBuffer( barrier.buffer );

            VkBufferMemoryBarrier memory_barrier = {};
            memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            memory_barrier.srcAccessMask = vk_src_access;
            memory_barrier.dstAccessMask = vk_dst_access;
            memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memory_barrier.buffer = buffer->buffer;
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
        BufferHandle src_buffer_handle,
        BufferHandle dst_buffer_handle,
        const std::vector<BufferCopyRegion>& regions
    )
    {
        if (!_is_recording || !_handle || regions.empty()) return;

        vk::Buffer* src = _device->getBuffer(src_buffer_handle);
        vk::Buffer* dst = _device->getBuffer(dst_buffer_handle);
        if (!src || !dst)
        {
            KEGE_LOG_ERROR << "invlide buffer handles " <<Log::nl;
            return;
        }

        std::vector< VkBufferCopy > vk_regions( regions.size() );
        for (const auto& region : regions)
        {
            vk_regions.push_back
            ({
                static_cast<VkDeviceSize>(region.src_offset),
                static_cast<VkDeviceSize>(region.dst_offset),
                static_cast<VkDeviceSize>(region.size)
            });
        }
        copyBuffer( *src, *dst, vk_regions );
    }

    void vk::CommandBuffer::copyBuffer(vk::Buffer& src, vk::Buffer& dst, const std::vector< VkBufferCopy >& regions)
    {
        if (src.buffer == VK_NULL_HANDLE || dst.buffer == VK_NULL_HANDLE ) { /* Error */ return; }
        vkCmdCopyBuffer
        (
            _handle,
            src.buffer,
            dst.buffer,
            static_cast<uint32_t>( regions.size() ),
            regions.data()
        );
    }

    void vk::CommandBuffer::copyTexture
    (
        ImageHandle src_texture_handle,
        ImageHandle dst_texture_handle,
        const std::vector<TextureCopyRegion>& regions
    )
    {
        if ( !_is_recording || !_handle || regions.empty() ) return;
        const Image* src_texture = _device->getImage( src_texture_handle );
        const Image* dst_texture = _device->getImage( dst_texture_handle );
        if ( !src_texture || !dst_texture ) { /* Error */ return; }

        std::vector<VkImageCopy> vk_regions;
        vk_regions.reserve(regions.size());
        for (const auto& region : regions)
        {
            VkImageCopy vk_region = {};
            // --- Translate src subresource ---
            vk_region.srcSubresource.aspectMask = getImageAspectFlags( src_texture->desc.format );
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
            vk_region.dstSubresource.aspectMask = getImageAspectFlags( dst_texture->desc.format );
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
            src_texture->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dst_texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            static_cast<uint32_t>(vk_regions.size()), vk_regions.data()
        );
    }

    void vk::CommandBuffer::clearDepthStencilTexture
    (
        ImageHandle texture_handle,
        float depth,
        uint32_t stencil,
        const std::vector< TextureSubresourceRange >& sub_resource_ranges
    )
    {
        if (!_is_recording || !_handle || sub_resource_ranges.empty() || texture_handle.id == 0)
        {
            if ( sub_resource_ranges.empty() )
            {
                Log::warning << "Warning: clearDepthStencilTexture called with empty ranges." <<Log::nl;
            }
            return;
        }

        // Resolve texture handle and validate format
        const Image* texture_internals = _device->getImage(texture_handle);
        if ( !texture_internals )
        {
            KEGE_LOG_ERROR<< "Invalid texture handle in clearDepthStencilTexture." <<Log::nl;
            return;
        }

        if ( !isDepthStencilFormat( texture_internals->desc.format ) )
        {
            KEGE_LOG_ERROR<< "clearDepthStencilTexture called on non-depth/stencil format texture." <<Log::nl;
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
            vk_range.aspectMask = convertImageAspect( abstract_range.aspect_mask );
            if ((vk_range.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) == 0)
            {
                Log::warning
                << "clearDepthStencilTexture range specifies no depth or stencil aspect. Inferring from format." <<Log::nl;

                // Infer aspect from format if not specified correctly in the range
                vk_range.aspectMask = getImageAspectFlags( texture_internals->desc.format );
                if ((vk_range.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) == 0)
                {
                    KEGE_LOG_ERROR
                    << "ERROR: Cannot infer depth/stencil aspect for clear operation on format "
                    << static_cast<int>(texture_internals->desc.format) <<Log::nl;

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
            ? (texture_internals->desc.mip_levels - abstract_range.base_mip_level)
            : abstract_range.level_count;

            vk_range.baseArrayLayer = abstract_range.base_array_layer;

            // Handle VK_REMAINING_ARRAY_LAYERS
            vk_range.layerCount = ( abstract_range.layer_count == VK_REMAINING_ARRAY_LAYERS )
            ? (texture_internals->desc.depth - abstract_range.base_array_layer)
            : abstract_range.layer_count;

            // Basic validation on counts
            if (vk_range.levelCount == 0 || vk_range.layerCount == 0)
            {
                KEGE_LOG_ERROR
                << "Warning: Skipping zero-sized subresource range in clearDepthStencilTexture." <<Log::nl;

                continue;
            }

            vk_image_subresource_ranges.push_back(vk_range);
        }

        if ( vk_image_subresource_ranges.empty() )
        {
            KEGE_LOG_ERROR
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
            texture_internals->image,
            required_layout, // Layout MUST be correct here
            &clear_value,
            static_cast< uint32_t >( vk_image_subresource_ranges.size() ),
            vk_image_subresource_ranges.data()
        );
    }

    void vk::CommandBuffer::clearColorTexture
    (
        ImageHandle texture_handle,
        const float color[4],
        const std::vector<TextureSubresourceRange>& sub_resource_ranges
    )
    {
         // Basic validation
        if (!_is_recording || !_handle || sub_resource_ranges.empty() || texture_handle.id < 0)
        {
            if ( sub_resource_ranges.empty() )
            {
                Log::warning << "Warning: clearColorTexture called with empty ranges.";
            }
            return;
        }

        // Resolve texture handle and validate format
        const Image* texture_internals = _device->getImage(texture_handle);
        if ( !texture_internals )
        {
            KEGE_LOG_ERROR<< "Invalid texture handle in clearColorTexture." <<Log::nl;
            return;
        }

        if ( isDepthStencilFormat( texture_internals->desc.format ) )
        {
            KEGE_LOG_ERROR<< "clearColorTexture called on depth/stencil format texture." <<Log::nl;
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
            vk_range.aspectMask = convertImageAspect( abstract_range.aspect_mask );
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
            ? (texture_internals->desc.mip_levels - abstract_range.base_mip_level)
            : abstract_range.level_count;

            vk_range.baseArrayLayer = abstract_range.base_array_layer;

            vk_range.layerCount = (abstract_range.layer_count == VK_REMAINING_ARRAY_LAYERS)
            ? (texture_internals->desc.depth - abstract_range.base_array_layer)
            : abstract_range.layer_count;

            if (vk_range.levelCount == 0 || vk_range.layerCount == 0) continue; // Skip empty ranges
            vk_image_subresource_ranges.push_back( vk_range );
        }


        if ( vk_image_subresource_ranges.empty() )
        {
            KEGE_LOG_ERROR
            << "Warning: No valid ranges remaining after translation for clearColorTexture." <<Log::nl;

            return; // No valid ranges processed
        }

        // --- IMPORTANT: Layout Assumption ---
        VkImageLayout required_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        // Record the Vulkan command
        vkCmdClearColorImage
        (
            _handle,
            texture_internals->image,
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
            attachments[i].aspectMask = convertImageAspect( clear_attachments[i].aspect_mask );
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
//    void vk::CommandBuffer::copyTexture(ImageHandle src_texture, ImageHandle dst_texture, const std::vector<TextureCopyRegion>& regions)
//    {}

    void vk::CommandBuffer::copyBufferToTexture(BufferHandle src_buffer, ImageHandle dst_texture, const std::vector<BufferTextureCopyRegion>& regions)
    {}

    void vk::CommandBuffer::copyTextureToBuffer(ImageHandle src_texture, BufferHandle dst_buffer, const std::vector<BufferTextureCopyRegion>& regions)
    {}



    VkImageLayout vk::CommandBuffer::stateToVkLayout(ResourceState state, Format format)const
    { // Or make it a free function
        switch ( state )
        {
            case ResourceState::Undefined:
                return VK_IMAGE_LAYOUT_UNDEFINED;

            case ResourceState::RenderTargetColor:
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            case ResourceState::RenderTargetDepthStencil:
                // Could refine based on format if separating depth/stencil-only layouts
                 if ( isDepthOnlyFormat( format ) ) return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                 if ( isStencilOnlyFormat( format ) ) return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            case ResourceState::ShaderResource:
                // Read-only access in shaders (textures, uniform texel buffers)
                if ( isDepthStencilFormat( format ) )
                {
                     // Depth/stencil formats often need a specific read-only layout
                     return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                     // Could refine to DEPTH_READ_ONLY / STENCIL_READ_ONLY if needed
                }
                else
                {
                    return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                }

            case ResourceState::UnorderedAccess:
                // Read/Write access in shaders (storage images/texel buffers)
                return VK_IMAGE_LAYOUT_GENERAL; // General layout is usually required for storage images

            case ResourceState::CopySrc:
                return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

            case ResourceState::CopyDst:
                return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

            case ResourceState::Present:
                return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            // States primarily relevant for Buffers (Layout doesn't apply)
            case ResourceState::VertexBuffer:
            case ResourceState::IndexBuffer:
            case ResourceState::UniformBuffer:
            case ResourceState::StorageBufferRead:
            case ResourceState::StorageBufferWrite:
            case ResourceState::IndirectArgument:
                // Buffers don't have image layouts. Return something reasonable or assert.
                // Returning UNDEFINED might be safest if called erroneously for a buffer.
                // std::cerr << "Warning: stateToVkLayout called for buffer-specific state." << std::endl;
                return VK_IMAGE_LAYOUT_UNDEFINED; // Or VK_IMAGE_LAYOUT_GENERAL? Needs care.

            // Add ResolveSrc/Dst cases if needed -> TRANSFER_SRC/DST usually appropriate
            // case ResourceState::ResolveSrc: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            // case ResourceState::ResolveDst: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

            default:
                KEGE_LOG_ERROR << "Warning: Unhandled ResourceState in stateToVkLayout. Defaulting to UNDEFINED." <<Log::nl;
                return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

}
