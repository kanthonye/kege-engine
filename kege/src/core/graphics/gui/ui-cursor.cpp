//
//  ui-cursor.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/14/25.
//

#include "ui-cursor.hpp"
#include "ui-layout.hpp"

namespace kege::ui{

    bool Cursor::update(const ui::WidgetId& widget_id)
    {
        if (!widget_id.id) return false;

        Widget* widget = _layout->elem(widget_id);
        if (!widget) return false;

        updateBlinker();
        handleClickAndSelection(widget_id, widget);
        processKeyboardInputs(widget_id, widget->text.font_size);

        return _current_edit.active;
    }

    void Cursor::handleClickAndSelection(const ui::WidgetId& widget_id, Widget* widget)
    {
        // Handle mouse interaction
        if ( _mouse->isDown(MouseButtonCode::Left) )
        {
            if ( !_initial_click_processed )
            {
                computeCursorPosition( widget_id );
            }

            if ( _mouse->moved() )
            {
                float drag_x = _mouse->getPosition().x - widget->rect.x;
                Cursor::PositionFromClick drag = getPositionFromClick(drag_x, _layout->_font, widget->text.font_size, _current_edit.buffer, *_current_edit.buffer_size);

                if (drag.pos != _position)
                {
                    _position = drag.pos;
                    _selection_active = true;
                    _visible = true;

                    float min_length = kege::min(drag.length, _click.length);

                    _rect_selection.x = widget->rect.x + min_length;
                    _rect_selection.y = _rect_cursor.y;
                    _rect_selection.height = _rect_cursor.height;
                    _rect_selection.width = kege::max(drag.length, _click.length) - kege::min(drag.length, _click.length);

                    _selection_end = _rect_selection.x + _rect_selection.width;
                }
            }
        }
        else if (_initial_click_processed)
        {
            _initial_click_processed = false;
        }
    }

    void Cursor::processKeyboardInputs(const ui::WidgetId& widget_id, int font_size)
    {
        Key const* keys = _keyboard->getActiveKeys();
        int key_count = _keyboard->getActiveKeyCount();
        // Process keyboard input
        for (int i = 0; i < key_count; ++i)
        {
            handleKeyInput
            (
                keys[i],
                _current_edit.type,
                _current_edit.buffer,
                *_current_edit.buffer_size,
                _current_edit.buffer_capacity,
                _layout->_font,
                font_size, widget_id
            );
        }
    }

