//
//  vulkan-command-buffer.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#ifndef vulkan_command_recorder_hpp
#define vulkan_command_recorder_hpp

#include "vk-shader-pipeline-layout.hpp"
#include "vk-command-encoder.hpp"

namespace kege::vk{

    // Forward Declarations
    class Device;
    
    /**
     * @brief Concrete implementation of the IGraphicsCommandBuffer interface for the Vulkan API.
     *
     * This class encapsulates a Vulkan command buffer object (VkCommandBuffer)
     * and provides methods to record graphics and compute commands using the
     * Vulkan API. It interacts with the Device to perform resource lookups
     * and other device-specific operations.
     */
    class CommandBuffer final : public kege::CommandBuffer {
    private:

        /**
         * @brief Constructor for the vk::CommandBuffer.
         *
         * @param device Pointer to the Device that owns this command buffer.
         * @param command_pool The Vulkan command pool that this buffer was allocated from.
         */
        CommandBuffer(vk::Device* device, kege::QueueType type, VkCommandPool command_pool);

    public:

        const vk::CommandBuffer* vk()const override{ return this; }
        vk::CommandBuffer* vk()override{ return this; }


        CommandEncoder* createCommandEncoder() override;
        kege::Semaphore* getSemaphore() override;

        /**
         * @fn bind
         * @brief Binds a shader resource set to the command buffer using a unique global uniform key that maps to the set.
         * 
         * @param global_uniform_key The key identifying the global uniform block to bind the set to.
         * @param set The Vulkan descriptor set to bind.
         * 
         * @return True if the binding was successful, false otherwise.
         */
        bool bind( uint32_t global_uniform_key, const VkDescriptorSet& set );

        void bindShaderResourceSet( const kege::ShaderResourceSet* set );

        /**
         * @brief Sets push constant data for the command buffer.
         *
         * @param stages Shader stages that will access the push constants.
         * @param offset Offset (in bytes) into the push constant range.
         * @param size Size (in bytes) of the push constant data.
         * @param data Pointer to the push constant data.
         */
        void setPushBlock
        ( 
            ShaderStageFlag stages, 
            uint32_t offset, 
            uint32_t size, 
            const void *data 
        )
        override;

        /**
         * @brief Binds a Vulkan graphics pipeline to the command buffer.
         *
         * @param pipeline Opaque handle to the graphics pipeline object.
         */
        void bindShaderPipeline(const ref::ShaderPipeline& pipeline) override;

        /**
         * @brief Transitions the layout of an image resource. This is necessary in Vulkan to ensure that
         * the image is in the correct layout for the intended usage (e.g., as a render target, shader read, etc.).
         */
        void transitionImageLayout
        (
            ref::Image image,
            kege::ImageLayout oldLayout,
            kege::ImageLayout newLayout
        )
        override;

        /**
         * @brief Issues an indexed indirect draw command.
         *
         * @param buffer The buffer containing the draw parameters.
         * @param offset The offset into the buffer where the draw parameters start.
         * @param draw_count The number of draws to execute.
         * @param stride The stride between draw parameters.
         */
        void drawIndexIndirect
        ( 
            const ref::Buffer& buffer, 
            uint64_t offset, 
            uint32_t draw_count, 
            uint32_t stride 
        )
        override;

        /**
         * @brief Issues an indirect draw command.
         *
         * @param buffer The buffer containing the draw parameters.
         * @param offset The offset into the buffer where the draw parameters start.
         * @param draw_count The number of draws to execute.
         * @param stride The stride between draw parameters.
         */
        void drawIndirect
        (
            const ref::Buffer& buffer, 
            uint64_t offset, 
            uint32_t draw_count, 
            uint32_t stride 
        )
        override;

        /**
         * @brief Issues a non-indexed draw command.
         *
         * @param vertex_count Number of vertices to draw.
         * @param instance_count Number of instances to draw.
         * @param first_vertex Index of the first vertex to draw.
         * @param first_instance Instance ID of the first instance to draw.
         */
        void draw
        (
            uint32_t vertex_count, 
            uint32_t instance_count, 
            uint32_t first_vertex, 
            uint32_t first_instance
        )
        override;

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
            int32_t vertex_offset,
            uint32_t first_instance
        )
        override;

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
         * @brief Begins recording commands into the Vulkan command buffer.
         *
         * @return True if the command buffer began recording successfully, false otherwise.
         */
        bool beginCommands( CommandBufferUsage usage = CommandBufferUsage::SimultaneousUse ) override;

        /**
         * @brief Ends recording commands into the Vulkan command buffer.
         *
         * @return True if the command buffer ended recording successfully, false otherwise.
         */
        bool endCommands() override;

        /**
         * @brief Checks if the Vulkan command buffer is currently recording commands.
         *
         * @return True if the command buffer is recording, false otherwise.
         */
        bool isRecording() const override { return _is_recording; }

