//
//  ui.hpp
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
        kege::ui::Style scroll_container;
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
        kege::ui::Style card2;

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
        kege::ui::Style group;
        Theme();
    };



    struct ScrubberState
    {
        uint32_t last_frame_value;


        enum Type{F32, F64, I32, I64, U32, U64};
        ui::Text text_value;

        char str[16];
        size_t size;

        TextFieldMode mode = TextFieldMode::Idle;
        float sensitivity;

        Type type;

        bool editing = false;
        bool edited = false;
        bool clamp = false;
    };


    class UI
    {
    public:

        struct TextField
        {
            ui::Cursor::InputType type;
            TextFieldMode& mode;

            size_t str_capacity;
            size_t& str_len;
            const char* str;
            bool& editing;
            bool& edited;
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

    public:

        bool input(const ui::ID& user_id, ui::Cursor::InputType type, TextFieldMode& mode, ui::Text& text);

        void labelScrubber(const ui::ID& user_id, const char* label, double& value);

        template<typename T>
        void labelScrubber
        (
            ScrubberState::Type type,
            const ui::ID& user_id,
            const kege::ui::Text& label,
            T& value,
            bool clamp = false,
            const T& min = 0,
            const T& max = 1
        )
        {
            _layout->push
            ({
                .padding = {10,5,10,5},
                //.border.corner_curves = {border_radius,0,0,border_radius},
                .color = 0xFFFFFF08,
                .gap = {2,2},
                .alignment =
                {
                    .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                    .direction = ui::AlignDir::RIGHT,
                    .items = ui::AlignItem::CENTER,
                },
                .width = ui::extend(),
                .height = ui::fixed(30),
            });
            _layout->text(label);
            this->scrubber< T >( type, user_id, value, clamp, min, max );
            _layout->pop();
        }

        bool labelInput(const char* label, const ui::ID& user_id, TextFieldMode& mode, ui::Text& text);

        void beginWindow(const ui::ID uid[3], ui::Rect& rect, const char* title, bool& close_window);
        void endWindow();

        template<typename Params>void pushDeferredOp(const ui::ID& id, ui::WidgetId index, ui::DeferredOperation fn, const Params& params)
        {
            _layout->pushDeferredOp< Params >( id, index, fn, params );
        }

        template<typename Params>void pushDeferredOpPtr(const ui::ID& id, ui::WidgetId index, ui::DeferredOperation fn, Params* params)
        {
            _layout->pushDeferredOpPtr< Params >( id, index, fn, params );
        }

        template<typename T> uint64_t getAddressAsInt(const T& value)
        {
            return reinterpret_cast<uint64_t>(&value);
        }

        bool submit(ui::ID user_id, const char* label);
        bool clickButton( ui::ID user_id, const ui::Text& text );


        bool button( ui::ID user_id, const ui::Text& text, const ui::Style* style = nullptr );
        bool button( const kege::ui::WidgetDesc& desc );


        bool collapsableHeader( const ui::ID& id, bool& expand, const ui::Text& text );
        bool checkbox( const ui::ID& id, const ui::Text& text, bool& expand );

        int collapsableRemovableHeaderInput( const ui::UID& uid, char* str, size_t& size );
        int collapsableRemovableHeader( const ui::UID& uid, const ui::Text& text );
        bool collapsableHeader( const ui::UID& uid, const ui::Text& text );
        int removableHeader( const ui::ID Id[4], const ui::Text& text );

        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        // ---  SCRUBBER                                                               ---
        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

        template< typename T >
        bool scrubber( const ui::ID& id, T& num, const T& min, const T& max, ScrubberState* state )
        {
            state->text_value.ptr = state->str;
            ui::WidgetId widget_id = _layout->put
            ({
                .user_id = id,
                .style = ( state->mode == TextFieldMode::Editing ) ? &_theme.scrubber_focus : &_theme.scrubber,
                .single_click = ui::ClickTrigger::Continuous,
                .double_click = ui::ClickTrigger::Immediate,
                .text = state->text_value,
            });

            bool active = false;
            if ( _layout->click( id ) && state->mode != TextFieldMode::Editing)
            {
                const kege::Mouse* mouse = _layout->inputManager()->getMouse();
                if ( mouse->moved() )
                {
                    if( 0.0 != mouse->getDelta().x )
                        num += mouse->getDelta().x * state->sensitivity;

                    if( 0.0 != mouse->getDelta().y )
                        num -= mouse->getDelta().y * state->sensitivity;
                    active = true;

                    if(state->clamp)
                    {
                        num = kege::clamp(num, min, max);
                    }

                    const char* format = "";
                    switch(state->type)
                    {
                        default:
                        case ScrubberState::F32: format = "%.3f"; break;
                        case ScrubberState::F64: format = "%.3f"; break;
                        case ScrubberState::I32: format = "%i"; break;
                        case ScrubberState::I64: format = "%i"; break;
                        case ScrubberState::U32: format = "%i"; break;
                        case ScrubberState::U64: format = "%i"; break;
                    }
                    snprintf(state->str, 16, format, num);
                    state->text_value = _layout->text(state->str, 20);
                    state->size = strlen(state->str);
                }
            }
            else
            {
                _layout->pushDeferredOp(id, widget_id, editTextOp, TextField
                {
                    .type = ui::Cursor::InputType::Numeric,
                    .mode = state->mode,
                    .edited = state->edited,
                    .editing = state->editing,
                    .str = state->str,
                    .str_len = state->size,
                    .str_capacity = 15
                });
            }

            return active || state->edited;
        }

        template< typename T >
        bool scrubber( ScrubberState::Type type, const ui::ID& id, T& num, bool clamp = false, const T& min = 0, const T& max = 1 )
        {
            ScrubberState* state;
            auto i = _scrubbers.find( id );
            if (i == _scrubbers.end())
            {
                state = &_scrubbers[ id ];
                const char* format = "";
                switch(type)
                {
                    default:
                    case ScrubberState::F32: format = "%.3f"; break;
                    case ScrubberState::F64: format = "%.3f"; break;
                    case ScrubberState::I32: format = "%i"; break;
                    case ScrubberState::I64: format = "%i"; break;
                }
                snprintf(state->str, 16, format, num);
                state->text_value = _layout->text(state->str, 20);
                state->text_value.color = 0xFFFFFFFF;
                state->sensitivity = 0.002;
                state->type = type;
                state->clamp = clamp;
            }
            else state = &i->second;
            return scrubber( id, num, min, max, state );
        }

        template< typename T >
        bool scrubber( ScrubberState::Type type, const ui::ID& id, T& num )
        {
            return scrubber<T>( type, id, num, false );
        }

        bool scrubber
        (
            const ui::ID& id,
            double& num,
            bool clamp = false,
            const double& min = 0.0,
            const double& max = 1.0
        )
        {
            return scrubber< double >( ScrubberState::F64, id, num, clamp, min, max );
        }
        bool scrubber
        (
            const ui::ID& id,
            int16_t layer,
            float& num,
            bool clamp = false,
            float min = 0.0,
            float max = 1.0
        )
        {
            return scrubber< float >( ScrubberState::F32, id, num, min, max, clamp );
        }
        bool scrubber
        (
            const ui::ID& id,
            int& num,
            int min = 0,
            int max = 1,
            bool clamp = false
        )
        {
            return scrubber< int >( ScrubberState::I32, id, num, clamp, min, max );
        }
        bool scrubber
        (
            const ui::ID& id,
            int64_t& num,
            bool clamp = false,
            int64_t min = 0,
            int64_t max = 1
        )
        {
            return scrubber< int64_t >( ScrubberState::I64, id, num, clamp, min, max );
        }

        template< typename T >
        bool scrubber( ScrubberState::Type type, const ui::ID& id, const ui::Text& name, T& num )
        {
            _layout->push({ .style = &_theme.scrubber_row });
            fittedLabel(name);
            scrubber< T >( type, id, num, 0, 0, false );
            _layout->pop();
            return false;
        }

        template< typename T >
        bool scrubber2( ScrubberState::Type type, const ui::ID uid[2], const ui::Text& name, T& x, T& y )
        {
            bool modified[2] = {};

            beginColumn();
            {
                label(name);

                beginColumn();
                ui::Text x_label{.ptr = "x:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
                modified[0] = scrubber(type, uid[0], x_label, x);

                ui::Text y_label{.ptr = "y:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
                modified[1] = scrubber(type, uid[1], y_label, y);
                endColumn();
            }
            endColumn();

            return modified[0] || modified[1];
        }

        template< typename T >
        bool scrubber3( ScrubberState::Type type, const ui::ID id[3], T& x, T& y, T& z )
        {
            bool modified[3] = {};

            beginColumn();
            {
                beginColumn();
                ui::Text x_label{.ptr = "x:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
                modified[0] = scrubber(type, id[0], x_label, x);

                ui::Text y_label{.ptr = "y:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
                modified[1] = scrubber(type, id[1], y_label, y);

                ui::Text z_label{.ptr = "z:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
                modified[2] = scrubber(type, id[2], z_label, z);
                endColumn();
            }
            endColumn();

            return modified[0] || modified[1] || modified[2];
        }

        template< typename T >
        bool scrubber4( ScrubberState::Type type, const ui::ID uid[4], const ui::Text& name, T& x, T& y, T& z, T& w )
        {
            bool modified[4] = {};

            beginColumn();
            {
                label(name);

                beginColumn();
                ui::Text x_label{.ptr = "x:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
                modified[0] = scrubber(type, uid[0], x_label, x);

                ui::Text y_label{.ptr = "y:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
                modified[1] = scrubber(type, uid[1], y_label, y);

                ui::Text z_label{.ptr = "z:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
                modified[2] = scrubber(type, uid[2], z_label, z);

                ui::Text w_label{.ptr = "w:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
                modified[2] = scrubber(type, uid[3], w_label, w);
                endColumn();
            }
            endColumn();

            return modified[0] || modified[1] || modified[2] || modified[3];
        }

        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        // ---  SLIDER                                                                 ---
        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

        template< typename T >
        static void slidebarOp(ui::Layout* layout, ui::ID user_id, ui::WidgetId widget_id, void* data)
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
        bool slidebar( const ui::ID uid[2], T& val, const T& min, const T& max )
        {
            ui::WidgetId widget_index = _layout->push
            ({
                .user_id = uid[0],
                .style = &_theme.slide_bar_track,
                .single_click = ui::ClickTrigger::Continuous
            });
            ui::WidgetId widget_id = _layout->put({ .user_id = uid[1], .style = &_theme.slide_bar });
            _layout->pop();

            _layout->pushDeferredOp(uid[1], widget_index, slidebarOp<T>, RangeParams<T>{min, max, &val, widget_id});

            return _layout->click( uid[0] );
        }




        
        bool labelOptions( const ui::UID& uid, const ui::Text& label, std::vector< ui::Text >& list, int& selection, bool vertical = false );
        bool options( const ui::UID& uid, std::vector< ui::Text >& list, int& selection );

        bool select( const ui::UID& uid, std::vector< ui::Text >& list, int& selection );
        bool tab( const ui::UID& uid, std::vector< ui::Text>& list, int& selection );
        void list( std::vector< ui::Text>& list );

        bool numSlideBar( ui::ID user_id, double* val, double min, double max );

        template< typename T >
        static void sliderOp(ui::Layout* layout, ui::ID user_id, ui::WidgetId widget_id, void* data)
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
        bool slider( const ui::ID uid[2], T& val, const T& min, const T& max )
        {
            _layout->push({ .user_id = uid[0], .style = &_theme.slider_track });
            ui::WidgetId widget_index = _layout->put
            ({
                .user_id = uid[1],
                .style = &_theme.slider_knob,
                .single_click = ui::ClickTrigger::Continuous
            });
            _layout->pop();
            _layout->pushDeferredOp( uid[1], widget_index, sliderOp<T>, RangeParams<T>{min,max,&val} );
            return _layout->click( uid[1] );
        }

        bool charButn( ui::ID user_id, const char* label, float x, float y );
        bool dotButn( ui::ID user_id );

        ui::WidgetId fittedLabel( const ui::Text& text, ui::Style* style = nullptr );
        ui::WidgetId label( const ui::Text& text, ui::Style* style = nullptr );

        bool radio( ui::ID user_id, const ui::Text& text, bool& state );
        bool radio( ui::ID user_id, bool& state );
        bool radio( ui::ID user_id, int16_t layer );

        ui::WidgetId beginList(int16_t layer);
        void endList();

        ui::WidgetId beginColumn( ui::Style* style = nullptr );
        void endColumn();

        ui::WidgetId beginRow( ui::Style* style = nullptr );
        void endRow();

        void beginScrollContainer( const ui::ID id[2] );
        void endScrollContainer();

        ui::WidgetId pushRoot( const ui::WidgetDesc& desc );
        ui::WidgetId putRoot( const ui::WidgetDesc& desc );
        void popRoot();

        ui::WidgetId push( const kege::ui::WidgetDesc& desc );
        ui::WidgetId put( const kege::ui::WidgetDesc& desc );
        ui::WidgetId text( const kege::ui::Text& text );
        void pop();

        void pushLayer( uint32_t index );
        bool popLayer();
        
        /**
         * Checks if the mouse pointer is being dragged.
         *
         * @return true if the pointer is being dragged, false otherwise.
         */
        const bool pointerDragging() const;

        bool mouseover( const ui::ID& id );
        bool click( const ui::ID& id );
        bool hot( const ui::ID& id );
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


        const kege::Theme& theme()const{ return _theme; };

        void begin( double dms );
        void end();

        ~UI();
        UI();

        enum { CHAR_BUFR_CAPACITY = 256 };

        static char* getCharBufr()
        {
            static char char_bufr[ CHAR_BUFR_CAPACITY ];
            return char_bufr;
        }

        size_t getCharBufrLen()const
        {
            return _str_len;
        }

    private:

        static void editTextOp(ui::Layout* layout, ui::ID user_id, ui::WidgetId index, void* data);

    private:



        kege::Ref< kege::ui::Layout > _layout;
        kege::Theme _theme;

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

        size_t _str_len;
        bool _editing;
        bool _edited;
    };



    template<typename T>
    class State{
    public:
        static T* get( uint64_t user_id)
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

        static std::unordered_map<uint64_t, T> _states;
    };

    template<typename T>
    std::unordered_map<uint64_t, T> State<T>::_states;
}

#endif /* gui_hpp */
