//
//  mouse.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/23/26.
//

#ifndef mouse_hpp
#define mouse_hpp

#include "../input/inputs.hpp"
#include "../input/thread-safe-vector.hpp"
#include "../app/app-window.hpp"

namespace kege{

    enum class MouseButtonCode : uint8_t
    {
        Left = 0,
        Right,
        Middle,
        Button4,
        Button5,
        Button6,
        Button7,
        Button8,

        Count  // Always last
    };

    enum struct ButtonAction : uint8_t
    {
        Release   = 0,
        Pressed,
        PressedRepeat,
        DoubleClick,
        DoubleClickRepeat,
    };

    struct MouseButtonInput
    {
        // registered click position
        double cx;
        double cy;

        // registered release position
        double rx;
        double ry;

        struct
        {
            MouseButtonCode button;
            ButtonAction action;
            Modifiers mods;
        };
    };

    struct MouseButtonState
    {
        MouseButtonInput input;
        double curr_time_stamp;
        double past_time_stamp;
        bool button_down;
        bool dragging;
    };

    class Mouse
    {
    public:

        enum {MAX_BUTTON_COUNT = 16};
        using Position = kege::dvec2;
        using ScrollDelta = kege::dvec2;

    public:

        void onMouseButton( int button, int action, int mods );
        void onCursorPosition( double xpos, double ypos );
        void onScroll( double xoffset, double yoffset );


        const MouseButtonState* const* getAllActiveButtonState()const;
        uint32_t getActiveButtonStateCount()const noexcept;

        MouseButtonInput getButtonInput(MouseButtonCode button) const noexcept;

        // Position in normalized coordinates [0, 1]
        kege::dvec2 getNormalizedPosition() const noexcept;

        // Position in clip space [-1, 1]
        kege::dvec2 getClipSpacePosition() const noexcept;

        // Position (relative to window, in pixels)
        Position getPosition() const noexcept;

        // Delta movement since last frame
        Position getDelta() const noexcept;

        // Scroll wheel
        ScrollDelta getScrollDelta() const noexcept;

        bool moved() const;

        void setWindowSize(const kege::vec2& size) noexcept;

        bool anyButtonDown() const noexcept;


        kege::dvec2 getReleasePosition(MouseButtonCode button) const;
        kege::dvec2 getClickPosition(MouseButtonCode button) const;

        bool isDoubleClick(MouseButtonCode button) const;
        bool isDragging(MouseButtonCode button) const;
        bool isPressed(MouseButtonCode button) const;
        bool isDown(MouseButtonCode button) const;

        Modifiers getModifiers() const;


        void begin();
        void end();

        Mouse();
        
    private:

        std::vector<MouseButtonState*> _active_states;
        std::array<MouseButtonState, 16> _states;
        Modifiers _modifiers;

        uint32_t _active_state_counter;

        kege::vec2 _window_size;
        ScrollDelta _scroll_delta;
        Position _last_position;
        Position _position;
        Position _delta;
        bool _pointer_moved;

        double _held_click_time_length;
        double _double_click_time_length;
    };

}

#endif /* mouse_hpp */
