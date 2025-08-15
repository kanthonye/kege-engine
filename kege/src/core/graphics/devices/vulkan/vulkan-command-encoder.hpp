//
//  vulkan-command-encoder.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/15/25.
//

#ifndef vulkan_command_encoder_hpp
#define vulkan_command_encoder_hpp

#include <vulkan/vulkan.h>
#include <vector>

#include "../../core/command-encoder.hpp"
#include "vulkan-resources.hpp"
#include "vulkan-descriptor-manager.hpp"


namespace kege::vk{

    class CommandBuffer; // Use forward declaration

    /**
     * @brief Concrete implementation of the IGraphicsCommandBuffer interface for the Vulkan API.
     *
     * This class encapsulates a Vulkan command buffer object (VkCommandBuffer)
     * and provides methods to record graphics and compute commands using the
     * Vulkan API. It interacts with the Device to perform resource lookups
     * and other device-specific operations.
     */
    class CommandEncoder final : public kege::CommandEncoder {
    public:

        /**
         * @brief Issues a non-indexed draw command.
         *
         * @param vertex_count Number of vertices to draw.
         * @param instance_count Number of instances to draw.
         * @param first_vertex Index of the first vertex to draw.
         * @param first_instance Instance ID of the first instance to draw.
         */
        void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) override;

        /**
         * @brief Issues an indexed draw command.
         *
         * @param index_count Number of indices to draw.
         * @param instance_count Number of instances to draw.
         * @param first_index Index of the first index to draw.
         * @param vertex_offset Integer offset added to the vertex index before indexing into the vertex buffer.
         * @param first_instance Instance ID of the first instance to draw.
         */
        void drawIndexed
        (
            uint32_t index_count,
            uint32_t instance_count,
            uint32_t first_index,
            int32_t  vertex_offset,
            uint32_t first_instance
        ) override;

        void drawIndexIndirect( BufferHandle buffer, VkDeviceSize offset, uint32_t draw_count, uint32_t stride )override;

        void drawIndirect( BufferHandle buffer, VkDeviceSize offset, uint32_t draw_count, uint32_t stride )override;

