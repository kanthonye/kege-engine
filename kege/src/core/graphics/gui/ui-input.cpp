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
            if ( input.key.state != 0 && !_curr_frame.button_down )
            {
                _curr_frame.button_down = true;

                double now = (double)clock() / (double)CLOCKS_PER_SEC;
                double time_span = (now - _click_time);
                if ( time_span > 0 && time_span <= _click_speed )
                {
                    _curr_frame.double_click = true;
                    std::cout <<"DOUBLE_CLICK\n";
                }
                else
                {
                    //std::cout <<"SINGLE_CLICK\n";
                    _curr_frame.single_click = true;
                }
                _curr_frame.click_position = _curr_frame.position;
            }
            else if ( input.key.state == 0 && _curr_frame.button_down )
            {
                _curr_frame.button_down = false;

                _click_time = (double)clock() / (double)CLOCKS_PER_SEC;
                _curr_frame.release_position = _curr_frame.position;
                _curr_frame.pointer_dragging = false;
                _curr_frame.double_click = false;
                _curr_frame.single_click = false;
                //std::cout <<"CLICK_RELEASE\n";
            }
        }
    }


    void Input::update( const InputEvents& inputs )
    {
        _key_count = 0;
        _last_frame = _curr_frame;
        //_curr_frame.button_down = false;
        _curr_frame.key_down = false;

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
                _curr_frame.key_down = true;
            }
            else
            {
                switch ( inputs[ i ].type )
                {
                    case kege::Input::MOUSEKEY: handleMouseClick( inputs[ i ] );
                        break;

                    case kege::Input::POINTER:
                    {
                        _curr_frame.position.x = inputs[ i ].coord.x;
                        _curr_frame.position.y = inputs[ i ].coord.y;
                        break;
                    }

                    case kege::Input::SCROLL:
                    {
                        _curr_frame.scroll.x = inputs[ i ].coord.x;
                        _curr_frame.scroll.y = inputs[ i ].coord.y;
                        break;
                    }

                    default: break;
                }
            }
        }

        _curr_frame.delta_position = _curr_frame.position - _last_frame.position;
        _curr_frame.scroll_offset = _curr_frame.scroll;
        _curr_frame.scroll = {};

        /** Handle mouse pointer dragging */
        if ( _curr_frame.delta_position.x != 0.0 || _curr_frame.delta_position.y != 0.0 )
        {
            if ( _last_frame.button_down )
            {
                _curr_frame.pointer_dragging = true;
            }
        }
        if ( !_last_frame.button_down )
        {
            _curr_frame.pointer_dragging = false;
        }
    }

//    const kege::dvec2& Input::releasedPosition()const
//    {
//        return _release_position;
//    }
//
//    const kege::dvec2& Input::clickPosition()const
//    {
//        return _click_position;
//    }
//
//    const kege::dvec2& Input::previousPosition()const
//    {
//        return _position[1];
//    }
//
//    const kege::dvec2& Input::currentPosition()const
//    {
//        return _position[0];
//    }
//
//    const kege::dvec2& Input::deltaPosition()const
//    {
//        return _delta_position;
//    }
//
//    const kege::dvec2& Input::scrollOffset()const
//    {
//        return _scroll_offset;
//    }
//
//    const bool Input::pointerDragging()const
//    {
//        return _pointer_dragging;
//    }
//
//    const bool Input::primaryClick()const
//    {
//        return _single_click;
//    }
//
//    const bool Input::doubleClick()const
//    {
//        return _double_click;
//    }

    const InputState& Input::getLastState()const
    {
        return _last_frame;
    }
    const InputState& Input::getCurrState()const
    {
        return _curr_frame;
    }

    bool Input::buttonDown()const
    {
        return _curr_frame.button_down;
    }

    bool Input::keyDown()const
    {
        return _curr_frame.key_down;
    }

    Input::Input()
    :   _click_time(0.0)
    ,   _key_count( 0 )
    ,   _curr_frame{}
    ,   _last_frame{}
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
