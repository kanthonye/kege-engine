

#ifndef kege_dynamic_state_hpp
#define kege_dynamic_state_hpp

namespace kege{

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
    
    enum class DynamicState : uint32_t
    {
        Null = 0,
        Viewport = 1,
        Scissor = 2,
        LineWidth = 4,
        DepthBias = 8,
        BlendConstants = 16,     // For constant color blending
        BlendBounds = 32,
        StencilCompareMask = 64,
        StencilWriteMask = 128,
        StencilReference = 256,
        CullMode = 512,
        FrontFace = 1024,
        PrimitiveTopology = 2048,
        DepthTestEnabled = 4096,
        PolygonMode = 1000455004,
        PrimitiveRestartEnable = 1000377004,
        DepthWriteEnable = 1000267007,
        DepthCompareOpP = 1000267008,
        DepthBoundTestEnable = 1000267009,
        StencilTestEnable = 1000267010,
        RasterizerDiscardEbale = 1000377001,
        // ... others
    };

    inline constexpr DynamicState operator|(DynamicState a, DynamicState b)
    {
        return static_cast<DynamicState>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline constexpr DynamicState operator&(DynamicState a, DynamicState b)
    {
        return static_cast<DynamicState>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline constexpr DynamicState& operator|=(DynamicState& a, DynamicState b)
    {
        a = a | b;
        return a;
    }

    inline constexpr DynamicState& operator&=(DynamicState& a, DynamicState b)
    {
        a = a & b;
        return a;
    }

    inline constexpr bool operator==(DynamicState& a, DynamicState b)
    {
        return (static_cast<uint32_t>(a) == static_cast<uint32_t>(b));
    }

    inline constexpr bool operator!=(DynamicState& a, DynamicState b)
    {
        return (static_cast<uint32_t>(a) != static_cast<uint32_t>(b));
    }

    inline constexpr bool check(DynamicState& a, DynamicState b)
    {
        return (a & b) == b;
    }

    struct DynamicStatePresets
    {
        static DynamicState minimal()
        {
            return DynamicState::Viewport | DynamicState::Scissor;
        }

        static DynamicState full()
        {
            return
            DynamicState::Viewport |
            DynamicState::Scissor |
            DynamicState::LineWidth |
            DynamicState::DepthBias |
            DynamicState::BlendConstants |
            DynamicState::StencilCompareMask |
            DynamicState::StencilWriteMask |
            DynamicState::StencilReference;
        }
    };
}

#endif // !kege_dynamic_state_hpp
