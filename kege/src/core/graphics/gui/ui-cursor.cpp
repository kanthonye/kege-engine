//
//  ui-cursor.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/14/25.
//

#include "ui-cursor.hpp"
#include "ui-layout.hpp"

namespace kege::ui{

    float Cursor::getClickToCursorOffset( const UID& uid, const ref::Font& font, int font_size, const char* str, size_t& size )
    {
        if ( size == 0 ) return 0;
        float length = 0;

        _position = 0;
        float char_width;
        float max_length = _input->_current_position.x - _layout->elem( uid )->rect.x;
        for ( const char* s = str; *s != 0; ++s )
        {
            char_width = font->getCharWidth( font_size, *s );
            if (length + char_width > max_length )
            {
                break;
            }
            length += char_width;
            _position += 1;
        }

        return length;
    }

    void Cursor::deleteSelection( int font_size, const ref::Font& font, char* str, size_t& size )
    {
        if (_anchor < _position)
        {
            for ( size_t i = _anchor; i < _position; ++i )
            {
                _offset -= font->getCharWidth( font_size, str[i] );
            }
            eraseChar( str, size, _anchor, _position );
            _position = _anchor;
        }
        else if (_position < _anchor)
        {
            for ( size_t i = _position; i < _anchor; ++i )
            {
                _offset -= font->getCharWidth( font_size, str[i] );
            }
            eraseChar( str, size, _position, _anchor );
            _anchor = _position;
        }
        _selection = false;
    }

    void Cursor::handleDeletion( int font_size, const ref::Font& font, char* str, size_t& size )
    {
        if ( _selection )
        {
            deleteSelection( font_size, font, str, size );
        }
        else if( 0 < _position )
        {
            _position -= 1;
            _offset -= font->getCharWidth( font_size, str[ _position ]);
            eraseChar( str, size, _position, _position + 1 );
        }
    }

    bool Cursor::onInput(Input::Type type, const ref::Font& font, int font_size, const kege::Input& input, char* str, size_t& size)
    {
        if ( !_reading_input ) return false;

        switch ( input.key.code )
        {
            case kege::KEY_BACKSPACE:
            {
                if( input.key.state != 0 )
                {
                    handleDeletion( font_size, font, str, size );
                }
                break;
            }

            case kege::KEY_LEFT_SHIFT:
            case kege::KEY_RIGHT_SHIFT:
            {
                if ( input.key.state == 0 )
                {
                    _input->_shift = false;
                }
                else
                {
                    _input->_shift = true;
                }
                break;
            }

            case kege::KEY_CAPS_LOCK:
            {
                if ( input.key.state == 0 )
                {
                    _input->_caplock = false;
                }
                else
                {
                    _input->_caplock = true;
                }
                break;
            }

            case kege::KEY_DELETE:
            {
                if ( input.key.state != 0 )
                {
                    handleDeletion( font_size, font, str, size );
                }
                break;
            }

            case kege::KEY_ENTER:
            {
                if ( input.key.state != 0 )
                {
                    _reading_input = false;
                    _position = size;
                }
                break;
            }

            case kege::KEY_LEFT:
            {
                if ( input.key.state != 0 )
                {
                    if ( _selection )
                    {
                        for ( size_t i = _position; i < _anchor; ++i )
                        {
                            _offset -= font->getCharWidth( font_size, str[i] );
                        }
                        _position = kege::min(_anchor, _position);
                        _selection = false;
                    }
                    else if ( _position > 0 )
                    {
                        size_t index = ( _position == size ) ? _position - 1: _position;
                        _offset -= font->getCharWidth( font_size, str[ index ]);
                        _position -= 1;
                    }
                }
                break;
            }

            case kege::KEY_RIGHT:
            {
                if ( input.key.state != 0 && _position < size )
                {
                    if ( _selection )
                    {
                        for ( size_t i = _anchor; i < _position; ++i )
                        {
                            _offset += font->getCharWidth( font_size, str[i] );
                        }
                        _anchor = _position;
                        _selection = false;
                    }
                    else
                    {
                        _offset += font->getCharWidth( font_size, str[ _position ]);
                        _position += 1;
                    }
                }
                break;
            }

            case kege::KEY_HOME:
            {
                if ( _position > size )
                {
                    _position = 0;
                }
                break;
            }

            case kege::KEY_END:
            {
                if ( _position > size )
                {
                    _position = size;
                }
                break;
            }

            default:
            {
                if ( input.key.state != 0 )
                {
                    if ( _selection )
                    {
                        deleteSelection( font_size, font, str, size );
                    }

                    switch ( type )
                    {
                        case 0:
                        {
                            if ( _input->_shift )
                            {
                                // Insert the character at the cursor position
                                insertChar( str, size, _position, 1, _input->_keymap[ input.key.code ].shifted );
                            }
                            else if
                            (
                                _input->_caplock &&
                                (
                                    ( 'A' <= input.key.code && 'Z' >= input.key.code ) ||
                                    ( 'a' <= input.key.code && 'z' >= input.key.code )
                                )
                            )
                            {
                                // Insert the character at the cursor position
                                insertChar( str, size, _position, 1, _input->_keymap[ input.key.code ].shifted );
                            }
                            else
                            {
                                insertChar( str, size, _position, 1, _input->_keymap[ input.key.code ].normal );
                            }
                            break;
                        }

                        case 1:
                        {
                            if ((input.key.code >= '0' && input.key.code <= '9') || input.key.code == '.' || input.key.code == '-')
                            {
                                // Ensure only one decimal point
                                if (input.key.code == '.' && strchr( str, '.' ) != nullptr )
                                {
                                    break;
                                }

                                // Ensure only one negative sign at the beginning
                                if (input.key.code == '-' && (_position != 0 || strchr( str, '-' ) != nullptr ))
                                {
                                    break;
                                }

                                // Insert the character at the cursor position
                                insertChar( str, size, _position, 1, _input->_keymap[ input.key.code ].normal );
                            }
                            break;
                        }
                    }

                    _offset += font->getCharWidth( font_size, str[ _position ]);
                    _position++;
                }
                break;
            }
        }
        return _reading_input;
    }

