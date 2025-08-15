//
//  vulkan-command-encoder.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/15/25.
//

#include "vulkan-device.hpp"
#include "vulkan-command-encoder.hpp"
#include "vulkan-command-buffer.hpp"

namespace kege::vk{

    CommandEncoder::~CommandEncoder()
    {
    }

    CommandEncoder::CommandEncoder()
    :   _command_buffer( nullptr )
    ,   _handle( VK_NULL_HANDLE )
    {}

    void CommandEncoder::bindDescriptorSets( DescriptorSetHandle handle, bool globally )
    {
        const DescriptorSet* set = _command_buffer->_device->getDescriptorSet( handle );
        const DescriptorSetLayout* dsl = _command_buffer->_device->getDescriptorSetLayout( set->layout );
        const VkDescriptorSet sets[] = { set->set };

        if ( globally )
        {
            for (const kege::PipelineLayoutHandle& layout : dsl->pipeline_layout_sets )
            {
                const PipelineLayout* vkpl = _command_buffer->_device->getPipelineLayout( layout );
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
    }

    void CommandEncoder::bindGraphicsPipeline( kege::PipelineHandle pipeline_handle )
    {
        const GraphicsPipeline* pipeline = _command_buffer->_device->getGraphicsPipeline( pipeline_handle );
        if ( !pipeline )
        {
            KEGE_LOG_ERROR << "Invalid pipeline handle in bindGraphicsPipeline." <<Log::nl;
             return;
        }

        vkCmdBindPipeline( _handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline );

        // Store layout for subsequent binds/push constants
        _current_pipeline_layout = _command_buffer->_device->getPipelineLayout( pipeline->desc.pipeline_layout );
        _current_pipeline_bindpoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    }

    void CommandEncoder::bindComputePipeline(PipelineHandle pipeline_handle)
    {
        const ComputePipeline* pipeline = _command_buffer->_device->getComputePipeline(pipeline_handle);
        if (!pipeline)
        {
            KEGE_LOG_ERROR << "Invalid pipeline handle in bindComputePipeline."<<Log::nl;
             return;
        }

        vkCmdBindPipeline( _handle, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->pipeline);
        _current_pipeline_layout = _command_buffer->_device->getPipelineLayout( pipeline->desc.pipeline_layout );
        _current_pipeline_bindpoint = VK_PIPELINE_BIND_POINT_COMPUTE;
    }

    void CommandEncoder::bindVertexBuffers
    (
        uint32_t first_binding,
        const std::vector<BufferHandle>& buffer_handles,
        const std::vector<uint64_t>& offsets
    )
    {
         if (buffer_handles.empty() || buffer_handles.size() != offsets.size()) return;

         std::vector<VkBuffer> vk_buffers;
         std::vector<VkDeviceSize> vk_offsets; // Vulkan uses VkDeviceSize for offsets
         vk_buffers.reserve( buffer_handles.size() );
         vk_offsets.reserve( buffer_handles.size() );

        for(size_t i = 0; i < buffer_handles.size(); ++i)
        {
            const Buffer* buffer_internals = _command_buffer->_device->getBuffer(buffer_handles[i]);
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

        vkCmdBindVertexBuffers( _handle, first_binding, static_cast<uint32_t>(vk_buffers.size()), vk_buffers.data(), vk_offsets.data());
    }

    void CommandEncoder::bindIndexBuffer(BufferHandle buffer_handle, uint64_t offset, bool use_uint16)
    {
        const Buffer* buffer_internals = _command_buffer->_device->getBuffer( buffer_handle );
        if (!buffer_internals)
        {
            KEGE_LOG_ERROR << "ERROR: Invalid buffer handle in bindIndexBuffer." <<Log::nl;
            return;
        }

        VkIndexType index_type = use_uint16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
        vkCmdBindIndexBuffer( _handle, buffer_internals->buffer, static_cast<VkDeviceSize>(offset), index_type);
    }

    void CommandEncoder::setPushConstants( ShaderStage stages, uint32_t offset, uint32_t size, const void *data )
    {
        VkShaderStageFlags stage_flags = convertShaderStageMask( stages );
        vkCmdPushConstants( _handle, _current_pipeline_layout->layout, stage_flags, offset, size, data );
    }

    void CommandEncoder::setViewport(const Viewport& viewport)
    {
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

    void CommandEncoder::setScissor(const Rect2D& rect)
    {
        VkRect2D vk_scissor = {};
        vk_scissor.offset = { rect.offset.x, rect.offset.y };
        vk_scissor.extent = { rect.extent.width, rect.extent.height };
        vkCmdSetScissor( _handle, 0, 1, &vk_scissor );
    }

    void CommandEncoder::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
    {
        vkCmdDraw( _handle, vertex_count, instance_count, first_vertex, first_instance );
    }

    void CommandEncoder::drawIndexed
    (
        uint32_t index_count,
        uint32_t instance_count,
        uint32_t first_index,
        int32_t vertex_offset,
        uint32_t first_instance
    )
    {
        vkCmdDrawIndexed( _handle, index_count, instance_count, first_index, vertex_offset, first_instance );
    }

    void CommandEncoder::drawIndexIndirect( BufferHandle buffer, VkDeviceSize offset, uint32_t draw_count, uint32_t stride )
    {
        const Buffer* vkbuf = _command_buffer->_device->getBuffer( buffer );
        if ( !vkbuf )
        {
            KEGE_LOG_ERROR << "ERROR: Invalid buffer handle in bindIndexBuffer." <<Log::nl;
            return;
        }
        vkCmdDrawIndexedIndirect( _handle, vkbuf->buffer, offset, draw_count, stride );
    }

    void CommandEncoder::drawIndirect( BufferHandle buffer, VkDeviceSize offset, uint32_t draw_count, uint32_t stride )
    {
        const Buffer* vkbuf = _command_buffer->_device->getBuffer( buffer );
        if ( !vkbuf )
        {
            KEGE_LOG_ERROR << "ERROR: Invalid buffer handle in bindIndexBuffer." <<Log::nl;
            return;
        }
        vkCmdDrawIndirect( _handle, vkbuf->buffer, offset, draw_count, stride );
    }

    void CommandEncoder::dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
    {
        vkCmdDispatch( _handle, group_count_x, group_count_y, group_count_z );
    }

    void CommandEncoder::pipelineBarrierBatch
    (
        const std::vector< AbstractResourceBarrier >& abstract_barriers,
        const ResourceRegistry& registry
    )
    {
        std::vector<VkImageMemoryBarrier> image_barriers;
        std::vector<VkBufferMemoryBarrier> buffer_barriers;
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

    void CommandEncoder::copyBuffer
    (
        BufferHandle src_buffer_handle,
        BufferHandle dst_buffer_handle,
        const std::vector<BufferCopyRegion>& regions
    )
    {
        if ( regions.empty() ) return;
        const Buffer* src_buffer = _command_buffer->_device->getBuffer(src_buffer_handle);
        const Buffer* dst_buffer = _command_buffer->_device->getBuffer(dst_buffer_handle);
        if (!src_buffer || !dst_buffer) { /* Error */ return; }

        std::vector<VkBufferCopy> vk_regions;
        vk_regions.reserve(regions.size());
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
            src_buffer->buffer,
            dst_buffer->buffer,
            static_cast<uint32_t>(vk_regions.size()),
            vk_regions.data()
        );
    }

    void CommandEncoder::copyTexture
    (
        ImageHandle src_texture_handle,
        ImageHandle dst_texture_handle,
        const std::vector<TextureCopyRegion>& regions
    )
    {
        if ( regions.empty() ) return;
        const Image* src_texture = _command_buffer->_device->getImage( src_texture_handle );
        const Image* dst_texture = _command_buffer->_device->getImage( dst_texture_handle );
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

    void CommandEncoder::clearDepthStencilTexture
    (
        ImageHandle texture_handle,
        float depth,
        uint32_t stencil,
        const std::vector< TextureSubresourceRange >& sub_resource_ranges
    )
    {
        if ( sub_resource_ranges.empty() || texture_handle.id == 0)
        {
            if ( sub_resource_ranges.empty() )
            {
                Log::warning << "Warning: clearDepthStencilTexture called with empty ranges.";
            }
            return;
        }

        // Resolve texture handle and validate format
        const Image* texture_internals = _command_buffer->_device->getImage(texture_handle);
        if ( !texture_internals )
        {
            KEGE_LOG_ERROR<< "Invalid texture handle in clearDepthStencilTexture.";
            return;
        }

        if ( !isDepthStencilFormat( texture_internals->desc.format ) )
        {
            KEGE_LOG_ERROR<< "clearDepthStencilTexture called on non-depth/stencil format texture.";
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

    void CommandEncoder::clearColorTexture
    (
        ImageHandle texture_handle,
        const float color[4],
        const std::vector<TextureSubresourceRange>& sub_resource_ranges
    )
    {
         // Basic validation
        if ( sub_resource_ranges.empty() || texture_handle.id < 0 )
        {
            if ( sub_resource_ranges.empty() )
            {
                Log::warning << "Warning: clearColorTexture called with empty ranges.";
            }
            return;
        }

        // Resolve texture handle and validate format
        const Image* texture_internals = _command_buffer->_device->getImage(texture_handle);
        if ( !texture_internals )
        {
            KEGE_LOG_ERROR<< "Invalid texture handle in clearColorTexture.";
            return;
        }

        if ( isDepthStencilFormat( texture_internals->desc.format ) )
        {
            KEGE_LOG_ERROR<< "clearColorTexture called on depth/stencil format texture.";
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


    void CommandEncoder::clearAttachments
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

    void CommandEncoder::copyBufferToTexture(BufferHandle src_buffer, ImageHandle dst_texture, const std::vector<BufferTextureCopyRegion>& regions)
    {}

    void CommandEncoder::copyTextureToBuffer(ImageHandle src_texture, BufferHandle dst_buffer, const std::vector<BufferTextureCopyRegion>& regions)
    {}

    VkImageLayout CommandEncoder::stateToVkLayout(ResourceState state, Format format)const
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

    bool CommandEncoder::isValid()const
    {
        return _handle != VK_NULL_HANDLE;
    }

}