    void Cursor::handleKeyInput
    (
        const kege::Key& key,
        InputType type,
        char*& buffer,
        size_t& buffer_size,
        size_t& buffer_capacity,
        const ref::Font& font,
        int font_size,
        const ui::WidgetId& widget_id
    )
    {
        if (key.action == KeyState::Release)
        {
            switch (key.code)
            {
                case kege::KEY_CAPS_LOCK:
                case kege::KEY_LEFT_SHIFT:
                case kege::KEY_RIGHT_SHIFT:
                case kege::KEY_LEFT_ALT:
                case kege::KEY_RIGHT_ALT:
                case kege::KEY_LEFT_CONTROL:
                case kege::KEY_RIGHT_CONTROL:
                case kege::KEY_LEFT_SUPER:
                case kege::KEY_RIGHT_SUPER:
                    break;

                default: return;
            }
        }

        _visible = true;
        _timer = 0;

        switch (key.code)
        {
            case kege::KEY_BACKSPACE:
                if (_selection_active)
                {
                    deleteSelection(buffer, buffer_size, font, font_size);
                }
                else if (_position > 0)
                {
                    _rect_cursor.x -= font->getCharWidth(font_size, buffer[_position - 1]);
                    eraseRange(buffer, buffer_size, _position - 1, _position);
                    _position--;
                }
                break;

            case kege::KEY_DELETE:
                if (_selection_active)
                {
                    deleteSelection(buffer, buffer_size, font, font_size);
                }
                else if (_position < buffer_size)
                {
                    eraseRange(buffer, buffer_size, _position, _position + 1);
                }
                break;

            case kege::KEY_LEFT:
                if (_position > 0)
                {
                    if (key.mods == Modifiers::Shift && !_selection_active)
                    {
                        _selection_anchor = _position;
                        _selection_active = true;
                    }
                    else if (key.mods != Modifiers::Shift)
                    {
                        if(_selection_active)
                        {
                            _selection_active = false;
                            _rect_cursor.x = _rect_selection.x;
                            _position = _selection_anchor;
                            break;
                        }
                    }

                    _position--;
                    _rect_cursor.x -= font->getCharWidth(font_size, buffer[_position]);
                }
                break;

            case kege::KEY_RIGHT:
                if (_position < buffer_size)
                {
                    if (key.mods == Modifiers::Shift && !_selection_active)
                    {
                        _selection_anchor = _position;
                        _selection_active = true;
                    }
                    else if (key.mods != Modifiers::Shift)
                    {
                        if(_selection_active)
                        {
                            _rect_cursor.x = _rect_selection.x + _rect_selection.width;
                            //_position = _selection_anchor;
                            _selection_active = false;
                            break;
                        }
                    }

                    _rect_cursor.x += font->getCharWidth(font_size, buffer[_position]);
                    _position++;
                }
                else if(_selection_active)
                {
                    _rect_cursor.x = _rect_selection.x + _rect_selection.width;
                    _selection_active = false;
                }
                break;

            case kege::KEY_HOME:
                _position = 0;
                _rect_cursor.x = 0;
                if (key.mods != Modifiers::Shift) _selection_active = false;
                break;

            case kege::KEY_END:
                _position = buffer_size;
                //_rect.x = getXOffsetForPosition(font, font_size, buffer, size);
                if (key.mods != Modifiers::Shift) _selection_active = false;
                break;

            case kege::KEY_ENTER:
            case kege::KEY_ESCAPE:
                stopEditing();
                break;

            case kege::KEY_CAPS_LOCK:
                _caplock = key.action != KeyState::Release;
                break;

            case kege::KEY_LEFT_SHIFT:
            case kege::KEY_RIGHT_SHIFT:
                _shift = key.action != KeyState::Release;
                break;

            case kege::KEY_LEFT_ALT:
            case kege::KEY_RIGHT_ALT:
                _alt = key.action != KeyState::Release;
                break;

            case kege::KEY_LEFT_CONTROL:
            case kege::KEY_RIGHT_CONTROL:
                _control = key.action != KeyState::Release;
                break;

            case kege::KEY_LEFT_SUPER:
            case kege::KEY_RIGHT_SUPER:
                _super = key.action != KeyState::Release;
                break;

            case kege::KEY_A:
                if (_control || _super) // Ctrl+A to select all
                {
                    selectAll(widget_id, buffer, buffer_size);
                }
                else
                {
                    // Handle regular 'A' key
                    int ch = _caplock ? 'A' : 'a';
                    if (validateCharacter(type, ch, buffer, _position))
                    {
                        insertCharacter(buffer, buffer_size, buffer_capacity, ch, font, font_size);
                    }
                }
                break;

            default:
            {
                int ch = key.code;
                // Handle character input
                if (validateCharacter(type, ch, buffer, _position))
                {
                    // Handle caps lock
                    if (!_shift && !_caplock)
                    {
                        ch = std::tolower(ch);
                        //ch = std::isupper(ch) ? std::tolower(ch) : std::toupper(ch);
                    }

                    if (ch != 0)
                    {
                        if (_selection_active)
                        {
                            deleteSelection(buffer, buffer_size, font, font_size);
                        }
                        insertCharacter(buffer, buffer_size, buffer_capacity, ch, font, font_size);
                    }
                }
                break;
            }
        }
    }

    void Cursor::insertCharacter
    (
        char*& buffer,
        size_t& buffer_size,
        size_t& buffer_capacity,
        char ch,
        const ref::Font& font,
        int font_size
    )
    {
        if (buffer_size >= buffer_capacity) return;

        insertString(buffer, buffer_size, buffer_capacity, _position, &ch, 1);
        _rect_cursor.x += font->getCharWidth(font_size, ch);
        _position++;
    }

