//
//  buffer.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/14/25.
//

#ifndef kege_graphics_buffer_hpp
#define kege_graphics_buffer_hpp

#include "../common/graphics-common.h"

namespace kege
{
    class Buffer;
}

namespace kege::vk
{
    class Buffer;
}

namespace kege::ref
{
    typedef kege::Ref< kege::Buffer > Buffer;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::Buffer > Buffer;
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
            const kege::ref::Buffer& src,
            uint64_t src_offset
        )= 0;

        virtual void copyToBuffer
        (
            uint64_t offset,
            uint64_t size,
            kege::ref::Buffer& dst,
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

#endif /* kege_graphics_buffer_hpp */
