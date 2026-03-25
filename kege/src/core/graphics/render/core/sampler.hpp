//
//  sampler.hpp
//  editor
//
//  Created by Kenneth Esdaile on 2/16/26.
//

#ifndef sampler_hpp
#define sampler_hpp

#include "image.hpp"
#include "../pipeline/depth-state.hpp"

namespace kege{

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
        ComparisonFunc compare_op = ComparisonFunc::Always;  ///< Depth comparison operator
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

    class Sampler : public kege::RefCounter
    {
    public:

        virtual const vk::Sampler* vk() const { return nullptr; }
        virtual vk::Sampler* vk() { return nullptr; }
        virtual ~Sampler() = default;

    protected:

        Sampler() = default;
    };

}
#endif /* sampler_hpp */
