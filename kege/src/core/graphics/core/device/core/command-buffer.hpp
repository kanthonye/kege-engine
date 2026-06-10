/**
 * @file graphics_command_buffer.hpp
 * @brief Defines an abstract interface for graphics command buffers.
 *
 * This interface provides a platform-agnostic way to record rendering and
 * compute commands. Implementations of this interface will encapsulate
 * the specific command buffer objects of the underlying graphics API
 * (e.g., Vulkan's VkCommandBuffer, Direct3D 12's ID3D12GraphicsCommandList).
 *
 * The interface includes methods for managing the command buffer lifecycle,
 * initiating and ending render passes (using dynamic rendering), binding
 * resources and pipelines, setting dynamic state, dispatching draw and
 * compute commands, managing synchronization (pipeline barriers), performing
 * data transfer operations (copies), and clearing attachments.
 *
 * The design emphasizes the integration with a Render Graph system, evident
 * in the `pipelineBarrierBatch` method which takes abstract resource barriers
 * and a resource registry. This allows the Render Graph to manage resource
 * states and transitions in a decoupled manner.
 */

#ifndef kege_command_buffer_hpp
#define kege_command_buffer_hpp

#include "sync/semaphore.hpp"
#include "command-encoder.hpp"
#include "rendering-info.hpp"

namespace kege
{
    class CommandBuffer;
}

namespace kege::vk
{
    class CommandBuffer;
}

namespace kege::ref
{
    typedef kege::Ref< kege::CommandBuffer > CommandBuffer;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::CommandBuffer > CommandBuffer;
}

namespace kege{

    /**
     * @brief Abstract interface for graphics command buffers.
     *
     * This interface provides methods for recording graphics and compute
     * commands in a platform-independent manner. Concrete implementations
     * will wrap the native command buffer objects of the underlying graphics API.
     */
    class CommandBuffer : public kege::RefCounter
    {
    public:

        virtual kege::Semaphore* getSemaphore() = 0;

        /**
         * @fn transitionImageLayout
         * @brief Transitions the layout of an image resource. This is necessary in Vulkan to ensure that
         * the image is in the correct layout for the intended usage (e.g., as a render target, shader read, etc.). 
         * The Render Graph is responsible for performing the necessary layout transitions before calling this function.
         * 
         * @param image The image resource to transition.
         * @param oldLayout The current layout of the image. This should match the layout that the Render Graph has 
         * transitioned the image to before this function is called.
         * @param newLayout The desired layout to transition the image to for subsequent usage in this command buffer.
         */
        virtual void transitionImageLayout
        (
            ref::Image image,
            kege::ImageLayout oldLayout,
            kege::ImageLayout newLayout
        ) = 0;

        virtual CommandEncoder* createCommandEncoder() = 0;

        /**
         * @brief Checks if the command buffer is currently recording commands.
         * @return True if the command buffer is recording, false otherwise.
         */
        virtual bool isRecording() const = 0;

        /**
         * @brief Begins recording commands into the command buffer.
         * @return True if the command buffer began recording successfully, false otherwise.
         */
        virtual bool beginCommands
        ( 
            CommandBufferUsage usage = CommandBufferUsage::SimultaneousUse 
        ) 
        = 0;

        /**
         * @brief Ends recording commands into the command buffer.
         * @return True if the command buffer ended recording successfully, false otherwise.
         */
        virtual bool endCommands() = 0;

        // virtual void reset() = 0; // Needed if command buffers are pooled/reused

        /**
         * @brief Begins a dynamic rendering pass.
         *
         * Assumes that the layouts of the attachments specified in `rendering_info`
         * are already in their `initial_layout` as defined in `RenderingAttachmentInfo`.
         * The Render Graph is responsible for performing these layout transitions
         * *before* this function is called.
         *
         * @param rendering_info Structure containing information about the rendering pass,
         * including render area and attachments.
         */
        virtual void beginRendering(const RenderingInfo& rendering_info) = 0;

        /**
         * @brief Ends the current dynamic rendering pass.
         *
         * After calling this function, the attachments will be in their
         * `final_layout` as defined in `RenderingAttachmentInfo`.
         */
        virtual void endRendering() = 0;

        virtual void bindShaderResourceSet( const kege::ShaderResourceSet* set ) = 0;

        /**
         * @brief Sets push constant data for the command buffer.
         *
         * @param stages Shader stages that will access the push constants.
         * @param offset Offset (in bytes) into the push constant range.
         * @param size Size (in bytes) of the push constant data.
         * @param data Pointer to the push constant data.
         */
        virtual void setPushBlock
        ( 
            ShaderStageFlag stages, 
            uint32_t offset, 
            uint32_t size, 
            const void *data 
        ) 
        = 0;

        /**
         * @brief Binds an index buffer to the command buffer.
         * @param buffer Opaque handle to the index buffer object.
         * @param offset Offset (in bytes) into the index buffer.
         * @param use_uint16 True if the index buffer contains 16-bit unsigned integers,
         * false if it contains 32-bit unsigned integers.
         */
        virtual void bindIndexBuffer
        ( 
            const ref::Buffer& buffer, 
            uint64_t offset, 
            bool use_uint16 
        ) 
        = 0;

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
        ) 
        = 0;

