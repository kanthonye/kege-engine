//
//  gui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 12/8/25.
//

#ifndef gui_hpp
#define gui_hpp

#include "../../core/graphics/gui/ui-layout.hpp"
#include "../../core/graphics/gui/ui-viewer.hpp"

//#define MAKE_ID(func) ((((uint64_t)(uintptr_t)&main) << 32) | __LINE__)

namespace kege{

    template< typename T >
    struct RangeParams
    {
        T  min;
        T  max;
        T* val;
        ui::WidgetId widget_id;
        float length;
    };


    struct ListElem
    {
        ui::Text text;
        ui::UserId user_id;
    };

    enum class TextFieldMode
    {
        Idle,       // Not focused
        Focused,    // Focused but not editing
        Editing     // Actively receiving input
    };



    struct Theme
    {
        kege::ui::Style close_butn;
        kege::ui::Style panel;
        kege::ui::Style panel_transparnt;
        kege::ui::Style hierarchy_panel;
        kege::ui::Style viewport_panel;
        kege::ui::Style padded_list;

        kege::ui::Style list;
        kege::ui::Style list_elem;
        kege::ui::Style list_elem_focus;

        kege::ui::Style select;
        kege::ui::Style select_elem;
        kege::ui::Style select_elem_focus;

        kege::ui::Style label;
        kege::ui::Style fitted_label;

        kege::ui::Style radio;
        kege::ui::Style radio_active;
        kege::ui::Style button;
        kege::ui::Style disabled_button;

        kege::ui::Style slider_knob;
        kege::ui::Style slider_track;
        kege::ui::Style slider_name;
        kege::ui::Style slider_value;
        kege::ui::Style slider_row;
        kege::ui::Style slider_container;
        kege::ui::Style slider_inner_container;
        kege::ui::Style slider_range;

        kege::ui::Style slide_bar_track;
        kege::ui::Style slide_bar;
        kege::ui::Style slide_bar_value;

        kege::ui::Style row;
        kege::ui::Style row_wrap;
        kege::ui::Style column;
        kege::ui::Style x_seperator;
        kege::ui::Style y_seperator;

        kege::ui::Style numeric;
        kege::ui::Style numeric_focus;
        kege::ui::Style numeric_value;
        kege::ui::Style numeric_label;

        kege::ui::Style scrubber;
        kege::ui::Style scrubber_label;
        kege::ui::Style scrubber_focus;
        kege::ui::Style scrubber_row;

        kege::ui::Style collapsable_header;
        kege::ui::Style collapsable_element;
        kege::ui::Style collapsable_element_hl;
        kege::ui::Style collapsable_removable_header;

        kege::ui::Style container_header;
        kege::ui::Style container_body;

        kege::ui::Style card;

        kege::ui::Style folder_icon;
        kege::ui::Style selected_item;
        kege::ui::Style selection_box;
        kege::ui::Style file_icon;
        kege::ui::Style tooltip;

        kege::ui::Style option;
        kege::ui::Style option_selection;
        kege::ui::Style option_list;
        kege::ui::Style option_hot;
        kege::ui::Style option_element;

        kege::ui::Style navbar;
        kege::ui::Style filter_container;
        kege::ui::Style filter_input;
        kege::ui::Style broswer_file_path;
        kege::ui::Style file_container;


        kege::ui::Style text_field_editing;
        kege::ui::Style text_field_focus;
        kege::ui::Style text_field_idle;

        kege::ui::Style hierarchy_main_bar;
        kege::ui::Style hierarchy_entity;
        kege::ui::Style hierarchy_entity_selected;

        kege::ui::Style tab;
        kege::ui::Style tab_elem;
        kege::ui::Style tab_label;
        kege::ui::Style tab_elem_focus;

        kege::ui::Style dock;
        kege::ui::Style ghost;
        Theme();
    };



    struct ScrubberState
    {
        enum Type{FLOAT, DOUBLE, INT32, INT64};
        ui::Text text_value;
        //ui::WidgetHandle handle;
        char str[16];
        size_t size;
        TextFieldMode mode;
        float sensitivity;
        uint32_t last_frame_value;
        Type type;
        bool edited = false;
    };


    class GUI
    {
    public:

        struct TextField
        {
            ui::Cursor::InputType type;
            TextFieldMode& mode;
            //ui::WidgetHandle& field;
            bool& edited;
            char* buffer;
            size_t buffer_capacity;
            size_t& current_size;
        };

        struct TextFieldState
        {
            TextFieldMode mode;
            bool edited;
        };

