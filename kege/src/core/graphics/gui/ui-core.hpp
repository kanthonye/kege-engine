//
//  ui-primitives.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/2/25.
//

#ifndef ui_primitives_hpp
#define ui_primitives_hpp

#include <stdlib.h>
#include "../../input/inputs.hpp"
#include "../../utils/log.hpp"
#include "../../utils/kege-string.hpp"
#include "../../utils/handle.hpp"
#include "../../math/algebra/vmath.hpp"
#include "../../graphics/font/font.hpp"
#include "../../resource/font-loader.hpp"
#include "../../graphics/font/font-creator.hpp"
#include "../../graphics/gui/uid.hpp"

namespace kege::ui{

    class Elem;
    class Input;
    class Layout;
    class Viewer;
    class Cursor;
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
        Independent,
    };

    enum struct AlignDir: uint8_t
    {
        HORIZONTAL,
        VERTICAL,
    };

    enum struct AlignDirX : uint8_t
    {
        ETW,
        WTE,
    };
    enum struct AlignDirY : uint8_t
    {
        NTS,
        STN,
    };

    enum struct AlignPosX: uint8_t
    {
        LEFT,
        RIGHT,
        CENTER,
    };

    enum struct AlignPosY: uint8_t
    {
        TOP,
        BOTTOM,
        CENTER,
    };

    struct AlignXY
    {
        AlignPosX x = AlignPosX::LEFT;
        AlignPosY y = AlignPosY::TOP;
    };

    struct AlignFlow
    {
        AlignDirX x = AlignDirX::WTE;
        AlignDirY y = AlignDirY::NTS;
    };

    struct Alignment
    {
        AlignFlow flow;
        AlignXY   origin;
        AlignXY   content;
        AlignDir  direction = AlignDir::HORIZONTAL;
        bool wrap_around = false;
    };

    typedef enum : uint8_t
    {
        SIZE_UNDEFINED,
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

    enum struct HoverTrigger: uint8_t
    {
        Disable,
        OnEnter,
        OnExit,
    };

    enum struct ClickTrigger: uint8_t
    {
        Disable,
        Continuous, // Action executes every frame while held
        Immediate, // Action triggers immediately when button goes down
        OnRelease, // Action triggers when mouse is released over button
                   // - Mouse was pressed DOWN over button
                   // - Mouse is released UP over button
                   // - Mouse cursor still inside button bounds
    };

    typedef std::function< void( kege::ui::Layout&, uint32_t ) > Callback;

    struct Text
    {
        kege::string text;
        float x, y;
        float width;
        float height;
    };

    struct TexrID
    {
        uint32_t id;
        float x, y;
        float width;
        float height;
    };
    

    struct Coord
    {
        float x = 0.f;
        float y = 0.f;
    };
    struct Extent
    {
        float width = 0.f;
        float height = 0.f;
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

    struct Background
    {
        Background(uint32_t img_index, const ui::Rect& texel);
        Background(const ui::Color& color);
        Background(uint32_t color);
        Background(){}
        union
        {
            Rect  texel;
            Color color;
        };
        uint32_t id;
    };

    struct TextFieldState
    {
        size_t cursor_pos = 0;
        size_t selection_start = 0;
        size_t selection_end = 0;
        bool has_focus = false;
        bool is_selected = false;
    };

    // shareable style that can be shared across many ui-content
    struct Style
    {
        Background  background;
        Color       color;
        Sizing      width;
        Sizing      height;
        Sizing      min_width;
        Sizing      min_height;
        Sizing      max_width;
        Sizing      max_height;
        Corners     border_radius;

        Padding     padding;
        Alignment   align;
        Extent2D    gap;
        Positioning position = Positioning::Relative;
        AlignText   align_text = AlignText::Left;
        int         font_size = 20;
        int32_t     zindex = 0;
        bool        clip_overflow = false;
        bool        wrap_around = false;
    };

    // Widget contains the widget specific data that specific to a widget
    struct alignas(8) Desc
    {
        Coord offset;

        kege::UID* id = nullptr;

        Style* style = nullptr;

        const char* text = nullptr;

        TexrID texr;

        bool enabled = true;
        bool visible = true;

        ClickTrigger single_click = ui::ClickTrigger::Disable;
        ClickTrigger double_click = ui::ClickTrigger::Disable;
    };


    // Widget contains the widget specific data that specific to a widget
    struct Widget
    {
        Id id;
        Id elem_id;

        Style* style = nullptr;

        bool enabled = true;
        bool visible = true;

        ClickTrigger single_click = ui::ClickTrigger::Disable;
        ClickTrigger double_click = ui::ClickTrigger::Disable;

        /**
         * note rect is recomputed every frame, rect is the visual shape of the gui shape
         */
        Rect rect = {};
        Rect offset = {};

        /**
         * text is the xy position of a text and the width and height that text span
         */
        mutable Text text = {};

        TexrID texr = {};
        uint32_t version    = 0;
        int32_t parent      = 0;
        int32_t head        = 0;
        int32_t tail        = 0;
        int32_t next        = 0;
        int32_t count       = 0;
    };

    struct Link
    {
        int32_t node_index = 0;
        int32_t next       = 0;
        int32_t prev       = 0;
    };
    
    struct DrawElem
    {
        ui::Rect rect;
        ui::Color color;
        ui::Rect texel;
        ui::Rect clip_rect;
        struct
        {
            float border_radius;
            float texture_id;
            float pad[2];
        };
    };
    typedef std::vector< ui::DrawElem > RenderData;

    ui::Sizing fixed(float size);
    ui::Sizing extend();
    ui::Sizing flexible();
    ui::Sizing percent(float percent);

    ui::Color rgb(uint32_t hex_color);
    ui::Color rgba(uint32_t hex_color);

    //ui::Background bgImage(int img_index, const ui::Rect& texel);
    //ui::Background bgColor(const ui::Color& color);
    //ui::Background bgColor(uint32_t color);


    struct HitRecord
    {
        Id hot     = {};
        Id active  = {};
        Id focus  = {};

        uint8_t  clicks     = 0;
        bool     release    = false;
    };

    struct AllocParam
    {
        size_t index;
        size_t size;
    };

    struct DeferredOp
    {
        typedef void (*Fn)(Layout* layout, const kege::UID& id, void* data);
        DeferredOp::Fn fn;
        AllocParam alloc;
        const kege::UID* id;
    };
}

#endif /* ui_primitives_hpp */
