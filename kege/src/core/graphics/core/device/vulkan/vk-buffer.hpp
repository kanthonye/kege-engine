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

        VkResult create( vk::Device* device, const kege::BufferDesc& desc );
        
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

        const vk::Buffer* vk() const override { return this; }
        vk::Buffer* vk() override { return this; }

        const VkBuffer& getHandle() const;
        const VkDeviceMemory& getMemoryHandle() const;


        Buffer( vk::Device* device, const kege::BufferDesc& desc );
        Buffer();
        
        ~Buffer();

    private:

        /** @brief The buffer's memory */
        VkDeviceMemory _memory = VK_NULL_HANDLE;

        /** @brief Native Vulkan buffer handle */
        VkBuffer _handle = VK_NULL_HANDLE;

        /** @brief Pointer to mapped memory region (only valid if buffer is host-visible) */
        void* _mapped_ptr = nullptr;

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
