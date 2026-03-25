

#ifndef kege_dynamic_state_hpp
#define kege_dynamic_state_hpp

namespace kege{

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
