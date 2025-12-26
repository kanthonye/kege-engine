//
//  ui-cursor.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/14/25.
//

#ifndef ui_cursor_hpp
#define ui_cursor_hpp

#include "../font/font.hpp"
#include "ui-input.hpp"
#include "ui-core.hpp"
#include "ui-elem.hpp"
#include "ui-aligner.hpp"
#include "ui-style-manager.hpp"
#include "ui-widget-manager.hpp"

namespace kege::ui{

    class Cursor
    {
    private:

        float getClickToCursorOffset( const UID& elem, const kege::string& text, int font_size, const ref::Font& font );
        bool onInput(Input::Type type, const ref::Font& font, int font_size, const kege::Input& input, kege::string& text );
        void deleteSelection( int font_size, const ref::Font& font, kege::string& text );
        void handleDeletion( int font_size, const ref::Font& font, kege::string& text );

    public:

        bool onInput(Input::Type type, const UID& elem, const ref::Font& font, kege::string* text);
        void update( double dms, ui::Input* input );
        Cursor( ui::Layout* layout );

    private:

        /**
         * @var _layout: The Layout this cursor is associated with.
         */
        ui::Layout* _layout;

        /**
         * @var _input: The Input object this cursor reads from.
         */
        ui::Input* _input;

        /**
         * @var _position: The character position/index with in the string.
         */
        size_t _position;
        size_t _anchor;

        float _selection_start;
        float _selection_end;

        /**
         * @var _offset: The x position offset of the cursor.
         */
        double _offset;

        /**
         * @var _timer: The blink timer of the cursor.
         */
        double _timer;

        /**
         * @var _x: The x position of the cursor.
         */
        double _x;

        /**
         * @var _y: The y position of the cursor.
         */
        double _y;

        /**
         * @var _height: The height of the cursor.
         */
        float _height;

        /**
         * @var _width: The width of the cursor.
         */
        float _width;

        /**
         * @var _visible: Indicate whether the cursor should be drawn or not. This
         * allows the implementation for blinking.
         */
        bool _visible;

        /**
         * @var _reading_input: Indicate whether this object is processing inputs.
         */
        bool _reading_input;

        /**
         * @var _editing: Indicate that this cursor is currently modifying a string.
         */
        bool _editing;

        bool _initial_click;
        bool _selection;

        friend ui::Layout;
        friend ui::Viewer;
    };
}
#endif /* ui_cursor_hpp */
