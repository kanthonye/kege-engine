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
#include "../graphics/core/graphics.hpp"
#include "../graphics/loaders/image-loader.hpp"

namespace kege::ui{

    class Input;
    class Layout;

    typedef const char* chrstr;

    enum struct AlignText : uint8_t
    {
        Center, Left, Right,
    };

    enum struct AlignSelf : uint8_t
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
        AlignText text = AlignText::Left;
        AlignSelf self = AlignSelf::Relative;
        AlignOrigin origin = ALIGN_TOP_LEFT;
        Direction direction = DIRECTION_LEFT_TO_RIGHT;
        bool wrap_around = false;
        bool immovable = false;
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

    struct BorderRadius
    {
        float top_left;
        float top_right;
        float bottom_right;
        float bottom_left;
    };

    struct Rect2D
    {
        float x, y;
        float width;
        float height;
    };

    struct Style
    {
        Background background;
        Color color;
        Padding padding;
        Sizing width;
        Sizing height;
        Alignment align;
        Extent2D gap;

        int border_radius = 0;
        int font_size = 20;

        bool visible = true;
        bool mouseover = true;
        bool clip_overflow = false;
        ClickTrigger click_trigger = ui::ClickTrigger::Disable;
        HoverTrigger hover_trigger = ui::HoverTrigger::Disable;
    };

    struct Text
    {
        const char* data = nullptr;
        float x = 0.f;
        float y = 0.f;
        float width = 0.f;
        float height = 0.f;
    };

    struct Info
    {
        Style style = {};

        Callback on_mouse_enter;
        Callback on_mouse_exit;
        Callback on_release;
        Callback on_click;
        Callback on_scroll;

        const char* text = nullptr;
        int style_index = 0;
        int id = 0;
        
        ClickTrigger click_trigger = ui::ClickTrigger::Disable;
        HoverTrigger hover_trigger = ui::HoverTrigger::Disable;
    };

    struct UIElem
    {
        Style style = {};
        Rect2D rect = {};
        Text text;

        ClickTrigger click_trigger = ui::ClickTrigger::Disable;
        HoverTrigger hover_trigger = ui::HoverTrigger::Disable;

        // the local x and y position
        float x = 0.f;
        float y = 0.f;

        Callback on_mouse_enter;
        Callback on_mouse_exit;
        Callback on_release;
        Callback on_click;
        Callback on_scroll;
    };

    struct Node
    {
        UIElem  elem = {};

        // level hold the node depth in the layout tree
        uint32_t level = 0;
        int32_t index = 0;
        int32_t id = 0; // elem id. hold the index of this element

        int32_t parent = 0; // the parent node index
        int32_t head = 0;
        int32_t tail = 0;
        int32_t next = 0;
        int32_t count = 0;

        int32_t next_free = 0;
        bool freed = false;
    };

    struct DrawElem
    {
        Rect2D rect;
        Color  color;
        Rect2D texel;
        Rect2D clip_rect;
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
    ui::Sizing extend(float percent);
    ui::Sizing flexible();
    ui::Sizing percent(float percent);

    ui::Color rgb(uint32_t hex_color);
    ui::Color rgba(uint32_t hex_color);

    ui::Background bgImage(const ui::Rect2D& texel);
    ui::Background bgColor(const ui::Color& color);
    ui::Background bgColor(uint32_t color);
}

#endif /* ui_primitives_hpp */
