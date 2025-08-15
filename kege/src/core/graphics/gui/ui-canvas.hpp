//
//  ui-canvas.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_canvas_hpp
#define ui_canvas_hpp

#include "ui-input.hpp"
#include "ui-viewer.hpp"
#include "ui-layout.hpp"

namespace kege::ui{

    class Canvas
    {
    public:

        /**
         * Calculates the cursor offset when a user clicks within an input field.
         * This allows the cursor to move to the clicked position.
         *
         * @param layout The layout of the ui element.
         * @param text The text content of the input field.
         * @param font_size The font size used to render the text.
         * @param cursor A pointer to the cursor position to update.
         *
         * @return The calculated cursor offset.
         */
        float getClickToCursorOffset( ui::UIElem& layout, const std::string& text, int font_size, int32_t* cursor, const kege::Font& font );

        /**
         * Handles text input events (e.g., typing characters).
         *
         * @param type The type of input event.
         * @param text A pointer to the text string being modified.
         * @param cursor A pointer to the cursor position in the text.
         * @param has_focused A pointer to the focus state of the input field.
         *
         * @return true if the input field remains focused, false otherwise.
         */
        bool onTextInput(Input::Type type, std::string* text, int32_t* cursor, bool* has_focused);

        /**
         * Tests whether a point is inside a UI element's bounding rectangle.
         *
         * @param p The point to test against the rect.
         * @param rect The rect to test against the point.
         *
         * @return true if the point is inside the UI element, false otherwise.
         */
        bool testPointVsRect( const kege::dvec2& p, const ui::Rect2D& rect ) const;

        bool mouseover( uint32_t id ) const;

        bool doubleClick( uint32_t id ) const;

        bool click( uint32_t id ) const;

        void setFocus( uint32_t id );

        /**
         * Clear the focus
         */
        void clearFocus();

        /**
         * Processes input events.
         *
         * @param inputs The current input events.
         */
        void updateInputStates( const InputEvents& inputs );

        /**
         * Retrieves the position where the mouse primary button was released.
         *
         * @return The release position as a 2D vector.
         */
        const kege::dvec2& releasedPosition() const;

        /**
         * Retrieves the position where the mouse primary button was pressed.
         *
         * @return The click position as a 2D vector.
         */
        const kege::dvec2& clickPosition() const;

        /**
         * Retrieves the previous position of the mouse pointer.
         *
         * @return The previous position as a 2D vector.
         */
        const kege::dvec2& previousPosition() const;

        /**
         * Retrieves the current position of the mouse pointer.
         *
         * @return The current position as a 2D vector.
         */
        const kege::dvec2& currentPosition() const;

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
         * Checks if the mouse primary button was clicked.
         *
         * @return true if the primary button was clicked, false otherwise.
         */
        const bool primaryClick() const;

        /**
         * Checks if the mouse primary button was double-clicked.
         *
         * @return true if the primary button was double-clicked, false otherwise.
         */
        const bool doubleClick() const;

       EID make( const Info& info );

        /**
         * Creates a UI element for the current frame.
         *
         * @param info The ui info.
         *
         * @return The index of the created UI element in the elements array.
         */
        UIElem& put( const EID& id );

        /**
         * Creates a parent UI element for the current frame and pushes it onto the parent stack.
         *
         * @param info The ui info.
         *
         * @return The index of the created UI element in the elements array.
         */
        UIElem& push( const EID& id );

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
        const kege::ui::UIElem* operator[](uint32_t eid) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param eid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::UIElem* operator[](uint32_t eid);

        //Style& styles( uint32_t style_index );

        bool validate( uint32_t node_id )const;

        bool parent( uint32_t node_id )const;

        uint32_t elemCount()const;

        /**
         * Begins the UI layout construction. Must be called before creating any UI elements.
         */
        void beginLayout( ui::Layout* layout );

        /**
         * Ends the UI layout construction. Must be called after all UI elements are created.
         */
        void endLayout();

        void view( kege::CommandBuffer* command, ui::Layout& layout );

        /**
         * Initializes the Core object.
         *
         * @return true if initialization is successful, false otherwise.
         */
        bool initialize( Graphics* graphics );

        /**
         * Uninitializes the Core object, releasing any allocated resources.
         */
        void shutdow();

        Canvas();

    private:

        /**
         * Retrieves the rendering data for all UI elements and stores it in the provided buffer.
         * This function is used to prepare the UI elements for rendering by filling a buffer
         * with drawable elements.
         *
         * @param drawbuffer A reference to a vector where the rendering data will be stored.
         *                   Each element in the vector represents a drawable UI element.
         * @param count A reference to an integer that will store the number of drawable elements
         *              added to the buffer.
         */
        void getRenderData( ui::Layout& layout, Node& node, const Rect2D& clip_rect, std::vector< kege::ui::DrawElem >& drawbuffer, uint32_t& count );

    public:

        enum { PREV, CURR, POST }; // Enum for tracking current and previous states.

        std::vector< ui::Layout* > _layouts;
        
        ui::Layout* _layout;
        ui::Viewer _viewer;
        ui::Input _input;
    };

}
#endif /* ui_canvas_hpp */
