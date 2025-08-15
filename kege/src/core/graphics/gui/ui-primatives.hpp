//
//  ui-primitives.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/2/25.
//

#ifndef ui_primitives_hpp
#define ui_primitives_hpp

#include <stdlib.h>
#include "../../math/algebra/vectors.hpp"
#include "../../graphics/core/graphics.hpp"

namespace kege::ui{

    class Input;
    class Layout;

    typedef const char* chrstr;

    class Core;

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
        NONE, VERTICALLY, HORIZONTALLY,
    }
    Direction;

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

    struct Alignment
    {
        AlignText text = AlignText::Left;
        AlignSelf self = AlignSelf::Relative;
        AlignOrigin origin = ALIGN_TOP_LEFT;
        Direction direction = HORIZONTALLY;
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
        int left;
        int above;
        int right;
        int below;
    };

    struct Color
    {
        float r,g,b,a;
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
            float pad[2];
        };
    };
    typedef std::vector< ui::DrawElem > RenderData;

    ui::Sizing fixed(float size);
    ui::Sizing extend();
    ui::Sizing extend(float percent);
    ui::Sizing flexible();
    ui::Sizing percent(float percent);

    ui::Color hexToRGB(uint32_t hex_color);
    ui::Color hexToRGBA(uint32_t hex_color);

    ui::Background bgImage(const ui::Rect2D& texel);
    ui::Background bgColor(const ui::Color& color);
    ui::Background bgColor(uint32_t color);
}

#endif /* ui_primitives_hpp */
