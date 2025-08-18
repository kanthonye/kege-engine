//
//  ui-input.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-input.hpp"

namespace kege::ui{

    bool Input::onKeyInput( Input::Type type, std::string& text, int& cursor,  bool& has_focused, const kege::Input& input)
    {
        if ( !has_focused ) return false;

        switch ( input.key.code )
        {
            case kege::KEY_BACKSPACE:
            {
                if( input.key.state != 0 && 0 < cursor)
                {
                    text.erase( cursor - 1, 1 );
                    cursor--;
                }
                break;
            }

            case kege::KEY_LEFT_SHIFT:
            case kege::KEY_RIGHT_SHIFT:
            {
                if ( input.key.state == 0 )
                {
                    _shift = false;
                }
                else
                {
                    _shift = true;
                }
                break;
            }

            case kege::KEY_CAPS_LOCK:
            {
                if ( input.key.state == 0 )
                {
                    _caplock = false;
                }
                else
                {
                    _caplock = true;
                }
                break;
            }

            case kege::KEY_DELETE:
            {
                if ( input.key.state != 0 )
                {
                    text.erase( cursor, 1 );
                }
                break;
            }

            case kege::KEY_ENTER:
            {
                if ( input.key.state != 0 )
                {
                    has_focused = false;
                    cursor = static_cast<uint32_t>( text.length() );
                }
                break;
            }

            case kege::KEY_LEFT:
            {
                if ( input.key.state != 0 && cursor > 0 )
                {
                    --cursor;
                }
                break;
            }

            case kege::KEY_RIGHT:
            {
                if ( input.key.state != 0 && cursor < text.length() )
                {
                    ++cursor;
                }
                break;
            }

            case kege::KEY_HOME:
            {
                if ( cursor > text.length() )
                {
                    cursor = 0;
                }
                break;
            }

            case kege::KEY_END:
            {
                if ( cursor > text.length() )
                {
                    cursor = static_cast<uint32_t>( text.length() );
                }
                break;
            }

            default:
            {
                if ( input.key.state != 0 )
                {
                    switch ( type )
                    {
                        case 0:
                        {
                            if ( _shift )
                            {
                                // Insert the character at the cursor position
                                text.insert( cursor, 1, _keymap[ input.key.code ].shifted );
                            }
                            else if
                            (
                                _caplock &&
                                (
                                    ( 'A' <= input.key.code && 'Z' >= input.key.code ) ||
                                    ( 'a' <= input.key.code && 'z' >= input.key.code )
                                )
                            )
                            {
                                // Insert the character at the cursor position
                                text.insert( cursor, 1, _keymap[ input.key.code ].shifted );
                            }
                            else
                            {
                                text.insert( cursor, 1, _keymap[ input.key.code ].normal );
                            }
                            break;
                        }

                        case 1:
                        {
                            if ((input.key.code >= '0' && input.key.code <= '9') || input.key.code == '.' || input.key.code == '-')
                            {
                                // Ensure only one decimal point
                                if (input.key.code == '.' && text.find('.') != std::string::npos)
                                {
                                    break;
                                }

                                // Ensure only one negative sign at the beginning
                                if (input.key.code == '-' && (cursor != 0 || text.find('-') != std::string::npos))
                                {
                                    break;
                                }

                                // Insert the character at the cursor position
                                text.insert( cursor, 1, _keymap[ input.key.code ].normal );
                            }
                            break;
                        }
                    }
                    cursor++;
                }
                break;
            }
        }
        return has_focused;
    }

    bool Input::onTextInput(Input::Type type, std::string* text, int32_t* cursor, bool* has_focused)
    {
        for (int i = 0; i < _key_count; ++i )
        {
            if( !onKeyInput( type, *text, *cursor, *has_focused, _keyboard_keys[i] ) )
            {
                *has_focused = false;
                break;
            }
        }
        return *has_focused;
    }

    void Input::processMouseInput( const kege::Input& input )
    {
        switch ( input.type )
        {
            case kege::Input::MOUSEKEY:
            {
                if ( input.key.code == 0 )
                {
                    if ( input.key.state != 0 && !_button_down )
                    {
                        double now = (double)clock() / (double)CLOCKS_PER_SEC;
                        double time_span = (now - _click_time);
                        if ( time_span > 0 && time_span <= _click_speed )
                        {
                            _double_click = true;
                            //std::cout <<"DOUBLE_CLICK\n";
                        }
                        else
                        {
                            //std::cout <<"SINGLE_CLICK\n";
                            _single_click = true;
                        }
                        _click_position = _current_position;
                        _button_down = true;
                    }
                    else if ( input.key.state == 0 && _button_down )
                    {
                        _click_time = (double)clock() / (double)CLOCKS_PER_SEC;
                        _release_position = _current_position;
                        _pointer_dragging = false;
                        _double_click = false;
                        _single_click = false;
                        _button_down = false;
                        //std::cout <<"CLICK_RELEASE\n";
                    }
                }
                break;
            }

            case kege::Input::POINTER:
            {
                _current_position.x = input.coord.x;
                _current_position.y = input.coord.y;
                break;
            }

            case kege::Input::SCROLL:
            {
                _scroll.x = input.coord.x;
                _scroll.y = input.coord.y;
                break;
            }

            default: break;
        }
    }

    void Input::processInputs( const InputEvents& inputs )
    {
        _key_count = 0;
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
            }
            else
            {
                processMouseInput( inputs[i] );
            }
        }

        _delta_position = _current_position - _previous_position;
        _previous_position = _current_position;

        _scroll_offset = _scroll;
        _scroll = {};
    }

    const kege::dvec2& Input::releasedPosition()const
    {
        return _release_position;
    }

    const kege::dvec2& Input::clickPosition()const
    {
        return _click_position;
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

    const bool Input::primaryClick()const
    {
        return _single_click;
    }

    const bool Input::doubleClick()const
    {
        return _double_click;
    }

    bool Input::buttonDown()const
    {
        return _button_down;
    }

    Input::Input()
    :   _click_time(0.0)
    ,   _current_position(-1, -1)
    ,   _previous_position(-1, -1)
    ,   _key_count( 0 )
    ,   _button_down( 0 )
    ,   _double_click( false )
    ,   _single_click( false )
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
