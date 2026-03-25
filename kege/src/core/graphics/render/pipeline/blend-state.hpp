/**
 * @file    blend-state.hpp
 * @brief   Component system base class for Entity-Component-System architecture
 *
 * @section LICENSE
 * MIT License
 * Copyright (c) 2026 Kenneth Anthony Esdaile
 *
 * @author  Kenneth Anthony Esdaile
 * @version 1.0.0
 * @date    2026-07-02
 */

#ifndef kege_blend_state_hpp
#define kege_blend_state_hpp

#include "../core/graphics-core.hpp"

namespace kege{

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
    enum class ColorWriteMask : uint8_t
    {
        Red     = 1 << 0,   // 0x01
        Green   = 1 << 1,   // 0x02
        Blue    = 1 << 2,   // 0x04
        Alpha   = 1 << 3,   // 0x08

        R       = 1,
        G       = 2,
        B       = 4,
        A       = 8,

        // Common combinations:
        RGB     = R | G | B,     // 0x07
        RGBA    = R | G | B | A, // 0x0F
        All     = R | G | B | A
    };

    inline constexpr ColorWriteMask operator|(ColorWriteMask a, ColorWriteMask b)
    {
        return static_cast< ColorWriteMask >(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline constexpr ColorWriteMask& operator|=(ColorWriteMask& a, ColorWriteMask b)
    {
        a = a | b;
        return a;
    }

    inline constexpr ColorWriteMask operator&(ColorWriteMask a, ColorWriteMask b)
    {
        return static_cast< ColorWriteMask >(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline constexpr ColorWriteMask& operator&=(ColorWriteMask& a, ColorWriteMask b)
    {
        a = a & b;
        return a;
    }

    inline constexpr bool operator!=(ColorWriteMask a, uint32_t b)
    {
        return static_cast< uint32_t >(a) != b;
    }

    inline constexpr bool operator==(uint32_t a, ColorWriteMask b)
    {
        return a == static_cast< uint32_t >(b);
    }

    /**
     * @brief Per-render-target blending configuration.
     */
    struct BlendState
    {
        /**
         * @brief Enable blending for this attachment.
         */
        bool enable = false;

        /**
         * @brief Source color blend factor.
         */
        BlendFactor src_color_blend = BlendFactor::SrcAlpha;

        /**
         * @brief Destination color blend factor.
         */
        BlendFactor dst_color_blend = BlendFactor::OneMinusSrcAlpha;

        /**
         * @brief Color blend operation.
         */
        BlendOp color_blend_op = BlendOp::Add;

        /**
         * @brief Source alpha blend factor.
         */
        BlendFactor src_alpha_blend = BlendFactor::One;

        /**
         * @brief Destination alpha blend factor.
         */
        BlendFactor dst_alpha_blend = BlendFactor::Zero;

        /**
         * @brief Alpha blend operation.
         */
        BlendOp alpha_blend_op = BlendOp::Add;

        /**
         * @brief Bitmask controlling which color channels are written.
         */
        ColorWriteMask color_write_mask = ColorWriteMask::All;
    };

    struct BlendStatePreset
    {
        // Common presets:
        static BlendState opaque()
        {
            return BlendState
            {
                .enable = false,
                .color_write_mask = ColorWriteMask::RGBA
            };
        }

        static BlendState alphaBlend()
        {
            return BlendState
            {
                .enable = true,
                .src_color_blend = BlendFactor::SrcAlpha,
                .dst_color_blend = BlendFactor::OneMinusSrcAlpha,
                .color_blend_op = BlendOp::Add,
                .src_alpha_blend = BlendFactor::One,
                .dst_alpha_blend = BlendFactor::OneMinusSrcAlpha,
                .alpha_blend_op = BlendOp::Add,
                .color_write_mask = ColorWriteMask::RGBA
            };
        }

        static BlendState additive()
        {
            return
            {
                .enable = true,
                .src_color_blend = BlendFactor::SrcAlpha,
                .dst_color_blend = BlendFactor::One,
                .color_blend_op = BlendOp::Add,
                .src_alpha_blend = BlendFactor::One,
                .dst_alpha_blend = BlendFactor::One,
                .alpha_blend_op = BlendOp::Add,
                .color_write_mask = ColorWriteMask::RGB  // Often don't write alpha
            };
        }

        static BlendState multiplicative()
        {
            return
            {
                .enable = true,
                .src_color_blend = BlendFactor::Zero,
                .dst_color_blend = BlendFactor::SrcColor,
                .color_blend_op = BlendOp::Add,
                .src_alpha_blend = BlendFactor::Zero,
                .dst_alpha_blend = BlendFactor::SrcAlpha,
                .alpha_blend_op = BlendOp::Add,
                .color_write_mask = ColorWriteMask::RGBA
            };
        }

        static BlendState preMultipliedAlpha()
        {
            return
            {
                .enable = true,
                .src_color_blend = BlendFactor::One,
                .dst_color_blend = BlendFactor::OneMinusSrcAlpha,
                .color_blend_op = BlendOp::Add,
                .src_alpha_blend = BlendFactor::One,
                .dst_alpha_blend = BlendFactor::OneMinusSrcAlpha,
                .alpha_blend_op = BlendOp::Add,
                .color_write_mask = ColorWriteMask::RGBA
            };
        }

        // For UI/overlays (preserve destination alpha)
        static BlendState uiAlphaBlend()
        {
            return
            {
                .enable = true,
                .src_color_blend = BlendFactor::SrcAlpha,
                .dst_color_blend = BlendFactor::OneMinusSrcAlpha,
                .color_blend_op = BlendOp::Add,
                .src_alpha_blend = BlendFactor::Zero,  // Don't modify dest alpha!
                .dst_alpha_blend = BlendFactor::One,
                .alpha_blend_op = BlendOp::Add,
                .color_write_mask = ColorWriteMask::RGBA
            };
        }

        // For deferred lighting (additive light accumulation)
        static BlendState lightAccumulation()
        {
            return
            {
                .enable = true,
                .src_color_blend = BlendFactor::One,
                .dst_color_blend = BlendFactor::One,
                .color_blend_op = BlendOp::Add,
                .src_alpha_blend = BlendFactor::One,
                .dst_alpha_blend = BlendFactor::One,
                .alpha_blend_op = BlendOp::Add,
                .color_write_mask = ColorWriteMask::RGB  // No alpha in light buffer
            };
        }

    //    // For advanced effects like soft particles, order-independent transparency
    //    static BlendState DualSourceAlphaBlend()
    //    {
    //        return
    //        {
    //            .enable = true,
    //            .src_color_blend = BlendFactor::SrcAlpha,
    //            .dst_color_blend = BlendFactor::OneMinusSrcAlpha,
    //            .color_blend_op = BlendOp::Add,
    //            .src_alpha_blend = BlendFactor::One,
    //            .dst_alpha_blend = BlendFactor::OneMinusSrcAlpha,
    //            .alpha_blend_op = BlendOp::Add,
    //            .color_write_mask = ColorWriteMask::All,
    //            .dualSourceBlendEnable = true  // For GL_ARB_blend_func_extended
    //        };
    //    }
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
    struct ColorBlendState
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
        std::vector< BlendState > attachments;

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
    };

}

#endif // kege_blend_state_hpp
