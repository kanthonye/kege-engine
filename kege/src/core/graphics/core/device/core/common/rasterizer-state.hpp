
#ifndef kege_rasterizer_state_hpp
#define kege_rasterizer_state_hpp

#include <stdlib.h>

namespace kege{

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


    struct Multisample
    {
        kege::SampleCount sample_count = SampleCount::Count1;
        bool sample_shading = false;
        bool alpha_to_coverage = false;
        bool alpha_to_one = false;
    };
    
    enum class FillMode : uint8_t
    {
        Fill,         // Normal filled triangles (DEFAULT)
        Line,     // Wireframe rendering
        Point         // Some APIs support point rendering
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

    struct DepthBias
    {
        bool  enable = false;
        float constant_factor = 0;
        float slope_factor = 0;
        float clamp = 0;
    };

    struct RasterizerState
    {
        kege::FillMode fill_mode = FillMode::Fill;
        kege::CullMode cull_mode = CullMode::Front;
        kege::FrontFace front_face = FrontFace::Clockwise;

        // Depth bias for shadow mapping
        kege::DepthBias depth_bias;

        // Other settings
        float line_width = 1;
        bool  depth_clamp_enable = true;
        bool  discard = false;

        bool  depth_clip_enable = true;     // Enable depth clipping
        bool  scissor_enable = false;       // Enable scissor test
        bool  multisample_enable = true;   // Enable multisampling
        bool  antialiased_line_enable = false; // Smooth lines
    };

    struct RasterizerStatePresets
    {
        // Common presets:
        static RasterizerState cullBack()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Fill,
                .cull_mode = CullMode::Back,
                .front_face = FrontFace::CounterClockwise,
                .depth_clip_enable = true,
                .scissor_enable = false,
                .multisample_enable = true
            };
        }

        static RasterizerState cullFront()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Fill,
                .cull_mode = CullMode::Front,
                .front_face = FrontFace::CounterClockwise,
                .depth_clip_enable = true
            };
        }

        static RasterizerState cullNone()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Fill,
                .cull_mode = CullMode::None,
                .front_face = FrontFace::CounterClockwise,
                .depth_clip_enable = true
            };
        }

        static RasterizerState doubleSided()
        {
            return cullNone();  // Same as cull none
        }

        static RasterizerState wireframe()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Line,
                .cull_mode = CullMode::None,
                .front_face = FrontFace::CounterClockwise,
                .line_width = 1.0f,
                .depth_clip_enable = true,
            };
        }

        // For shadow mapping (reduce acne)
        static RasterizerState shadowCaster()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Fill,
                .cull_mode = CullMode::Back,
                .front_face = FrontFace::CounterClockwise,
                .depth_bias.enable = true,
                .depth_bias.clamp = 0.0f,
                .depth_bias.constant_factor = 2.0f,  // Adjust based on scene scale
                .depth_bias.slope_factor = 2.0f,
                .depth_clip_enable = true,
            };
        }

        // For cube/point shadow maps (cull front faces)
        static RasterizerState shadowCasterPointLight()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Fill,
                .cull_mode = CullMode::Front,  // Cull front for point light shadows
                .front_face = FrontFace::CounterClockwise,
                .depth_bias.enable = true,
                .depth_bias.constant_factor = 2.0f,
                .depth_bias.slope_factor = 2.0f,
                .depth_clip_enable = true
            };
        }

        // For UI/2D rendering (no culling, no depth bias)
        static RasterizerState ui()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Fill,
                .cull_mode = CullMode::None,
                .front_face = FrontFace::CounterClockwise,
                .depth_bias.enable = false,
                .depth_clip_enable = true,
                .scissor_enable = true  // UI often uses scissor rects
            };
        }

        // For fullscreen quads (post-processing)
        static RasterizerState fullscreenQuad()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Fill,
                .cull_mode = CullMode::None,  // Or CullMode::Back with CCW quad
                .front_face = FrontFace::CounterClockwise,
                .depth_bias.enable = false,
                .depth_clip_enable = true
            };
        }

        // For planar reflections (reverse culling)
        static RasterizerState reflection()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Fill,
                .cull_mode = CullMode::Front,  // Reverse culling!
                .front_face = FrontFace::CounterClockwise,
                .depth_clip_enable = true
            };
        }

        // For decals (special handling)
        static RasterizerState decal()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Fill,
                .cull_mode = CullMode::Front,  // Or CullMode::None based on technique
                .front_face = FrontFace::CounterClockwise,
                .depth_bias.enable = true,
                .depth_bias.constant_factor = -1.0f,  // Negative bias for decals
                .depth_clip_enable = true
            };
        }

        // For foliage/billboards (often double-sided)
        static RasterizerState foliage()
        {
            return RasterizerState
            {
                .fill_mode = FillMode::Fill,
                .cull_mode = CullMode::None,  // Double-sided leaves
                .front_face = FrontFace::CounterClockwise,
                .depth_bias.enable = false,
                .depth_clip_enable = true
            };
        }
    };

}

#endif // !kege_rasterizer_state_hpp