    void Cursor::deleteSelection(char*& str, size_t& size, const ref::Font& font, int font_size)
    {
        if (!_selection_active) return;

        size_t start = std::min(_position, _selection_anchor);
        size_t end = std::max(_position, _selection_anchor);

        if(_selection_active)
        {
            _rect_cursor.x = _rect_selection.x + _rect_selection.width;
        }
        // Adjust cursor position and visual offset
        for (size_t i = start; i < end; ++i)
        {
            _rect_cursor.x -= font->getCharWidth(font_size, str[i]);
        }

        eraseRange(str, size, start, end);
        _position = start;
        _selection_active = false;
    }

    int Cursor::insertString
    (
        char*& buffer,
        size_t& buffer_size,
        size_t& buffer_capacity,
        size_t pos,
        const char* chars,
        size_t count
    )
    {
        // Validate parameters
        if (!buffer || buffer_capacity == 0) return -1;
        if (!chars && count > 0) return -1;
        // Check buffer size doesn't exceed capacity
        if (buffer_size >= buffer_capacity) return -1;

        // Calculate new size
        size_t new_size = buffer_size + count;

        // Check if we have enough capacity (including null terminator)
        if (new_size >= buffer_capacity) return -1;

        if (pos > buffer_size) pos = buffer_size;

        // Special case: appending at the end
        if (pos == buffer_size)
        {
            memcpy(buffer + buffer_size, chars, count);
            buffer[new_size] = '\0';
            buffer_size = new_size;
            return 0;
        }

        // Special case: inserting at the beginning
        if (pos == 0)
        {
            // Move entire existing content
            memmove(buffer + count, buffer, buffer_size + 1); // +1 for null term
            memcpy(buffer, chars, count);
            buffer_size = new_size;
            return 0;
        }

        // General case: insert in the middle
        // Move the tail portion (from pos to end)
        char* tail_start = buffer + pos;
        size_t tail_length = buffer_size - pos + 1; // +1 for null term

        memmove(tail_start + count, tail_start, tail_length);

        // Copy the new characters
        memcpy(tail_start, chars, count);

        buffer_size = new_size;
        return 0;
    }

    void Cursor::updateBlinker()
    {
        if ( !_current_edit.active ) return;

        /* dont blick if -> selection is active, dragging to select range, or clicking to select range */
        if ( _selection_active || _mouse->isDragging(MouseButtonCode::Left) || _keyboard->anyKeyDown() )
        {
            _blink_enabled = false;
            _timer = 0;
        }
        else
        {
            _blink_enabled = true;
        }

        if ( _blink_enabled )
        {
            /* Blink every 0.5 seconds */
            _timer += _layout->_dms;
            if (_timer >= 0.5f)
            {
                _visible = !_visible;
                _timer = 0;
            }
        }
    }
    
    Cursor::PositionFromClick Cursor::getPositionFromClick(float click_x, const ref::Font& font, int font_size, const char* str, size_t size) const
    {
        if (size == 0) return {0,0.f};

        float accumulated_width = 0.0f;
        size_t pos = 0;

        for (size_t i = 0; i < size && str[i] != '\0'; ++i)
        {
            float char_width = font->getCharWidth(font_size, str[i]);
            float half_width = char_width * 0.5f;

            // If click is past the midpoint of this character, move to next position
            if (click_x > accumulated_width + half_width)
            {
                pos = i + 1;
            }

            if (click_x <= accumulated_width + half_width)
            {
                break;
            }

            accumulated_width += char_width;
        }

        return Cursor::PositionFromClick{std::min(pos, size), accumulated_width};
    }

//    float Cursor::getClickToCursorOffset( const WidgetHandle& uid, const ref::Font& font, int font_size, const char* str, size_t& size )
//    {
//        if ( size == 0 ) return 0;
//        float length = 0;
//
//        _position = 0;
//        float char_width;
//        float max_length = _layout->_input->_current_position.x - _layout->elem( uid )->rect.x;
//        for ( const char* s = str; *s != 0; ++s )
//        {
//            char_width = font->getCharWidth( font_size, *s );
//            if (length + char_width > max_length )
//            {
//                break;
//            }
//            length += char_width;
//            _position += 1;
//        }
//
//        return length;
//    }

