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

    class Input;
    class Layout;
    class Viewer;
    class Cursor;

    /**
     * Text alignment options.
     */
    enum struct AlignText : uint8_t
    {
        Center, Left, Right,
    };

    /**
     * Positioning options for UI elements.
     */
    enum struct Positioning : uint8_t
    {
        Relative,
        Absolute,
        Independent,
    };

    // X axis alignment options
    enum struct AlignPosX: uint8_t
    {
        LEFT,
        RIGHT,
        CENTER,
    };

    // Y axis alignment options
    enum struct AlignPosY: uint8_t
    {
        TOP,
        BOTTOM,
        CENTER,
    };

    /**
     * XY Alignment structure
     */
    struct AlignXY // 2 byte
    {
        AlignPosX x = AlignPosX::LEFT;
        AlignPosY y = AlignPosY::TOP;
    };

    /**
     * Alignment flow direction
     */
    enum struct AlignDirX : uint8_t
    {
        ETW,
        WTE,
    };

    /**
     * Alignment flow direction
     */
    enum struct AlignDirY : uint8_t
    {
        NTS,
        STN,
    };

    /**
     * Alignment flow structure
     */
    struct AlignFlow // 2 byte
    {
        AlignDirX x = AlignDirX::WTE;
        AlignDirY y = AlignDirY::NTS;
    };

    /**
     * Alignment direction
     */
    enum struct AlignDir: uint8_t
    {
        HORIZONTAL,
        VERTICAL,
    };

    /**
     * Alignment structure
     */
    struct alignas(8) Alignment
    {
        AlignFlow flow; // 2 byte
        AlignXY   origin; // 2 byte
        AlignXY   content; // 2 byte
        AlignDir  direction = AlignDir::HORIZONTAL; // 1 byte
        bool wrap_around = false; // 1 byte
    };

    /**
     * SizingType defines the different types of sizing for UI elements.
     */
    enum struct SizingType: uint8_t
    {
        None,
        Fixed,
        Percent,
        Flexible,
        Extend,
    };

    /**
     * Sizing structure defines the size and type of sizing for UI elements.
     */
    struct Sizing
    {
        float size = 0.f;
        SizingType type = SizingType::None;
    };

    /**
     * Padding structure defines the padding values for UI elements.
     */
    struct Padding
    {
        uint16_t left;
        uint16_t above;
        uint16_t right;
        uint16_t below;
    };

    /**
     * Color structure defines RGBA color values.
     */
    struct Color
    {
        float r = 1.f;
        float g = 1.f;
        float b = 1.f;
        float a = 1.f;
    };

    /**
     * HoverTrigger defines the different types of hover triggers for UI elements.
     */
    enum struct HoverTrigger: uint8_t
    {
        Disable,
        OnEnter,
        OnExit,
    };

    /**
     * ClickTrigger defines the different types of click triggers for UI elements.
     */
    enum struct ClickTrigger: uint8_t
    {
        Disable,

        /**
         * Action executes every frame while held
         */
        Continuous,

        /**
         * Action triggers immediately when button goes down
         */
        Immediate,

        /**
         * Action triggers when mouse is released over button
         * - Mouse was pressed DOWN over button
         * - Mouse is released UP over button
         * - Mouse cursor still inside button bounds
         */
        OnRelease,
    };

    /**
     * Coord structure defines a 2D coordinate.
     */
    struct alignas(8) Coord
    {
        float x = 0.f;
        float y = 0.f;
    };

    /**
     * Extent structure defines width and height dimensions.
     */
    struct alignas(8) Extent
    {
        float width = 0.f;
        float height = 0.f;
    };

    /**
     * Rect structure defines a rectangle with position and size.
     */
    struct Rect
    {
        float x, y;
        float width;
        float height;
    };

    /**
     * CornerCurves structure defines the radius for each corner of a rectangle.
     */
    struct CornerCurves
    {
        uint16_t top_left = 0;
        uint16_t top_right = 0;
        uint16_t bottom_left = 0;
        uint16_t bottom_right = 0;
    };

    /**
     * Border structure defines the border properties of a UI element.
     */
    struct alignas(16) Border
    {
        /**
         * corner_curves: hold the radius for each corner of the rectangle
         */
        ui::CornerCurves corner_curves;  // 4 shorts = 8 bytes

        /**
         border_color hold the border color as a hex value.
         note the value is unpacked on the GPU to make a vec4
         */
        uint32_t color;

        /**
         * border_width hold the width of the border in pixels
         */
        uint32_t width;
    };

    /**
     * TexrInfo structure defines texture information for a UI element.
     */
    union TexrInfo
    {
        struct
        {
            uint16_t index;
            uint16_t id;
        };
        uint32_t value;
    };


    enum struct BackgroundType : uint8_t {COLOR, IMAGE};

    /**
     * Background structure defines the background properties of a UI element.
     */
    struct alignas(16) Background
    {
        enum Type{COLOR, IMAGE};

        Background(uint32_t img_index, const ui::Rect& texel);
        Background(const ui::Color& color);
        Background(uint32_t color);
        Background(){}

        Rect texel;
        TexrInfo info;
        uint32_t color;
        BackgroundType type;
    };


    struct alignas(8) Text // 34 bytes
    {
        Text(const char* text, bool modified = false)
        :   ptr( text )
        ,   width(0.f)
        ,   height(0.f)
        ,   x(0.f)
        ,   y(0.f)
        ,   color(0xFFFFFFFF)
        ,   size( 20 )
        ,   align( AlignText::Left )
        ,   modified( modified )
        {}

        Text()
        :   ptr( nullptr )
        ,   width(0.f)
        ,   height(0.f)
        ,   x(0.f)
        ,   y(0.f)
        ,   color(0xFFFFFFFF)
        ,   size( 20 )
        ,   align( AlignText::Left )
        ,   modified( false )
        {}

        const char* ptr;   // 8 bytes
        size_t length; // 8 bytes

        struct // 8 bytes
        {
            float width = 0.f;
            float height = 0.f;
            float x = 0.f;
            float y = 0.f;
        };

        struct // 8 bytes
        {
            uint32_t color;
            uint16_t size;
            AlignText align;
            bool modified;
        };
    };



    // shareable style that can be shared across many ui-content
    struct alignas(16) Style
    {
        Background  background;
        uint32_t    text_color;

        Sizing      width;
        Sizing      height;
        //Sizing      min_width;
        //Sizing      min_height;
        //Sizing      max_width;
        //Sizing      max_height;

        Border      border;

        Alignment   align;
        AlignText   align_text;

        Padding     padding;
        Extent      gap;

        Positioning position = Positioning::Relative;

        int32_t     zindex = 0;
        int32_t     font_size = 0;
        bool        clip_overflow = false;
    };

    /**
     * WidgetDesc contains the description data for a widget
     * used during creation and setup of a widget
     * aligned to 16 bytes for optimal memory access
     */
    struct alignas(16) WidgetDesc
    {
        /**
         * id: The unique identifier for this widget.
         */
        ui::UID* uid; // 16 bytes

        /**
         * rect: hold the position and size of the ui element
         */
        ui::Rect rect;  // 16 bytes

        /**
         * texel: hold the uv coordinates for the texture
         */
        ui::Border border; // 16 bytes

        /**
         * text: is a data structure that hold text rendering infor
         */
        Text text; // 34 bytes

        /**
         * padding: hold the padding for this ui element
         */
        Padding padding;// 8 bytes

        /**
         * style: hold the style pointer for this ui element
         */
        Style* style = nullptr; // 8 byte

        /**
         * align: hold the alignment for this ui element
         */
        Alignment alignment;

        /**
         * color is a hex color value holding the rgba value for each color channel.
         note the value is unpacked on the GPU to make a vec4
         */
        uint32_t color; // 4 byte

        /**
         * texr_info hold the texture information for this ui element
         */
        TexrInfo texr_info; // 2 short = 4 bytes

        /**
         * layer: hold the layer of this ui element
         */
        uint16_t layer = 0;

        /**
         * single_click: hold the single click trigger type
         */
        ClickTrigger single_click = ui::ClickTrigger::Disable;

        /**
         * double_click: hold the double click trigger type
         */
        ClickTrigger double_click = ui::ClickTrigger::Disable;

        /**
         * position: hold the positioning type of the widget
         */
        Positioning position = Positioning::Relative;
        
        /**
         * enabled: hold the enabled state of the widget
         */
        bool enabled = true;

        /**
         * visible: hold the visible state of the widget
         */
        bool visible = true;


        //ui::Rect offset;
    };


    /**
     * Widget contains the widget specific data that specific to a widget
     * aligned to 16 bytes for optimal memory access
     */
    struct alignas(16) Widget
    {
        /**
         * id: The unique identifier for this widget.
         */
        ui::EID id; // 16 bytes

        /**
         * rect: hold the position and size of the ui element
         */
        ui::Rect rect;  // 4 float = 16 bytes

        /**
         * texel: hold the uv coordinates for the texture
         */
        ui::Rect texel;  // 4 float = 16 bytes

        /**
         * border: hold the border information for this ui element
         */
        ui::Border border; // 16 bytes

        /**
         * text: is a data structure that hold text rendering infor
         */
        Text text;

        /**
         * padding: hold the padding for this ui element
         */
        Padding padding;// 8 bytes

        /**
         * align: hold the alignment for this ui element
         */
        Alignment alignment;

        /**
         * style: hold the style pointer for this ui element
         */
        Style* style = nullptr; // 8 byte

        /**
         * color is a hex color value holding the rgba value for each color channel.
         note the value is unpacked on the GPU to make a vec4
         */
        uint32_t color; // 4 byte

        /**
         * texr_info hold the texture information for this ui element
         */
        TexrInfo texr_info; // 2 short = 4 bytes

        /**
         * layer: hold the layer of this ui element
         */
        //uint32_t version = 0;
        
        /**
         * layer: hold the layer of this ui element
         */
        uint32_t layer = 0;  // 4 byte

        /**
         * doubly linked list pointers and parent/child relationship
         */
        int32_t head        = 0;
        
        /**
         * tail: hold the last child widget index
         */
        int32_t tail        = 0;

        /**
         * next: hold the next sibling widget index
         */
        int32_t next        = 0;

        /**
         * prev: hold the previous sibling widget index
         */
        int32_t prev        = 0;

        /**
         * parent: hold the parent widget index
         */
        int32_t parent      = 0;

        /**
         * count: hold the number of child widgets
         */
        int32_t count       = 0;

        /**
         * font_size: hold the font size for text rendering
         */
        float font_size = 20;
        
        /**
         * single_click: hold the single click trigger type
         */
        ClickTrigger single_click = ui::ClickTrigger::Disable;

        /**
         * double_click: hold the double click trigger type
         */
        ClickTrigger double_click = ui::ClickTrigger::Disable;

        /**
         * position: hold the positioning type of the widget
         */
        Positioning position = Positioning::Relative;
        
        /**
         * enabled: hold the enabled state of the widget
         */
        bool enabled = true;

        /**
         * visible: hold the visible state of the widget
         */
        bool visible = true;

        /**
         * clip_overflow: hold the clip overflow state of the widget
         */
        bool clip_overflow = false;
    };



