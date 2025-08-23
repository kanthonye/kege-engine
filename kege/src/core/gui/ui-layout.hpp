//
//  ui-layout.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_layout_hpp
#define ui_layout_hpp

#include "font.hpp"
#include "ui-aligner.hpp"
#include "ui-primatives.hpp"

namespace kege::ui{

    class EID;
    class Input;
    class Canvas;


    class Layout
    {
    private:

        enum { PREV, CURR, POST };

        struct IndexID
        {
            uint32_t id = 0;
            uint32_t index = 0;
            uint32_t level = 0;
            uint32_t clicks = 0;
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
        bool testPointVsRect( const kege::dvec2& p, const ui::Rect2D& rect )const;

        /**
         * Checks if mouse pointer is over ui element.
         *
         * @param eid The ui element layout.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        bool mouseover( ui::EID& eid ) const;

        /**
         * Checks if a ui-element that is associated with geven id was double clicked on.
         *
         * @param eid The given id.
         *
         * @return true if the element was double clicked on, false otherwise.
         */
        bool doubleClick( ui::EID& eid ) const;

        /**
         * Checks if a ui-element that is associated with geven id was single clicked on.
         *
         * @param eid The given id.
         *
         * @return true if the ui-element was single clicked on, false otherwise.
         */
        bool click( ui::EID& id ) const;

        /**
         * Checks if a ui-element that is associated with geven id has focus.
         *
         * @param eid The given id.
         *
         * @return true if the ui-element has focus, false otherwise.
         */
        bool hasFocus( ui::EID& eid )const;

        /**
         * Set the id of the ui-element to focus on.
         *
         * @param id The given id.
         */
        void setFocus( ui::EID& eid );

        /**
         * Creates a UI element with the give info.
         *
         * @param info contains the attribute the ui requires.
         *
         * @return The element id.
         */
        EID make( const Info& info );

        /**
         * Creates a parent UI element with the give info.
         *
         * @param eid This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        UIElem& push( const EID& eid );

        /**
         * Creates a UI element with the give info.
         *
         * @param eid This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        UIElem& put( const EID& eid );

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
        const kege::ui::UIElem& operator[](uint32_t eid) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param eid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::UIElem& operator[](uint32_t eid);

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param eid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Node& nodes(uint32_t eid) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param eid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Node& nodes(uint32_t eid);

        /**
         * Resize total number of layout elements.
         *
         * @param max_elements The maximum number of UI elements the system can manage.
         */
        void resize( uint32_t max_elements );

        /**
         * Check if index, index to a valid element.
         *
         * @param index The index to check.
         * @return True if index is valid, falss otherwise.
         */
        bool validate( uint32_t index )const;

        /**
         * Check if the element at the given index is a perent.
         *
         * @param index The index to check.
         * @return True if the element at the given index is a parent, falss otherwise.
         */
        bool parent( uint32_t index )const;

        /**
         * Begins the UI layout construction. Must be called before creating any UI elements.
         */
        void begin( ui::Input* input );

        /**
         * Ends the UI layout construction. Must be called after all UI elements are created.
         */
        void end();

        //uint32_t insertStyle( const std::string& sid, const Style& style );
        //uint32_t getStyleID( const std::string& sid );
        //Style* getStyle( uint32_t index );
        ui::Input* input();

        Layout();

    private:

        void handleButtonDownEvents();
        void handleButtonUpEvents();
        void handleMouseOverEvents( uint32_t root );
        void findNewHotElement( uint32_t root );
        void handleEvents( uint32_t root = 1 );

        void dupId( uint32_t src_index, uint32_t* dst_index );
        void recycleId( uint32_t index );
        uint32_t genId();


    private:

        std::vector< kege::ui::Node > _nodes;
        uint32_t _count;

        ui::Input* _input;

        uint32_t _parent; // Tracks the current parent element in the UI hierarchy.

        uint32_t _level; // Tracks the current level in the UI hierarchy.

        kege::ui::Aligner _aligner; // Handles alignment of UI elements.

        mutable IndexID _curr_active_elem;
        mutable IndexID _prev_active_elem;

        mutable IndexID _curr_hot_elem;
        mutable IndexID _prev_hot_elem;

        mutable IndexID _focus;
        mutable uint32_t _focus_id;

        mutable IndexID _clicked;

        int32_t _root;
        friend Aligner;
        friend Canvas;
        friend EID;



        struct ID
        {
            int16_t duplicates = 0; // reference counter
            int16_t index;
            int16_t next;
            int16_t prev;
        };
        std::vector< Layout::ID > _id_pool;


        int32_t _recycled_node_head;
        int32_t _recycled_node_tail;
        int32_t _recycled_node_count;
        int32_t _recycled_id;
        int32_t _available_id;
    };



    class EID
    {
    public:

        UIElem* operator ->()
        {
            return &layout->operator[]( index );
        }

        // Move assignment operator
        EID& operator =(EID&& other) noexcept
        {
            index = other.index;
            layout = other.layout;
            other.index = 0;
            other.layout = nullptr;
            return *this;
        }

        EID& operator =( const EID& other )
        {
            layout = other.layout;
            layout->dupId( other.index, &index );
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
        {
            other.index = 0;
            other.layout = nullptr;
        }

        EID( uint32_t index, Layout* layout )
        :   index( index )
        ,   layout( layout )
        {}

        EID( const EID& other )
        :   index( other.index )
        ,   layout( other.layout )
        {
            layout->dupId( other.index, &index );
        }

        EID()
        :   index( 0 )
        ,   layout( nullptr )
        {}

        ~EID()
        {
            if( layout )
            {
                layout->recycleId( index );
                layout = nullptr;
            }
        }

    private:

        uint32_t index;
        Layout* layout;
        friend Layout;
        friend Canvas;
    };

}

#endif /* ui_layout_hpp */
