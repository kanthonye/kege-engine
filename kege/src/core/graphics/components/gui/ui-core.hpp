//
//  ui-primitives.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/2/25.
//

#ifndef ui_common_hpp
#define ui_common_hpp

#include <map>
#include <stdlib.h>
#include "../../../input/input-manager.hpp"
#include "../../../input/inputs.hpp"
#include "../../../utils/log.hpp"
#include "../../../utils/kege-string.hpp"
#include "../../../utils/handle.hpp"
#include "../../../math/algebra/vmath.hpp"
#include "../../../resource/font-loader.hpp"
#include "../../../graphics/components/font/font.hpp"
#include "../../../graphics/components/font/font-creator.hpp"
#include "../../../graphics/components/gui/uid.hpp"
#include "../font/font.hpp"

#define KEGE_OPT_STRUCT(Name, T) \
struct Name { \
    Name(T v) : value(v), has(true) {} \
    Name() : value(T{}), has(false) {} \
    T value{}; \
    bool has = false; \
}

namespace kege{
    class GUI;
}

namespace kege::ui{

    class Renderer;
    class Layer;
    class Layout;
    class Cursor;

    union NodeId
    {
        NodeId(uint32_t i,uint32_t v): index(i), version(v) {}
        NodeId(): id(0) {}
        struct
        {
            uint32_t index;
            uint32_t version;
        };
        uint64_t id;
    };
    constexpr inline bool operator==(const NodeId &a, const NodeId &b) { return a.id == b.id; }
    constexpr inline bool operator!=(const NodeId &a, const NodeId &b) { return a.id != b.id; }

    struct WidgetHandle
    {
        //private:
        ui::ID user_id;
        NodeId node_id;
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

   struct AlignDir // 2 byte
   {
       enum D: uint8_t
       {
           NILL,
           RIGHT,
           LEFT,
           DOWN,
           UP,
       };

       AlignDir::D x;
       AlignDir::D y;
   };

    enum struct AlignItem
    {
        START,
        CENTER,
        END,
    };

    struct alignas(4) AlignGap
    {
        uint16_t width = 0;
        uint16_t height = 0;
    };

    /**
     * Alignment structure
     */
    struct alignas(8) Alignment
    {
        AlignGap  gap;
        AlignType type;
        AlignPos  origin; // 2 byte
        AlignPos  content; // 2 byte
        AlignDir  direction = {AlignDir::RIGHT, AlignDir::DOWN};
        //AlignItem items = AlignItem::START;
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

    KEGE_OPT_STRUCT(Color, uint32_t);
    /**
     * Color structure defines RGBA color values.
     */
//    struct Color
//    {
//        Color(uint32_t c):value(c), has(true) {}
//        Color():value(0), has(false) {}
//        uint32_t value = 0;
//        bool has = false;
//    };

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

    struct Text // 34 bytes
    {
        float x = 0.f;
        float y = 0.f;
        float width = 0.f;
        uint32_t font_size = 20;
        uint32_t color = 0xFFFFFFFF;
        const char* data = nullptr;
        operator bool()const{return data != nullptr;}
    };

    struct Quad;
    struct Rect
    {
        float x = 0.0, y = 0.0;
        float width = 0.0;
        float height = 0.0;
        Rect& operator =(const kege::ui::Quad& rect);
    };

    struct Quad
    {
        float x = 0.0;
        float y = 0.0;
        float width = 0.0;
        float height = 0.0;
        uint32_t color = 0.0;
        Quad& operator =(const kege::ui::Rect& rect);
//        Quad(float x, float y, float width, float height, uint32_t color);
//        Quad(float x, float y, float width, float height);
//        Quad(const kege::ui::Rect& rect);
//        Quad(const kege::ui::Quad& quad);
//        Quad();
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
     * TexrId structure defines texture information for a UI element.
     */
    union TexrId
    {
        struct
        {
            uint16_t index = 0;
            uint16_t id = 0;
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
        Background(const kege::vec4& color);
        Background(uint32_t color);
        Background(){}

