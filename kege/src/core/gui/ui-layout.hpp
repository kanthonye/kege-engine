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

    struct AddStyle
    {
        std::string name_id;
        ui::Style style;
    };

    class Layout
    {
    private:

        enum { PREV, CURR, POST };

        struct IndexID
        {
            uint32_t id = 0;
            uint32_t index = 0;
            uint32_t depth = 0;
            uint32_t clicks = 0;
        };

        struct ID
        {
            // the number of duplicates, reference counter
            int16_t duplicates = 0;

            // the node index
            int16_t node;

            // next free id
            int16_t next;

            // previous free id
            int16_t prev;
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
        EID make( const Content& info );

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
        const kege::ui::Content* operator[](NodeIndex eid) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param eid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Content* operator[](NodeIndex eid);

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

        /**
         * Resize total number of layout elements.
         *
         * @param max_elements The maximum number of UI elements the system can manage.
         */
        void resize( uint32_t max_elements );

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

        /**
         * Retrieves the input handler associated with the layout.
         *
         * @return A pointer to the input handler.
         */
        ui::Input* input();

        Layout();

    private:

        void handleButtonDownEvents();
        void handleButtonUpEvents();
        void handleMouseOverEvents();
        void findNewHotElement( uint32_t root );
        void handleEvents( uint32_t root = 1 );

        void dupId( uint32_t src_index, uint32_t* dst_index );
        void recycleId( uint32_t index );
        uint32_t genId();


    private:

        std::map< std::string, int > _style_index_map;
        std::vector< ui::Style > _styles;
        uint32_t _style_indexer;

        std::vector< kege::ui::Content > _contents;
        std::vector< kege::ui::Node > _nodes;
        uint32_t _node_counter;

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

        Content* operator ->()
        {
            return &layout->_contents[ index ];
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
