//
//  vk-buffer.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/14/25.
//

#ifndef vk_buffer_hpp
#define vk_buffer_hpp

#include  "vk-utils.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan buffer resources
     *
     * Encapsulates a VkBuffer along with its memory allocation and metadata.
     * Uses VMA (Vulkan Memory Allocator) for efficient memory management.
     */
    class Buffer final : public kege::Buffer
    {
    public:

        struct Source
        {
            /** @brief The buffer's memory */
            VkDeviceMemory memory = VK_NULL_HANDLE;

            /** @brief Native Vulkan buffer handle */
            VkBuffer handle = VK_NULL_HANDLE;

            /** @brief Pointer to mapped memory region (only valid if buffer is host-visible) */
            void* mapped_ptr = nullptr;
        };

    public:

        VkResult create( const kege::BufferDesc& desc );
        
        // Copy between buffers
        void copyFromBuffer
        (
            uint64_t offset,
            uint64_t size,
            const ref::Buffer& src,
            uint64_t src_offset
        )override;

        void copyToBuffer
        (
            uint64_t offset,
            uint64_t size,
            ref::Buffer& dst,
            uint64_t dst_offset
        )override;

        // Copy data to/from buffer
        void copyFrom(const void* data, uint64_t size, uint64_t offset = 0)override;
        void copyTo(void* data, uint64_t size, uint64_t offset = 0)const override;

        // Map buffer memory for CPU access (if cpuAccessible is true)
        const void* map(uint64_t offset = 0, uint64_t size = 0)const override;
        void* map(uint64_t offset = 0, uint64_t size = 0)override;
        void unmap()override;

        const void* data()const override;
        void* data()override;

        bool resize( uint64_t size )override;
        bool empty()const override;
        void clear() override;

        int frames()const override;
        int getFrameIndex()const;

        const vk::Buffer* vk() const override { return this; }
        vk::Buffer* vk() override { return this; }

        const Buffer::Source& getSource( int i ) const { return _sources[ i ]; }
        Buffer::Source& getSource( int i ) { return _sources[ i ]; }

        const VkBuffer& getCurrHandle() const;


        Buffer( vk::Device* device, const kege::BufferDesc& desc );
        ~Buffer();

    private:

        Buffer::Source _sources[ MAX_FRAMES_IN_FLIGHT ];
        int _frames;

        //VkMemoryPropertyFlags _memory_properties;
        //VkBufferUsageFlags _usage;
        vk::Device* _device;
        friend vk::Device;

        friend List< vk::Buffer >;
        vk::Buffer* next;
        vk::Buffer* prev;
    };

}
#endif /* vk_buffer_hpp */
