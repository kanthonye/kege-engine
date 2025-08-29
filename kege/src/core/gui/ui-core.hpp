//
//  ui-primitives.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/2/25.
//

#ifndef ui_primitives_hpp
#define ui_primitives_hpp

#include <stdlib.h>
#include "../input/inputs.hpp"
#include "../utils/log.hpp"
#include "../math/algebra/vmath.hpp"
#include "../graphics/font/font.hpp"
#include "../graphics/font/font-loader.hpp"
#include "../graphics/font/font-creator.hpp"
#include "../graphics/loaders/image-loader.hpp"

namespace kege::ui{

    class Input;
    class Layout;
    typedef uint32_t NodeIndex;

    typedef const char* chrstr;

    enum struct AlignText : uint8_t
    {
        Center, Left, Right,
    };

    enum struct Positioning : uint8_t
    {
        Relative,
        Absolute,
    };

    typedef enum : uint8_t
    {
        ALIGN_TOP_LEFT,
        ALIGN_TOP_CENTER,
        ALIGN_TOP_RIGHT,
        ALIGN_LEFT_CENTER,
        ALIGN_CENTER_CENTER,
        ALIGN_RIGHT_CENTER,
        ALIGN_BOTTOM_LEFT,
        ALIGN_BOTTOM_CENTER,
        ALIGN_BOTTOM_RIGHT,
    }
    AlignOrigin;

    typedef enum : uint8_t
    {
        DIRECTION_LEFT_TO_RIGHT,
        DIRECTION_RIGHT_TO_LEFT,
        DIRECTION_TOP_TO_BOTTOM,
        DIRECTION_BOTTOM_TO_TOP,
    } Direction;

    struct Alignment
    {
        AlignOrigin origin = ALIGN_TOP_LEFT;
        Direction direction = DIRECTION_LEFT_TO_RIGHT;
    };

    typedef enum : uint8_t
    {
        SIZE_FIXED,
        SIZE_PERCENT,
        SIZE_FLEXIBLE,
        SIZE_EXTEND,
    }
    SizingType;

    struct Sizing
    {
        float size;
        SizingType type;
    };

    struct Padding
    {
        float left;
        float above;
        float right;
        float below;
    };

    struct Color
    {
        float r = 1.f;
        float g = 1.f;
        float b = 1.f;
        float a = 1.f;
    };

    struct Range
    {
        float min, max;
    };

    typedef enum : uint8_t
    {
        BACKGROUND_COLOR,
        BACKGROUND_IMAGE
    }
    BackgroundType;

    struct Background
    {
        Color color;
        BackgroundType type;
    };


    enum struct HoverTrigger: uint8_t
    {
        Disable,
        OnEnter,
        OnExit,
    };

    enum struct ClickTrigger: uint8_t
    {
        Disable,
        OnClick,
        OnRelease,
    };

    //    enum struct InteractionFlags: uint8_t
    //    {
    //        CLICKABLE  = 1,
    //        HOVERABLE  = 2,
    //        SCROLLABLE = 4,
    //        DRAGGAABLE = 8,
    //    };
    //
    //    inline constexpr InteractionFlags operator |( InteractionFlags a, InteractionFlags b )
    //    {
    //        return static_cast< InteractionFlags >( static_cast< uint32_t >( a ) | static_cast< uint32_t >( b ) );
    //    }
    //
    //    inline constexpr InteractionFlags& operator |=( InteractionFlags& a, InteractionFlags b )
    //    {
    //        return a = a | b;
    //    }
    //
    //    inline constexpr InteractionFlags operator&( InteractionFlags a, InteractionFlags b )
    //    {
    //        return static_cast< InteractionFlags >( static_cast< uint32_t >( a ) & static_cast< uint32_t >( b ) );
    //    }
    //
    //    inline constexpr InteractionFlags& operator &=( InteractionFlags& a, InteractionFlags b )
    //    {
    //        return a = a & b;
    //    }


    typedef std::function< void( kege::ui::Layout&, uint32_t ) > Callback;

    struct Text
    {
        const char* text = nullptr;
        float x, y;
        float width;
        float height;
    };

    struct Rect
    {
        float x, y;
        float width;
        float height;
    };

    struct Corners
    {
        float top_left = 0;
        float top_right = 0;
        float bottom_left = 0;
        float bottom_right = 0;
    };


    // shareable style that can be shared across many ui-content
    struct Style
    {
        Background  background;
        Color       color;
        Padding     padding;
        Sizing      width;
        Sizing      height;
        Alignment   align;
        Extent2D    gap;
        Corners     border_radius;
        Positioning position = Positioning::Relative;
        AlignText   align_text = AlignText::Left;
        int         font_size = 20;
        int32_t     zindex = 0;
        bool        clip_overflow = false;
        bool        wrap_around = false;
    };

    // Widget contains the widget specific data that specific to a widget
    struct Widget
    {
        Style* style = nullptr;
        uint32_t id = 0;

        float x = 0.f;
        float y = 0.f;

        bool mouseover = true;
        bool visible = true;

        ClickTrigger trigger = ui::ClickTrigger::Disable;

        Callback on_release;
        Callback on_scroll;
        Callback on_click;
        Callback on_enter;
        Callback on_exit;

        /**
         * note rect is recomputed every frame, rect is the visual shape of the gui shape
         */
        Rect rect = {};

        /**
         * text is the xy position of a text and the width and height that text span
         */
        mutable Text text = {};
    };

    // Node contains the widget specific data that specific to a widget
    struct Node
    {
        Widget* content    = nullptr;
        uint32_t depth      = 0;
        uint32_t index      = 0;
        uint32_t id         = 0;

        int32_t parent      = 0;
        int32_t head        = 0;
        int32_t tail        = 0;
        int32_t next        = 0;
        int32_t count       = 0;

        int32_t next_free   = 0;
        bool    freed       = false;
    };

    struct DrawElem
    {
        ui::Rect rect;
        ui::Color  color;
        ui::Rect texel;
        ui::Rect clip_rect;
        struct
        {
            float border_radius;
            float isfont;
            float texture_id;
            float pad;
        };
    };
    typedef std::vector< ui::DrawElem > RenderData;

    ui::Sizing fixed(float size);
    ui::Sizing extend();
    ui::Sizing flexible();
    ui::Sizing percent(float percent);

    ui::Color rgb(uint32_t hex_color);
    ui::Color rgba(uint32_t hex_color);

    ui::Background bgImage(const ui::Rect& texel);
    ui::Background bgColor(const ui::Color& color);
    ui::Background bgColor(uint32_t color);
}

#endif /* ui_primitives_hpp */