//
//    // Widget contains the widget specific data that specific to a widget
//    struct Widget
//    {
//        Id id;
//        Id elem_id;
//
//        Color color = {};
//        Rect rect = {};
//        Rect offset = {};
//
//        Style* style = nullptr;
//
//        bool enabled = true;
//        bool visible = true;
//
//        ClickTrigger single_click = ui::ClickTrigger::Disable;
//        ClickTrigger double_click = ui::ClickTrigger::Disable;
//
//        /**
//         * text is the xy position of a text and the width and height that text span
//         */
//        mutable Text text = {};
//
//        int16_t layer = 0;
//        
//        TexrID texr = {};
//
//        uint32_t version    = 0;
//        int32_t parent      = 0;
//
//        int32_t head        = 0;
//        int32_t tail        = 0;
//        int32_t next        = 0;
//        int32_t count       = 0;
//    };



    ui::Sizing fixed(float size);
    ui::Sizing extend();
    ui::Sizing flexible();
    ui::Sizing percent(float percent);

    ui::Color rgb(uint32_t hex_color);
    ui::Color rgba(uint32_t hex_color);

    uint32_t packRGBA8(float r, float g, float b, float a);
    inline uint32_t packRGBA8(const ui::Color& colr){ return packRGBA8(colr.r, colr.g, colr.b, colr.a); }
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
        typedef void (*Fn)(Layout* layout, const ui::UID& id, void* data);
        DeferredOp::Fn fn;
        AllocParam alloc;
        const ui::UID* id;
    };
}

#endif /* ui_primitives_hpp */
