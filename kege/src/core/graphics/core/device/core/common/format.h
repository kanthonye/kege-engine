//
//  format.h
//  KE-GE
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#ifndef format_h
#define format_h
namespace kege{

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

}
#endif /* format_h */
