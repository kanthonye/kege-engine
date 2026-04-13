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

#define KEGE_MAKE_ID() ((((uint64_t)(uintptr_t)__FILE__) << 32) | __LINE__)

#define ID_FILE_BITS     32
#define ID_LINE_BITS     16
#define ID_CHILD_BITS    16

#define ID_FILE_SHIFT    (ID_LINE_BITS + ID_CHILD_BITS)  // 32
#define ID_LINE_SHIFT    ID_CHILD_BITS                       // 16
#define ID_CHILD_SHIFT   0

#define ID_FILE_MASK     ((1ULL << ID_FILE_BITS) - 1)
#define ID_LINE_MASK     ((1ULL << ID_LINE_BITS) - 1)
#define ID_CHILD_MASK    ((1ULL << ID_CHILD_BITS) - 1)

// Generate base ID (without child index)
#define BASE_ID() \
    ((((uint64_t)(uintptr_t)__FILE__ & ID_FILE_MASK) << ID_FILE_SHIFT) | \
     ((__LINE__ & ID_LINE_MASK) << ID_LINE_SHIFT))


namespace kege::ui{

    class Input;
    class Layout;
    class Viewer;
    class Cursor;

    union WidgetId
    {
        WidgetId(uint32_t i,uint32_t v): index(i), version(v) {}
        WidgetId(): id(0) {}
        struct
        {
            uint32_t index;
            uint32_t version;
        };
        uint64_t id;
    };
    constexpr inline bool operator==(const WidgetId &a, const WidgetId &b) { return a.id == b.id; }
    constexpr inline bool operator!=(const WidgetId &a, const WidgetId &b) { return a.id != b.id; }

    struct WidgetHandle
    {
        //private:
        ui::ID user_id;
        WidgetId widget_id;
        friend class Layout;
    };

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

    enum struct AlignType: uint8_t
    {
        LIST,
        GRID,
    };

    // X axis alignment options
    enum struct AlignX: uint8_t
    {
        LEFT,
        RIGHT,
        CENTER,
    };

    // Y axis alignment options
    enum struct AlignY: uint8_t
    {
        TOP,
        BOTTOM,
        CENTER,
    };

    /**
     * XY Alignment structure
     */
    struct AlignPos // 2 byte
    {
        AlignX x = AlignX::LEFT;
        AlignY y = AlignY::TOP;
    };

    /**
     * Alignment direction
     */
    enum struct AlignDir: uint8_t
    {
        LEFT,
        RIGHT,
        UP,
        DOWN,
        CENTER,
        CENTER_X,
        CENTER_Y,
    };

    struct AlignWrap
    {
        bool enable = false;
        AlignDir direction;
    };

    enum struct AlignItem
    {
        START,
        CENTER,
        END,
    };

    /**
     * Alignment structure
     */
    struct alignas(8) Alignment
    {
        AlignType type;
        AlignPos  origin; // 2 byte
        AlignPos  content; // 2 byte
        AlignDir  direction = AlignDir::RIGHT; // 1 byte
        AlignItem items = AlignItem::START;
        AlignWrap wrap; // 2 byte
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

   struct alignas(4) Gap
   {
       uint16_t width = 0;
       uint16_t height = 0;
   };

    /**
     * Rect structure defines a rectangle with position and size.
     */
    struct Rect
    {
        float x = 0.0, y = 0.0;
        float width = 0.0;
        float height = 0.0;
    };

    /**
     * CornerCurves structure defines the radius for each corner of a rectangle.
     */
    struct CornerCurves
    {
        ~CornerCurves() = default;
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
        const char* ptr = nullptr;   // 8 bytes
        float x = 0.f;
        float y = 0.f;
        float width = 0.f;
        float height = 20.f;
        uint16_t font_size = 20;
        uint32_t color = 0xFFFFFFFF;
        AlignText align = AlignText::Left;
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

        Alignment   alignment;
        AlignText   align_text;

        Padding     padding;
        Gap         gap;

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
        ui::ID user_id;

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
        const Style* style = nullptr; // 8 byte

        /**
         * align: hold the alignment for this ui element
         */
        Alignment alignment;

        /**
         * color is a hex color value holding the rgba value for each color channel.
         note the value is unpacked on the GPU to make a vec4
         */
        uint32_t color; // 4 byte


        ui::Rect texel;

        /**
         * texr_info hold the texture information for this ui element
         */
        TexrInfo texr_info; // 2 short = 4 bytes

        Gap gap;

        Sizing width;
        Sizing height;

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
        bool inactive = false;

        /**
         * enabled: hold the enabled state of the widget
         */
        bool mouseover = true;

        /**
         * visible: hold the visible state of the widget
         */
        bool visible = true;

        /**
         * clip_overflow: hold the clip overflow state of the widget
         */
        bool clip_overflow = false;
    };





    struct Root
    {
        int32_t head        = 0;
        int32_t tail        = 0;
        int32_t count       = 0;

        int32_t curr_parent = 0;
        int32_t id = 0;
    };

    struct LayerNode
    {
        int32_t next        = 0;
        int32_t prev        = 0;

        /**
         * layer: hold the layer of this ui element
         */
        uint32_t layer = 0;  // 4 byte
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
        ui::ID user_id;
        uint32_t index = 0;
        uint32_t version = 0;

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

        Sizing width;
        Sizing height;

        /**
         * style: hold the style pointer for this ui element
         */
        const Style* style = nullptr; // 8 byte

        /**
         * color is a hex color value holding the rgba value for each color channel.
         note the value is unpacked on the GPU to make a vec4
         */
        uint32_t color; // 4 byte

        /**
         * texr_info hold the texture information for this ui element
         */
        TexrInfo texr_info; // 2 short = 4 bytes


        Gap gap;
        
        /**
         * layer: hold the layer of this ui element
         */
        LayerNode layer;

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
         * inactive: hold the inactive state of the widget
         */
        bool inactive = false;

        /**
         * enabled: hold the mouseover state of the widget
         */
        bool mouseover = true;

        /**
         * visible: hold the visible state of the widget
         */
        bool visible = true;

        /**
         * clip_overflow: hold the clip overflow state of the widget
         */
        bool clip_overflow = false;
    };



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



    enum UILayer
    {
        LAYER_BASE,
        LAYER_BASE_OVERLAY,
        LAYER_WINDOW,
        LAYER_WINDOW_OVERLAY,
        LAYER_DRAGGING,
        LAYER_DRAGGING_OVERLAY,
        LAYER_MAX_COUNT,
    };

    struct Layer
    {
        void pushIfRoot(Widget& widget)
        {
            if( current_root != 0 ) return;

            current_root = widget.index;

            if ( head == 0 )
            {
                tail = head = widget.index;
                widget.layer.next = 0;
                widget.layer.prev = 0;
            }
            else
            {
                widget.layer.next = 0;
                widget.layer.prev = tail;
                widget.layer.next = widget.index;
                tail = widget.index;
            }

            count++;
        }
        void popIfRoot(std::vector< kege::ui::Widget >& widgets, uint32_t index)
        {
            if ( current_root == index )
            {
                if( widgets[ index ].parent )
                {
                    current_root = widgets[ widgets[ index ].parent ].index;
                    current_root = 0;
                }
            }
        }

        void reset()
        {
            current_root = 0;
            count = 0;
            head = 0;
            tail = 0;
        }
        
        Layer()
        : current_root(0)
        , head(0)
        , tail(0)
        , count(0)
        {}

        //std::vector< uint32_t > roots;
        uint32_t current_root;
        uint32_t head;
        uint32_t tail;
        int32_t count;
    };
}

#endif /* ui_primitives_hpp */
