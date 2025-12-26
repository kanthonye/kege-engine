//
//  ui-input.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-input.hpp"

namespace kege::ui{

    void Input::handleMouseClick( const kege::Input& input )
    {
        if ( input.key.code == 0 )
        {
            if ( input.key.state != 0 && !_clicks[ input.key.code ].down )
            {
                _clicks[ input.key.code ].down = true;

                double now = (double)clock() / (double)CLOCKS_PER_SEC;
                double time_span = (now - _click_time);
                if ( time_span > 0 && time_span <= _click_speed )
                {
                    _clicks[ input.key.code ].clicks = 2;
                    std::cout <<"DOUBLE_CLICK\n";
                }
                else
                {
                    _clicks[ input.key.code ].clicks = 1;
                }
                _clicks[ input.key.code ].position = _current_position;
                _button_down = true;
            }
            else if ( input.key.state == 0 && _clicks[ input.key.code ].down )
            {

                _clicks[ input.key.code ].position = _current_position;
                _clicks[ input.key.code ].down = false;
                _clicks[ input.key.code ].clicks = 0;

                _click_time = (double)clock() / (double)CLOCKS_PER_SEC;
                //_curr_frame.release_position = _curr_frame.position;
                //_curr_frame.pointer_dragging = false;
                //_curr_frame.single_click = false;
                //std::cout <<"CLICK_RELEASE\n";
                _button_down = false;
            }
        }
    }


    void Input::update( const InputEvents& inputs )
    {
        _key_count = 0;
        //_last_frame = _curr_frame;
        //_curr_frame.button_down = false;
        _key_down = false;

        for (int i=0; i<inputs.size(); ++i)
        {
            if ( inputs[ i ].type == kege::Input::KEYBOARD )
            {
                if ( _key_count == _keyboard_keys.size() )
                {
                    _keyboard_keys.push_back( inputs[ i ] );
                    _key_count++;
                }
                else
                {
                    _keyboard_keys[ _key_count ] = inputs[ i ];
                    _key_count++;
                }
                _key_down = true;
            }
            else
            {
                switch ( inputs[ i ].type )
                {
                    case kege::Input::MOUSEKEY: handleMouseClick( inputs[ i ] );
                        break;

                    case kege::Input::POINTER:
                    {
                        _current_position.x = inputs[ i ].coord.x;
                        _current_position.y = inputs[ i ].coord.y;
                        break;
                    }

                    case kege::Input::SCROLL:
                    {
                        _scroll_offset.x = inputs[ i ].coord.x;
                        _scroll_offset.y = inputs[ i ].coord.y;
                        break;
                    }

                    default: break;
                }
            }
        }

        _delta_position = _current_position - _previous_position;
        _previous_position = _current_position;
        //_curr_frame.scroll_offset = _curr_frame.scroll;
        //_curr_frame.scroll = {};

        /** Handle mouse pointer dragging */
        if ( _delta_position.x != 0.0 || _delta_position.y != 0.0 )
        {
            if ( _button_down )
            {
                _pointer_dragging = true;
            }
        }
        if ( !_button_down )
        {
            _pointer_dragging = false;
        }
    }

    const kege::dvec2& Input::previousPosition()const
    {
        return _previous_position;
    }

    const kege::dvec2& Input::currentPosition()const
    {
        return _current_position;
    }

    const kege::dvec2& Input::deltaPosition()const
    {
        return _delta_position;
    }

    const kege::dvec2& Input::scrollOffset()const
    {
        return _scroll_offset;
    }

    const bool Input::pointerDragging()const
    {
        return _pointer_dragging;
    }

    bool Input::buttonDown()const
    {
        return _button_down;
    }

    const Input::Click& Input::getClick( int i )const
    {
        return _clicks[i];
    }

    bool Input::keyDown()const
    {
        return _key_down;
    }

    Input::Input()
    :   _click_time(0.0)
    ,   _key_count( 0 )
    ,   _previous_position(0, 0)
    ,   _current_position(0, 0)
    ,   _delta_position(0, 0)
    ,   _scroll_offset(0, 0)
    ,   _pointer_dragging(false)
    ,   _button_down(false)
    {
        _click_speed = 0.032;
        _caplock = false;
        _shift = false;

        // Letters A-Z
        for (char c = 'A'; c <= 'Z'; ++c)
        {
            _keymap[c].normal = std::tolower(c);
            _keymap[c].shifted = c;
        }

        // Numbers and special symbols
        _keymap['0'] = {'0', ')'}; _keymap['1'] = {'1', '!'}; _keymap['2'] = {'2', '@'};
        _keymap['3'] = {'3', '#'}; _keymap['4'] = {'4', '$'}; _keymap['5'] = {'5', '%'};
        _keymap['6'] = {'6', '^'}; _keymap['7'] = {'7', '&'}; _keymap['8'] = {'8', '*'};
        _keymap['9'] = {'9', '('};

        // Other characters
        _keymap['-'] = {'-', '_'}; _keymap['='] = {'=', '+'};
        _keymap['['] = {'[', '{'}; _keymap[']'] = {']', '}'};
        _keymap[';'] = {';', ':'}; _keymap['\''] = {'\'', '"'};
        _keymap[','] = {',', '<'}; _keymap['.'] = {'.', '>'};
        _keymap['/'] = {'/', '?'}; _keymap['\\'] = {'\\', '|'};
        _keymap['`'] = {'`', '~'};
        _keymap[' '] = {' ', ' '};
    }

}
