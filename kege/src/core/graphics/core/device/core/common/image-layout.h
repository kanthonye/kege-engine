//
//  image-layout.h
//  KE-GE
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#ifndef image_layout_h
#define image_layout_h

namespace kege{

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

    struct Offset2D
    {
        int32_t    x;
        int32_t    y;
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

    struct Extent2D
    {
        uint32_t width;
        uint32_t height;
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

    struct ClearRect
    {
        Rect2D      rect;
        uint32_t    base_array_layer;
        uint32_t    layer_count;
    };

    enum struct ImageAspectFlag
    {
        None = 0x00000000, // No aspect selected (invalid in most operations)
        Color   = 0x00000001,
        Depth   = 0x00000002,
        Stencil = 0x00000004,
        DepthStencil = Depth | Stencil
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

    struct ClearAttachment
    {
        ImageAspectFlag aspect_mask;
        uint32_t        attachment_index;
        ClearValue      clear_value;
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
     * @brief Enumeration of texture dimensionalities and types.
     *
     * Defines the fundamental structure of texture resources.
     */
    enum class ImageType
    {
        Type1D,        ///< 1-dimensional texture
        Type1DArray,    ///< Array of 1D textures
        Type2D,         ///< 2-dimensional texture
        Type2DArray,    ///< Array of 2D textures
        TypeCube,       ///< Cube map texture (6 faces)
        TypeCubeArray,  ///< Array of cube map textures
        Type3D          ///< 3-dimensional (volumetric) texture
    };

    enum struct ImageUsage: uint32_t
    {
        Undefined       = 0,
        TransferSrc     = 1 << 1,
        TransferDst     = 1 << 2,
        Sampled         = 1 << 3,
        Storage         = 1 << 4,
        Color           = 1 << 5,
        DepthStencil    = 1 << 6,
        Transient       = 1 << 7,
        Input           = 1 << 8,
        HostTransfer    = 1 << 9,
        Present         = 1 << 10,
    };

    inline constexpr ImageUsage operator | (const ImageUsage& a, const ImageUsage& b)
    {
        return static_cast< ImageUsage >(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline constexpr ImageUsage operator&(const ImageUsage& a, const ImageUsage& b)
    {
        return static_cast< ImageUsage >(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
    inline constexpr ImageUsage& operator |=( ImageUsage a, const ImageUsage& b)
    {
        return a = a | b;
    }
    inline constexpr ImageUsage& operator &=( ImageUsage a, const ImageUsage& b)
    {
        return a = a & b;
    }
    inline constexpr bool checkFlag( ImageUsage a, const ImageUsage& b)
    {
        return (static_cast< uint32_t >( a ) & static_cast< uint32_t >( b )) != 0;
    }


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

}
#endif /* image_layout_h */
