//
//  ui-layout.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_layout_hpp
#define ui_layout_hpp

#include "../../input/input-manager.hpp"
#include "../font/font.hpp"
#include "ui-resizer.hpp"
#include "ui-aligner.hpp"
#include "ui-core.hpp"
#include "ui-cursor.hpp"
#include "ui-post-layout-ops.hpp"
#include "ui-style-manager.hpp"

namespace kege::ui{

    class Input;



    struct Record
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

    class Layout : public kege::RefCounter
    {
    private:

        enum { PREV, CURR, POST };

        struct State
        {
            uint32_t id = 0;
            uint32_t depth = 0;
            uint16_t clicks = 0;
            bool active;
        };

        struct InputState
        {
            bool  left_down = false;
            bool  left_pressed = false;
            bool  left_released = false;
            bool  double_click = false;
        };

    public:

        template<typename Params> void pushDeferredOp(ui::ID user_id, WidgetId widget_index, DeferredOperation fn, const Params& params)
        {
            _deferred_operations.push< Params >(user_id, widget_index, fn, params);
        }

        template<typename Params> void pushDeferredOpPtr(ui::ID user_id, WidgetId widget_index, DeferredOperation fn, Params* params)
        {
            _deferred_operations.pushPtr< Params >(user_id, widget_index, fn, params);
        }


        void onWindowResize(int width, int height);

        /**
         * Creates a parent UI element with the give info.
         *
         * @param desc This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        WidgetId pushRoot( const WidgetDesc& desc );
        WidgetId putRoot( const WidgetDesc& desc );

        /**
         * Pops the current parent UI element from the parent stack.
         */
        void popRoot();

        /**
         * Creates a parent UI element with the give info.
         *
         * @param desc This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        WidgetId push( const WidgetDesc& desc );

        /**
         * Pops the current parent UI element from the parent stack.
         *
         * @return The index of the popped UI element in the elements array.
         */
        uint32_t pop();

        /**
         * Creates a UI element with the give description.
         *
         * @param desc Description of the ui element
         *
         * @return reference to the ui element.
         */
        WidgetId put( const WidgetDesc& desc );


        uint32_t computeExtent( int font_size, const char* text, float& width, float& height );
        bool onNumericInput(const WidgetId& widget_id, char* str, size_t& size);
        bool onTextInput(const WidgetId& widget_id, char* str, size_t& size);


        /**
         * Checks if geven point and rectangular shape intersects.
         *
         * @param p The given point to test the rectangle against.
         * @param rect The rectangle to test the point against.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        bool testPointVsRect( const kege::dvec2& p, const ui::Rect& rect )const;

        inline bool intersect(const ui::Rect& m, const ui::Rect& n)const
        {
            return
            (
                m.x < n.x + n.width &&  // rect1 left edge is left of rect2 right edge
                m.x + m.width > n.x &&  // rect1 right edge is right of rect2 left edge
                m.y < n.y + n.height && // rect1 top edge is above rect2 bottom edge
                m.y + m.height > n.y    // rect1 bottom edge is below rect2 top edge
            );
        }

        Text text( const char* str, int font_size );

        kege::dvec2 getClickPosition( MouseButtonCode code ) const;

        /**
         * Retrieves the current position of the mouse pointer.
         *
         * @return The current position as a 2D vector.
         */
        kege::dvec2 getPointerPosition() const;

        /**
         * Retrieves the delta (change) in the mouse pointer's position.
         *
         * @return The delta position as a 2D vector.
         */
        kege::dvec2 getPointerDelta() const;

        /**
         * Retrieves the mouse scroll offset.
         *
         * @return The scroll offset as a 2D vector.
         */
        kege::dvec2 getScrollOffset() const;

        /**
         * Checks if the mouse pointer is being dragged.
         *
         * @return true if the pointer is being dragged, false otherwise.
         */
        bool isPointerDragging() const;

        /**
         * Checks if mouse pointer is over ui element.
         *
         * @param uid The ui element layout.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        bool mouseover( const ui::ID& uid ) const;

        /**
         * Checks if a ui-element that is associated with geven id was double clicked on.
         *
         * @param uid The given id.
         *
         * @return true if the element was double clicked on, false otherwise.
         */
        bool doubleClick( const ui::ID& uid ) const;

