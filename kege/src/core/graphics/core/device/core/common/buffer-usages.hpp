//
//  buffer-usages.hpp
//  editor
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#ifndef buffer_usages_hpp
#define buffer_usages_hpp

#include <map>
#include <string>
#include "memory-usage.h"

namespace kege{

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
    
    BufferUsages stringToBufferUsage( const std::string& name );

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

#endif /* buffer_usages_hpp */