        Rect texel;
        TexrId info;
        uint32_t color;
        BackgroundType type;
    };




//    // shareable style that can be shared across many ui-content
//    struct alignas(16) Style
//    {
//        Background  background;
//        uint32_t    text_color;
//
//        Sizing      width;
//        Sizing      height;
//        //Sizing      min_width;
//        //Sizing      min_height;
//        //Sizing      max_width;
//        //Sizing      max_height;
//
//        Border      border;
//
//        /**
//         * corner_curves: hold the radius for each corner of the rectangle
//         */
//        ui::CornerCurves corner_curves;
//
//        Alignment   alignment;
//        AlignText   align_text;
//
//        Padding     padding;
//        Gap         gap;
//
//        Positioning position = Positioning::Relative;
//
//        int32_t     zindex = 0;
//        uint32_t     font_size = 0;
//        bool        clip_overflow = false;
//    };

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
        kege::ui::ID user_id;

        /**
         * rect: hold the position and size of the ui element
         */
        kege::ui::Rect rect;  // 16 bytes

        /**
         * texel: hold the uv coordinates for the texture
         */
        kege::ui::Border border; // 16 bytes

        /**
         * corner_curves: hold the radius for each corner of the rectangle
         */
        kege::ui::CornerCurves corner_curves;  // 4 shorts = 8 bytes

        /**
         * text: is a data structure that hold text rendering infor
         */
        kege::ui::Text text; // 34 bytes

        /**
         * padding: hold the padding for this ui element
         */
        kege::ui::Padding padding;// 8 bytes

        /**
         * style: hold the style pointer for this ui element
         */
        //const Style* style = nullptr; // 8 byte

        /**
         * align: hold the alignment for this ui element
         */
        kege::ui::Alignment alignment;

        /**
         * color is a hex color value holding the rgba value for each color channel.
         note the value is unpacked on the GPU to make a vec4
         */
        kege::ui::Color color; // 4 byte

        bool text_wrap = false;

        kege::ui::Rect texel;

        /**
         * texr_info hold the texture information for this ui element
         */
        TexrId texr_info; // 2 short = 4 bytes

        kege::ui::Sizing width;
        kege::ui::Sizing height;

        /**
         * single_click: hold the single click trigger type
         */
        kege::ui::ClickTrigger single_click = ui::ClickTrigger::Disable;

        /**
         * double_click: hold the double click trigger type
         */
        kege::ui::ClickTrigger double_click = ui::ClickTrigger::Disable;

        /**
         * position: hold the positioning type of the widget
         */
        kege::ui::Positioning position = Positioning::Relative;

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





    struct LayerNode
    {
        int32_t next        = 0;
        int32_t prev        = 0;
        int32_t head        = 0;
        int32_t tail        = 0;

       uint32_t parent = 0;

        /**
         * layer: hold the layer of this ui element
         */
        uint32_t layer = 0;
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
        //const Style* style = nullptr; // 8 byte

        /**
         * color is a hex color value holding the rgba value for each color channel.
         note the value is unpacked on the GPU to make a vec4
         */
        Color color; // 4 byte

        /**
         * texr_info hold the texture information for this ui element
         */
        TexrId texr_info; // 2 short = 4 bytes

        /**
         * layer: hold the layer of this ui element
         */
        LayerNode layer;

        /**
         * doubly linked list pointers and parent/child relationship
         */
        int32_t head = 0;

        /**
         * tail: hold the last child widget index
         */
        int32_t tail = 0;

        /**
         * next: hold the next sibling widget index
         */
        int32_t next = 0;

        /**
         * prev: hold the previous sibling widget index
         */
        int32_t prev = 0;

        /**
         * parent: hold the parent widget index
         */
        int32_t parent = 0;

        /**
         * count: hold the number of child widgets
         */
        int32_t count = 0;

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

    kege::vec3 rgb(uint32_t hex_color);
    kege::vec4 rgba(uint32_t hex_color);

    uint32_t packRGBA8(float r, float g, float b, float a);
    inline uint32_t packRGBA8(const kege::vec4& colr){ return packRGBA8(colr.x, colr.y, colr.z, colr.w); }
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

    struct Record //Handle
    {
        uint64_t user_id = 0;
        uint32_t index = 0;
    };

    struct HitRecord
    {
        /**
         * The "pressing" record represents the UI element that is currently being pressed on by
         * the user. It is used to track the element that is actively being interacted with, allowing
         * the system to respond to user input (e.g., processing click on release).
         */
        Record pressing;

        /**
         * The "focus" record represents the UI element that currently has focus, meaning it is the
         * active element that can receive input from the user. It is used to track which element is
         * currently selected or active, allowing the system to direct user input to the appropriate
         * element (e.g., keyboard input).
         */
        Record focus;

