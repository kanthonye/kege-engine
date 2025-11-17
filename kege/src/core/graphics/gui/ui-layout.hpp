//
//  ui-layout.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_layout_hpp
#define ui_layout_hpp

#include "../font/font.hpp"
#include "ui-input.hpp"
#include "ui-aligner.hpp"
#include "ui-core.hpp"
#include "ui-cursor.hpp"
#include "ui-style-manager.hpp"
#include "ui-widget-manager.hpp"

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

    public:

        kege::vec2 computeExtent( int font_size, const char* text );

        bool onNumericInput(const ui::Elem& elem, kege::string* text);
        bool onTextInput(const ui::Elem& elem, kege::string* text);


        /**
         * Checks if geven point and rectangular shape intersects.
         *
         * @param p The given point to test the rectangle against.
         * @param rect The rectangle to test the point against.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        bool testPointVsRect( const kege::dvec2& p, const ui::Rect& rect )const;

        /**
         * Retrieves the current position of the mouse pointer.
         *
         * @return The current position as a 2D vector.
         */
        const kege::dvec2& pointerPosition() const;

        /**
         * Retrieves the delta (change) in the mouse pointer's position.
         *
         * @return The delta position as a 2D vector.
         */
        const kege::dvec2& deltaPosition() const;

        /**
         * Retrieves the mouse scroll offset.
         *
         * @return The scroll offset as a 2D vector.
         */
        const kege::dvec2& scrollOffset() const;

        /**
         * Checks if the mouse pointer is being dragged.
         *
         * @return true if the pointer is being dragged, false otherwise.
         */
        const bool pointerDragging() const;

        /**
         * Checks if mouse pointer is over ui element.
         *
         * @param elem The ui element layout.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        bool mouseover( const ui::Elem& elem ) const;

        /**
         * Checks if a ui-element that is associated with geven id was double clicked on.
         *
         * @param elem The given id.
         *
         * @return true if the element was double clicked on, false otherwise.
         */
        bool doubleClick( const ui::Elem& elem ) const;

        /**
         * Checks if a ui-element that is associated with geven id was single clicked on.
         *
         * @param elem The given id.
         *
         * @return true if the ui-element was single clicked on, false otherwise.
         */
        bool click( const ui::Elem& elem ) const;

        /**
         * Checks if a ui-element that is associated with geven id has focus.
         *
         * @param elem The given id.
         *
         * @return true if the ui-element has focus, false otherwise.
         */
        bool hasFocus( const ui::Elem& elem )const;

        /**
         * Set the id of the ui-element to focus on.
         *
         * @param elem The given id.
         */
        void setFocus( const ui::Elem& elem );

        /**
         * Creates a UI element with the give info.
         *
         * @param info contains the attribute the ui requires.
         *
         * @return The element id.
         */
        Elem make( const Widget& info );

        /**
         * Creates a parent UI element with the give info.
         *
         * @param elem This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        uint32_t push( const Elem& elem );

        /**
         * Creates a UI element with the give info.
         *
         * @param elem This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        uint32_t put( const Elem& elem );

        /**
         * Pops the current parent UI element from the parent stack.
         *
         * @return The index of the popped UI element in the elements array.
         */
        uint32_t pop();

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param elem The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* operator[]( const Elem& elem ) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param elem The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* operator[]( const Elem& elem );

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param elem The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* operator[](NodeIndex elem) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param elem The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* operator[](NodeIndex elem);

        /**
         * Retrieves the parent index of a UI element.
         */
        NodeIndex parent( NodeIndex elem )const;

        /**
         * Retrieves the head index of a UI element.
         */
        NodeIndex head( NodeIndex elem )const;

        /**
         * Retrieves the tail index of a UI element.
         */
        NodeIndex tail( NodeIndex elem )const;

        /**
         * Retrieves the next sibling index of a UI element.
         */
        NodeIndex next( NodeIndex elem )const;

        /**
         * Retrieves the number of children of a UI element.
         */
        uint32_t count( NodeIndex elem )const;

        /**
         * Adds a new style to the layout system.
         *
         * @param style The style to add.
         *
         * @return The index of the newly added style.
         */
        uint32_t addStyle( const AddStyle& style );

        /**
         * Retrieves a style by its name identifier.
         *
         * @param name_id The name identifier of the style.
         *
         * @return A pointer to the style if found, nullptr otherwise.
         */
        ui::Style* getStyleByName( const std::string& name_id );

        /**
         * Retrieves a style by its index.
         *
         * @param index The index of the style.
         *
         * @return A pointer to the style if index is valid, nullptr otherwise.
         */
        ui::Style* getStyleByID( int index );
        
        bool loadStyles( const std::string& filename );

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
        ui::Input* input();

        uint32_t getHeight()const;
        uint32_t getWidth()const;
        uint32_t count()const;

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
        void begin( double dms, ui::Input* input );

        /**
         * Ends the UI layout construction. Must be called after all UI elements are created.
         */
        void end();


        Layout(uint32_t width, uint32_t height);

    private:

        void handleButtonDownEvents();
        void handleButtonUpEvents();
        void handleMouseOverEvents();
        void findNewHotElement( uint32_t root );
        void handleEvents( uint32_t root = 1 );

    private:

        WidgetManager _widget_manager;
        StyleManager _style_manager;

        std::vector< kege::ui::Node > _nodes;
        uint32_t _node_counter;

        ui::Cursor _cursor;

        ref::Font _font;
        ui::Input* _input;

        mutable State _active[2];
        mutable State _hot[2];
        mutable State _clicked;
        mutable State _focus;

        uint32_t _parent; // Tracks the current parent element in the UI hierarchy.

        int32_t _root;

        uint32_t _height;
        uint32_t _width;
        bool _button_down;

        friend Cursor;
        friend Viewer;
        friend Elem;
    };

}

#endif /* ui_layout_hpp */
