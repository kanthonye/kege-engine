//
//  ui-layout.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_layout_hpp
#define ui_layout_hpp

#include "font.hpp"
#include "ui-input.hpp"
#include "ui-aligner.hpp"
#include "ui-core.hpp"
#include "ui-style-manager.hpp"
#include "ui-widget-manager.hpp"

namespace kege::ui{

    class EID;
    class Input;


    class Layout
    {
    private:

        enum { PREV, CURR, POST };

        struct State
        {
            uint32_t id = 0;
            uint32_t index = 0;
            uint32_t depth = 0;
            uint16_t clicks = 0;
            bool active;
        };

    public:

        float getClickToCursorOffset( ui::EID& id, const std::string& text, int font_size, int32_t* cursor, const kege::Font& font );

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
         * Checks if mouse pointer is over ui element.
         *
         * @param eid The ui element layout.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        bool mouseover( const ui::EID& eid ) const;

        /**
         * Checks if a ui-element that is associated with geven id was double clicked on.
         *
         * @param eid The given id.
         *
         * @return true if the element was double clicked on, false otherwise.
         */
        bool doubleClick( const ui::EID& eid ) const;

        /**
         * Checks if a ui-element that is associated with geven id was single clicked on.
         *
         * @param eid The given id.
         *
         * @return true if the ui-element was single clicked on, false otherwise.
         */
        bool click( const ui::EID& eid ) const;

        /**
         * Checks if a ui-element that is associated with geven id has focus.
         *
         * @param eid The given id.
         *
         * @return true if the ui-element has focus, false otherwise.
         */
        bool hasFocus( const ui::EID& eid )const;

        /**
         * Set the id of the ui-element to focus on.
         *
         * @param eid The given id.
         */
        void setFocus( const ui::EID& eid );

        /**
         * Creates a UI element with the give info.
         *
         * @param info contains the attribute the ui requires.
         *
         * @return The element id.
         */
        EID make( const Widget& info );

        /**
         * Creates a parent UI element with the give info.
         *
         * @param eid This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        uint32_t push( const EID& eid );

        /**
         * Creates a UI element with the give info.
         *
         * @param eid This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        uint32_t put( const EID& eid );

        /**
         * Pops the current parent UI element from the parent stack.
         *
         * @return The index of the popped UI element in the elements array.
         */
        uint32_t pop();

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param eid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* operator[]( const EID& eid ) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param eid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* operator[]( const EID& eid );

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param eid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* operator[](NodeIndex eid) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param eid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* operator[](NodeIndex eid);

        /**
         * Retrieves the parent index of a UI element.
         */
        NodeIndex parent( NodeIndex eid )const;

        /**
         * Retrieves the head index of a UI element.
         */
        NodeIndex head( NodeIndex eid )const;

        /**
         * Retrieves the tail index of a UI element.
         */
        NodeIndex tail( NodeIndex eid )const;

        /**
         * Retrieves the next sibling index of a UI element.
         */
        NodeIndex next( NodeIndex eid )const;

        /**
         * Retrieves the number of children of a UI element.
         */
        uint32_t count( NodeIndex eid )const;

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
        void begin( ui::Input* input );

        /**
         * Ends the UI layout construction. Must be called after all UI elements are created.
         */
        void end();

        Layout();

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

        ui::Input* _input;

        ui::Aligner _aligner; // Handles alignment of UI elements.

        mutable State _curr_active;
        mutable State _prev_active;
        mutable State _curr_hot;
        mutable State _prev_hot;
        mutable State _focus;
        
        mutable uint32_t _focus_id;

        uint32_t _parent; // Tracks the current parent element in the UI hierarchy.

        uint32_t _level; // Tracks the current level in the UI hierarchy.

        int32_t _root;

        bool _button_down;

        friend Aligner;
        friend EID;
    };



    class EID
    {
    public:

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* operator->() const
        {
            return &layout->_widget_manager[ index ];
        }

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* operator->()
        {
            return &layout->_widget_manager[ index ];
        }

        /**
         * Checks if mouse pointer is over ui element.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        //bool mouseover() const;

        /**
         * Checks if a ui-element that is associated with geven id was double clicked on.
         *
         * @return true if the element was double clicked on, false otherwise.
         */
        //bool doubleClick() const;

        /**
         * Checks if a ui-element that is associated with geven id was single clicked on.
         *
         * @return true if the ui-element was single clicked on, false otherwise.
         */
        //bool click() const;

        /**
         * Checks if a ui-element that is associated with geven id has focus.
         *
         * @return true if the ui-element has focus, false otherwise.
         */
        //bool hasFocus()const;

        /**
         * Set the id of the ui-element to focus on.
         */
        //void setFocus();

        /**
         * Retrieves the parent index of a UI element.
         */
        //NodeIndex parent()const;

        /**
         * Retrieves the head index of a UI element.
         */
        //NodeIndex head()const;

        /**
         * Retrieves the tail index of a UI element.
         */
        //NodeIndex tail()const;

        /**
         * Retrieves the next sibling index of a UI element.
         */
        ///NodeIndex next()const;

        /**
         * Retrieves the number of children of a UI element.
         */
        //uint32_t count()const;

        //void push();

        /**
         * Creates a UI element with the give info.
         */
        //void put();

        /**
         * Pops the current parent UI element from the parent stack.
         */
        //void pop();


        // Move assignment operator
        EID& operator =(EID&& other) noexcept
        {
            index = other.index;
            layout = other.layout;
            node_index = other.node_index;

            other.index = 0;
            other.node_index = 0;
            other.layout = nullptr;

            return *this;
        }

        EID& operator =( const EID& other )
        {
            index = other.index;
            layout = other.layout;
            node_index = other.node_index;
            layout->_widget_manager.duplicate( other.index, &index );
            return *this;
        }

        operator size_t()const
        {
            return index;
        }

        operator uint32_t()const
        {
            return index;
        }

        operator int32_t()const
        {
            return index;
        }

        // Move constructor
        EID( EID&& other ) noexcept
        :   index( other.index )
        ,   layout( other.layout )
        ,   node_index( other.node_index )
        {
            other.index = 0;
            other.node_index = 0;
            other.layout = nullptr;
        }

        EID( uint32_t index, Layout* layout )
        :   index( index )
        ,   layout( layout )
        ,   node_index( 0 )
        {}

        EID( const EID& other )
        :   index( other.index )
        ,   layout( other.layout )
        ,   node_index( other.node_index )
        {
            layout->_widget_manager.duplicate( other.index, &index );
        }

        EID()
        :   layout( nullptr )
        ,   index( 0 )
        ,   node_index( 0 )
        {}

        ~EID()
        {
            if( layout )
            {
                layout->_widget_manager.recycle( index );
                layout = nullptr;
            }
        }

    private:

        Layout* layout;

        uint32_t index;
        mutable NodeIndex node_index;

        friend Layout;
    };

}

#endif /* ui_layout_hpp */
