//
//  ui-cursor.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/14/25.
//

#include "ui-cursor.hpp"
#include "ui-layout.hpp"

namespace kege::ui{

    float Cursor::getClickToCursorOffset( const ui::Elem& elem, const kege::string& text, int font_size, const ref::Font& font )
    {
        uint32_t node_index = _layout->_widget_manager.getNodeIndex( elem._handle );
        float length = 0;

        _position = 0;
        float char_width;
        float max_length = _input->_curr_frame.position.x - _layout->_nodes[ node_index ].widgit->rect.x;
        for ( const char* s = text.c_str(); *s != 0; ++s )
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

    void Cursor::deleteSelection( int font_size, const ref::Font& font, kege::string& text )
    {
        if (_anchor < _position)
        {
            for ( size_t i = _anchor; i < _position; ++i )
            {
                _offset -= font->getCharWidth( font_size, text[i] );
            }
            text.erase( _anchor, _position );
            _position = _anchor;
        }
        else if (_position < _anchor)
        {
            for ( size_t i = _position; i < _anchor; ++i )
            {
                _offset -= font->getCharWidth( font_size, text[i] );
            }
            text.erase( _position, _anchor );
            _anchor = _position;
        }
        _selection = false;
    }

    void Cursor::handleDeletion( int font_size, const ref::Font& font, kege::string& text )
    {
        if ( _selection )
        {
            deleteSelection( font_size, font, text );
        }
        else if( 0 < _position )
        {
            _position -= 1;
            _offset -= font->getCharWidth( font_size, text[ _position ]);
            text.erase( _position, _position + 1 );
        }
    }

    bool Cursor::onInput(Input::Type type, const ref::Font& font, int font_size, const kege::Input& input, kege::string& text)
    {
        if ( !_reading_input ) return false;

        switch ( input.key.code )
        {
            case kege::KEY_BACKSPACE:
            {
                if( input.key.state != 0 )
                {
                    handleDeletion( font_size, font, text );
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
                    handleDeletion( font_size, font, text );
                }
                break;
            }

            case kege::KEY_ENTER:
            {
                if ( input.key.state != 0 )
                {
                    _reading_input = false;
                    _position = static_cast<uint32_t>( text.length() );
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
                            _offset -= font->getCharWidth( font_size, text[i] );
                        }
                        _position = kege::min(_anchor, _position);
                        _selection = false;
                    }
                    else if ( _position > 0 )
                    {
                        size_t index = ( _position == text.length() ) ? _position - 1: _position;
                        _offset -= font->getCharWidth( font_size, text[ index ]);
                        _position -= 1;
                    }
                }
                break;
            }

            case kege::KEY_RIGHT:
            {
                if ( input.key.state != 0 && _position < text.length() )
                {
                    if ( _selection )
                    {
                        for ( size_t i = _anchor; i < _position; ++i )
                        {
                            _offset += font->getCharWidth( font_size, text[i] );
                        }
                        _anchor = _position;
                        _selection = false;
                    }
                    else
                    {
                        _offset += font->getCharWidth( font_size, text[ _position ]);
                        _position += 1;
                    }
                }
                break;
            }

            case kege::KEY_HOME:
            {
                if ( _position > text.length() )
                {
                    _position = 0;
                }
                break;
            }

            case kege::KEY_END:
            {
                if ( _position > text.length() )
                {
                    _position = static_cast<uint32_t>( text.length() );
                }
                break;
            }

            default:
            {
                if ( input.key.state != 0 )
                {
                    if ( _selection )
                    {
                        deleteSelection( font_size, font, text );
                    }

                    switch ( type )
                    {
                        case 0:
                        {
                            if ( _input->_shift )
                            {
                                // Insert the character at the cursor position
                                text.insert( _position, 1, _input->_keymap[ input.key.code ].shifted );
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
                                text.insert( _position, 1, _input->_keymap[ input.key.code ].shifted );
                            }
                            else
                            {
                                text.insert( _position, 1, _input->_keymap[ input.key.code ].normal );
                            }
                            break;
                        }

                        case 1:
                        {
                            if ((input.key.code >= '0' && input.key.code <= '9') || input.key.code == '.' || input.key.code == '-')
                            {
                                // Ensure only one decimal point
                                if (input.key.code == '.' && text.find('.') )
                                {
                                    break;
                                }

                                // Ensure only one negative sign at the beginning
                                if (input.key.code == '-' && (_position != 0 || text.find('-') ))
                                {
                                    break;
                                }

                                // Insert the character at the cursor position
                                text.insert( _position, 1, _input->_keymap[ input.key.code ].normal );
                            }
                            break;
                        }
                    }

                    _offset += font->getCharWidth( font_size, text[ _position ]);
                    _position++;
                }
                break;
            }
        }
        return _reading_input;
    }

    bool Cursor::onInput(Input::Type type, const ui::Elem& elem, const ref::Font& font, kege::string* text)
    {
        int font_size = _layout->_widget_manager[ elem._handle ].style->font_size;
        if (elem->rect.height > 0) _height = elem->rect.height;
        _x = elem->rect.x;
        _y = elem->rect.y;
        _reading_input = true;

        /**
         On initial click, compute the offset where the cursor should be
         */
        if ( _input->buttonDown() || !_editing )
        {
            if( !_initial_click )
            {
                _offset = getClickToCursorOffset( elem, *text, font_size, font );
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

        if( _input->_last_frame.pointer_dragging && _editing )
        {
            _selection_end = getClickToCursorOffset( elem, *text, font_size, font );
            _selection = true;
        }

        /**
         Process input entries
         */
        for (int i = 0; i < _input->_key_count; ++i )
        {
            if( !onInput( type, font, font_size, _input->_keyboard_keys[i], *text ) )
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
            if ( _input->keyDown() || _selection || _input->_last_frame.pointer_dragging )
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
