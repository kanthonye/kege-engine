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

        template<typename Params> void pushDeferredOp(const ui::UID* id, DeferredOperation fn, const Params& params)
        {
            _deferred_operations.push< Params >(id, fn, params);
        }

        template<typename Params> void pushDeferredOpPtr(const ui::UID* id, DeferredOperation fn, Params* params)
        {
            _deferred_operations.pushPtr< Params >(id, fn, params);
        }


        void onWindowResize(int width, int height);

        /**
         * Creates a parent UI element with the give info.
         *
         * @param desc This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        kege::ui::Widget* pushRoot( const WidgetDesc& desc );
        kege::ui::Widget* putRoot( const WidgetDesc& desc );

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
        kege::ui::Widget* push( const WidgetDesc& desc );

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
        kege::ui::Widget* put( const WidgetDesc& desc );


        uint32_t computeExtent( int font_size, const char* text, float& width, float& height );
        bool onNumericInput(const UID& elem, char* str, size_t& size);
        bool onTextInput(const UID& elem, char* str, size_t& size);


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
        bool mouseover( const UID& uid ) const;

        /**
         * Checks if a ui-element that is associated with geven id was double clicked on.
         *
         * @param uid The given id.
         *
         * @return true if the element was double clicked on, false otherwise.
         */
        bool doubleClick( const UID& uid ) const;

        /**
         * Checks if a ui-element that is associated with geven id was single clicked on.
         *
         * @param uid The given id.
         *
         * @return true if the ui-element was single clicked on, false otherwise.
         */
        bool click( const UID& uid ) const;

        /**
         * Checks if a ui-element that is associated with geven id has focus.
         *
         * @param uid The given id.
         *
         * @return true if the ui-element has focus, false otherwise.
         */
        bool hasFocus( const UID& uid )const;

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param uid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* elem( const UID& uid ) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param uid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* elem( const UID& uid );
        kege::ui::Widget* elemParent( const UID& uid );

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

        bool buttonDown()const;

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

        Id getHotElem(uint32_t root, bool button = false);
        Id getHotElem(bool button = false);
        uint32_t find(uint32_t node_index, const ui::Id& id);
        uint32_t find(const ui::Id& id);
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

        HitRecord _hit_record;

        mutable Id _active;
        mutable Id _focus;
        mutable Id _hot;
        mutable Id _hit;

        double _dms;
        
        uint32_t _current_parent;
        uint32_t _active_index;
        uint32_t _hot_index;

        uint32_t _height;
        uint32_t _width;

        bool _click_registered;

        friend Layer;
        friend Resizer;
        friend Aligner;
        friend Cursor;
        friend Viewer;
    };

}

#endif /* ui_layout_hpp */