    bool Cursor::onInput(Input::Type type, const UID& uid, const ref::Font& font, char* str, size_t& size)
    {
        Widget* widget = _layout->elem( uid );
        int font_size = widget->text.size;
        if (widget->rect.height > 0) _height = widget->rect.height;
        _x = widget->rect.x;
        _y = widget->rect.y;
        _reading_input = true;

        /**
         On initial click, compute the offset where the cursor should be
         */
        if ( _input->buttonDown() || !_editing )
        {
            if( !_initial_click )
            {
                _offset = getClickToCursorOffset( uid, font, font_size, str, size );
                _initial_click = true;
                _anchor = _position;
                _selection = false;
            }
            _editing = true;
        }
        else if( !_input->buttonDown() && _initial_click )
        {
            _initial_click = false;
        }

        if( _input->_pointer_dragging && _editing )
        {
            _selection_end = getClickToCursorOffset( uid, font, font_size, str, size );
            _selection = true;
        }

        /**
         Process input entries
         */
        for (int i = 0; i < _input->_key_count; ++i )
        {
            if( !onInput( type, font, font_size, _input->_keyboard_keys[i], str, size ) )
            {
                _selection = false;
                _editing = false;
                _position = 0;
                _offset = 0;
                break;
            }
        }
        return _reading_input;
    }

    void Cursor::update( double dms, ui::Input* input )
    {
        _input = input;

        if ( _reading_input )
        {
            /* Blink every 0.5 seconds */
            _timer += dms;
            if (_timer >= 0.5f)
            {
                _visible = !_visible;
                _timer = 0;
            }

            /* dont blick if -> selection is active, dragging to select range, or clicking to select range */
            if ( _input->keyDown() || _selection || _input->_pointer_dragging )
            {
                _visible = true;
                _timer = 0;
            }

            _reading_input = false;
        }
        /* if no longer reading input and in editing mode, disable editing mode */
        else if( !_reading_input && _editing )
        {
            _editing = false;
        }
    }


    void Cursor::insertChar( char* str, size_t& length, size_t pos, size_t count, char ch )
    {
        if (count == 0) return;

        if (pos > length)
        {
            pos = length; // clamp to end
        }

        size_t new_capacity = length + count;
        char* new_str = new char[new_capacity + 1];
        if (str)
        {
            std::memcpy(new_str, str, length + 1);
            delete[] str;
        }
        new_str[ new_capacity ] = 0;
        length = new_capacity;
        str = new_str;

        // Move existing tail to make room (including null terminator)
        std::memmove(str + pos + count, str + pos, length - pos + 1);

        // Fill the gap with `ch`
        for (size_t i = 0; i < count; ++i)
        {
            str[pos + i] = ch;
        }

        length = new_capacity;
    }
    void Cursor::eraseChar(char* str, size_t& length, size_t begin, size_t end)
    {
        if (begin >= length) return;

        // Clamp end
        if (end > length)
            end = length;

        // Nothing to remove
        if (begin >= end)
            return;

        const size_t removeCount = end - begin;
        const size_t tailCount   = length - end;

        // Shift the tail left
        if (tailCount > 0)
        {
            memmove(str + begin, str + end, tailCount);
        }

        length -= removeCount;
        str[length] = '\0';
    }


    Cursor::Cursor( ui::Layout* layout )
    :   _layout( layout )
    ,   _input( nullptr )
    ,   _position(0)
    ,   _anchor(0)
    ,   _offset(0)
    ,   _timer(0.0)
    ,   _x(0.0)
    ,   _y(0.0)
    ,   _height(15)
    ,   _width(2)
    ,   _selection_start(0)
    ,   _selection_end(0)
    ,   _visible(false)
    ,   _reading_input(false)
    ,   _editing(false)
    {}
}
