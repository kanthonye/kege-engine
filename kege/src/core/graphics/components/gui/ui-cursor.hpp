//
//  ui-cursor.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/14/25.
//

#ifndef ui_cursor_hpp
#define ui_cursor_hpp

#include "../../../input/input-manager.hpp"
#include "../font/font.hpp"
#include "ui-core.hpp"
#include "ui-aligner.hpp"
#include "ui-style-manager.hpp"

namespace kege::ui{

    class Cursor
    {
    public:

        enum class InputType
        {
            Any,      // Accept any characters
            Numeric,  // Only numbers, decimal point, minus sign
            Integer,  // Only numbers and minus sign
            Alpha,    // Only letters
            Custom    // Custom validation
        };

        struct EditingState
        {
            ui::ID user_id;
            char* buffer = nullptr;
            size_t* buffer_size = nullptr;
            size_t buffer_capacity = 0;

            InputType type;

            bool active = false;
        };

        void startEditing
        (
            const ui::ID user_id,
            const WidgetId& widget_id,
            InputType type,
            char* buffer,
            size_t& buffer_size,
            size_t buffer_capacity
        );

        void selectAll(const ui::WidgetId& widget_id, const char* str, size_t size);
        void computeCursorPosition(const ui::WidgetId& widget_id);
        void stopEditing();
        bool update(const ui::WidgetId& widget_id);

        const ui::Rect& getSelectionRect()const;
        uint32_t getCursorColor()const;
        bool isSelectionActive()const;
        bool isVisible()const;
        Cursor( kege::GUI* gui );
        
    private:


        void handleKeyInput
        (
            const kege::Key& key,
            InputType type,
            char*& buffer, size_t& buffer_size, size_t& buffer_capacity,
            const ref::Font& font,
            int font_size,
             const ui::WidgetId& widget_id
        );

        void insertCharacter
        (
            char*& buffer,
            size_t& buffer_size,
            size_t& buffer_capacity,
            char ch,
            const ref::Font& font,
            int font_size
        );
        
        int insertString
        (
            char*& buffer,
            size_t& buffer_size,
            size_t& buffer_capacity,
            size_t pos,
            const char* chars,
            size_t count
        );

        struct PositionFromClick
        {
            size_t pos;
            float length;
        };

        void handleClickAndSelection(const ui::WidgetId& widget_id, Widget* widget);
        void processKeyboardInputs(const ui::WidgetId& widget_id, int font_size);

        void deleteSelection(char*& str, size_t& size, const ref::Font& font, int font_size);
        void eraseRange(char*& str, size_t& size, size_t begin, size_t end);
        bool validateCharacter(InputType type, int ch, const char* str, size_t pos) const;

        PositionFromClick getPositionFromClick
        (
            float click_x,
            const ref::Font& font,
            int font_size,
            const char* str,
            size_t size
        )
        const;

        void updateBlinker();

    private:

        const kege::Keyboard* _keyboard;
        const kege::Mouse* _mouse;

        EditingState _current_edit;

        ui::Rect _rect_selection;
        ui::Rect _rect_cursor;

        uint32_t _color_selection;
        uint32_t _color_cursor;

        /**
         * @var _layout: The Layout this cursor is associated with.
         */
        kege::GUI* _gui;

        Cursor::PositionFromClick _click;

        /**
         * @var _position: The character position/index with in the string.
         */
        size_t _position;

        size_t _selection_anchor;

        float _selection_end;

        /**
         * @var _timer: The blink timer of the cursor.
         */
        double _timer;

        bool _selection_active;
        bool _blink_enabled;
        
        /**
         * @var _visible: Indicate whether the cursor should be drawn or not. This
         * allows the implementation for blinking.
         */
        bool _visible;


        bool _initial_click_processed;

        bool _control;
        bool _caplock;
        bool _shift;
        bool _super;
        bool _alt;
        bool _tab;

        friend ui::Layout;
        friend ui::Renderer;
    };
}
#endif /* ui_cursor_hpp */