        /**
         * Checks if a ui-element that is associated with geven id was single clicked on.
         *
         * @param uid The given id.
         *
         * @return true if the ui-element was single clicked on, false otherwise.
         */
        bool click( const ui::ID& uid ) const;

        /**
         * Checks if a ui-element that is associated with geven id has focus.
         *
         * @param uid The given id.
         *
         * @return true if the ui-element has focus, false otherwise.
         */
        bool hasFocus( const ui::ID& uid )const;

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param uid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* elem( const WidgetId& uid ) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param uid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* elem( const WidgetId& uid );
        kege::ui::Widget* elemParent( const WidgetId& uid );

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* operator[](uint32_t index) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* operator[](uint32_t index);

        /**
         * Retrieves the parent index of a UI element.
         */
        uint32_t parent( uint32_t index )const;

        /**
         * Retrieves the head index of a UI element.
         */
        uint32_t head( uint32_t index )const;

        /**
         * Retrieves the tail index of a UI element.
         */
        uint32_t tail( uint32_t index )const;

        /**
         * Retrieves the next sibling index of a UI element.
         */
        uint32_t next( uint32_t index )const;

        /**
         * Retrieves the number of children of a UI element.
         */
        uint32_t count( uint32_t index )const;

        /**
         * Sets the current font for rendering text.
         *
         * @param font The font to set.
         */
        void setFont(const ref::Font& font);

        /**
         * Retrieves the current font.
         *
         * @return The current font.
         */
        const ref::Font& getFont() const;
        
        void createLayers( uint32_t quantity );

        /**
         * Resize total number of layout elements.
         *
         * @param max_elements The maximum number of UI elements the system can manage.
         */
        void resize( uint32_t max_elements );

        bool leftClickDown()const;

        /**
         * Retrieves the input handler associated with the layout.
         *
         * @return A pointer to the input handler.
         */
        const kege::InputManager* inputManager()const;
        Cursor* cursor();

        uint32_t getHeight()const;
        uint32_t getWidth()const;
        uint32_t count()const;

        bool hasHit()const;

        /**
         * Check if index, index to a valid element.
         *
         * @param index The index to check.
         * @return True if index is valid, falss otherwise.
         */
        bool validate( uint32_t index )const;

        /**
         * Begins the UI layout construction. Must be called before creating any UI elements.
         */
        void begin( double dms );

        /**
         * Ends the UI layout construction. Must be called after all UI elements are created.
         */
        void end();

        Layout(kege::InputManager* input_manager, uint32_t width, uint32_t height, uint32_t quantity );
        ~Layout();

    private:

        void setWidgetParameters(uint32_t index, const WidgetDesc& desc);
        void resolveParentChildRelation(uint32_t index);
        void addToDesignatedLayer(uint32_t index, const WidgetDesc& desc);

        Record getHotElem(uint32_t root, bool button = false);
        Record getHotElem(bool button = false);
        uint32_t find(uint64_t user_id, uint32_t index);
        uint32_t find(uint64_t user_id);
        void handleInputEvents();

    private:

        const kege::InputManager* _input_manager;
        const kege::Mouse* _mouse;

        std::vector< kege::ui::Widget > _widgets;
        uint32_t _widget_count;

        ui::PostLayoutOpsExecutor _deferred_operations;

        // used for ording widget render order
        kege::array< ui::Layer > _layers;

        kege::array< uint32_t > _roots;
        uint32_t _root_count;

        kege::array< int32_t > _parent_stack;
        int32_t _parent_stack_count;

        ui::Cursor _cursor;

        ref::Font _font;



        mutable HitRecord _curr;
        mutable HitRecord _next;

        double _dms;
        
        uint32_t _current_parent;

        uint32_t _height;
        uint32_t _width;

        bool _click_registered;
        bool _left_click_down;
        bool _left_click_state;
        bool _butn_down;

        friend Layer;
        friend Resizer;
        friend Aligner;
        friend Cursor;
        friend Viewer;
    };

}

#endif /* ui_layout_hpp */
