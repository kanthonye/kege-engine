//
//  command-encoder.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/15/25.
//

#ifndef command_encoder_hpp
#define command_encoder_hpp

#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

#include "graphics-core.hpp"
#include "shader-set.hpp"

namespace kege{

    /**
     * @brief Abstract interface for graphics command buffers.
     *
     * This interface provides methods for recording graphics and compute
     * commands in a platform-independent manner. Concrete implementations
     * will wrap the native command buffer objects of the underlying graphics API.
     */
    class CommandEncoder
    {
    protected:

        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         */
        virtual ~CommandEncoder() = default;

    public:

        // --- Resource Binding ---

        /**
         * @brief Binds a shader set to the command buffer.
         *
         * @param set_index The index location where the shader set will be bind.
         * @param set The shader set object to bind.
         * @return True if binding succeeded, false otherwise.
         */
        virtual bool bind( int32_t set_index, const ref::ShaderSet& set ) = 0;

        /**
         * @brief Binds a shader set to the command buffer.
         *
         * @param set The shader set object to bind.
         * @return True if binding succeeded, false otherwise.
         */
        virtual bool bind( const ref::ShaderSet& set ) = 0;

        /**
         * @brief Sets push constant data for the command buffer.
         *
         * @param stages Shader stages that will access the push constants.
         * @param offset Offset (in bytes) into the push constant range.
         * @param size Size (in bytes) of the push constant data.
         * @param data Pointer to the push constant data.
         */
        virtual void setPushBlock( ShaderStageFlag stages, uint32_t offset, uint32_t size, const void *data ) = 0;

        /**
         * @brief Binds an index buffer to the command buffer.
         * @param buffer Opaque handle to the index buffer object.
         * @param offset Offset (in bytes) into the index buffer.
         * @param use_uint16 True if the index buffer contains 16-bit unsigned integers,
         * false if it contains 32-bit unsigned integers.
         */
        virtual void bindIndexBuffer(const ref::Buffer& buffer, uint64_t offset, bool use_uint16) = 0;

        /**
         * @brief Binds one or more vertex buffers to the command buffer.
         * @param first_binding The first vertex buffer binding slot to use.
         * @param buffers Vector of opaque handles to the vertex buffer objects.
         * @param offsets Vector of offsets (in bytes) into each vertex buffer. Must have the same size as `buffer_handles`.
         */
        virtual void bindVertexBuffers
        (
            uint32_t first_binding,
            const std::vector< ref::Buffer >& buffers,
            const std::vector<uint64_t>& offsets
        ) = 0;

        /**
         * @brief Binds a graphics pipeline to the command buffer.
         * @param pipeline Opaque handle to the graphics pipeline object.
         */
        virtual void bindShaderPipeline(const ref::ShaderPipeline& pipeline) = 0;


        // --- Drawing / Dispatching ---

        virtual void drawIndexIndirect( const ref::Buffer& buffer, uint64_t offset, uint32_t draw_count, uint32_t stride ) = 0;

        virtual void drawIndirect( const ref::Buffer& buffer, uint64_t offset, uint32_t draw_count, uint32_t stride ) = 0;

        /**
         * @brief Issues an indexed draw command.
         * @param index_count Number of indices to draw.
         * @param instance_count Number of instances to draw.
         * @param first_index Index of the first index to draw.
         * @param vertex_offset Integer offset added to the vertex index before indexing into the vertex buffer.
         * @param first_instance Instance ID of the first instance to draw.
         */
        virtual void drawIndexed
        (
            uint32_t index_count,
            uint32_t instance_count,
            uint32_t first_index,
            int32_t vertex_offset,
            uint32_t first_instance
        )
        = 0;

        /**
         * @brief Issues a non-indexed draw command.
         * @param vertex_count Number of vertices to draw.
         * @param instance_count Number of instances to draw.
         * @param first_vertex Index of the first vertex to draw.
         * @param first_instance Instance ID of the first instance to draw.
         */
        virtual void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) = 0;

        // --- Compute ---

        /**
         * @brief Dispatches a compute shader.
         * @param group_count_x Number of workgroups to dispatch in the X dimension.
         * @param group_count_y Number of workgroups to dispatch in the Y dimension.
         * @param group_count_z Number of workgroups to dispatch in the Z dimension.
         */
        virtual void dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) = 0;

        // --- Dynamic State ---

        /**
         * @brief Sets the viewport for rendering.
         * @param viewport Structure containing the viewport parameters.
         */
        virtual void setViewport(const Viewport& viewport) = 0;

        /**
         * @brief Sets the scissor rectangle for rendering.
         * @param rect Structure containing the scissor rectangle parameters.
         */
        virtual void setScissor(const Rect2D& rect) = 0;

        // --- Synchronization ---

        /**
         * @brief Executes a batch of pipeline barriers.
         *
         * This function takes a vector of abstract resource barriers (provided by the
         * Render Graph) and a resource registry to resolve the logical resource
         * handles to their physical device handles for the current frame. It assumes
         * that the resource lookup within the implementation will succeed.
         *
         * @param abstract_barriers Vector of abstract resource barrier descriptions.
         * @param registry Function object that maps logical Render Graph IDs to physical
         * device handles (BufferHandle or ref::Image).
         */