        /**
         * @brief Dispatches a compute shader.
         *
         * @param group_count_x Number of workgroups to dispatch in the X dimension.
         * @param group_count_y Number of workgroups to dispatch in the Y dimension.
         * @param group_count_z Number of workgroups to dispatch in the Z dimension.
         */
        void dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) override;

        // --- IGraphicsCommandBuffer Interface Implementation ---

        /**
         * @brief Binds one or more vertex buffers to the command buffer.
         *
         * @param first_binding The first vertex buffer binding slot to use.
         * @param buffer_handles Vector of opaque handles to the vertex buffer objects.
         * @param offsets Vector of offsets (in bytes) into each vertex buffer. Must have the same size as `buffer_handles`.
         */
        void bindVertexBuffers(uint32_t first_binding, const std::vector<BufferHandle>& buffer_handles, const std::vector<uint64_t>& offsets) override;

        /**
         * @brief Binds a Vulkan index buffer to the command buffer.
         *
         * @param buffer_handle Opaque handle to the index buffer object.
         * @param offset Offset (in bytes) into the index buffer.
         * @param use_uint16 True if the index buffer contains 16-bit unsigned integers,
         * false if it contains 32-bit unsigned integers.
         */
        void bindIndexBuffer(BufferHandle buffer_handle, uint64_t offset, bool use_uint16) override;

        void bindDescriptorSets( DescriptorSetHandle handle, bool globally = false ) override;

        void setPushConstants( ShaderStage stages, uint32_t offset, uint32_t size, const void *data )override;

        /**
         * @brief Binds a Vulkan graphics pipeline to the command buffer.
         *
         * @param pipeline_handle Opaque handle to the graphics pipeline object.
         */
        void bindGraphicsPipeline(PipelineHandle pipeline_handle) override;

        /**
         * @brief Binds a Vulkan compute pipeline to the command buffer.
         *
         * @param pipeline_handle Opaque handle to the compute pipeline object.
         */
        void bindComputePipeline(PipelineHandle pipeline_handle) override;

        /**
         * @brief Sets the viewport for rendering.
         *
         * @param viewport Structure containing the viewport parameters.
         */
        void setViewport(const Viewport& viewport) override;

        /**
         * @brief Sets the scissor rectangle for rendering.
         *
         * @param rect Structure containing the scissor rectangle parameters.
         */
        void setScissor(const Rect2D& rect) override;

        /**
         * @brief Executes a batch of Vulkan pipeline barriers based on abstract resource barriers.
         *
         * This function translates the abstract barrier descriptions into Vulkan-specific
         * VkMemoryBarrier, VkBufferMemoryBarrier, and VkImageMemoryBarrier structures
         * and inserts them into the command stream. It uses the provided resource registry
         * to look up the physical Vulkan handles of the involved resources.
         *
         * @param abstract_barriers Vector of abstract resource barrier descriptions.
         * @param registry Function object that maps logical Render Graph IDs to physical
         * device handles (Buffer* or Image*).
         */
        void pipelineBarrierBatch(const std::vector<AbstractResourceBarrier>& abstract_barriers, const ResourceRegistry& registry) override;

        /**
         * @brief Copies data between two Vulkan buffers.
         *
         * Assumes that the buffers are in a suitable layout for transfer operations.
         *
         * @param src_buffer Handle to the source buffer.
         * @param dst_buffer Handle to the destination buffer.
         * @param regions Vector of buffer copy regions specifying the source and
         * destination offsets and the size of the data to copy.
         */
        void copyBuffer(BufferHandle src_buffer, BufferHandle dst_buffer, const std::vector<BufferCopyRegion>& regions) override;

        /**
         * @brief Copies data between two Vulkan textures.
         *
         * Assumes that the textures are in a suitable layout for transfer operations.
         *
         * @param src_texture Handle to the source texture.
         * @param dst_texture Handle to the destination texture.
         * @param regions Vector of texture copy regions specifying the subresource
         * layers, offsets, and extents of the data to copy.
         */
        void copyTexture(ImageHandle src_texture, ImageHandle dst_texture, const std::vector<TextureCopyRegion>& regions) override;

        /**
         * @brief Copies data from a Vulkan buffer to a Vulkan texture.
         *
         * Assumes that the buffer and texture are in suitable layouts for transfer
         * operations.
         *
         * @param src_buffer Handle to the source buffer.
         * @param dst_texture Handle to the destination texture.
         * @param regions Vector of buffer-to-texture copy regions specifying the buffer
         * offset and layout, and the texture subresource layers, offset,
         * and extent of the data to copy.
         */
        void copyBufferToTexture(BufferHandle src_buffer, ImageHandle dst_texture, const std::vector<BufferTextureCopyRegion>& regions) override;

        /**
         * @brief Copies data from a Vulkan texture to a Vulkan buffer.
         *
         * Assumes that the texture and buffer are in suitable layouts for transfer
         * operations.
         *
         * @param src_texture Handle to the source texture.
         * @param dst_buffer Handle to the destination buffer.
         * @param regions Vector of texture-to-buffer copy regions specifying the texture
         * subresource layers, offset, and extent of the data to copy,
         * and the destination buffer offset and layout.
         */
        void copyTextureToBuffer(ImageHandle src_texture, BufferHandle dst_buffer, const std::vector<BufferTextureCopyRegion>& regions) override;

        /**
         * @brief Clears the color values of a Vulkan texture.
         *
         * Assumes that the texture layout is suitable for clearing.
         *
         * @param texture Handle to the texture to clear.
         * @param color Array of four floats representing the RGBA color to clear to.
         * @param ranges Vector of texture subresource ranges to clear.
         * (Note: The actual structure `TextureSubresourceRange` needs to be defined).
         */
        void clearColorTexture(ImageHandle texture, const float color[4], const std::vector<TextureSubresourceRange>& ranges) override;

        /**
         * @brief Clears the depth and/or stencil values of a Vulkan texture.
         *
         * Assumes that the texture layout is suitable for clearing.
         *
         * @param texture Handle to the texture to clear.
         * @param depth The depth value to clear to.
         * @param stencil The stencil value to clear to.
         * @param ranges Vector of texture subresource ranges to clear.
         * (Note: The actual structure `TextureSubresourceRange` needs to be defined).
         */
        void clearDepthStencilTexture(ImageHandle texture, float depth, uint32_t stencil, const std::vector<TextureSubresourceRange>& ranges) override;

        void clearAttachments
        (
            const std::vector< kege::ClearAttachment >& clear_attachments,
            const std::vector< kege::ClearRect >& clear_rects
        )
        override;

        /**
         * @brief Helper function to convert an abstract RenderGraph::ResourceState and Format to a Vulkan VkImageLayout.
         *
         * This function needs access to the Device or utility functions that handle
         * the mapping between abstract resource states and Vulkan-specific image layouts,
         * taking into account the format of the image.
         *
         * @param state The abstract resource state.
         * @param format The format of the texture.
         * @return The corresponding Vulkan VkImageLayout.
         */
        VkImageLayout stateToVkLayout(ResourceState state, Format format) const;

        bool isValid()const;

        /**
         * @brief Default destructor. No specific Vulkan resources are owned directly by this class.
         */
        ~CommandEncoder();
        CommandEncoder();

    private:

        const PipelineLayout* _current_pipeline_layout;
        VkPipelineBindPoint _current_pipeline_bindpoint;

        vk::CommandBuffer* _command_buffer;
        VkCommandBuffer _handle;

        friend vk::CommandBuffer;
    };

}
#endif /* vulkan_command_encoder_hpp */