        /**
         * The "hit" record represents the UI element that is currently being interacted with,
         * such as the one that is clicked on. It is used to track the element that is currently
         * active or targeted by user input, allowing the system to respond accordingly
         * (e.g., highlighting, triggering events, etc.).
         */
        Record hit;

        /**
         * The "hot" record represents the UI element that is currently under the mouse cursor
         * or being interacted with. It is used to track which element is active or highlighted
         * based on user input, such as mouse movement or clicks.
         */
        Record hot;

        /**
         * The "clicks" field represents the number of clicks that have occurred on a UI element.
         * It is used to track the number of times a user has clicked on a specific element, allowing
         * the system to respond to single clicks, double clicks, or multiple clicks as needed
         * (e.g., triggering different actions based on the number of clicks).
         */
        uint8_t  clicks = 0;
    };

    bool testPointVsRect( const kege::dvec2& p, const kege::ui::Rect& rect );
    bool testPointVsRect( const kege::dvec2& p, const kege::ui::Quad& rect );

    bool checkOverlap(const kege::ui::Rect& a, const kege::ui::Rect& b);
    bool checkOverlap(const kege::ui::Quad& a, const kege::ui::Quad& b);
    bool checkOverlap(const kege::ui::Quad& q, const kege::ui::Rect& r);

    inline bool testPointVsRect( const kege::ui::Rect& rect, const kege::dvec2& p )
    {
        return testPointVsRect(p, rect);
    }

    inline bool testPointVsRect( const kege::ui::Quad& quad, const kege::dvec2& p )
    {
        return testPointVsRect(p, quad);
    }
    
    inline bool checkOverlap(const kege::ui::Rect& r, const kege::ui::Quad& q)
    {
        return checkOverlap(q, r);
    }








    struct NID
    {
        private: uint32_t index;
        friend kege::GUI;
    };

    struct WID
    {
        uint32_t index = 0;
    };

    /**
     * WidgetDesc contains the description data for a widget
     * used during creation and setup of a widget
     * aligned to 16 bytes for optimal memory access
     */
    struct Style
    {
        /**
         * position: hold the positioning type of the widget
         */
        kege::ui::Positioning position = Positioning::Relative;

        kege::ui::Sizing width;
        kege::ui::Sizing height;
        
        /**
         * color is a hex color value holding the rgba value for each color channel.
         note the value is unpacked on the GPU to make a vec4
         */
        Color color;

        /**
         * padding: hold the padding for this ui element
         */
        kege::ui::Padding padding;// 8 bytes

        /**
         * align: hold the alignment for this ui element
         */
        kege::ui::Alignment alignment;

        kege::ui::CornerCurves corner_curves;

        /**
         * texel: hold the uv coordinates for the texture
         */
        kege::ui::Border border; // 16 bytes

        /**
         * text: is a data structure that hold text rendering infor
         */
        uint32_t font_size = 20;
        kege::ui::AlignText text_align = AlignText::Left;

        kege::ui::Rect texr_info;

        /**
         * texr_info hold the texture information for this ui element
         */
        kege::ui::TexrId texr_id; // 2 short = 4 bytes
    };

//    struct ElemDesc
//    {
//        const kege::ui::Style* style;
//        kege::ui::Rect rect;
//        const char* text;
//    };

    struct Elem
    {
        /**
         * position: hold the positioning type of the widget
         */
        kege::ui::Positioning position = Positioning::Relative;

        float x, y;
        kege::ui::Sizing width;
        kege::ui::Sizing height;

        /**
         * color is a hex color value holding the rgba value for each color channel.
         note the value is unpacked on the GPU to make a vec4
         */
        kege::ui::Color quad_color;
        kege::ui::Color text_color;

        /**
         * padding: hold the padding for this ui element
         */
        kege::ui::Padding padding;// 8 bytes

        /**
         * align: hold the alignment for this ui element
         */
        kege::ui::Alignment alignment;

        /**
         * corner_curves: hold the value for the corner curvature
         */
        kege::ui::CornerCurves corner_curves;

        /**
         * texel: hold the uv coordinates for the texture
         */
        kege::ui::Border border; // 16 bytes

        /**
         * text: is a data structure that hold text rendering infor
         */
        uint32_t font_size;
        kege::ui::AlignText text_align;
        bool text_wrap = false;

        kege::ui::Rect texr_info;