        struct NumState
        {
            int mode;
            float num;
            size_t size;
            char str[16];
            uint32_t last_frame_value;
        };
        struct CollapsableRemovableHeader
        {
            ui::WidgetHandle uid[4];
            int mode;
            bool state;
            uint32_t last_frame_value;
        };

        struct CollapsableHeader
        {
            ui::WidgetHandle uid[3];
            bool state;
            uint32_t last_frame_value;
        };

        struct RemovableHeader
        {
            bool state;
            uint32_t last_frame_value;
        };

//        struct InputText
//        {
//            ui::WidgetHandle uid[2];
//            int mode;
//            uint32_t last_frame_value;
//        };

        struct InputNumeric
        {
            size_t size = 16;
            ui::WidgetHandle uid[3];
            char str[16];
            int mode = 0;
            uint32_t last_frame_value = 0;
        };

        struct SliderState
        {
            ui::WidgetHandle uid[2];
        };

        struct SliderUIState
        {
            ui::WidgetHandle uid[2];
            char str[3][16];
            ui::Text text[3];
        };

        struct RadioState
        {
            ui::WidgetHandle uid;
            bool state;
        };

        struct OptionState
        {
            ui::WidgetId index;
            int selection = 0;
            bool state;
        };

        struct ScrollContainer
        {
            uint64_t clipper;
            uint64_t scroller;
            float scroll_amount;
            int frame_id;
        };

        template<typename Params>void pushDeferredOp(ui::UserId user_id, ui::WidgetId index, ui::DeferredOperation fn, const Params& params)
        {
            _layout->pushDeferredOp< Params >( user_id, index, fn, params );
        }
        template<typename Params>void pushDeferredOpPtr(ui::UserId user_id, ui::WidgetId index, ui::DeferredOperation fn, Params* params)
        {
            _layout->pushDeferredOpPtr< Params >( user_id, index, fn, params );
        }

        template<typename T> uint64_t getAddressAsInt(const T& value)
        {
            return reinterpret_cast<uint64_t>(&value);
        }

        std::vector<ListElem>& getListState(ui::UserId user_id)
        {
            return _list_elements[user_id];
        }


        bool submit(ui::UserId user_id, const char* label);
        void text(const char* label, char* input, size_t& size, size_t capacity);
        void input(ui::UserId user_id, const char* label, double& value);


        int collapsableRemovableHeaderInput( ui::UserId user_id, int16_t layer, char* str, size_t& size );
        int collapsableRemovableHeader( ui::UserId user_id, int16_t layer, const ui::Text& text );
        bool collapsableHeader( ui::UserId user_id, int16_t layer, const ui::Text& text );
        int removableHeader( ui::UserId user_id, int16_t layer, const ui::Text& text );

        static void textFieldOp(ui::Layout* layout, ui::UserId user_id, ui::WidgetId index, void* data);
        bool textField( ui::UserId user_id, int16_t layer, char* buffer, size_t buffer_capacity, size_t& current_size );


        template< typename T >
        bool scrubber( ui::UserId user_id, int16_t layer, T& num, ScrubberState* state )
        {
            ui::WidgetId widget_index = _layout->put
            ({
                .user_id = user_id,
                .style = ( state->mode == TextFieldMode::Editing ) ? &_theme.scrubber_focus : &_theme.scrubber,
                .single_click = ui::ClickTrigger::Continuous,
                .double_click = ui::ClickTrigger::Immediate,
                .text = state->text_value,
            });

            bool active = false;
            if ( _layout->click( user_id ) && state->mode != TextFieldMode::Editing)
            {
                const kege::Mouse* mouse = _layout->inputManager()->getMouse();
                if ( mouse->moved() )
                {
                    if( 0.0 != mouse->getDelta().x )
                        num += mouse->getDelta().x * state->sensitivity;

                    if( 0.0 != mouse->getDelta().y )
                        num -= mouse->getDelta().y * state->sensitivity;
                    active = true;

                    const char* format = "";
                    switch(state->type)
                    {
                        default:
                        case ScrubberState::FLOAT:  format = "%.3f"; break;
                        case ScrubberState::DOUBLE: format = "%.3"; break;
                        case ScrubberState::INT32:  format = "%i"; break;
                        case ScrubberState::INT64:  format = "%i"; break;
                    }
                    snprintf(state->str, 16, format, num);
                    state->text_value = _layout->text(state->str, 20);
                }
            }
            else
            {
                _layout->pushDeferredOp(user_id, widget_index, textFieldOp, TextField
                {
                    .type = ui::Cursor::InputType::Numeric,
                    .edited = state->edited,
                    .mode = state->mode,
                    .buffer = state->str,
                    .current_size = state->size,
                    .buffer_capacity = 15,
                });
            }

            return active || state->edited;
        }
        bool scrubber( ui::UserId user_id, int16_t layer, int& num );
        bool scrubber( ui::UserId user_id, int16_t layer, float& num );
        bool scrubber( ui::UserId user_id, int16_t layer, double& num );
        bool scrubber( ui::UserId user_id, int16_t layer, const ui::Text& name, int& num );
        bool scrubber( ui::UserId user_id, int16_t layer, const ui::Text& name, float& num );
        bool scrubber( ui::UserId user_id, int16_t layer, const ui::Text& name, double& num );
        bool scrubber3( ui::UserId user_id, int16_t layer, const ui::Text& name, float& x, float& y, float& z );
        bool scrubber4( ui::UserId user_id, int16_t layer, const ui::Text& name, float& x, float& y, float& z, float& w );


