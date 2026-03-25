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

    enum struct Result
    {
        UNKNOWN_ERROR,
        ERROR_DEVICE_LOST,
        SUCCESS,
        INVALID_ID,
        INVALID_OPERATION,
        FAILED_CREATING_OBJECT,
        FAILED_TO_SHUTDOWN,
        OUT_OF_BOUND_INDEX,
        OUTDATED_SWAPCHAIN,
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

    enum struct PipelineType
    {
        Graphics = 1,
        Compute,
        RayTracing,
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
     * @brief Depth comparison operations.
     *
     * Used for shadow map sampling and depth tests.
     */
    // TODO: deprecated
//    enum class CompareOp
//    {
//        Never,        ///< Never pass the comparison
//        Less,         ///< Pass if incoming < stored
//        Equal,        ///< Pass if incoming == stored
//        LessEqual,    ///< Pass if incoming <= stored
//        Greater,      ///< Pass if incoming > stored
//        NotEqual,     ///< Pass if incoming != stored
//        GreaterEqual, ///< Pass if incoming >= stored
//        Always        ///< Always pass the comparison
//    };

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
     * @brief Polygon rendering mode.
     */
//    enum class PolygonMode
//    {
//        Invalid,
//        Fill,  ///< Solid polygons (default)
//        Line,  ///< Wireframe rendering
//        Point  ///< Vertex points only
//    };

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

    struct SubmitInfo
    {
        kege::ref::CommandBuffer command_buffer;
        kege::ref::Semaphore render_complete_semaphore;
        std::vector< kege::ref::Semaphore > wait_semaphores;
        std::vector< kege::PipelineStageFlag > wait_stages;
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

    struct InputAssembly
    {
        kege::PrimitiveTopology topology = kege::PrimitiveTopology::TriangleStrip;
        bool primitive_restart = true;
    };

}

#endif /* graphics_enums_and_types_hpp */
