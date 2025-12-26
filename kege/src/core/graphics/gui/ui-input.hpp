//
//  ui-input.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_input_hpp
#define ui_input_hpp

#include "ui-core.hpp"

namespace kege::ui{

    class Input : public kege::RefCounter
    {
    public:


        struct Click
        {
            // Position where the mouse button was released.
            kege::dvec2 position;
            int  clicks;
            bool down;
        };

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

        struct State
        {
            // Position where the mouse button was released.
            kege::dvec2 release_position;

            // Position where the mouse button was clicked.
            kege::dvec2 click_position;

            // The current position of the mouse pointer.
            kege::dvec2 position;

            // Delta (change) in the current mouse pointer's position.
            kege::dvec2 delta_position;

            kege::dvec2 scroll_offset; // Mouse scroll offset.
            kege::dvec2 scroll;

            // Indicates if the mouse pointer is being dragged.
            bool pointer_dragging;

            // Indicates if the primary mouse button was clicked.
            bool single_click;

            // Indicates if a double-click occurred.
            bool double_click;

            // Indicates if any mouse button is down
            int button_down;

            // Indicates if any keyboard key is down
            int key_down;
        };

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
        void update( const InputEvents& inputs );

//        /**
//         * Retrieves the position where the mouse primary button was released.
//         *
//         * @return The release position as a 2D vector.
//         */
//        const kege::dvec2& releasedPosition() const;
//
//        /**
//         * Retrieves the position where the mouse primary button was pressed.
//         *
//         * @return The click position as a 2D vector.
//         */
//        const kege::dvec2& clickPosition() const;

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
//
//        /**
//         * Checks if the mouse primary button was clicked.
//         *
//         * @return true if the primary button was clicked, false otherwise.
//         */
//        const bool primaryClick() const;
//
//        /**
//         * Checks if the mouse primary button was double-clicked.
//         *
//         * @return true if the primary button was double-clicked, false otherwise.
//         */
//        const bool doubleClick() const;

        const Click& getClick( int i )const;

        bool buttonDown()const;
        bool keyDown()const;

        Input();

    private:

        void handleMouseClick( const kege::Input& input );

    public:

        std::vector< kege::Input > _keyboard_keys; // The array of keyboard inputs.
        int _key_count;// Tracks the current _key_count of keyboard inputs.

        KeyMapping _keymap[128];
        bool _caplock; // Indicates if a capclock is down.
        bool _shift; // Indicates if a shift key is donw.

        //Input::State _curr_frame;
        //Input::State _last_frame;


        // Position where the mouse button was clicked.
        kege::dvec2 _previous_position;
        kege::dvec2 _current_position;
        kege::dvec2 _delta_position;
        kege::dvec2 _scroll_offset;
        Click _clicks[ 32 ];

        bool _pointer_dragging;

        // Indicates if any mouse button is down
        std::atomic<bool> _button_down;

        // Indicates if any keyboard key is down
        std::atomic<bool> _key_down;

//        kege::dvec2 _release_position; // Position where the mouse button was released.
//        kege::dvec2 _click_position;   // Position where the mouse button was clicked.
//
//        kege::dvec2 _position[2];  // [Current, Previous] position of the mouse pointer.
//        kege::dvec2 _delta_position;    // Delta (change) in the mouse pointer's position.
//
//        kege::dvec2 _scroll_offset; // Mouse scroll offset.
//        kege::dvec2 _scroll;
//
//        bool _pointer_dragging; // Indicates if the mouse pointer is being dragged.
//        bool _single_click;    // Indicates if the primary mouse button was clicked.
//        bool _double_click;     // Indicates if a double-click occurred.
//
//        int _button_down[2];
//        int _key_down[2];

        double _click_speed; // Time threshold for detecting double-clicks.
        double _click_time;  // Time of the last click event.
    };

}
#endif /* ui_input_hpp */
