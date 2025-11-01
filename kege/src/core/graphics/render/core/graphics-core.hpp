//
//  graphics-enums-and-types.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/24/25.
//

/**
 * @file graphics_descriptors.h
 * @brief Core descriptors and structures for the graphics API
 *
 * This file contains comprehensive descriptors for configuring various
 * aspects of a modern graphics pipeline, including swapchains, device features,
 * and descriptor set updates.
 */

#ifndef graphics_enums_and_types_hpp
#define graphics_enums_and_types_hpp

#include <stdio.h>
#include <cstdint>
#include <vector>
#include <string>
#include <memory> // For potential smart pointers later, though using opaque handles initially
#include <variant> // Useful for certain descriptions
#include <functional>
#include <stdexcept>

#include "../../../memory/ref.hpp"
#include "../../../utils/log.hpp"
#include "../../../math/algebra/vectors.hpp"
#include "../../../math/algebra/matrices.hpp"
#include "../../../math/algebra/vmath.hpp"
#include "declarations.hpp"

namespace kege{

    enum{ MAX_FRAMES_IN_FLIGHT = 2 };

    enum struct ErrorType
    {
        NULL_OBJECT,
        INVALID_OPERATION,
        INVALID_ID,
        UNSUPPORTED_QUEUE_TYPE,
        INVALID_DEVICE_QUEUE_TYPE,
        OUT_OF_BOUND_INDEX,
        FAILED_TO_ACQUIRE_NEXT_IMAGE,
        FAILED_TO_INITALIZED,
        FAILED_TO_SHUTDOWN,
    };

    struct BufferViewHandle
    {
        inline operator bool()const{ return id >= 0; }
        int32_t id = -1;
    };

    struct ShaderHandle
    {
        inline operator bool()const{ return id >= 0; }
        int32_t id = -1;
    };

    struct PipelineLayoutHandle { int32_t id = -1; };

    struct PipelineHandle
    {
        inline operator bool()const{ return id >= 0; }
        int32_t id = -1;
    };

    struct UniformSetLayout
    {
        inline operator bool()const{ return id >= 0; }
        int32_t id = -1;
    };

    inline bool operator==(const kege::ShaderHandle& a, const kege::ShaderHandle& b){ return a.id == b.id; }
    inline bool operator!=(const kege::ShaderHandle& a, const kege::ShaderHandle& b){ return a.id != b.id; }
    inline bool operator <(const kege::ShaderHandle& a, const kege::ShaderHandle& b){ return a.id  < b.id; }

    inline bool operator==(const kege::PipelineLayoutHandle& a, const kege::PipelineLayoutHandle& b){ return a.id == b.id; }
    inline bool operator!=(const kege::PipelineLayoutHandle& a, const kege::PipelineLayoutHandle& b){ return a.id != b.id; }
    inline bool operator <(const kege::PipelineLayoutHandle& a, const kege::PipelineLayoutHandle& b){ return a.id  < b.id; }

    inline bool operator==(const kege::PipelineHandle& a, const kege::PipelineHandle& b){ return a.id == b.id; }
    inline bool operator!=(const kege::PipelineHandle& a, const kege::PipelineHandle& b){ return a.id != b.id; }
    inline bool operator <(const kege::PipelineHandle& a, const kege::PipelineHandle& b){ return a.id  < b.id; }

    inline bool operator==(const kege::UniformSetLayout& a, const kege::UniformSetLayout& b){ return a.id == b.id; }
    inline bool operator!=(const kege::UniformSetLayout& a, const kege::UniformSetLayout& b){ return a.id != b.id; }
    inline bool operator <(const kege::UniformSetLayout& a, const kege::UniformSetLayout& b){ return a.id  < b.id; }

    using GraphicsSurface = void*;

    // --- Core Enums ---

    enum class GraphicsAPI
    {
        Vulkan,
        D3D12,
        Metal,
    };

    enum struct PhysicalDeviceType
    {
        Other,
        IntegratedGpu,
        DiscreteGpu,
        VirtualGpu,
        DeviceCpu
    };

    /**
     * @brief Enumeration of queue types.
     *
     * Specifies the type of command queue that can execute commands.
     */
    enum class QueueType
    {
        Invalid,
        Graphics,  ///< Graphics queue capable of all operations
        Compute,   ///< Compute-only queue
        Transfer   ///< Transfer-only queue
    };

    enum struct CommandBufferUsage
    {
        OneTimeSubmit = 0x1,
        RenderPassContinue = 0x2,
        SimultaneousUse = 0x4
    };

    constexpr inline bool operator <<= ( const CommandBufferUsage& usage, const CommandBufferUsage& other )
    {
        return (int( usage ) & int( other )) == int( other );
    }

   enum //QueueType
   {
       QUEUE_TYPE_INVALID,
       QUEUE_TYPE_GRAPHICS,
       QUEUE_TYPE_COMPUTE,
       QUEUE_TYPE_TRANSFER,
   };

    enum struct FenceStatus
    {
        Success,
        NotReady,
        DeviceLost,
    };

    enum class CommandBufferLevel
    {
        Primary = 0,
        Secondary = 1,
    };

    struct CommandBufferAllocateInfo
    {
        QueueType          type;
        CommandBufferLevel level;
    };

    /**
     * @brief Namespace defining flags for render graph resource access.
     *
     * These flags represent the types of access that are performed on a
     * resource during a particular pipeline stage. They are crucial for
     * ensuring proper synchronization and data coherency between different
     * stages that might be accessing the same resource.
     */
    enum struct AccessFlags: uint32_t
    {
        None = 0,
        IndirectCommandRead = 1 << 0,
        IndexRead           = 1 << 1,
        VertexBufferRead    = 1 << 2,
        UniformRead         = 1 << 3,
        InputRead           = 1 << 4,
        ShaderRead          = 1 << 5,
        ShaderWrite         = 1 << 6,
        ColorRead           = 1 << 7,
        ColorWrite          = 1 << 8,
        DepthStencilRead    = 1 << 9,
        DepthStencilWrite   = 1 << 10,
        TransferRead        = 1 << 11,
        TransferWrite       = 1 << 12,
        HostRead            = 1 << 13,
        HostWrite           = 1 << 14,

        MemoryRead = HostRead | UniformRead | InputRead | ShaderRead |
                     ColorRead | DepthStencilRead | TransferRead |
                     IndirectCommandRead | IndexRead | VertexBufferRead,

        MemoryWrite = HostWrite | ShaderWrite | ColorWrite |
                      DepthStencilWrite | TransferWrite,

        All = 0xFFFFFFFF,
    };

    /**
     * @brief Enables bitwise OR operation for GraphAccessFlags.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise OR of the two flags.
     */
    inline AccessFlags operator|(AccessFlags a, AccessFlags b)
    {
        return static_cast< AccessFlags >( static_cast<uint32_t>(a) | static_cast<uint32_t>(b) );
    }