    void Cursor::eraseRange(char*& str, size_t& size, size_t begin, size_t end)
    {
        if (begin >= size || begin >= end) return;

        end = std::min(end, size);
        size_t remove_count = end - begin;

        // Shift characters left
        std::memmove(str + begin, str + end, size - end + 1); // +1 for null terminator
        size -= remove_count;
    }

    bool Cursor::validateCharacter(InputType type, int ch, const char* str, size_t pos) const
    {
        switch (type)
        {
            case InputType::Numeric:
                if (ch == '-') return pos == 0 && strchr(str, '-') == nullptr;
                if (ch == '.') return strchr(str, '.') == nullptr;
                return std::isdigit(ch);

            case InputType::Integer:
                if (ch == '-') return pos == 0 && strchr(str, '-') == nullptr;
                return std::isdigit(ch);

            case InputType::Alpha:
                return std::isalpha(ch);

            case InputType::Any:
                return ch >= 32 && ch <= 126; // Printable ASCII

            default:
                return false;
        }
    }

    void Cursor::startEditing(const ui::ID user_id, const WidgetId& widget_id, InputType type, char* buffer, size_t& buffer_size, size_t buffer_capacity)
    {
        Widget* widget = _layout->elem(widget_id);
        if (!widget) return;

        if (widget->rect.height > 0) _rect_cursor.height = widget->rect.height;
        _rect_cursor.x = widget->rect.x;
        _rect_cursor.y = widget->rect.y;

        _current_edit.user_id = user_id;
        _current_edit.buffer_capacity = buffer_capacity;
        _current_edit.buffer_size = &buffer_size;
        _current_edit.buffer = buffer;
        _current_edit.active = true;

        _position = buffer_size;
        _selection_active = false;
    }

    void Cursor::stopEditing()
    {
        _current_edit.active = false;
        _selection_active = false;
        _blink_enabled = false;
        _visible = false;
    }
    
    void Cursor::selectAll(const ui::WidgetId& widget_id, const char* str, size_t size)
    {
        _selection_anchor = 0;
        _position = size;
        _selection_active = true;
        _visible = true;

        Widget* widget = _layout->elem(widget_id);
        if (widget)
        {
            Cursor::PositionFromClick result = getPositionFromClick(widget->rect.x + widget->rect.width, _layout->_font, widget->text.font_size, str, size);
            _rect_selection.x = widget->rect.x;
            _rect_selection.y = widget->rect.y;
            _rect_selection.height = widget->rect.height;
            _rect_selection.width = result.length;
        }
    }

    void Cursor::computeCursorPosition(const ui::WidgetId& widget_id)
    {
        Widget* widget = _layout->elem(widget_id);
        if (!widget) return;

        float click_x = _mouse->getPosition().x - widget->rect.x;
        _click = getPositionFromClick(click_x, _layout->_font, widget->text.font_size, _current_edit.buffer, *_current_edit.buffer_size);
        _position = _click.pos;
        _selection_anchor = _position;
        _selection_active = false;
        _initial_click_processed = true;

        _rect_cursor.x = widget->rect.x + _click.length;
    }

    Cursor::Cursor( ui::Layout* layout )
    :   _layout( layout )
    ,   _keyboard( layout->_input_manager->getKeyboard() )
    ,   _mouse( layout->_input_manager->getMouse() )
    ,   _position(0)
    ,   _timer(0.0)
    ,   _rect_cursor{0.f, 0.f, 2.f, 15.f}
    ,   _rect_selection{0.f, 0.f, 2.f, 15.f}
    ,   _color_cursor(0xFFFFFFFF)
    ,   _color_selection(0xFFFFFF40)
    ,   _selection_end(0)
    ,   _visible(false)
    ,   _current_edit{}
    {}
}
