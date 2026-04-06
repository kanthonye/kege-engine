//
//  mouse.cpp
//  editor
//
//  Created by Kenneth Esdaile on 1/23/26.
//

#include "mouse.hpp"
namespace kege{
    
    void Mouse::onMouseButton( int code, int action, int mods )
    {
        if( code >= MAX_BUTTON_COUNT ) return;

        _modifiers = (Modifiers)mods;
        //std::cout <<"action: "<< action <<"\n";
        _states[ code ].input.action = (ButtonAction)action;
        _states[ code ].input.mods = (Modifiers)mods;
        _states[ code ].input.button = (MouseButtonCode)code;

        if ( _active_state_counter < _active_states.size() )
        {
            _active_states[ _active_state_counter ] = &_states[ code ];
            _active_state_counter += 1;
        }

        // double click implementation
        // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
        if ( _states[ code ].input.action == ButtonAction::Pressed )
        {
            if ( !_states[ code ].button_down )
            {
                _states[ code ].past_time_stamp = _states[ code ].curr_time_stamp;
                _states[ code ].curr_time_stamp = (double)clock() / (double)CLOCKS_PER_SEC;
                _states[ code ].button_down = true;

                // update click  position
                _states[ code ].input.cx = _position.x;
                _states[ code ].input.cy = _position.y;
                //std::cout <<"PRESSED\n";
            }

            double time_laps = _states[ code ].curr_time_stamp - _states[ code ].past_time_stamp;
            //std::cout <<time_laps<<"\n";
            if ( time_laps <= _double_click_time_length )
            {
                _states[ code ].input.action = ButtonAction::DoubleClick;
                _states[ code ].dragging = false;
                //std::cout <<"DOUBLE_CLICK\n";
            }
        }
        else if ( _states[ code ].input.action == ButtonAction::Release || !_states[ code ].button_down )
        {
            _states[ code ].button_down = false;

            // update release position
            _states[ code ].input.rx = _position.x;
            _states[ code ].input.ry = _position.y;
        }
    }
    
    void Mouse::onCursorPosition( double xpos, double ypos )
    {
        _position.x = xpos;
        _position.y = ypos;
    }

    void Mouse::onScroll( double xoffset, double yoffset )
    {
        _scroll_delta.x = xoffset;
        _scroll_delta.y = yoffset;
    }

    const MouseButtonState* const* Mouse::getAllActiveButtonState()const
    {
        return _active_states.data();
    }
    
    uint32_t Mouse::getActiveButtonStateCount()const noexcept
    {
        return _active_state_counter;
    }

    MouseButtonInput Mouse::getButtonInput(MouseButtonCode button) const noexcept
    {
        return _states[ (int)button ].input;
    }

    // Position in normalized coordinates [0, 1]
    kege::dvec2 Mouse::getNormalizedPosition() const noexcept
    {
        auto pos = getPosition();
        return kege::dvec2(pos.x / _window_size.x, pos.y / _window_size.y);
    }

    // Position in clip space [-1, 1]
    kege::dvec2 Mouse::getClipSpacePosition() const noexcept
    {
        const kege::dvec2 pos = getNormalizedPosition();
        return kege::dvec2(pos.x * 2.0f - 1.0f, 1.0f - pos.y * 2.0f);
    }

    // Position (relative to window, in pixels)
    Mouse::Position Mouse::getPosition() const noexcept
    {
        return _position;
    }

    // Delta movement since last frame
    Mouse::Position Mouse::getDelta() const noexcept
    {
        return _delta;
    }

    // Scroll wheel
    Mouse::ScrollDelta Mouse::getScrollDelta() const noexcept
    {
        return _scroll_delta;
    }

    void Mouse::setWindowSize(const kege::vec2& size) noexcept
    {
        _window_size = size;
    }

    bool Mouse::moved() const
    {
        return _pointer_moved;

    }

    bool Mouse::anyButtonDown() const noexcept
    {
        return _active_state_counter != 0;
    }

    bool Mouse::isDragging(MouseButtonCode button) const
    {
        return _states[ (int)button ].dragging;
    }

    bool Mouse::isDoubleClick( MouseButtonCode button ) const
    {
        return _states[(int)button].input.action == ButtonAction::DoubleClick;
    }

    kege::dvec2 Mouse::getReleasePosition(MouseButtonCode button) const
    {
        const MouseButtonInput& input = _states[ (int)button ].input;
        return kege::dvec2( input.rx, input.ry );
    }

    kege::dvec2 Mouse::getClickPosition(MouseButtonCode button) const
    {
        const MouseButtonInput& input = _states[ (int)button ].input;
        return kege::dvec2( input.cx, input.cy );
    }

    bool Mouse::isPressed( MouseButtonCode button ) const
    {
        return _states[(int)button].input.action == ButtonAction::Pressed;
    }

    bool Mouse::isDown( MouseButtonCode button ) const
    {
        const MouseButtonInput& input = _states[ (int)button ].input;
        return input.action != ButtonAction::Release;
    }

    Modifiers Mouse::getModifiers() const
    {
        return _modifiers;
    }

    void Mouse::begin()
    {
        _delta.x = _position.x - _last_position.x;
        _delta.y = _position.y - _last_position.y;
        _last_position.x = _position.x;
        _last_position.y = _position.y;

        _pointer_moved = _delta.x != 0.f || _delta.y != 0.f;;

        double now = (double)clock() / (double)CLOCKS_PER_SEC;
        for (int i=0; i<_states.size(); ++i)
        {
            MouseButtonState* state = &_states[i];

            if ( state->input.action == ButtonAction::Release )
            {
                state->dragging = false;
                continue;
            }

            if( now - state->curr_time_stamp >= _held_click_time_length )
            {
                if (state->input.action == ButtonAction::Pressed)
                {
                    state->input.action = ButtonAction::PressedRepeat;
                }
                else if (state->input.action == ButtonAction::DoubleClick)
                {
                    state->input.action = ButtonAction::DoubleClickRepeat;
                }
                state->dragging = _pointer_moved;
            }
        }
    }

    void Mouse::end()
    {
        _active_state_counter = 0;
        _scroll_delta = {0.0, 0.0};
    }

    Mouse::Mouse()
    {
        _active_state_counter = 0;
        _held_click_time_length = 0.05;
        _double_click_time_length = 0.06;
        _active_states.resize( MAX_BUTTON_COUNT );
        for (int i=0; i<_states.size(); ++i)
        {
            MouseButtonState* state = &_states[i];
            state->input.action = ButtonAction::Release;
            state->dragging = false;
        }
    }
}