        /**
         * @brief Binds a graphics pipeline to the command buffer.
         * @param pipeline Opaque handle to the graphics pipeline object.
         */
        virtual void bindShaderPipeline
        (
            const ref::ShaderPipeline& pipeline
        ) 
        = 0;

        /**
         * @brief Sets the viewport for rendering.
         * @param viewport Structure containing the viewport parameters.
         */
        virtual void setViewport(const Viewport& viewport) = 0;

        /**
         * @brief Sets the scissor rectangle for rendering.
         * @param scissor Structure containing the scissor rectangle parameters.
         */
        virtual void setScissor(const Scissor& scissor) = 0;

        /**
         * @brief Issues an indirect indexed draw command.
         * @param buffer Opaque handle to the buffer containing the draw parameters.
         * @param offset Offset (in bytes) into the buffer where the draw parameters start.
         * @param draw_count Number of draws to issue.
         */
        virtual void drawIndexIndirect
        ( 
            const ref::Buffer& buffer, 
            uint64_t offset, 
            uint32_t draw_count, 
            uint32_t stride 
        ) = 0;

        /**
         * @brief Issues an indirect draw command.
         * @param buffer Opaque handle to the buffer containing the draw parameters.
         * @param offset Offset (in bytes) into the buffer where the draw parameters start.
         * @param draw_count Number of draws to issue.
         */
        virtual void drawIndirect
        (
            const ref::Buffer& buffer, 
            uint64_t offset, 
            uint32_t draw_count, 
            uint32_t stride 
        ) = 0;

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
        virtual void draw
        (
            uint32_t vertex_count, 
            uint32_t instance_count, 
            uint32_t first_vertex, 
            uint32_t first_instance
        )
        = 0;

        /**
         * @brief Dispatches a compute shader.
         * @param group_count_x Number of workgroups to dispatch in the X dimension.
         * @param group_count_y Number of workgroups to dispatch in the Y dimension.
         * @param group_count_z Number of workgroups to dispatch in the Z dimension.
         */
        virtual void dispatch
        (
            uint32_t group_count_x, 
            uint32_t group_count_y, 
            uint32_t group_count_z
        ) 
        = 0;

        /**
         * @brief Issues a pipeline barrier to synchronize resource access and layout transitions.
         * @param image_barriers Vector of image memory barriers specifying the synchronization and layout transitions for images.
         * @param buffer_barriers Vector of buffer memory barriers specifying the synchronization for buffers.
         */
        virtual void pipelineBarrier
        (
            std::vector< ImageMemoryBarrier >& image_barriers,
            std::vector< BufferMemoryBarrier >& buffer_barriers
        )
        = 0;

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
        virtual void copyImage
        (
            const ref::Image& src, 
            ref::Image& dst, 
            const std::vector<ImageCopyRegion>& regions
        )
        = 0;

        /**
         * @brief Copies data from a buffer to a texture.
         *
         * Assumes that the buffer and texture are in suitable layouts for transfer
         * operations (e.g., `TRANSFER_SRC_OPTIMAL` and `TRANSFER_DST_OPTIMAL`).
         *
         * @param src ref to the source buffer.
         * @param dst ref to the destination texture.
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
         * @param src ref to the source texture.
         * @param dst ref to the destination buffer.
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

        /**
         * @brief Clears the color values of a texture.
         *
         * Assumes that the texture layout is suitable for clearing
         * (e.g., `TRANSFER_DST_OPTIMAL` or `GENERAL`).
         *
         * @param image ref to the texture to clear.
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
         * @param image ref to the texture to clear.
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

        /**
         * @brief Clears the attachments of a framebuffer.
         *
         * @param clear_attachments Vector of attachments to clear.
         * @param clear_rects Vector of rectangles to clear.
         */
        virtual void clearAttachments
        (
            const std::vector< kege::ClearAttachment >& clear_attachments,
            const std::vector< kege::ClearRect >& clear_rects
        )
        = 0;

        /**
         * @brief Gets the Vulkan command buffer handle.
         * @return Pointer to the Vulkan command buffer.
         */
        virtual const vk::CommandBuffer* vk()const{ return nullptr; }

        /**
         * @brief Gets the Vulkan command buffer handle.
         * @return Pointer to the Vulkan command buffer.
         */
        virtual vk::CommandBuffer* vk(){ return nullptr; }
        
        /**
         * @brief Gets the type of the queue to which this command buffer belongs.
         * @return The queue type.
         */
        QueueType getQueueType()const
        {
            return _queue_type;
        }

        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         */
        virtual ~CommandBuffer() = default;

        /**
         * @brief default constructor
         */
        CommandBuffer()
        :   _queue_type( QueueType::Graphics )
        ,   _id( -1 )
        {}

    protected:

        QueueType _queue_type;
        int32_t _id;
    };

}

#endif /* kege_command_buffer_hpp */