        template< typename T >
        static void slidebarOp(ui::Layout* layout, ui::UserId user_id, ui::WidgetId widget_id, void* data)
        {
            //layout->elemParent(id)->uid
            RangeParams<T>* params = reinterpret_cast<RangeParams<T>*>(data);
            float length = layout->elem(widget_id)->rect.width;
            float& w = layout->elem(params->widget_id)->rect.width;

            // calculate where the slider should be (base on the numeric value) before moving it.
            w = (length * (*params->val - params->min)) / (params->max - params->min);

            if( layout->click( user_id ) )
            {
                // update slide bar width
                w += layout->getPointerDelta().x;
                // clamp slide bar width
                w = (w < 0)? 0.f : (w > length) ? length: w;
                // calculate numeric value base on slide bar width
                *params->val = (params->max - params->min) * (w / length) + params->min;
            }
            else if( layout->doubleClick( user_id ) )
            {
                // update slide bar width
                w = layout->getClickPosition(kege::MouseButtonCode::Left).x - layout->elem(widget_id)->rect.x;
                // clamp slide bar width
                w = (w < 0)? 0.f : (w > length) ? length: w;
                // calculate numeric value base on slide bar width
                *params->val = (params->max - params->min) * (w / length) + params->min;
            }
        }

        template< typename T >
        bool slidebar( uint64_t user_id, int16_t layer, T* val, T min, T max )
        {
            ui::WidgetId widget_index = _layout->push
            ({
                .layer = layer,
                .user_id = user_id,
                .style = &_theme.slide_bar_track,
                .single_click = ui::ClickTrigger::Continuous
            });
            _layout->put({ .layer = layer, .user_id = user_id + 1, .style = &_theme.slide_bar });
            _layout->pop();

            _layout->pushDeferredOp(user_id, widget_index, slidebarOp<T>, RangeParams<T>{min, max, val, user_id + 1});

            return _layout->click( user_id );
        }

        bool numeric( int16_t layer, ui::UserId user_id, double& num );

        bool options( ui::UserId user_id, int16_t layer, std::vector<ListElem>& list, int& selection );
        bool select( int16_t layer, std::vector<ListElem>& list, int& selection );
        void list( int16_t layer, std::vector<ListElem>& list, int selection = -1 );
        bool tab( int16_t layer, std::vector<ListElem>& list, int& selection );

        bool numSlideBar( ui::UserId user_id, int16_t layer, double* val, double min, double max );

        template< typename T >
        static void sliderOp(ui::Layout* layout, ui::UserId user_id, ui::WidgetId widget_id, void* data)
        {
            RangeParams<T>* params = reinterpret_cast<RangeParams<T>*>(data);
            const ui::Widget* track = layout->elemParent(widget_id);
            ui::Widget* knob = layout->elem(widget_id);

            float length = track->rect.width - knob->rect.width;

            // calculate where the slider should be (base on the numeric value) before moving it.
            float pos = (length * (*params->val - params->min)) / (params->max - params->min);

            if( layout->click( user_id ) )
            {
                // update slider position
                pos += layout->getPointerDelta().x;
                // keep the slider with in bound
                //pos = kege::min(length, kege::min(0.f, pos));
                pos = kege::clamp(pos, 0.f, length);
                // calculate the new numeric value base on the updated position
                *params->val = (params->max - params->min) * (pos / length) + params->min;
            }

            knob->rect.x += pos;
        }

        template< typename T >
        bool slider( uint64_t user_id, int16_t layer, T* val, T min, T max )
        {
            uint64_t butn_id = user_id + 1;
            _layout->push({ .layer = layer, .user_id = user_id, .style = &_theme.slider_track });
            ui::WidgetId widget_index = _layout->put
            ({
                .layer = layer, .user_id = butn_id,
                .style = &_theme.slider_knob,
                .single_click = ui::ClickTrigger::Continuous
            });
            _layout->pop();
            _layout->pushDeferredOp( butn_id, widget_index, sliderOp<T>, RangeParams<T>{min,max,val} );
            return _layout->click( butn_id );
        }

