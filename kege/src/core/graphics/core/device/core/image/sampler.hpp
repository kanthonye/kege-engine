//
//  sampler.hpp
//  editor
//
//  Created by Kenneth Esdaile on 2/16/26.
//

#ifndef sampler_hpp
#define sampler_hpp

#include "image.hpp"

namespace kege
{
    class Sampler;
}

namespace kege::vk
{
    class Sampler;
}

namespace kege::ref
{
    typedef kege::Ref< kege::Sampler > Sampler;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::Sampler > Sampler;
}

namespace kege{

    /**
     * @brief Sampler description structure.
     *
     * Combines all filtering and addressing parameters into a complete
     * sampler configuration.
     */
    struct SamplerDesc
    {
        Filter min_filter = Filter::Linear;       ///< Minification filter
        Filter mag_filter = Filter::Linear;       ///< Magnification filter
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