//        virtual void pipelineBarrierBatch
//        (
//            const std::vector< AbstractResourceBarrier >& abstract_barriers,
//            const ResourceRegistry& registry
//        )
//        = 0;

        // Optional: Single simple barrier (less common for RG)
        // virtual void pipelineBarrier(...) = 0;

        // --- Transfer / Copy Operations ---

        /**
         * @brief Copies data between two buffers.
         *
         * Assumes that the buffers are in a suitable layout for transfer operations
         * (e.g., `TRANSFER_SRC_OPTIMAL` and `TRANSFER_DST_OPTIMAL`).
         *
         * @param src Handle to the source buffer.
         * @param dst Handle to the destination buffer.
         * @param regions Vector of buffer copy regions specifying the source and
         * destination offsets and the size of the data to copy.
         */
        virtual void copyBuffer
        (
            const ref::Buffer& src, ref::Buffer& dst,
            const std::vector<BufferCopyRegion>& regions
        )
        = 0;

        /**
         * @brief Copies data between two textures.
         *
         * Assumes that the textures are in a suitable layout for transfer operations
         * (e.g., `TRANSFER_SRC_OPTIMAL` and `TRANSFER_DST_OPTIMAL`).
         *
         * @param src Handle to the source texture.
         * @param dst Handle to the destination texture.
         * @param regions Vector of texture copy regions specifying the subresource
         * layers, offsets, and extents of the data to copy.
         */
        virtual void copyImage(const ref::Image& src, ref::Image& dst, const std::vector<ImageCopyRegion>& regions) = 0;

        /**
         * @brief Copies data from a buffer to a texture.
         *
         * Assumes that the buffer and texture are in suitable layouts for transfer
         * operations (e.g., `TRANSFER_SRC_OPTIMAL` and `TRANSFER_DST_OPTIMAL`).
         *
         * @param src Handle to the source buffer.
         * @param dst Handle to the destination texture.
         * @param regions Vector of buffer-to-texture copy regions specifying the buffer
         * offset and layout, and the texture subresource layers, offset,
         * and extent of the data to copy.
         */
        virtual void copyBufferToImage
        (
            const ref::Buffer& src,
            ref::Image& dst,
            const std::vector<BufferTextureCopyRegion>& regions
        )
        = 0;

        /**
         * @brief Copies data from a texture to a buffer.
         *
         * Assumes that the texture and buffer are in suitable layouts for transfer
         * operations (e.g., `TRANSFER_SRC_OPTIMAL` and `TRANSFER_DST_OPTIMAL`).
         *
         * @param src Handle to the source texture.
         * @param dst Handle to the destination buffer.
         * @param regions Vector of texture-to-buffer copy regions specifying the texture
         * subresource layers, offset, and extent of the data to copy,
         * and the destination buffer offset and layout.
         */
        virtual void copyTextureToBuffer
        (
            const ref::Image& src, ref::Buffer& dst,
            const std::vector<BufferTextureCopyRegion>& regions
        )
        = 0;

        // virtual void blitTexture(...) = 0; // Optional for scaling/format conversion

        // --- Clears ---

        /**
         * @brief Clears the color values of a texture.
         *
         * Assumes that the texture layout is suitable for clearing
         * (e.g., `TRANSFER_DST_OPTIMAL` or `GENERAL`).
         *
         * @param image Handle to the texture to clear.
         * @param color Array of four floats representing the RGBA color to clear to.
         * @param ranges Vector of texture subresource ranges to clear.
         * (Note: The actual structure `ImageSubresourceRange` is not defined here).
         */
        virtual void clearColorTexture
        (
            ref::Image& image,
            const float color[4],
            const std::vector< ImageSubresourceRange >& ranges
        )
        = 0;

        /**
         * @brief Clears the depth and/or stencil values of a texture.
         *
         * Assumes that the texture layout is suitable for clearing
         * (e.g., `TRANSFER_DST_OPTIMAL` or `GENERAL`).
         *
         * @param image Handle to the texture to clear.
         * @param depth The depth value to clear to.
         * @param stencil The stencil value to clear to.
         * @param ranges Vector of texture subresource ranges to clear.
         * (Note: The actual structure `ImageSubresourceRange` is not defined here).
         */
        virtual void clearDepthStencilTexture
        (
            ref::Image& image,
            float depth, uint32_t stencil,
            const std::vector< ImageSubresourceRange >& ranges
        )
        = 0;

        virtual void clearAttachments
        (
            const std::vector< kege::ClearAttachment >& clear_attachments,
            const std::vector< kege::ClearRect >& clear_rects
        )
        = 0;

        // Add other commands as needed: indirect draw/dispatch, queries, etc.


        uint32_t id()const{ return _id; }


        QueueType getQueueType()const
        {
            return _queue_type;
        }

        CommandEncoder()
        :   _queue_type( QueueType::Graphics )
        ,   _id( -1 )
        {}

    protected:

        QueueType _queue_type;
        int32_t _id;
    };

}

#endif /* command_encoder_hpp */