        /**
         * @brief Begins a dynamic rendering pass using Vulkan's VK_KHR_dynamic_rendering extension.
         *
         * Assumes that the image layouts of the attachments are already set to the
         * `initial_layout` specified in the `rendering_info`.
         *
         * @param rendering_info Structure containing information about the dynamic rendering pass.
         */
        void beginRendering(const RenderingInfo& rendering_info) override;

        /**
         * @brief Ends the current dynamic rendering pass.
         *
         * After calling this function, the attachments will be in their
         * `final_layout` as defined in `RenderingAttachmentInfo`.
         */
        void endRendering() override;

        /**
         * @brief Binds one or more vertex buffers to the command buffer.
         *
         * @param first_binding The first vertex buffer binding slot to use.
         * @param buffers Vector of opaque handles to the vertex buffer objects.
         * @param offsets Vector of offsets (in bytes) into each vertex buffer. Must have the same size as `buffer_handles`.
         */
        void bindVertexBuffers
        (
            uint32_t first_binding, 
            const std::vector< ref::Buffer >& buffers, 
            const std::vector<uint64_t>& offsets
        )
        override;

        /**
         * @brief Binds a Vulkan index buffer to the command buffer.
         *
         * @param buffer Opaque handle to the index buffer object.
         * @param offset Offset (in bytes) into the index buffer.
         * @param use_uint16 True if the index buffer contains 16-bit unsigned integers,
         * false if it contains 32-bit unsigned integers.
         */
        void bindIndexBuffer( const ref::Buffer& buffer, uint64_t offset, bool use_uint16) override;

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
        void setScissor(const Scissor& scissor) override;

        /**
         * @brief Inserts a pipeline barrier into the command buffer to synchronize access to resources.
         *
         * This function allows you to specify memory barriers for images and buffers to ensure proper
         * synchronization between different stages of the pipeline and different command buffers.
         * The `image_barriers` vector contains barriers for image resources, while the `buffer_barriers` 
         * vector contains barriers for buffer resources. Each barrier specifies the source and destination 
         * access types, pipeline stages, and other relevant information needed to perform the 
         * synchronization correctly.
         * 
         * @param image_barriers Vector of image memory barriers to insert.
         * @param buffer_barriers Vector of buffer memory barriers to insert.
         */
        void pipelineBarrier
        (
            std::vector< ImageMemoryBarrier >& image_barriers,
            std::vector< BufferMemoryBarrier >& buffer_barriers
        )
        override;

        /**
         * @brief Copies data between two Vulkan buffers.
         *
         * Assumes that the buffers are in a suitable layout for transfer operations.
         *
         * @param src Handle to the source buffer.
         * @param dst Handle to the destination buffer.
         * @param regions Vector of buffer copy regions specifying the source and
         * destination offsets and the size of the data to copy.
         */
        void copyBuffer
        (
            const ref::Buffer& src, ref::Buffer& dst,
            const std::vector<BufferCopyRegion>& regions
        )
        override;
        
        /**
         * @brief Copies data between two Vulkan buffers.
         *
         * Assumes that the buffers are in a suitable layout for transfer operations.
         *
         * @param src Handle to the source buffer.
         * @param dst Handle to the destination buffer.
         * @param regions Vector of buffer copy regions specifying the source and
         * destination offsets and the size of the data to copy.
         */
        void copyBuffer
        ( 
            const vk::Buffer& src, 
            vk::Buffer& dst, 
            const std::vector< VkBufferCopy >& regions
        );

        /**
         * @brief Copies data between two Vulkan textures.
         *
         * Assumes that the textures are in a suitable layout for transfer operations.
         *
         * @param src Handle to the source texture.
         * @param dst Handle to the destination texture.
         * @param regions Vector of texture copy regions specifying the subresource
         * layers, offsets, and extents of the data to copy.
         */
        void copyImage
        (
            const ref::Image& src, 
            ref::Image& dst, 
            const std::vector<ImageCopyRegion>& regions
        ) 
        override;

        /**
         * @brief Copies data from a Vulkan buffer to a Vulkan texture.
         *
         * Assumes that the buffer and texture are in suitable layouts for transfer
         * operations.
         *
         * @param src Handle to the source buffer.
         * @param dst Handle to the destination texture.
         * @param regions Vector of buffer-to-texture copy regions specifying the buffer
         * offset and layout, and the texture subresource layers, offset,
         * and extent of the data to copy.
         */
        void copyBufferToImage
        (
            const ref::Buffer& src, ref::Image& dst,
            const std::vector<BufferTextureCopyRegion>& regions
        )
        override;

        /**
         * @brief Copies data from a Vulkan texture to a Vulkan buffer.
         *
         * Assumes that the texture and buffer are in suitable layouts for transfer
         * operations.
         *
         * @param src Handle to the source texture.
         * @param dst Handle to the destination buffer.
         * @param regions Vector of texture-to-buffer copy regions specifying the texture
         * subresource layers, offset, and extent of the data to copy,
         * and the destination buffer offset and layout.
         */
        void copyTextureToBuffer
        (
            const ref::Image& src, ref::Buffer& dst,
            const std::vector<BufferTextureCopyRegion>& regions
        )
        override;

