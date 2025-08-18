//
//  ui-input.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_input_hpp
#define ui_input_hpp

#include "ui-layout.hpp"
#include "ui-primatives.hpp"

namespace kege::ui{

    class Input
    {
    public:

        enum Type
        {
            INPUT_TEXT,
            INPUT_NUMERIC,
        };

        struct KeyMapping
        {
            char normal;
            char shifted;
        };


        /**
         * Handles keyboard input for text entry.
         *
         * @param type The type of input event (e.g., key press, key release).
         * @param text The current text in the input field.
         * @param cursor The current cursor position in the text.
         * @param has_focused Indicates whether the input field is currently focused.
         * @param input The input event data.
         *
         * @return true if the input field remains focused, false otherwise.
         */
        bool onKeyInput(Input::Type type, std::string& text, int& cursor, bool& has_focused, const kege::Input& input);

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
         * Processes mouse input events.
         *
         * @param input The input event data.
         */
        void processMouseInput( const kege::Input& input );

        /**
         * Processes input events.
         *
         * @param inputs The current input events.
         */
        void processInputs( const InputEvents& inputs );

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

        bool buttonDown()const;

        Input();

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

    private:

        struct IndexID
        {
            //uint32_t id;
            uint32_t level;
            uint32_t clicks = 0;
            const void* layout_id;
        };

    public:

        std::vector< kege::Input > _keyboard_keys; // The array of keyboard inputs.
        int _key_count;// Tracks the current _key_count of keyboard inputs.

        KeyMapping _keymap[128];
        bool _caplock; // Indicates if a capclock is down.
        bool _shift; // Indicates if a shift key is donw.

        kege::dvec2 _release_position; // Position where the mouse button was released.
        kege::dvec2 _click_position;   // Position where the mouse button was clicked.

        kege::dvec2 _previous_position; // Previous position of the mouse pointer.
        kege::dvec2 _current_position;  // Current position of the mouse pointer.
        kege::dvec2 _delta_position;    // Delta (change) in the mouse pointer's position.

        kege::dvec2 _scroll_offset; // Mouse scroll offset.
        kege::dvec2 _scroll;

        double _click_speed; // Time threshold for detecting double-clicks.
        double _click_time;  // Time of the last click event.

        bool _pointer_dragging; // Indicates if the mouse pointer is being dragged.
        bool _single_click;    // Indicates if the primary mouse button was clicked.
        bool _double_click;     // Indicates if a double-click occurred.

        int _button_down;
//        int _button_up;
        friend Canvas;
    };

}
#endif /* ui_input_hpp */