        /**
         * texr_info hold the texture information for this ui element
         */
        kege::ui::TexrId texr_id; // 2 short = 4 bytes

        kege::ui::WID wid;
    };

    struct NodeDesc
    {
        kege::ui::ID user_id;
        kege::ui::WID wid;
        
        kege::ui::Quad quad;
        kege::ui::Text text;

        /**
         * single_click: hold the single click trigger type
         */
        kege::ui::ClickTrigger single_click = ui::ClickTrigger::Disable;

        /**
         * double_click: hold the double click trigger type
         */
        kege::ui::ClickTrigger double_click = ui::ClickTrigger::Disable;

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
         * selectable: hold the selectable state of the widget
         */
        bool selectable = false;

        /**
         * clip_overflow: hold the clip overflow state of the widget
         */
        bool clip_overflow = false;

        const char* name = nullptr;
    };

//    struct Link
//    {
//        /**
//         * doubly linked list pointers and parent/child relationship
//         */
//        int32_t head = 0;
//
//        /**
//         * tail: hold the last child widget index
//         */
//        int32_t tail = 0;
//
//        /**
//         * next: hold the next sibling widget index
//         */
//        int32_t next = 0;
//
//        /**
//         * prev: hold the previous sibling widget index
//         */
//        int32_t prev = 0;
//
//        /**
//         * count: hold the number of child widgets
//         */
//        int32_t count = 0;
//    };

   struct Node
   {
       ui::ID user_id;
       ui::WID wid;

       kege::ui::Quad quad;
       kege::ui::Text text;

       /**
        * single_click: hold the single click trigger type
        */
       ClickTrigger single_click = ui::ClickTrigger::Disable;

       /**
        * double_click: hold the double click trigger type
        */
       ClickTrigger double_click = ui::ClickTrigger::Disable;

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
        * selectable: hold the selectable state of the widget
        */
       bool selectable = false;

       /**
        * clip_overflow: hold the clip overflow state of the widget
        */
       bool clip_overflow = false;

       /**
        * doubly linked list pointers and parent/child relationship
        */
       int32_t head = 0;

       /**
        * tail: hold the last child widget index
        */
       int32_t tail = 0;

       /**
        * next: hold the next sibling widget index
        */
       int32_t next = 0;

       /**
        * prev: hold the previous sibling widget index
        */
       int32_t prev = 0;

       /**
        * parent: hold the parent widget index
        */
       int32_t parent = 0;

       /**
        * count: hold the number of child widgets
        */
       int32_t count = 0;

       /**
        * layer: hold the layer of this ui element
        */
       ui::LayerNode layer;

       uint32_t index = 0;
       uint32_t version = 0;
       const char* name;
   };

}

namespace kege::ui{

    /**
     * DrawInstance hold the data required to draw a single ui element
     */
    struct DrawInstance
    {
        /**
         * Border structure defines the border properties of a UI element.
         */
        struct alignas(16) DIBorder
        {
            CornerCurves radius;

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
         * rect: hold the position and size of the ui element
         */
        kege::ui::Rect rect;  // 16 bytes

        /**
         * texel: hold the uv coordinates for the texture
         */
        kege::ui::Rect texel;  // 16 bytes

        /**
         * corner_curves: hold the radius for each corner of the rectangle
         */
        DIBorder border;  // 16 bytes

        /**
         * clip_rect: hold the clipping rectangle for this ui element
         */
        kege::ui::Rect clip_rect;  // 16 bytes

        struct // 16 bytes
        {
            /**
             * color is a hex color value holding the rgba value for each color channel.
             note the value is unpacked on the GPU to make a vec4
             */
            uint32_t color;

            /**
             * texr_info hold the texture information for this ui element
             */
            TexrId texr_info;

            float pad[2];
        };
    };

    struct DrawBatch
    {
        uint32_t first_element;
        uint32_t element_count;
        uint32_t first_instance;
        uint32_t instance_count;

        uint32_t push_block_size = 0;
        uint32_t push_block_offset = 0;
        ShaderStageFlag push_block_stages;

        kege::mat44 transform;
    };

    struct DrawRecord
    {
        uint64_t pipeline_id;
        uint64_t material_id;
        uint64_t mesh_id;

        kege::mat44 projection;
        kege::vec4 screen_info;
        std::vector< kege::ui::DrawBatch > batches;
    };
}


#endif /* ui_common_hpp */
