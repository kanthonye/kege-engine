

#ifndef kege_depth_state_hpp
#define kege_depth_state_hpp

#include "../core/graphics-core.hpp"

namespace kege{

    enum class ComparisonFunc : uint8_t
    {
        Never,         // Always fails
        Less,          // src < dst (DEFAULT for depth)
        Equal,         // src == dst
        LessEqual,     // src <= dst
        Greater,       // src > dst
        NotEqual,      // src != dst
        GreaterEqual,  // src >= dst
        Always         // Always passes
    };

//    enum class StencilOp : uint8_t
//    {
//        Keep,          // Keep current value
//        Zero,          // Set to 0
//        Replace,       // Replace with reference value
//        IncrementClamp, // Increment, clamp to max
//        DecrementClamp, // Decrement, clamp to 0
//        IncrementWrap,  // Increment, wrap on overflow
//        DecrementWrap,  // Decrement, wrap on underflow
//        Invert         // Bitwise invert
//    };

    struct FaceOperations
    {
        StencilOp fail_op;      // Stencil test fails
        StencilOp pass_op;      // Both stencil and depth tests pass
        StencilOp depth_fail_op; // Stencil passes, depth fails
        ComparisonFunc compare_op;
        uint32_t compare_mask;
        uint32_t write_mask;
    };

    struct DepthBound
    {
        float min = 0.f, max = 1.f;
    };


    struct Depth
    {
        // Depth testing
        bool enable;
        bool write;
        ComparisonFunc compare_op;

        bool bounds_test;
        DepthBound bounds;
    };

    struct Stencil
    {
        // Stencil testing (front and back can differ)
        bool enable = false;
        uint32_t reference;  // Runtime stencil reference
        FaceOperations front;
        FaceOperations back;
    };

    struct DepthStencil
    {
        Depth depth;
        Stencil stencil;
    };

    struct DepthStencilPresets
    {
        // Common presets:
        static DepthStencil depthTestWrite()
        {
            return DepthStencil
            {
                .depth = Depth
                {
                    .enable = true,
                    .write = true,
                    .bounds_test = false,
                    .compare_op = ComparisonFunc::Less,
                }
            };
        }

        static DepthStencil depthTestNoWrite()
        {
            return DepthStencil
            {
                .depth = Depth
                {
                    .enable = true,
                    .write = false,
                    .bounds_test = false,
                    .compare_op = ComparisonFunc::Less,
                }
            };
        }

        static DepthStencil depthTestEqual()
        {
            return DepthStencil
            {
                .depth = Depth
                {
                    .enable = true,
                    .write = false,
                    .bounds_test = false,
                    .compare_op = ComparisonFunc::Equal,
                }
            };
        }

        static DepthStencil depthDisabled()
        {
            return DepthStencil
            {
                .depth = Depth
                {
                    .enable = false,
                    .write = false,
                    .bounds_test = false,
                }
            };
        }

        static DepthStencil reverseZ()
        {
            return DepthStencil
            {
                .depth = Depth
                {
                    .enable = true,
                    .write = true,
                    .bounds_test = false,
                    .compare_op = ComparisonFunc::Greater,
                }
            };
        }

        // For UI/overlays (enable but usually no write)
        static DepthStencil uiOverlay()
        {
            return DepthStencil
            {
                .depth = Depth
                {
                    .enable = true,
                    .write = false,
                    .bounds_test = false,
                    .compare_op = ComparisonFunc::LessEqual,
                }
            };
        }

        // For skybox (depth equal to far plane)
        static DepthStencil skybox()
        {
            return DepthStencil
            {
                .depth = Depth
                {
                    .enable = true,
                    .write = false,
                    .bounds_test = false,
                    .compare_op = ComparisonFunc::LessEqual,
                }
            };
        }

        // Stencil examples:

        // Stencil masking for portal rendering
        static DepthStencil stencilWriteMask()
        {
            return DepthStencil
            {
                .depth = Depth
                {
                    .enable = true,
                    .write = true,
                    .bounds_test = true,
                    .compare_op = ComparisonFunc::Less,
                },
                .stencil = Stencil
                {
                    .enable = true,
                    .reference = 1,
                    .front =
                    {
                        .fail_op = StencilOp::Keep,
                        .pass_op = StencilOp::Replace,  // Write reference value
                        .depth_fail_op = StencilOp::Keep,
                        .compare_op = ComparisonFunc::Always,
                        .compare_mask = 0xFF,
                        .write_mask = 0xFF
                    },
                    .back =
                    {
                        .fail_op = StencilOp::Keep,
                        .pass_op = StencilOp::Replace,  // Write reference value
                        .depth_fail_op = StencilOp::Keep,
                        .compare_op = ComparisonFunc::Always,
                        .compare_mask = 0xFF,
                        .write_mask = 0xFF
                    }
                }
            };
        }

        // Stencil enable for masking
        static DepthStencil stencilTestMask(uint32_t mask_value)
        {
            return DepthStencil
            {
                .depth = Depth
                {
                    .enable = true,
                    .write = true,
                    .bounds_test = true,
                    .compare_op = ComparisonFunc::Less,
                },
                .stencil = Stencil
                {
                    .enable = true,
                    .reference = mask_value,
                    .front =
                    {
                        .fail_op = StencilOp::Keep,
                        .pass_op = StencilOp::Keep,  // Write reference value
                        .depth_fail_op = StencilOp::Keep,
                        .compare_op = ComparisonFunc::Equal,
                        .compare_mask = 0xFF,
                        .write_mask = 0x00
                    },
                    .back =
                    {
                        .fail_op = StencilOp::Keep,
                        .pass_op = StencilOp::Keep,  // Write reference value
                        .depth_fail_op = StencilOp::Keep,
                        .compare_op = ComparisonFunc::Equal,
                        .compare_mask = 0xFF,
                        .write_mask = 0x00
                    }
                }
            };
        }

        // For deferred decals (mark volume in stencil)
        static DepthStencil decalVolume()
        {
            return DepthStencil
            {
                .depth = Depth
                {
                    .enable = true,
                    .write = true,
                    .bounds_test = true,
                    .compare_op = ComparisonFunc::Less,
                },
                .stencil = Stencil
                {
                    .enable = true,
                    .reference = 128,
                    .front =
                    {
                        .fail_op = StencilOp::Keep,
                        .pass_op = StencilOp::Replace,
                        .depth_fail_op = StencilOp::Keep,
                        .compare_op = ComparisonFunc::Always,
                        .compare_mask = 0xFF,
                        .write_mask = 0xFF
                    },
                    .back =
                    {
                        .fail_op = StencilOp::Keep,
                        .pass_op = StencilOp::Replace,
                        .depth_fail_op = StencilOp::Keep,
                        .compare_op = ComparisonFunc::Always,
                        .compare_mask = 0xFF,
                        .write_mask = 0xFF
                    }
                }
            };
        }
    };

}

#endif // !kege_depth_state_hpp
