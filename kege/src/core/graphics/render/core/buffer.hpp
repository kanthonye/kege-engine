//
//  buffer.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/14/25.
//

#ifndef kege_graphics_buffer_hpp
#define kege_graphics_buffer_hpp

#include "graphics-core.hpp"

namespace kege{

    /**
     * @brief Describes a buffer memory barrier.
     *
     * Contains all the information needed to synchronize access to a buffer resource
     * between different pipeline stages, access types, and/or queue families.
     *
     * Buffer barriers are used to ensure proper memory visibility and execution
     * ordering when buffers are accessed in different ways throughout a frame.
     */
    struct BufferMemoryBarrier
    {
        std::string resource_name;  ///< Name of the buffer in the RenderGraph (for debugging/tracking)

        /// @name Access Synchronization
        /// @{
        AccessFlags src_access;     ///< Access type before the barrier (e.g., what operations were done)
        AccessFlags dst_access;     ///< Access type after the barrier (e.g., what operations will be done)
        /// @}

        /// @name Pipeline Synchronization
        /// @{
        PipelineStageFlag src_stage;   ///< Pipeline stage where previous accesses completed
        PipelineStageFlag dst_stage;   ///< Pipeline stage where subsequent accesses will begin
        /// @}

        /// @name Queue Family Ownership
        /// @{
        QueueType src_queue = QueueType::Graphics; ///< Queue family that previously owned the buffer
        QueueType dst_queue = QueueType::Graphics; ///< Queue family that will own the buffer
        /// @}

        /// @name Buffer Region Specification
        /// @{
        size_t offset = 0;        ///< Starting byte offset of the affected buffer region
        size_t size = ~0ull;      ///< Size of the affected region in bytes (~0ull means entire buffer)
        kege::ref::Buffer buffer;
        /// @}
    };
    
    /**
     * @brief Bitmask flags specifying buffer usage capabilities.
     *
     * These flags determine how a buffer can be used and what operations
     * are valid for the buffer resource. Multiple flags can be combined.
     *
     * @note Usage flags may affect memory allocation and performance characteristics.
     */
    enum class BufferUsages : uint32_t
    {
        None                = 0, ///< No specific usage (invalid for creation)
        CopySrc             = 1 << 0, ///< Can be source of copy operations (GPU-GPU transfers)
        CopyDst             = 1 << 1, ///< Can be destination of copy operations (GPU-GPU transfers)
        VertexBuffer        = 1 << 2, ///< Can be bound as vertex input buffer
        IndexBuffer         = 1 << 3, ///< Can be bound as index buffer
        UniformBuffer       = 1 << 4, ///< Can be bound as uniform buffer (UBO/CBV)
        StorageBuffer       = 1 << 5, ///< Can be bound as storage buffer (SSBO/UAV)
        IndirectBuffer      = 1 << 6, ///< Can be used for indirect draw/dispatch commands
        UniformTexelBuffer  = 1 << 7, ///< Can be bound as uniform buffer (UBO/CBV)
        StorageTexelBuffer  = 1 << 8, ///< Can be bound as storage buffer (SSBO/UAV)
        ShaderDeviceAddress = 1 << 9, ///< Can get GPU virtual address (for bindless/buffer-device-address)
    };

    // Enable bitwise operations for BufferUsages

    inline constexpr BufferUsages operator ~ (BufferUsages a)
    {
        return static_cast<BufferUsages>(~static_cast<uint32_t>(a));
    }

    inline constexpr BufferUsages operator | (BufferUsages a, BufferUsages b)
    {
        return static_cast<BufferUsages>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline constexpr BufferUsages operator & (BufferUsages a, BufferUsages b)
    {
        return static_cast<BufferUsages>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline constexpr BufferUsages& operator |= (BufferUsages& a, BufferUsages b)
    {
        return a = a | b;
    }

    inline constexpr BufferUsages& operator &= (BufferUsages& a, BufferUsages b)
    {
        return a = a & b;
    }

    inline bool any(BufferUsages flags)
    {
        return static_cast<uint32_t>(flags) != 0;
    }

    /**
     * @brief Describes the properties and configuration of a buffer resource.
     */
    struct BufferDesc
    {
        uint64_t size = 0;
        const void* data = nullptr;
        BufferUsages usage = BufferUsages::None;
        MemoryUsage memory_usage = MemoryUsage::GpuOnly;
        int frames = 1; // frames_in_flight
        const char* name = "";
    };

    /**
     * @brief Calculates appropriate alignment for uniform buffers
     * @return Alignment requirement in bytes based on device limits
     */
    constexpr uint64_t getUniformBufferAlignment(BufferUsages usage)
    {
        return bool(usage & BufferUsages::UniformBuffer) ? 256 : 1;
    }

}


namespace kege{

    // Abstract Buffer class
    class Buffer : public kege::RefCounter {
    public:

        // Copy between buffers
        virtual void copyFromBuffer
        (
            uint64_t offset,
            uint64_t size,
            const ref::Buffer& src,
            uint64_t src_offset
        )= 0;

        virtual void copyToBuffer
        (
            uint64_t offset,
            uint64_t size,
            ref::Buffer& dst,
            uint64_t dst_offset
        )= 0;

        // Copy data to/from buffer
        virtual void copyFrom(const void* data, uint64_t size, uint64_t offset = 0) = 0;
        virtual void copyTo(void* data, uint64_t size, uint64_t offset = 0)const = 0;

        // Map buffer memory for CPU access (if cpuAccessible is true)
        virtual const void* map(uint64_t offset = 0, uint64_t size = 0)const = 0;
        virtual void* map(uint64_t offset = 0, uint64_t size = 0) = 0;
        virtual void unmap() = 0;

        virtual const void* data()const = 0;
        virtual void* data() = 0;

        virtual bool resize( uint64_t size ) = 0;
        virtual bool empty()const = 0;
        virtual void clear() = 0;

        virtual int frames()const = 0;
        // Get buffer properties

        BufferUsages getBufferUsageFlags() const { return _buffer_usages; }
        MemoryUsage getMemoryUsage() const { return _memory_usage; }
        uint64_t size() const { return _size; };

        virtual const vk::Buffer* vk() const { return nullptr; }
        virtual vk::Buffer* vk() { return nullptr; }

    protected:

//        Buffer( BufferUsages buffer_usages, MemoryUsage memory_usage, uint64_t size )
//        :   _buffer_usages( buffer_usages )
//        ,   _memory_usage( memory_usage )
//        ,   _size( size )
//        {}

        virtual ~Buffer() = default;

    protected:

        BufferUsages _buffer_usages;
        MemoryUsage _memory_usage;
        uint64_t _size;

        friend kege::Ref< kege::Buffer >;
    };

}

namespace std{

    template <> struct hash< kege::ref::Buffer >
    {
        std::size_t operator()( const kege::ref::Buffer& buffer ) const
        {
            return ( size_t )( buffer.ref() );
        }
    };
}
#endif /* kege_graphics_buffer_hpp */
