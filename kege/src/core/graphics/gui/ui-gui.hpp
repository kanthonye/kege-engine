//
//  ui-gui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/23/26.
//

#ifndef ui_gui_hpp
#define ui_gui_hpp

#include "ui-core.hpp"

namespace kege::ui{

    struct ClickInfo
    {
        // Position where the mouse button was released.
        kege::dvec2 position;
        int  clicks;
        bool down;
    };


    class GUI : public kege::RefCounter
    {
    public:

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
         */
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

        /**
         */
        Text text( const char* str, int font_size );

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

        /**
         * Retrieves the input handler associated with the layout.
         *
         * @return A pointer to the input handler.
         */
        const ui::Input* input()const;

        /**
         */
        Cursor* cursor();

        /**
         * Retrieves the mouse scroll offset.
         *
         * @return The scroll offset as a 2D vector.
         */
        const kege::dvec2& getScrollOffset() const;

        /**
         * Retrieves the delta (change) in the mouse pointer's position.
         *
         * @return The delta position as a 2D vector.
         */
        const kege::dvec2& getPointerDelta() const;

        /**
         * Retrieves the current position of the mouse pointer.
         *
         * @return The current position as a 2D vector.
         */
        const kege::dvec2& getPointer() const;

        /**
         * Checks if the mouse pointer is being dragged.
         *
         * @return true if the pointer is being dragged, false otherwise.
         */
        const bool isPointerDragging() const;

        /**
         */
        const ClickInfo& getClickInfo( int i )const;

        /**
         */
        bool buttonDown()const;

        /**
         */
        bool keyDown()const;

        /**
         */
        uint32_t count()const;

        /**
         * Begins the UI layout construction. Must be called before creating any UI elements.
         */
        void begin( double dms );

        /**
         * Ends the UI layout construction. Must be called after all UI elements are created.
         */
        void end();

        /**
         */
        GUI(uint32_t max_widgets, uint32_t max_layers);

    private:

    };

}
#endif /* ui_gui_hpp */
