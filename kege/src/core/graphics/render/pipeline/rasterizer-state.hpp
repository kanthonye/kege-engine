
#ifndef kege_rasterizer_state_hpp
#define kege_rasterizer_state_hpp

#include "../core/graphics-core.hpp"
#include "../core/sampler.hpp"

namespace kege{

    struct Multisample
    {
        SampleCount sample_count = SampleCount::Count1;
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

//    enum class CullMode : uint8_t
//    {
//        None,          // Render both sides
//        Front,         // Cull front-facing triangles
//        Back,          // Cull back-facing triangles (DEFAULT)
//        // FrontAndBack // Both sides culled (useful for debugging)
//    };
//
//    enum class FrontFace : uint8_t
//    {
//        Clockwise,     // CW vertices are front-facing (DEFAULT for D3D)
//        CounterClockwise // CCW vertices are front-facing (DEFAULT for OpenGL)
//    };

    struct DepthBias
    {
        bool  enable = false;
        float constant_factor = 0;
        float slope_factor = 0;
        float clamp = 0;
    };

    struct RasterizerState
    {
        FillMode fill_mode = FillMode::Fill;
        CullMode cull_mode = CullMode::Front;
        FrontFace front_face = FrontFace::Clockwise;

        // Depth bias for shadow mapping
        DepthBias depth_bias;

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
                .depth_clip_enable = true,
                .line_width = 1.0f
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