    /**
     * @brief Enables bitwise AND operation for GraphAccessFlags.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise AND of the two flags.
     */
    inline AccessFlags operator&(AccessFlags a, AccessFlags b)
    {
        return static_cast<AccessFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    /**
     * @brief Enables bitwise XOR operation for GraphAccessFlags.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise XOR of the two flags.
     */
    inline AccessFlags operator^(AccessFlags a, AccessFlags b)
    {
        return static_cast<AccessFlags>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
    }

    /**
     * @brief Enables bitwise NOT operation for GraphAccessFlags.
     * @param a The flag to negate.
     * @return The bitwise NOT of the flag.
     */
    inline AccessFlags operator~(AccessFlags a)
    {
        return static_cast<AccessFlags>(~static_cast<uint32_t>(a));
    }

    /**
     * @brief Enables the |= assignment operator for AccessFlags.
     * @param a The flag to modify.
     * @param b The flag to OR with.
     * @return A reference to the modified flag.
     */
    inline AccessFlags& operator|=(AccessFlags& a, AccessFlags b)
    {
        return a = a | b;
    }

    /**
     * @brief Enables the &= assignment operator for AccessFlags.
     * @param a The flag to modify.
     * @param b The flag to AND with.
     * @return A reference to the modified flag.
     */
    inline AccessFlags& operator&=(AccessFlags& a, AccessFlags b)
    {
        return a = a & b;
    }

    /**
     * @brief Enables the ^= assignment operator for AccessFlags.
     * @param a The flag to modify.
     * @param b The flag to XOR with.
     * @return A reference to the modified flag.
     */
    inline AccessFlags& operator^=(AccessFlags& a, AccessFlags b)
    {
        return a = a ^ b;
    }

    /**
     * @brief Enables the != assignment operator for AccessFlags.
     * @param a The flag to modify.
     * @param i The flag to != with.
     * @return True if not equal, false otherwise.
     */
    inline bool operator|=(const AccessFlags& a, uint32_t i)
    {
        return static_cast<uint32_t>(a) != i;
    }

    /**
     * @brief Enables the != assignment operator for AccessFlags.
     * @param i The flag to != with.
     * @param a The flag to modify.
     * @return True if not equal, false otherwise.
     */
    inline bool operator|=(uint32_t i, const AccessFlags& a)
    {
        return static_cast<uint32_t>(a) != i;
    }

    /**
     * @brief Enables the == assignment operator for AccessFlags.
     * @param a The flag to modify.
     * @param i The flag to != with.
     * @return True if equal, false otherwise.
     */
    inline bool operator==(const AccessFlags& a, uint32_t i)
    {
        return static_cast<uint32_t>(a) == i;
    }

    /**
     * @brief Enables the != assignment operator for AccessFlags.
     * @param i The flag to != with.
     * @param a The flag to modify.
     * @return True if equal, false otherwise.
     */
    inline bool operator==(uint32_t i, const AccessFlags& a)
    {
        return static_cast<uint32_t>(a) == i;
    }

    inline bool hasFlag( AccessFlags flags, AccessFlags mask )
    {
        return static_cast< uint32_t >( flags & mask ) != 0;
    }

    inline bool isWriteAccess(AccessFlags access)
    {
        return (access & AccessFlags::MemoryWrite) != AccessFlags::None;
    }

    inline bool isReadAccess(AccessFlags access)
    {
        return (access & AccessFlags::MemoryRead ) != AccessFlags::None;
    }


    /**
     * @brief Memory usage patterns for buffer and texture allocations.
     *
     * Defines how memory will be accessed by CPU and GPU, which affects:
     * - Performance characteristics
     * - Memory placement (device-local vs host-visible)
     * - Mapping requirements
     * - Coherency behavior
     */
    enum class MemoryUsage
    {
        /**
         * @brief Device-local memory, optimal for GPU-only resources.
         *
         * Characteristics:
         * - Fastest GPU access (discrete cards: VRAM)
         * - CPU cannot map or access directly
         * - Requires staging buffers for CPU updates
         *
         * Typical uses:
         * - Static vertex/index buffers
         * - Frequently sampled textures
         * - Render targets
         */
        GpuOnly,

        /**
         * @brief Host-visible memory for frequent CPU-to-GPU transfers.
         *
         * Characteristics:
         * - CPU-writable (may be coherent or require explicit flushes)
         * - Slower GPU access than GpuOnly
         * - Often unified memory on integrated GPUs
         *
         * Typical uses:
         * - Dynamic uniform buffers
         * - Frequently updated vertex buffers
         * - Staging buffers for GpuOnly resources
         */
        CpuToGpu,

        /**
         * @brief Host-visible memory for GPU-to-CPU readback.
         *
         * Characteristics:
         * - GPU-writable, CPU-readable
         * - Requires explicit synchronization (map/unmap)
         * - May need cache invalidation on CPU side
         *
         * Typical uses:
         * - Frame capture buffers
         * - Compute shader output
         * - Occlusion query results
         */
        GpuToCpu,

        /**
         * @brief CPU-only allocations (rarely needed for GPU resources).
         *
         * Characteristics:
         * - Not accessible by GPU
         * - May be pageable system memory
         * - Exists mainly for utility allocations
         *
         * Typical uses:
         * - Intermediate processing buffers
         * - Asset loading temporary storage
         * - Debugging helpers
         */
        CpuOnly,

        /**
         * @brief Transient GPU memory for short-lived attachments.
         *
         * Characteristics:
         * - May use lazy allocation
         * - Optimized for renderpass attachments
         * - Implementation may use memory pooling
         *
         * Typical uses:
         * - MSAA color/depth targets
         * - Intermediate render passes
         * - Tile-based rendering buffers
         */
        // GpuLazy,  // Reserved for future expansion
    };

    /**
     * @brief Check if memory usage requires host-visible allocation
     */
    inline constexpr bool isHostVisible(MemoryUsage usage) {
        return usage == MemoryUsage::CpuToGpu ||
               usage == MemoryUsage::GpuToCpu ||
               usage == MemoryUsage::CpuOnly;
    }

    /**
     * @brief Check if memory usage implies frequent CPU writes
     */
    inline constexpr bool isCpuWriteCombined(MemoryUsage usage) {
        return usage == MemoryUsage::CpuToGpu;
    }

    /**
     * @brief Check if memory usage implies CPU readback capability
     */
    inline constexpr bool supportsCpuReadback(MemoryUsage usage) {
        return usage == MemoryUsage::GpuToCpu ||
               usage == MemoryUsage::CpuOnly;
    }

    /**
     * @brief Bitmask flags representing shader stages in the rendering pipeline.
     *
     * Used to specify which stages a shader module targets or which pipeline stages
     * should be active. Values can be combined using bitwise OR.
     */
    enum class ShaderStageFlag : uint32_t
    {
        Invalid = 0, 
        Vertex = 1 << 0,                  ///< Vertex shader stage
        Fragment = 1 << 1,                ///< Fragment (Pixel) shader stage
        Compute = 1 << 2,                 ///< Compute shader stage
        Geometry = 1 << 3,                ///< Geometry shader stage
        TessellationControl = 1 << 4,     ///< Tessellation Control (Hull) shader stage
        TessellationEvaluation = 1 << 5,  ///< Tessellation Evaluation (Domain) shader stage

        // Predefined combinations
        AllGraphics = Vertex | Fragment | Geometry | TessellationControl | TessellationEvaluation,
        //AllGraphics = (1 << 6) - 1,       ///< All graphics pipeline stages (excludes Compute)
        All = AllGraphics | Compute       ///< All possible shader stages
    };

    inline constexpr ShaderStageFlag operator|(ShaderStageFlag a, ShaderStageFlag b)
    {
        return static_cast<ShaderStageFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline constexpr ShaderStageFlag operator&(ShaderStageFlag a, ShaderStageFlag b)
    {
        return static_cast<ShaderStageFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline constexpr ShaderStageFlag& operator|=(ShaderStageFlag& a, ShaderStageFlag b)
    {
        a = a | b;
        return a;
    }

    inline constexpr ShaderStageFlag& operator&=(ShaderStageFlag& a, ShaderStageFlag b)
    {
        a = a & b;
        return a;
    }

    inline constexpr bool operator==(ShaderStageFlag& a, ShaderStageFlag b)
    {
        return (static_cast<uint32_t>(a) == static_cast<uint32_t>(b));
    }

    inline constexpr bool operator!=(ShaderStageFlag& a, ShaderStageFlag b)
    {
        return (static_cast<uint32_t>(a) != static_cast<uint32_t>(b));
    }


    struct Offset2D
    {
        int32_t    x;
        int32_t    y;
    };

    struct Extent2D
    {
        uint32_t width;
        uint32_t height;
    };

    /**
     * @brief Defines a 3D offset within a texture.
     */
    struct Offset3D
    {
        int32_t    x;
        int32_t    y;
        int32_t    z;
    };

    /**
     * @brief Defines the 3D extent (size) of a region.
     */
    struct Extent3D
    {
        uint32_t    width;
        uint32_t    height;
        uint32_t    depth;
    };


    /**
     * @brief Basic structure defining a rectangle.
     *
     * Used for specifying scissor rectangles and rendering areas. Coordinates
     * are typically in pixels.
     */
    struct Rect2D
    {
        Offset2D    offset;
        Extent2D    extent;
    };

    /**
     * @brief Union-like structure to hold the clear value for color or depth/stencil attachments.
     *
     * Using a struct for simplicity, a proper union or std::variant might be
     * more type-safe in a production environment.
     */
    struct ClearValue
    {
        float color[4] = {0.0f, 0.0f, 0.0f, 1.0f}; ///< Clear color as RGBA.
        /**
         * @brief Structure to hold the clear values for depth and stencil attachments.
         */
        struct DepthStencil
        {
            float depth = 1.0f;   ///< Clear depth value.
            uint32_t stencil = 0; ///< Clear stencil value.
        } depth_stencil; ///< Depth and stencil clear values.
    };

    /**
     * @brief Basic structure defining a viewport.
     *
     * A viewport defines the mapping from normalized device coordinates to
     * window coordinates. It specifies the rectangular region of the framebuffer
     * that will be rendered to.
     */
    struct Viewport
    {
        float x = 0.0f;        ///< Left coordinate of the viewport rectangle.
        float y = 0.0f;        ///< Top coordinate of the viewport rectangle.
        float width = 0.0f;    ///< Width of the viewport rectangle.
        float height = 0.0f;   ///< Height of the viewport rectangle.
        float min_depth = 0.0f; ///< Minimum depth value.
        float max_depth = 1.0f; ///< Maximum depth value.
    };

    /**
     * @brief Basic structure defining a rectangle.
     *
     * Used for specifying scissor rectangles and rendering areas. Coordinates
     * are typically in pixels.
     */
    struct Scissor
    {
        int32_t x = 0;        ///< Left coordinate of the rectangle.
        int32_t y = 0;        ///< Top coordinate of the rectangle.
        uint32_t width = 0;   ///< Width of the rectangle.
        uint32_t height = 0;  ///< Height of the rectangle.
    };


    enum class Format
    {
        undefined,

        r_u8, rg_u8, rgb_u8, rgba_u8,

        r_s8, rg_s8, rgb_s8, rgba_s8,

        r_u8_norm, rg_u8_norm, rgb_u8_norm, rgba_u8_norm,
        r_s8_norm, rg_s8_norm, rgb_s8_norm, rgba_s8_norm,

        r_8_srgb, rg_8_srgb, rgb_8_srgb, rgba_8_srgb,

        bgr_8_srbg, bgra_8_srbg,

        bgr_u8, bgr_s8, bgr_s8_norm, bgr_u8_norm,

        bgra_u8, bgra_s8, bgra_s8_norm, bgra_u8_norm, bgra_u8_norm_srbg,

        r_u16, rg_u16, rgb_u16, rgba_u16,
        r_s16, rg_s16, rgb_s16, rgba_s16,
        r_f16, rg_f16, rgb_f16, rgba_f16,

        r_u16_norm, rg_u16_norm, rgb_u16_norm, rgba_u16_norm,
        r_s16_norm, rg_s16_norm, rgb_s16_norm, rgba_s16_norm,

        r_u32, rg_u32, rgb_u32, rgba_u32,
        r_s32, rg_s32, rgb_s32, rgba_s32,
        r_f32, rg_f32, rgb_f32, rgba_f32,

        r_u64, rg_u64, rgb_u64, rgba_u64,
        r_s64, rg_s64, rgb_s64, rgba_s64,
        r_f64, rg_f64, rgb_f64, rgba_f64,

        depth_16,
        depth_32,
        depth_16_stencil_8,
        depth_24_stencil_8,
        depth_32_stencil_8,

        stencil_u8
    };

    constexpr uint32_t sizeOfFormat(Format format)
    {
        switch (format)
        {
            // 8-bit formats
            case Format::r_u8:
            case Format::r_s8:
            case Format::r_u8_norm:
            case Format::r_s8_norm:
            case Format::r_8_srgb:
            case Format::stencil_u8:
                return 1;

            case Format::rg_u8:
            case Format::rg_s8:
            case Format::rg_u8_norm:
            case Format::rg_s8_norm:
            case Format::rg_8_srgb:
                return 2;

            case Format::rgb_u8:
            case Format::rgb_s8:
            case Format::rgb_u8_norm:
            case Format::rgb_s8_norm:
            case Format::rgb_8_srgb:
            case Format::bgr_u8:
            case Format::bgr_s8:
            case Format::bgr_s8_norm:
            case Format::bgr_u8_norm:
            case Format::bgr_8_srbg:
                return 3;

            case Format::rgba_u8:
            case Format::rgba_s8:
            case Format::rgba_u8_norm:
            case Format::rgba_s8_norm:
            case Format::rgba_8_srgb:
            case Format::bgra_u8:
            case Format::bgra_s8:
            case Format::bgra_s8_norm:
            case Format::bgra_u8_norm:
            case Format::bgra_u8_norm_srbg:
            case Format::bgra_8_srbg:
                return 4;

            // 16-bit formats (2 bytes per channel)
            case Format::r_u16:
            case Format::r_s16:
            case Format::r_f16:
            case Format::r_u16_norm:
            case Format::r_s16_norm:
            case Format::depth_16:
                return 2;

            case Format::rg_u16:
            case Format::rg_s16:
            case Format::rg_f16:
            case Format::rg_u16_norm:
            case Format::rg_s16_norm:
                return 4;

            case Format::rgb_u16:
            case Format::rgb_s16:
            case Format::rgb_f16:
            case Format::rgb_u16_norm:
            case Format::rgb_s16_norm:
                return 6;

            case Format::rgba_u16:
            case Format::rgba_s16:
            case Format::rgba_f16:
            case Format::rgba_u16_norm:
            case Format::rgba_s16_norm:
                return 8;

            // 32-bit formats (4 bytes per channel)
            case Format::r_u32:
            case Format::r_s32:
            case Format::r_f32:
            case Format::depth_32:
                return 4;

            case Format::rg_u32:
            case Format::rg_s32:
            case Format::rg_f32:
                return 8;

            case Format::rgb_u32:
            case Format::rgb_s32:
            case Format::rgb_f32:
                return 12;

            case Format::rgba_u32:
            case Format::rgba_s32:
            case Format::rgba_f32:
                return 16;

            // 64-bit formats (8 bytes per channel)
            case Format::r_u64:
            case Format::r_s64:
            case Format::r_f64:
                return 8;

            case Format::rg_u64:
            case Format::rg_s64:
            case Format::rg_f64:
                return 16;

            case Format::rgb_u64:
            case Format::rgb_s64:
            case Format::rgb_f64:
                return 24;

            case Format::rgba_u64:
            case Format::rgba_s64:
            case Format::rgba_f64:
                return 32;

            // Depth + stencil combined formats
            case Format::depth_16_stencil_8:
                return 3; // 2 bytes depth + 1 byte stencil
            case Format::depth_24_stencil_8:
                return 4; // 3 bytes depth + 1 byte stencil
            case Format::depth_32_stencil_8:
                return 5; // 4 bytes depth + 1 byte stencil

            default:
                return 0; // undefined or unknown
        }
    }

    /**
     * @brief Enumeration of supported multisample anti-aliasing (MSAA) sample counts.
     *
     * Used for render targets and textures that support multisampling.
     */
    enum class SampleCount
    {
        Count1 = 1,    ///< No multisampling (1 sample per pixel)
        Count2 = 2,    ///< 2x MSAA
        Count4 = 4,    ///< 4x MSAA (common default)
        Count8 = 8,    ///< 8x MSAA
        Count16 = 16,  ///< 16x MSAA (high quality)
        Count32 = 32,  ///< 32x MSAA (very high quality)
        Count64 = 64   ///< 64x MSAA (extremely high quality)
    };

    /**
     * ---- Image and Sampler Operation / Parameters
     */

    /**
     * @brief Texture filtering modes for minification and magnification.
     *
     * Determines how texels are sampled when the texture is stretched or shrunk.
     */
    enum class Filter
    {
        Nearest,  ///< Nearest-neighbor filtering (blocky/pixelated)
        Linear    ///< Bilinear interpolation (smooth blending)
    };

    /**
     * @brief Mipmap filtering modes.
     *
     * Controls how mipmap levels are selected and blended during sampling.
     */
    enum class MipmapMode
    {
        Nearest,  ///< Use nearest mipmap level
        Linear    ///< Blend between two nearest mipmap levels (trilinear filtering)
    };

    /**
     * @brief Texture addressing modes.
     *
     * Defines how texture coordinates outside [0,1] range are handled.
     */
    enum class AddressMode
    {
        Repeat,              ///< Tile the texture (wrap around)
        MirroredRepeat,      ///< Tile with mirroring at integer boundaries
        ClampToEdge,         ///< Clamp to edge texels
        ClampToBorder,       ///< Use specified border color
        MirrorClampToEdge    ///< Mirror once then clamp to edge (Vulkan 1.2+)
    };

    /**
     * @brief Border colors for clamped textures.
     *
     * Predefined colors used when AddressMode is ClampToBorder.
     */
    enum class BorderColor
    {
        FloatTransparentBlack,  ///< (0.0f, 0.0f, 0.0f, 0.0f)
        IntTransparentBlack,    ///< (0, 0, 0, 0)
        FloatOpaqueBlack,       ///< (0.0f, 0.0f, 0.0f, 1.0f)
        IntOpaqueBlack,         ///< (0, 0, 0, 1)
        FloatOpaqueWhite,       ///< (1.0f, 1.0f, 1.0f, 1.0f)
        IntOpaqueWhite          ///< (1, 1, 1, 1)
    };

    /**
     * @brief Depth comparison operations.
     *
     * Used for shadow map sampling and depth tests.
     */
    enum class CompareOp
    {
        Never,        ///< Never pass the comparison
        Less,         ///< Pass if incoming < stored
        Equal,        ///< Pass if incoming == stored
        LessEqual,    ///< Pass if incoming <= stored
        Greater,      ///< Pass if incoming > stored
        NotEqual,     ///< Pass if incoming != stored
        GreaterEqual, ///< Pass if incoming >= stored
        Always        ///< Always pass the comparison
    };

    /**
     * @brief Stencil operations.
     */
    enum class StencilOp
    {
        Keep,               ///< Preserve existing stencil value
        Zero,               ///< Set stencil value to 0
        Replace,            ///< Set stencil value to reference
        IncrementAndClamp,  ///< Increment and clamp to max value
        DecrementAndClamp,  ///< Decrement and clamp to 0
        Invert,             ///< Bitwise invert stencil value
        IncrementAndWrap,   ///< Increment with wrap-around
        DecrementAndWrap    ///< Decrement with wrap-around
    };

    /**
     * @brief Enumeration of image layouts.
     *
     * Describes the possible layouts an image can be in, which affects how
     * the image data is organized in memory and how it can be accessed.
     */
    enum class ImageLayout
    {
        Undefined,          ///< Initial undefined layout
        General,            ///< General-purpose layout
        Color,              ///< Optimal for color attachment access
        Depth,              ///< Optimal for depth attachment access
        DepthRead,
        Stencil,
        StencilRead,
        DepthStencil,       ///< Optimal for depth/stencil attachment access
        DepthStencilRead,   ///< Optimal for depth/stencil attachment access
        DepthRead_Stencil,
        Depth_StencilRead,
        ShaderRead,         ///< Optimal for shader read-only access
        TransferSrc,        ///< Optimal for transfer source operations
        TransferDst,        ///< Optimal for transfer destination operations
        PreInitialized,
        Present,            ///< Optimal for presentation engine access
        HostRead,           ///< Layout suitable for CPU reading
        HostWrite,          ///< Layout suitable for CPU writing
        // Add more as needed...
    };

//    enum class ImageUsageFlags
//    {
//        = 1 << ImageLayout::General,
//        = 1 << ImageLayout::Color,
//        = 1 << ImageLayout::Depth,
//        = 1 << ImageLayout::DepthRead,
//        = 1 << ImageLayout::Stencil,
//        = 1 << ImageLayout::StencilRead,
//        = 1 << ImageLayout::DepthStencil,
//        = 1 << ImageLayout::DepthStencilRead,
//        = 1 << ImageLayout::DepthRead_Stencil,
//        = 1 << ImageLayout::Depth_StencilRead,
//        = 1 << ImageLayout::ShaderRead,
//        = 1 << ImageLayout::TransferSrc,
//        = 1 << ImageLayout::TransferDst,
//        = 1 << ImageLayout::PreInitialized,
//        = 1 << ImageLayout::Present,
//        = 1 << ImageLayout::HostRead,
//        = 1 << ImageLayout::HostWrite,
//    };
//
//    inline constexpr bool checkFlag( ImageUsageFlags a, const int b)
//    {
//        return (static_cast< uint32_t >( a ) & static_cast< uint32_t >( b )) != 0;
//    }

    /**
     * @brief Sampler description structure.
     *
     * Combines all filtering and addressing parameters into a complete
     * sampler configuration.
     */
    struct SamplerDesc
    {
        Filter mag_filter = Filter::Linear;       ///< Magnification filter
        Filter min_filter = Filter::Linear;       ///< Minification filter
        MipmapMode mipmap_mode = MipmapMode::Linear; ///< Mipmap filtering
        AddressMode address_mode_u = AddressMode::Repeat; ///< U-coordinate addressing
        AddressMode address_mode_v = AddressMode::Repeat; ///< V-coordinate addressing
        AddressMode address_mode_w = AddressMode::Repeat; ///< W-coordinate addressing
        CompareOp compare_op = CompareOp::Always;  ///< Depth comparison operator
        bool compare_enable = false;
        float mip_lod_bias = 0.0f;                ///< Mipmap LOD bias
        float min_lod = 0.0f;                    ///< Minimum mipmap level
        float max_lod = 0.0f;                 ///< Maximum mipmap level
        BorderColor border_color = BorderColor::FloatTransparentBlack; ///< Border color
        bool anisotropy_enable = false;           ///< Enable anisotropic filtering
        float max_anisotropy = 1.0f;             ///< Maximum anisotropy level (1-16)
        const char* name;
    };


    /**
     * @brief Creates a default sampler description with common settings.
     */
    inline SamplerDesc createDefaultSamplerDesc()
    {
        return
        {
            .mag_filter = Filter::Linear,
            .min_filter = Filter::Linear,
            .mipmap_mode = MipmapMode::Linear,
            .address_mode_u = AddressMode::Repeat,
            .address_mode_v = AddressMode::Repeat,
            .address_mode_w = AddressMode::Repeat
        };
    }

    /**
     * @brief Creates a point-sampled (non-filtered) sampler description.
     */
    inline SamplerDesc createPointClampedSamplerDesc()
    {
        SamplerDesc desc;
        desc.mag_filter = Filter::Nearest;
        desc.min_filter = Filter::Nearest;
        desc.mipmap_mode = MipmapMode::Nearest;
        desc.address_mode_u = AddressMode::ClampToEdge;
        desc.address_mode_v = AddressMode::ClampToEdge;
        desc.address_mode_w = AddressMode::ClampToEdge;
        return desc;
    }

    /**
     * ---- Pipeline State Description Structs ----
     */

    /**
     * @brief Describes a shader module for pipeline creation.
     *
     * Contains the shader code and metadata needed to create a shader module.
     * Supports both precompiled bytecode and potentially runtime compilation.
     */
    struct ShaderDesc
    {
        /**
         * @brief Shader bytecode in the target API's format.
         *
         * Supported formats:
         * - Vulkan: SPIR-V bytecode
         * - Direct3D 12: DXIL/DXBC
         * - Metal: AIR (Apple Intermediate Representation) or source
         *
         * @note For better cross-API compatibility, prefer SPIR-V where possible.
         */
        std::vector<uint32_t> byte_code;

        /**
         * @brief Entry point function name in the shader.
         *
         * Defaults to "main" which is conventional for most shading languages.
         * For HLSL, this would match the function name marked as [shader("type")].
         */
        std::string entry_point = "main";

        /**
         * @brief Which pipeline stage this shader targets.
         *
         * Must match the actual shader type (e.g., a fragment shader can't be
         * specified as ShaderStageFlag::Vertex).
         */
        ShaderStageFlag stage = ShaderStageFlag::Vertex;

        /**
         * @brief Debug label for the shader module.
         *
         * Visible in graphics debuggers like RenderDoc or NVIDIA Nsight.
         * Highly recommended for better debugging experience.
         */
        std::string name = "";
    };


    /**
     * @brief Validates that the shader description is complete.
     * @return true if valid, false if missing critical data
     */
    inline bool isValid( const ShaderDesc& desc )
    {
        return !desc.byte_code.empty() && !desc.entry_point.empty() && desc.stage != ShaderStageFlag(0);
    }

    /**
     * @brief Creates a compute shader description from SPIR-V bytecode.
     * @param code SPIR-V bytecode vector
     * @param entry Entry point name (defaults to "main")
     * @return Configured ShaderDesc
     */
    inline ShaderDesc createCompute(const std::vector<uint32_t>& code, std::string name = "ComputeShader", const std::string& entry = "main")
    {
        return ShaderDesc
        {
            .byte_code = code,
            .entry_point = entry,
            .stage = ShaderStageFlag::Compute,
            .name = name
        };
    }

    /**
     * @brief Creates a vertex+fragment shader pair description.
     * @param vs_code Vertex shader bytecode
     * @param fs_code Fragment shader bytecode
     * @return Pair of ShaderDesc objects
     */
    inline std::pair<ShaderDesc, ShaderDesc> createStandardPipeline
    (
        const std::vector<uint32_t>& vs_code,
        const std::vector<uint32_t>& fs_code,
        const std::string& vertex_entry = "main",
        const std::string& fragment_entry = "main"
    )
    {
        return std::pair<ShaderDesc, ShaderDesc>
        {
            {vs_code, vertex_entry, ShaderStageFlag::Vertex, "VertexShader"},
            {fs_code, fragment_entry, ShaderStageFlag::Fragment, "FragmentShader"}
        };
    }

    /**
     * @brief Input data rate classification.
     */
    enum struct VertexInputRate
    {
        Vertex,   ///< Data advances per vertex (standard vertex buffers)
        Instance  ///< Data advances per instance (instanced rendering)
    };

    /**
     * @brief Describes the usage of a member within a push constant block.
     */
    enum struct ShaderVarType : char
    {
        Undefined, Bool, Sint, Uint, Float, Double,
        Vec2,  Vec3,  Vec4,
        Vec2I, Vec3I, Vec4I,
        Vec2U, Vec3U, Vec4U,
        Vec2D, Vec3D, Vec4D,
        Mat2,  Mat3,  Mat4
    };

    int sizeOf( ShaderVarType t );
    std::string toString( ShaderVarType t );

    /**
     * @brief Describes how vertex data is organized in memory for a single binding.
     */
    struct VertexInputBindingDesc // TODO: deprecated
    {
        /**
         * @brief Binding number that matches the binding in shader.
         *
         * Must be less than the device's maxVertexInputBindings limit.
         */
        uint32_t binding = 0;

        /**
         * @brief Byte stride between consecutive elements.
         *
         * For vertex rate: distance between vertices
         * For instance rate: distance between instances
         */
        uint32_t stride = 0;

        /**
         * @brief Specifies whether data is per-vertex or per-instance.
         *
         * @note For instanced rendering, typically:
         * - Vertex-rate data contains mesh geometry
         * - Instance-rate data contains transformation matrices
         */
        VertexInputRate input_rate = VertexInputRate::Vertex;
    };

    /**
     * @brief Describes how vertex attributes are fetched from a binding.
     */
    struct VertexInputAttributeDesc {
        /**
         * @brief Shader input location (layout(location=X) in GLSL).
         */
        uint32_t location = 0;

        /**
         * @brief Binding number this attribute pulls from.
         *
         * Must match a binding description's binding number.
         */
        uint32_t binding = 0;

        /**
         * @brief Format/type of the attribute data.
         *
         * Common formats:
         * - Format::RGB32_SFLOAT for vec3
         * - Format::RGBA8_UNORM for normalized uchar4
         * - Format::RG32_UINT for uvec2
         */
        ShaderVarType type = ShaderVarType::Undefined;

        /**
         * @brief Byte offset from start of vertex data.
         *
         * For interleaved attributes, this is the offset within the vertex structure.
         */
        uint32_t offset = 0;

        /**
         * @brief Specifies whether data is per-vertex or per-instance.
         *
         * @note For instanced rendering, typically:
         * - Vertex-rate data contains mesh geometry
         * - Instance-rate data contains transformation matrices
         */
        VertexInputRate input_rate = VertexInputRate::Vertex;

        std::string name;
    };

    /**
     * @brief Complete vertex input state description.
     */
    struct VertexBufferLayout
    {
        /**
         * @brief List of vertex attributes.
         *
         * Defines how to interpret data from the bound buffers.
         */
        std::vector< VertexInputAttributeDesc > attributes;

        /**
         * @brief Byte stride between consecutive elements.
         *
         * For vertex rate: distance between vertices
         * For instance rate: distance between instances
         */
        std::vector< uint32_t > strides;
    };

    /**
     * @brief Primitive topology types.
     */
    enum class PrimitiveTopology
    {
        Invalid,
        PointList,        ///< List of individual points
        LineList,         ///< List of separate line segments (2 verts per line)
        LineStrip,        ///< Connected line segments (1 vert per new line)
        TriangleList,     ///< List of separate triangles (3 verts per tri)
        TriangleStrip,    ///< Connected triangles (1 vert per new tri)
        // Advanced topologies:
        LineListAdjacency,         ///< For geometry shader line processing
        LineStripAdjacency,        ///< For geometry shader line processing
        TriangleListAdjacency,     ///< For geometry shader triangle processing
        TriangleStripAdjacency,    ///< For geometry shader triangle processing
        PatchList                 ///< For tessellation (control points)
    };

    /**
     * @brief Describes how vertices are assembled into primitives.
     */
    struct InputAssemblyStateDesc
    {
        /**
         * @brief How to interpret vertex data.
         *
         * Common choices:
         * - TriangleList for standard rendering
         * - TriangleStrip for efficient terrain rendering
         * - PatchList for tessellation
         */
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;

        /**
         * @brief Enables primitive restart functionality.
         *
         * When true:
         * - For list topologies: special index value (0xFFFF/0xFFFFFFFF) starts new primitive
         * - For strip topologies: special index value restarts the strip
         */
        bool primitive_restart_enable = false;

        /**
         * @brief Gets the number of vertices per primitive.
         *
         * @return Vertices per primitive (e.g., 3 for triangles) or 0 for variable
         */
        constexpr uint32_t verticesPerPrimitive() const
        {
            switch (topology) {
                case PrimitiveTopology::PointList: return 1;
                case PrimitiveTopology::LineList:
                case PrimitiveTopology::LineStrip: return 2;
                case PrimitiveTopology::TriangleList:
                case PrimitiveTopology::TriangleStrip: return 3;
                case PrimitiveTopology::PatchList: return 0; // Depends on patch size
                default: return 0;
            }
        }
    };

    /**
     * @brief Polygon rendering mode.
     */
    enum class PolygonMode
    {
        Invalid,
        Fill,  ///< Solid polygons (default)
        Line,  ///< Wireframe rendering
        Point  ///< Vertex points only
    };

    /**
     * @brief Face culling mode.
     */
    enum class CullMode
    {
        None,         ///< No culling
        Front,        ///< Cull front-facing polygons
        Back,         ///< Cull back-facing polygons (default)
        FrontAndBack  ///< Cull all polygons (uncommon)
    };

    /**
     * @brief Winding order for front face determination.
     */
    enum class FrontFace
    {
        CounterClockwise,  ///< CCW winding is front-facing (OpenGL standard)
        Clockwise          ///< CW winding is front-facing (Direct3D standard)
    };

    /**
     * @brief Rasterization state configuration.
     *
     * Controls how primitives are converted to fragments during rasterization.
     */
    struct RasterizationStateDesc
    {
        /**
         * @brief Enable depth clamping (disables clipping against near/far planes).
         *
         * Requires GPU feature `depthClamp`. Useful for shadow maps or
         * when needing to render geometry beyond standard depth bounds.
         */
        bool depth_clamp_enable = false;

        /**
         * @brief Completely disables rasterization output.
         *
         * When true:
         * - Vertex processing still occurs
         * - No fragments are generated
         * - Useful for transform feedback or vertex shader-only pipelines
         */
        bool rasterizer_disable = false;

        /**
         * @brief How to render polygons.
         *
         * @note `Line` and `Point` modes may require GPU features.
         */
        PolygonMode polygon_mode = PolygonMode::Fill;

        /**
         * @brief Which polygon faces to cull.
         *
         * Back face culling is standard for opaque geometry.
         */
        CullMode cull_mode = CullMode::None;

        /**
         * @brief Which vertex winding order indicates front-facing.
         *
         * Should match your mesh data and coordinate system.
         */
        FrontFace front_face = FrontFace::CounterClockwise;

        /**
         * @brief Enable depth bias for shadow mapping.
         *
         * Helps prevent shadow acne by adding small depth offset.
         */
        bool depth_bias_enable = false;

        /**
         * @brief Constant depth bias factor.
         *
         * Applied as: depthBias = constantFactor + slopeFactor * maxSlope
         */
        float depth_bias_constant_factor = 0.0f;

        /**
         * @brief Maximum depth bias value.
         *
         * Clamps the total depth bias to this value.
         */
        float depth_bias_clamp = 0.0f;

        /**
         * @brief Slope-dependent depth bias factor.
         *
         * Scales with polygon slope to handle angled surfaces.
         */
        float depth_bias_slope_factor = 0.0f;

        /**
         * @brief Line width for wireframe rendering.
         *
         * @note Values >1.0 may require GPU feature `wideLines`.
         */
        float line_width = 1.0f;
    };


    /**
     * @brief Multisample anti-aliasing (MSAA) configuration.
     */
    struct MultisampleStateDesc {
        /**
         * @brief Number of samples per pixel.
         *
         * Must match render target sample count.
         */
        SampleCount rasterization_samples = SampleCount::Count1;

        /**
         * @brief Enable per-sample shading.
         *
         * When true, invokes fragment shader per sample rather than per pixel.
         * Requires GPU feature `sampleRateShading`.
         */
        bool sample_shading_enable = false;

        /**
         * @brief Minimum fraction of samples to shade.
         *
         * Range [0,1] where:
         * - 0.0: Implementation-defined minimum
         * - 1.0: Full per-sample shading
         */
        float min_sample_shading = 1.0f;

        /**
         * @brief Enable alpha-to-coverage.
         *
         * Converts alpha values to coverage mask for MSAA.
         * Useful for foliage and other alpha-tested geometry.
         */
        bool alpha_to_coverage_enable = false;

        /**
         * @brief Enable alpha-to-one.
         *
         * Forces fragment alpha to 1.0 after testing.
         */
        bool alpha_to_one_enable = false;
    };

    /**
     * @brief Stencil operation description.
     */
    struct StencilOpState
    {
        /**
         * @brief Operation when stencil test fails.
         */
        StencilOp fail_op = StencilOp::Keep;

        /**
         * @brief Operation when stencil test passes but depth fails.
         */
        StencilOp depth_fail_op = StencilOp::Replace;

        /**
         * @brief Operation when both stencil and depth tests pass.
         */
        StencilOp pass_op = StencilOp::Keep;

        /**
         * @brief Comparison operator for stencil test.
         */
        CompareOp compare_op = CompareOp::Equal;

        /**
         * @brief Bitmask for stencil comparison.
         */
        uint32_t compare_mask = 0xFFFFFFFF;

        /**
         * @brief Bitmask controlling which bits are written.
         */
        uint32_t write_mask = 0xFFFFFFFF;

        /**
         * @brief Reference value for stencil comparison.
         */
        uint32_t reference = 1;
    };

    /**
     * @brief Depth and stencil test configuration.
     */
    struct DepthStencilStateDesc
    {
        /**
         * @brief Enable depth testing.
         */
        bool depth_test_enable = true;

        /**
         * @brief Enable writes to depth buffer.
         */
        bool depth_write_enable = true;

        /**
         * @brief Depth comparison operator.
         *
         * Standard depth test uses CompareOp::Less for realistic occlusion.
         */
        CompareOp depth_compare_op = CompareOp::Less;

        /**
         * @brief Enable stencil testing.
         */
        bool stencil_test_enable = false;

        /**
         * @brief Stencil operations for front-facing polygons.
         */
        StencilOpState front_op;

        /**
         * @brief Stencil operations for back-facing polygons.
         */
        StencilOpState back_op;
    };

    /**
     * @brief Blending factors.
     */
    enum class BlendFactor
    {
        Zero, One,
        SrcColor, OneMinusSrcColor,
        DstColor, OneMinusDstColor,
        SrcAlpha, OneMinusSrcAlpha,
        DstAlpha, OneMinusDstAlpha,
        ConstantColor, OneMinusConstantColor,
        ConstantAlpha, OneMinusConstantAlpha,
        SrcAlphaSaturate,
        Src1Color, OneMinusSrc1Color,
        Src1Alpha, OneMinusSrc1Alpha
    };

    /**
     * @brief Blending operations.
     */
    enum class BlendOp
    {
        Add,          ///< source + destination
        Subtract,     ///< source - destination
        ReverseSubtract, ///< destination - source
        Min,          ///< min(source, destination)
        Max           ///< max(source, destination)
        // Add other BlendOp values here if your enum has more
    };


    /**
     * @brief Color component write mask.
     */
    enum class ColorComponentFlags : uint8_t
    {
        R = 1, G = 2, B = 4, A = 8,
        All = R | G | B | A
    };

    inline constexpr ColorComponentFlags operator|(ColorComponentFlags a, ColorComponentFlags b)
    {
        return static_cast< ColorComponentFlags >(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline constexpr ColorComponentFlags& operator|=(ColorComponentFlags& a, ColorComponentFlags b)
    {
        a = a | b;
        return a;
    }

    inline constexpr ColorComponentFlags operator&(ColorComponentFlags a, ColorComponentFlags b)
    {
        return static_cast< ColorComponentFlags >(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline constexpr ColorComponentFlags& operator&=(ColorComponentFlags& a, ColorComponentFlags b)
    {
        a = a & b;
        return a;
    }

    inline constexpr bool operator!=(ColorComponentFlags a, uint32_t b)
    {
        return static_cast< uint32_t >(a) != b;
    }

    inline constexpr bool operator==(uint32_t a, ColorComponentFlags b)
    {
        return a == static_cast< uint32_t >(b);
    }

    /**
     * @brief Per-render-target blending configuration.
     */
    struct ColorBlendAttachmentState
    {
        /**
         * @brief Enable blending for this attachment.
         */
        bool blend_enable = false;

        /**
         * @brief Source color blend factor.
         */
        BlendFactor src_color_blend_factor = BlendFactor::SrcAlpha;

        /**
         * @brief Destination color blend factor.
         */
        BlendFactor dst_color_blend_factor = BlendFactor::OneMinusSrcAlpha;

        /**
         * @brief Color blend operation.
         */
        BlendOp color_blend_op = BlendOp::Add;

        /**
         * @brief Source alpha blend factor.
         */
        BlendFactor src_alpha_blend_factor = BlendFactor::One;

        /**
         * @brief Destination alpha blend factor.
         */
        BlendFactor dst_alpha_blend_factor = BlendFactor::Zero;

        /**
         * @brief Alpha blend operation.
         */
        BlendOp alpha_blend_op = BlendOp::Add;

        /**
         * @brief Bitmask controlling which color channels are written.
         */
        ColorComponentFlags color_write_mask = ColorComponentFlags::All;

        /**
         * @brief Creates standard alpha blending configuration.
         */
        static ColorBlendAttachmentState createAlphaBlending()
        {
            return ColorBlendAttachmentState
            {
                .blend_enable = true,
                .src_color_blend_factor = BlendFactor::SrcAlpha,
                .dst_color_blend_factor = BlendFactor::OneMinusSrcAlpha,
                .color_blend_op = BlendOp::Add,
                .src_alpha_blend_factor = BlendFactor::One,
                .dst_alpha_blend_factor = BlendFactor::Zero,
                .alpha_blend_op = BlendOp::Add
            };
        }

        /**
         * @brief Creates additive blending configuration.
         */
        static ColorBlendAttachmentState createAdditiveBlending()
        {
            return ColorBlendAttachmentState
            {
                .blend_enable = true,
                .src_alpha_blend_factor = BlendFactor::One,
                .dst_alpha_blend_factor = BlendFactor::Zero,
                .color_blend_op = BlendOp::Add
            };
        }
    };

    /**
     * @brief Logical operation to apply (when logic_op_enable is true).
     */
    enum class ColorBlendLogicOp
    {
        Clear, And, AndReverse, Copy, AndInverted, NoOp, Xor, Or, Nor,
        Equiv, Invert, OrReverse, CopyInverted, OrInverted, Nand, Set
    };

    /**
     * @brief Global color blending state configuration.
     *
     * Controls how final fragment colors are combined with render target contents.
     */
    struct ColorBlendStateDesc
    {
        /**
         * @brief Enable logical operations instead of blending.
         *
         * When true:
         * - Blending is disabled
         * - Simple bitwise operations are applied
         * - Requires GPU feature `logic_op`
         */
        bool logic_op_enable = false;

        /**
         * @brief Selected logical operation.
         *
         * Common operations:
         * - Copy: Default (overwrite destination)
         * - And: Useful for stencil-like effects
         * - Xor: Useful for highlighting
         */
        ColorBlendLogicOp logic_op = ColorBlendLogicOp::Copy;

        /**
         * @brief Per-attachment blending states.
         *
         * Must match the number of color attachments in the render pass.
         * Each entry controls blending for one render target.
         */
        std::vector< ColorBlendAttachmentState > attachments;

        /**
         * @brief Blend constants for certain blend factors.
         *
         * Used when blend factors reference:
         * - BlendFactor::ConstantColor
         * - BlendFactor::OneMinusConstantColor
         * - BlendFactor::ConstantAlpha
         * - BlendFactor::OneMinusConstantAlpha
         */
        float blend_constants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        /**
         * @brief Creates default blending state for opaque rendering.
         */
        static ColorBlendStateDesc createOpaque()
        {
            return
            {
                .logic_op_enable = false,
                .attachments = { ColorBlendAttachmentState() } // Default is no blending
            };
        }
    };

    /**
     * ---- Pipeline Layout State Description Structs ----
     */

    /**
     * @brief Types of resource descriptors for shader binding.
     */
    enum class DescriptorType
    {
        Invalid,
        Sampler,               ///< Standalone sampler
        CombinedImageSampler,   ///< Image+sampler combined (common for textures)
        SampledImage,          ///< Separate sampled image
        StorageImage,          ///< Writable storage image
        UniformTexelBuffer,    ///< Formatted buffer (UBO-like but as buffer texture)
        StorageTexelBuffer,    ///< Writable formatted buffer
        UniformBuffer,         ///< Standard uniform buffer (UBO)
        StorageBuffer,         ///< Writable storage buffer (SSBO)
        UniformBufferDynamic,  ///< UBO with dynamic offset
        StorageBufferDynamic,  ///< SSBO with dynamic offset
        InputAttachment        ///< Framebuffer input (for subpasses)
    };

    /**
     * @brief Convert engine descriptor type to a human-readable C string
     *
     * @param type Engine descriptor type
     * @return Null-terminated C string name of the descriptor type
     */
    const char* descriptorTypeCString( DescriptorType type );

    /**
     * @brief Describes a push constant range.
     */
    struct PushConstantRange
    {
        /**
         * @brief Shader stages that can access these push constants.
         */
        ShaderStageFlag stage_flags = ShaderStageFlag::All;

        /**
         * @brief Byte offset of push constant range.
         */
        uint32_t offset = 0;

        /**
         * @brief Size in bytes of push constant range.
         *
         * Must be:
         * - Multiple of 4
         * - Within device limits (typically 128-256 bytes)
         */
        uint32_t size = 0;
    };

    /**
     * @brief Describes a pipeline layout (descriptor sets + push constants).
     */
    struct PipelineLayoutDesc
    {
        /**
         * @brief Descriptor set layouts used by this pipeline.
         */
        std::vector<UniformSetLayout> descriptor_set_layouts;

        /**
         * @brief Push constant ranges used by this pipeline.
         */
        std::vector<PushConstantRange> push_constant_ranges;

        /**
         * @brief Debug name for graphics debugging tools.
         */
        std::string name = "";

        /**
         * @brief Validates that the layout is properly configured.
         */
        bool validate() const {
            for (const auto& range : push_constant_ranges) {
                if (range.size == 0 || (range.size % 4) != 0) {
                    return false;
                }
            }
            return true;
        }
    };

    /**
     * @brief Describes a complete graphics pipeline configuration.
     */
    struct GraphicsPipelineDesc
    {
        /**
         * @brief Pipeline layout defining resource bindings.
         */
        PipelineLayoutHandle pipeline_layout;

        // Shader stages
        std::vector< ShaderHandle > shader_stages;

        // Fixed-function states
        VertexBufferLayout vertex_input_state;
        InputAssemblyStateDesc input_assembly_state;
        RasterizationStateDesc rasterization_state;
        MultisampleStateDesc multisample_state;
        DepthStencilStateDesc depth_stencil_state;
        ColorBlendStateDesc color_blend_state;

        // Render target information
        std::vector< Format > color_attachment_formats; ///< Must match render pass
        Format depth_attachment_format = Format::depth_32;
        Format stencil_attachment_format = Format::undefined;
        SampleCount render_sample_count = SampleCount::Count1;

        /**
         * @brief Debug name for graphics debugging tools.
         */
        std::string name = "";
    };

    /**
     * @brief Describes a compute pipeline configuration.
     */
    struct ComputePipelineDesc
    {
        PipelineLayoutHandle pipeline_layout;  ///< Resource bindings layout
        ShaderHandle compute_shader;           ///< Required compute shader
        std::string name = "";           ///< Debug identifier

        /**
         * @brief Validates that the description is complete.
         */
        bool validate() const
        {
            return compute_shader.id != 0 && pipeline_layout.id != 0;
        }
    };

    struct PipelineRenderingCreateInfo
    {
        std::vector< Format > color_attachment_formats;
        Format depth_attachment_aormat;
        Format stencil_attachment_format;
        uint32_t view_mask;
    };

    /**
     * ---- Swapchain State Description Structs ----
     */

    /**
     * @brief Defines how the presentation engine synchronizes with display refresh
     */
    enum class PresentMode
    {
        Immediate,  ///< No sync, display as soon as rendered (potential tearing)
        Fifo,       ///< Strict vsync, queue frame until next vblank (highest latency)
        FifoRelaxed, ///< Vsync usually, but can tear if rendering falls behind (reduced stutter)
        Mailbox     ///< Triple buffering with replacement, low latency but high GPU usage
    };

    /**
     * ---- Device State Description Structs ----
     */

    /**
     * @struct QueueFamilyIndices
     * @brief Holds queue family indices for various queue types
     */
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> compute_family;
        std::optional<uint32_t> transfer_family;
        std::optional<uint32_t> present_family;

        /**
         * @brief Check if all required queue families are available
         * @return true if all required queue families are available
         */
        bool isComplete() const
        {
            return graphics_family.has_value() &&
                   compute_family.has_value() &&
                   transfer_family.has_value() &&
                   present_family.has_value();
        }
    };


    /**
     * @brief Specifies how the attachment's contents should be handled at the beginning of the render pass.
     */
    enum class AttachmentLoadOp
    {
        Load,     ///< Load existing contents of the attachment.
        Clear,    ///< Clear the attachment to a specified value.
        DontCare  ///< Contents of the attachment are undefined at the start.
    };

    /**
     * @brief Specifies how the attachment's contents should be handled at the end of the render pass.
     */
    enum class AttachmentStoreOp
    {
        Store,    ///< Store the rendered contents back to memory.
        DontCare, ///< Contents of the attachment are undefined after the render pass.
        None      ///< Do not store the results (e.g., for read-only attachments in some APIs).
    };


    /**
     * @brief Simple resource registry concept.
     *
     * This is a function pointer that maps a logical Render Graph resource ID
     * to its corresponding physical device handle (void pointer) for the current
     * frame. The `is_texture` flag helps the registry return the correct handle type.
     * A more sophisticated registry might be needed depending on the resource
     * allocation and management strategy.
     *
     * @param logical_id The logical identifier of the resource from the Render Graph.
     * @param is_texture True if the resource is a texture, false if it's a buffer.
     * @return A constant void pointer to the physical device handle of the resource.
     */
    using ResourceRegistry = std::function<const void*(uint64_t logical_id, bool is_texture)>;

    /**
     * @brief Describes a color or depth/stencil attachment for the `beginRendering` command.
     *
     * This struct contains information about the textures that will serve as
     * color or depth/stencil attachments during a dynamic rendering pass. It
     * includes the handle to the physical image view, load and store operations,
     * clear values, and the initial and final layouts of the attachment.
     *
     * The Render Graph is responsible for ensuring that the image layouts are
     * transitioned to the `initial_layout` *before* `beginRendering` is called.
     * Similarly, after `endRendering`, the layouts will be in the `final_layout`.
     */
    struct RenderingAttachmentInfo
    {
        ref::Image image{}; ///< Handle to the physical TextureView used as the attachment.

        ClearValue clear_value; ///< The value to clear the attachment to if `load_op` is `Clear`.
        AttachmentLoadOp load_op = AttachmentLoadOp::DontCare;   ///< The load operation for this attachment.
        AttachmentStoreOp store_op = AttachmentStoreOp::Store;     ///< The store operation for this attachment.

        ImageLayout image_layout = ImageLayout::Undefined;
        //ResourceState initial_layout = ResourceState::Undefined; ///< Expected layout of the attachment at the start of `beginRendering`.
        //ResourceState final_layout = ResourceState::Undefined;   ///< Desired layout of the attachment after `endRendering`.
    };

    /**
     * @brief Information structure for beginning a dynamic rendering pass.
     *
     * This struct contains all the necessary details to start rendering without
     * relying on traditional render pass objects. It includes the render area,
     * layer count, and the descriptions of the color and depth/stencil attachments.
     */
    struct RenderingInfo
    {
        Rect2D render_area{};       ///< Specifies the rectangular rendering bounds.
        uint32_t layer_count = 1; ///< Number of layers to render (for array textures or multiview).
        // uint32_t view_mask = 0; // For multiview rendering (API-specific).

        std::vector<RenderingAttachmentInfo> color_attachments; ///< Vector of color attachments.
        std::optional<RenderingAttachmentInfo> depth_attachment;   ///< Optional depth attachment.
        std::optional<RenderingAttachmentInfo> stencil_attachment; ///< Optional stencil attachment.
    };

    /**
     * @brief Structure describing a region for buffer copy operations.
     *
     * Specifies the source and destination offsets within the buffers,
     * as well as the size of the memory region to be copied.
     */
    struct BufferCopyRegion
    {
        uint64_t src_offset = 0; ///< Offset in bytes from the start of the source buffer.
        uint64_t dst_offset = 0; ///< Offset in bytes from the start of the destination buffer.
        uint64_t size = 0;       ///< Number of bytes to copy.
    };

    /**
     * @brief Defines the subresource layers of a texture.
     *
     * Specifies the mip level, base array layer, and the number of layers
     * to be considered for the operation.
     */
    struct SubresourceLayers
    {
        uint32_t mip_level = 0;       ///< Mipmap level to start from.
        uint32_t base_array_layer = 0; ///< First array layer to consider.
        uint32_t layer_count = 1;     ///< Number of array layers to consider.
        //ImageAspectFlags aspect_mask; // Could be added if aspect needs explicit specification.
    };

    enum struct ImageAspectFlag
    {
        None = 0x00000000, // No aspect selected (invalid in most operations)
        Color   = 0x00000001,
        Depth   = 0x00000002,
        Stencil = 0x00000004,
        DepthStencil = Depth | Stencil
    };

    struct ClearRect
    {
        Rect2D      rect;
        uint32_t    base_array_layer;
        uint32_t    layer_count;
    };

    struct ClearAttachment
    {
        ImageAspectFlag aspect_mask;
        uint32_t        attachment_index;
        ClearValue      clear_value;
    };

    /**
     * @brief Structure describing a region for texture copy operations.
     *
     * Defines the subresource layers and 3D offsets for both the source and
     * destination textures, along with the 3D extent of the region to be copied.
     */
    struct ImageCopyRegion
    {
        SubresourceLayers src_subresource; // Subresource layers for the source texture.
        SubresourceLayers dst_subresource; // Subresource layers for the destination texture.

        Offset3D src_offset; // 3D offset for the source texture.
        Offset3D dst_offset; // 3D offset for the destination texture.

        Extent3D extent; // 3D extent of the copy region.
    };

    /**
     * @brief Structure defining a subresource range for texture operations (e.g., clearing).
     *
     * This structure specifies the mip levels and array layers affected by an operation.
     * It's used in functions like `clearColorTexture` and `clearDepthStencilTexture`.
     */
    struct ImageSubresourceRange
    {
        ImageAspectFlag aspect_mask; ///< Aspect mask indicating which parts of the image are affected (e.g., color, depth, stencil).
        uint32_t base_mip_level;     ///< First mipmap level to affect.
        uint32_t level_count;        ///< Number of mipmap levels to affect.
        uint32_t base_array_layer;   ///< First array layer to affect.
        uint32_t layer_count;        ///< Number of array layers to affect.
    };

    /**
     * @brief Structure describing a region for copy operations between a buffer and a texture.
     *
     * Defines the offset in the buffer, row and image height for buffer layout,
     * the subresource layers and offset in the texture, and the 3D extent of the
     * texture region involved in the copy.
     */
    struct BufferTextureCopyRegion
    {
        uint64_t buffer_offset = 0;       ///< Offset in bytes from the start of the buffer.
        uint32_t buffer_row_length = 0;   ///< Number of bytes per row of the texture data in the buffer (0 for tightly packed).
        uint32_t buffer_image_height = 0; ///< Height of the texture data in the buffer in rows (0 for tightly packed).

        /**
         * @brief Defines the subresource layers of the texture.
         *
         * Specifies the mip level, base array layer, and the number of layers
         * to be considered for the operation.
         */
        SubresourceLayers texture_subresource; ///< Subresource layers for the texture.

        /**
         * @brief Defines a 3D offset within the texture.
         */
        Offset3D texture_offset; ///< 3D offset for the texture.

        /**
         * @brief Defines the 3D extent (size) of the texture region.
         */
        Extent3D texture_extent; ///< 3D extent of the texture region.
    };


    // Helper function to check if a format is depth/stencil
    inline bool isDepthStencilFormat(Format format)
    {
        switch (format)
        {
            case Format::depth_16_stencil_8:
            case Format::depth_24_stencil_8:
            case Format::depth_32_stencil_8:
            case Format::stencil_u8:
            case Format::depth_16:
            case Format::depth_32:
                return true;

            default: return false;
        }
    }

    inline bool isDepthOnlyFormat(Format format)
    {
        switch (format)
        {
            case Format::depth_16:
            case Format::depth_32:
                return true;

            default: return false;
        }
    }

    inline bool isStencilOnlyFormat(Format format)
    {
        return format == Format::stencil_u8;
    }


    /**
     * @brief Namespace defining flags for render graph stages.
     *
     * These flags represent different stages within the graphics pipeline
     * where memory dependencies and synchronization might be necessary.
     * They are used in barrier descriptions to specify the source and
     * destination stages of a resource transition.
     */
    enum struct PipelineStageFlag : uint32_t
    {
        None = 0,
        TopOfPipe                    = 1 << 1,
        DrawIndirect                 = 1 << 2,
        VertexInput                  = 1 << 3,
        VertexShader                 = 1 << 4,
        TessellationControlShader    = 1 << 5,
        TessellationEvaluationShader = 1 << 6,
        GeometryShader               = 1 << 7,
        FragmentShader               = 1 << 8,
        EarlyFragmentTests           = 1 << 9,
        LateFragmentTests            = 1 << 10,
        ColorOutput                  = 1 << 11,
        ComputeShader                = 1 << 12,
        Transfer                     = 1 << 13,
        Host                         = 1 << 14,
        RayTrace                     = 1 << 15,
        BottomOfPipe                 = 1 << 16,
        AllCommands = 0xFFFFFFFF,
        AllGraphics = VertexInput | VertexShader | TessellationControlShader |
                      TessellationEvaluationShader | GeometryShader | FragmentShader |
                      EarlyFragmentTests | LateFragmentTests | ColorOutput,
        AllTransfer = Transfer,
    };

    /**
     * @brief Enables bitwise OR operation for PipelineStageFlag.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise OR of the two flags.
     */
    inline PipelineStageFlag operator|(PipelineStageFlag a, PipelineStageFlag b)
    {
        return static_cast< PipelineStageFlag >( static_cast<uint32_t>(a) | static_cast<uint32_t>(b) );
    }

    /**
     * @brief Enables bitwise AND operation for PipelineStageFlag.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise AND of the two flags.
     */
    inline PipelineStageFlag operator&(PipelineStageFlag a, PipelineStageFlag b)
    {
        return static_cast< PipelineStageFlag >( static_cast<uint32_t>(a) & static_cast<uint32_t>(b) );
    }

    /**
     * @brief Enables bitwise XOR operation for PipelineStageFlag.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise XOR of the two flags.
     */
    inline PipelineStageFlag operator^(PipelineStageFlag a, PipelineStageFlag b)
    {
        return static_cast<PipelineStageFlag>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
    }

    /**
     * @brief Enables bitwise NOT operation for PipelineStageFlag.
     * @param a The flag to negate.
     * @return The bitwise NOT of the flag.
     */
    inline PipelineStageFlag operator~(PipelineStageFlag a)
    {
        return static_cast<PipelineStageFlag>(~static_cast<uint32_t>(a));
    }

    /**
     * @brief Enables the |= assignment operator for PipelineStageFlag.
     * @param a The flag to modify.
     * @param b The flag to OR with.
     * @return A reference to the modified flag.
     */
    inline PipelineStageFlag& operator|=(PipelineStageFlag& a, PipelineStageFlag b)
    {
        return a = a | b;
    }

    /**
     * @brief Enables the &= assignment operator for PipelineStageFlag.
     * @param a The flag to modify.
     * @param b The flag to AND with.
     * @return A reference to the modified flag.
     */
    inline PipelineStageFlag& operator&=(PipelineStageFlag& a, PipelineStageFlag b)
    {
        return a = a & b;
    }

    /**
     * @brief Enables the ^= assignment operator for PipelineStageFlag.
     * @param a The flag to modify.
     * @param b The flag to XOR with.
     * @return A reference to the modified flag.
     */
    inline PipelineStageFlag& operator^=(PipelineStageFlag& a, PipelineStageFlag b)
    {
        return a = a ^ b;
    }

    /**
     * @brief Enables the == operator for PipelineStageFlag.
     * @param a The flag to modify.
     * @param b The integer
     * @return A true if equal, false otherwise.
     */
    inline bool operator==(PipelineStageFlag a, int b)
    {
        return static_cast<uint32_t>(a) == b;
    }

    /**
     * @brief Enables the == operator for PipelineStageFlag.
     * @param a The integer
     * @param b The flag to modify.
     * @return A true if equal, false otherwise.
     */
    inline bool operator==(int a, PipelineStageFlag b)
    {
        return static_cast<uint32_t>(b) == a;
    }

    /**
     * @brief Enables the != operator for PipelineStageFlag.
     * @param a The flag to modify.
     * @param b The integer
     * @return A true if equal, false otherwise.
     */
    inline bool operator!=(PipelineStageFlag a, int b)
    {
        return static_cast<uint32_t>(a) != b;
    }

    /**
     * @brief Enables the != operator for PipelineStageFlag.
     * @param a The integer
     * @param b The flag to modify.
     * @return A true if equal, false otherwise.
     */
    inline bool operator!=(int a, PipelineStageFlag b)
    {
        return static_cast<uint32_t>(b) != a;
    }

    /**
     * @brief Describes an image memory barrier.
     *
     * Contains all the information needed to transition an image resource
     * between different states, including layout, access patterns, and
     * pipeline stages.
     */
    struct ImageMemoryBarrier
    {
        std::string resource_name;  ///< Name of the image in the RenderGraph

        /// @name Transition Metadata
        /// @{
        ImageLayout old_layout;    ///< Layout before the barrier
        ImageLayout new_layout;    ///< Layout after the barrier
        AccessFlags src_access;     ///< Access type before the barrier
        AccessFlags dst_access;     ///< Access type after the barrier
        PipelineStageFlag src_stage;   ///< Pipeline stage before the barrier
        PipelineStageFlag dst_stage;   ///< Pipeline stage after the barrier
        /// @}

        QueueType src_queue = QueueType::Graphics; ///< Source queue family
        QueueType dst_queue = QueueType::Graphics; ///< Destination queue family

        ImageSubresourceRange subresource_range; ///< Affected subresource range
        ref::Image image;
    };

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




    struct SubmitInfo
    {
        kege::ref::CommandBuffer command_buffer;
        kege::ref::Semaphore render_complete_semaphore;
        std::vector< kege::ref::Semaphore > wait_semaphores;
        std::vector< kege::PipelineStageFlag > wait_stages;
    };
}

#endif /* graphics_enums_and_types_hpp */