        bool button( ui::UserId user_id, int16_t layer, const ui::Text& text, ui::Style* style = nullptr );
        bool button( const kege::ui::WidgetDesc& desc );

        bool charButn( ui::UserId user_id, const char* label, float x, float y );
        bool dotButn( ui::UserId user_id, int16_t layer );

        ui::WidgetId fittedLabel( int16_t layer, const ui::Text& text, ui::Style* style = nullptr );
        ui::WidgetId label( int16_t layer, const ui::Text& text, ui::Style* style = nullptr );

        bool radio( ui::UserId user_id, int16_t layer, const ui::Text& text, bool& state );
        bool radio( ui::UserId user_id, int16_t layer, bool& state );
        bool radio( ui::UserId user_id, int16_t layer );

        ui::WidgetId beginList(int16_t layer);
        void endList();

        ui::WidgetId beginColumn( int16_t layer, ui::Style* style = nullptr );
        void endColumn();

        ui::WidgetId beginRow( int16_t layer, ui::Style* style = nullptr );
        void endRow();

        void beginScrollContainer( ui::UserId user_id, int16_t layer );
        void endScrollContainer();

        ui::WidgetId pushRoot( const ui::WidgetDesc& desc );
        ui::WidgetId putRoot( const ui::WidgetDesc& desc );
        void popRoot();

        ui::WidgetId push( const kege::ui::WidgetDesc& desc );
        ui::WidgetId put( const kege::ui::WidgetDesc& desc );
        void pop();

        /**
         * Checks if the mouse pointer is being dragged.
         *
         * @return true if the pointer is being dragged, false otherwise.
         */
        const bool pointerDragging() const;

        bool mouseover( uint64_t user_id );
        bool click( uint64_t user_id );
        bool hot( uint64_t user_id );
        bool leftClickDown()const;
        bool dragging()const;

        ui::Layout* layout()
        {
            return _layout.ref();
        }


        /**
         * Checks if geven point and rectangular shape intersects.
         *
         * @param p The given point to test the rectangle against.
         * @param rect The rectangle to test the point against.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        inline bool testPointVsRect( const kege::dvec2& p, const ui::Rect& rect )const
        {
            return _layout->testPointVsRect( p, rect );
        }


        inline bool intersect(const ui::Rect& m, const ui::Rect& n)const
        {
            return _layout->intersect( m, n );
        }

        ui::Widget* get( ui::WidgetId index );
        //kege::ui::Style* getStyle( int index );
        bool initialize(Ref< ui::Layout > layout);


        void createLayers( uint32_t quantity );
        
        vec2d deltaPointer()const;
        vec2d pointer()const;

        void begin( double dms );
        void end();

        ~GUI();
        GUI();





        kege::Ref< kege::ui::Layout > _layout;
        kege::Theme _theme;

        std::unordered_map<uint64_t, std::vector<ListElem>> _list_elements;

        std::unordered_map<uint64_t, CollapsableRemovableHeader> _collapsable_removable_headers;
        std::unordered_map<uint64_t, CollapsableHeader> _collapsable_headers;
        std::unordered_map<uint64_t, OptionState> _option_states;
        std::unordered_map<uint64_t, RemovableHeader> _removable_headers;
        std::unordered_map<uint64_t, InputNumeric> _numeric_states;
        std::unordered_map<uint64_t, ScrubberState> _scrubbers;
        std::unordered_map<uint64_t, TextFieldState> _input_texts;
        std::unordered_map<uint64_t, SliderState> _slider_states;
        std::unordered_map<uint64_t, SliderUIState> _slider_ui_states;
        std::unordered_map<uint64_t, RadioState> _radio_states;
        std::unordered_map<uint64_t, ScrollContainer> _scroll_containers;

        struct UIID
        {
            ui::WidgetHandle id;
            int count = 0;
        };

        std::vector< UIID > _uiids;
        uint32_t _frame_index;
        //kege::ui::Viewer _viewer;
        //kege::ui::Input _input;
    };



    template<typename T>
    class State{
    public:
        static T* get( ui::UserId user_id)
        {
            auto itr = _states.find(user_id);
            if( itr != _states.end() )
            {
                return &_states[user_id];
                //itr->second;
            }
            return &itr->second;
        }
    private:

        static std::unordered_map<ui::UserId, T> _states;
    };

    template<typename T>
    std::unordered_map<ui::UserId, T> State<T>::_states;
}

#endif /* gui_hpp */