        /**
         * @brief Clears the color values of a Vulkan texture.
         *
         * Assumes that the texture layout is suitable for clearing.
         *
         * @param image Handle to the texture to clear.
         * @param color Array of four floats representing the RGBA color to clear to.
         * @param ranges Vector of texture subresource ranges to clear.
         * (Note: The actual structure `ImageSubresourceRange` needs to be defined).
         */
        void clearColorTexture
        (
            ref::Image& image, 
            const float color[4], 
            const std::vector<ImageSubresourceRange>& ranges
        ) 
        override;

        /**
         * @brief Clears the depth and/or stencil values of a Vulkan texture.
         *
         * Assumes that the texture layout is suitable for clearing.
         *
         * @param image Handle to the texture to clear.
         * @param depth The depth value to clear to.
         * @param stencil The stencil value to clear to.
         * @param ranges Vector of texture subresource ranges to clear.
         * (Note: The actual structure `ImageSubresourceRange` needs to be defined).
         */
        void clearDepthStencilTexture
        (
            ref::Image& image, 
            float depth, 
            uint32_t stencil, 
            const std::vector<ImageSubresourceRange>& ranges
        ) 
        override;

        void clearAttachments
        (
            const std::vector< kege::ClearAttachment >& clear_attachments,
            const std::vector< kege::ClearRect >& clear_rects
        )
        override;

        // --- Vulkan Specific ---

        /**
         * @brief Returns the underlying Vulkan command buffer handle.
         *
         * This can be useful for interacting directly with the Vulkan API if needed.
         *
         * @return The VkCommandBuffer handle.
         */
        VkCommandBuffer getVkCommandBuffer() const { return _handle; }

        /**
         * @brief Returns the Vulkan command pool that this command buffer belongs to.
         *
         * @return The VkCommandPool handle.
         */
        VkCommandPool getVkCommandPool() const { return _command_pool; }

        /**
         * @brief Returns the Vulkan command buffer handle. This is an alias for 
         * getVkCommandBuffer() and is used when a pointer to the command buffer is needed.
         */
        const VkCommandBuffer& handle()const{ return _handle; }

        /**
         * @brief Default destructor. No specific Vulkan resources are owned directly by this class.
         */
        ~CommandBuffer();
       CommandBuffer();

    private:

        /**
         * The secondary command buffer objects
         */
        std::vector< vk::CommandEncoder* > _command_encoders;

        kege::ref::Semaphore _semaphore;

        /**
         * The dynamic rendering color attachment formats for secondary command beffers inhertance info
         */
        std::vector< VkFormat > _color_attachment_formats;

        /**
         * The dynamic rendering depth attachment format for secondary command beffers inhertance info
         */
        VkFormat _depth_attachment_format;

        /**
         * The dynamic rendering stencil attachment format for secondary command beffers inhertance info
         */
        VkFormat _stencil_attachment_format;

        /**
         * Pointer to the Device that owns this command buffer. Used for resource lookups and other device-specific operations.
         */
        vk::Device* _device;

        /**
         * The Vulkan command pool that this command buffer was allocated from.
         */
        VkCommandPool _command_pool;

        /**
         * The actual Vulkan command buffer handle.
         */
        VkCommandBuffer _handle;

        /**
         * Flag indicating whether the command buffer is currently recording commands.
         */
        bool _is_recording = false;

        /**
         * The pipeline layout that is currently bound.
         */
        const vk::ShaderPipelineLayout* _bind_pipeline_layout;

        /**
         * The pipeline that is currently bound.
         */
        const vk::ShaderPipeline* _bind_pipeline;

        /**
         * The current pipeline bind point.
         */
        VkPipelineBindPoint _current_pipeline_bindpoint;

        /**
         * The number of descriptor sets that are currently bound.
         */
        int _sets_bind_state;

        uint32_t _encoder_count;

    private:

        /**
         * Function pointers for Vulkan dynamic rendering commands. These are loaded 
         * at runtime using vkGetInstanceProcAddr and are used to call the 
         * VK_KHR_dynamic_rendering extension functions
         */
        static PFN_vkCmdBeginRendering vkCmdBeginRenderingPfn;

        /**
         * @brief Function pointer for vkCmdEndRendering. Loaded at runtime using 
         * vkGetInstanceProcAddr and used to call the VK_KHR_dynamic_rendering 
         * extension function to end a dynamic rendering pass.
         */
        static PFN_vkCmdEndRendering vkCmdEndRenderingPfn;

    private:

        friend vk::CommandEncoder;
        friend vk::FrameRenderer;
        friend vk::Device;
        friend vk::List< vk::CommandBuffer >;
        
        vk::CommandBuffer* prev;
        vk::CommandBuffer* next;
    };

}
#endif /* vulkan_command_recorder_hpp */
