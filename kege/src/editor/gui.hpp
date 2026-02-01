//
//  gui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 12/8/25.
//

#ifndef gui_hpp
#define gui_hpp

#include "../core/graphics/gui/ui-layout.hpp"
#include "../core/graphics/gui/ui-viewer.hpp"

namespace kege{

    struct RangeParams
    {
        double  min;
        double  max;
        double* val;
        ui::UID* id;
        float length;
    };


    struct ListElem
    {
        ui::Text text;
        ui::UID uid;
    };

    enum class TextFieldMode
    {
        Idle,       // Not focused
        Focused,    // Focused but not editing
        Editing     // Actively receiving input
    };



    struct Theme
    {
        kege::ui::Style panel;
        kege::ui::Style panel_transparnt;
        kege::ui::Style padded_list;

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

        kege::ui::Style row;
        kege::ui::Style row_wrap;
        kege::ui::Style column;
        kege::ui::Style x_seperator;
        kege::ui::Style y_seperator;


        kege::ui::Style scrubber;
        kege::ui::Style scrubber_label;
        kege::ui::Style scrubber_focus;
        kege::ui::Style scrubber_row;

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

        enum
        {
            Label, FittedLabel,
            
            Slider, SliderTrack,
            SlideBar, SlideBarTrack, SlideBarNumber,

            List, ListElem, ListSelection,
            Tab, TabElem, TabSelection,

            VPanel, Row,
            Numeric, NumericLabel, NumericFocus, NumericValue,

            DropDown,
            CollapsableHeader,


            Hierarchy, Viewport,
            DockNorizonal, DockVertical,

            FlexH, FlexW,
            CloseButn,
            COUNT,
        };

        Theme();

        kege::ui::Style styles[Theme::COUNT];
    };

    class GUI
    {
    public:

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
            ui::UID uid[4];
            int mode;
            bool state;
            uint32_t last_frame_value;
        };

        struct CollapsableHeader
        {
            ui::UID uid[3];
            bool state;
            uint32_t last_frame_value;
        };

        struct RemovableHeader
        {
            ui::UID uid[4];
            bool state;
            uint32_t last_frame_value;
        };

        struct TextFieldState
        {
            ui::UID field;
            TextFieldMode mode = TextFieldMode::Idle;
            bool edited;

            //size_t cursor_pos = 0;
            //size_t selection_start = 0;
            //bool has_selection = false;
            //std::chrono::steady_clock::time_point last_click_time;
            //static constexpr auto double_click_threshold = std::chrono::milliseconds(500);
        };

//        struct InputText
//        {
//            ui::UID uid[2];
//            int mode;
//            uint32_t last_frame_value;
//        };

        struct Scrubber
        {
            enum Type{FLOAT, DOUBLE, INT32, INT64};
            ui::Text text_value;
            ui::UID uid;
            char str[16];
            size_t size;
            int mode;
            float sensitivity;
            uint32_t last_frame_value;
            Type type;
        };

        struct InputNumeric
        {
            size_t size = 16;
            ui::UID uid[3];
            char str[16];
            int mode = 0;
            uint32_t last_frame_value = 0;
        };

        struct SliderState
        {
            ui::UID uid[2];
        };

        struct SliderUIState
        {
            ui::UID uid[2];
            char str[3][16];
            ui::Text text[3];
        };

        struct RadioState
        {
            ui::UID uid;
            bool state;
        };

        struct OptionState
        {
            ui::UID uid[3];
            int selection = 0;
            bool state;
        };

        struct ScrollContainer
        {
            ui::UID clipper;
            ui::UID scroller;
            float scroll_amount;
            int frame_id;
        };


        template<typename Params>void pushDeferredOp(const ui::UID* id, ui::DeferredOperation fn, const Params& params)
        {
            _layout->pushDeferredOp< Params >( id, fn, params );
        }
        template<typename Params>void pushDeferredOpPtr(const ui::UID* id, ui::DeferredOperation fn, Params* params)
        {
            _layout->pushDeferredOpPtr< Params >( id, fn, params );
        }

        template<typename T> uint64_t getAddressAsInt(const T& value)
        {
            return reinterpret_cast<uint64_t>(&value);
        }

        std::vector<ListElem>& getListState(uint64_t id)
        {
            return _list_elements[id];
        }


        int collapsableRemovableHeaderInput( int16_t layer, uint64_t id, char* str, size_t& size );
        int collapsableRemovableHeader( int16_t layer, uint64_t id, const ui::Text& text );
        bool collapsableHeader( int16_t layer, uint64_t id, const ui::Text& text );
        int removableHeader( int16_t layer, uint64_t id, const ui::Text& text );

        bool textField( int16_t layer, uint64_t id, char* buffer, size_t buffer_capacity, size_t& current_size );

        bool scrubber4( int16_t layer, uint64_t id, const ui::Text& name, float& x, float& y, float& z, float& w );
        bool scrubber3( int16_t layer, uint64_t id, const ui::Text& name, float& x, float& y, float& z );

        bool scrubber( int16_t layer, uint64_t id, const ui::Text& name, double& num );
        bool scrubber( int16_t layer, uint64_t id, const ui::Text& name, float& num );
        bool scrubber( int16_t layer, uint64_t id, double& num );
        bool scrubber( int16_t layer, uint64_t id, float& num );

        bool numeric( int16_t layer, uint64_t id, double& num );

        bool options( int16_t layer, uint64_t id, std::vector<ListElem>& list, int& selection );
        bool select( int16_t layer, ui::Style* style, std::vector<ListElem>& list, int& selection );
        bool select( int16_t layer, std::vector<ListElem>& list, int& selection );
        void list( int16_t layer, std::vector<ListElem>& list, int selection = -1 );
        bool tab( int16_t layer, std::vector<ListElem>& list, int& selection );

        bool numSlideBar( int16_t layer, ui::UID (&id)[2], double* val, double min, double max );
        bool slidebar( int16_t layer, ui::UID (&id)[2], double* val, double min, double max );
        bool slider( int16_t layer, ui::UID (&id)[2], double* val, double min, double max );

        bool sliderUI( int16_t layer, const ui::Text& text, uint64_t id, double* val, double min, double max );
        bool slider( int16_t layer, uint64_t id, double* val, double min, double max );

        bool button( int16_t layer, ui::UID& id, const ui::Text& text, ui::Style* style = nullptr );
        bool button( const kege::ui::WidgetDesc& desc );
        bool circleButton( int16_t layer, ui::UID& id );

        ui::Widget* fittedLabel( int16_t layer, const ui::Text& text, ui::Style* style = nullptr );
        ui::Widget* label( int16_t layer, const ui::Text& text, ui::Style* style = nullptr );
        bool radio( int16_t layer, ui::UID& uid, const ui::Text& text, bool& state );
        bool radio( int16_t layer, ui::UID& id, bool& state );
        bool radio( int16_t layer, uint64_t id );

        ui::Widget* beginList(int16_t layer);
        void endList();

        ui::Widget* beginColumn( int16_t layer, ui::Style* style = nullptr );
        void endColumn();

        ui::Widget* beginRow( int16_t layer, ui::Style* style = nullptr );
        void endRow();

        void beginScrollContainer( int16_t layer, uint64_t id );
        void endScrollContainer();

       kege::ui::Widget* pushRoot( const ui::WidgetDesc& desc );
       kege::ui::Widget* putRoot( const ui::WidgetDesc& desc );
       void popRoot();

        void pushHPanel( int16_t layer );
        void pushVPanel( int16_t layer );

        void push( const kege::ui::WidgetDesc& desc );
        void put( const kege::ui::WidgetDesc& desc );
        void pop();

        /**
         * Checks if the mouse pointer is being dragged.
         *
         * @return true if the pointer is being dragged, false otherwise.
         */
        const bool pointerDragging() const;

        bool mouseover( ui::UID& id );
        bool click( ui::UID& id );
        bool hot( ui::UID& id );
        bool buttonDown()const;
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

        ui::Widget* get( ui::UID& id );
        kege::ui::Style* getStyle( int index );
        bool initialize(Ref< ui::Layout > layout);


        void createLayers( uint32_t quantity );
        
        vec2d deltaPointer()const;
        vec2d pointer()const;

        void begin( double dms );
        void end();

        ~GUI();
        GUI();



        template< typename T >
        bool scrubber( int16_t layer, Scrubber* state, T& num )
        {
            bool active = false;
            if ( _layout->buttonDown() )
            {
                if ( _layout->click( state->uid ) )
                {
                    const kege::Mouse* mouse = _layout->inputManager()->getMouse();
                    if ( mouse->moved() )
                    {
                        if( 0.0 != mouse->getDelta().x )
                            num += mouse->getDelta().x * state->sensitivity;

                        if( 0.0 != mouse->getDelta().y )
                            num -= mouse->getDelta().y * state->sensitivity;
                        active = true;

                        if(state->type == Scrubber::FLOAT)
                            snprintf(state->str, 16, "%.3f", num);
                        if(state->type == Scrubber::DOUBLE)
                            snprintf(state->str, 16, "%.3f", num);
                        
                        state->text_value = _layout->text(state->str, 20);
                    }
                }
                else if ( _layout->doubleClick( state->uid ) )
                {
                    state->mode = 2;
                }
                else
                {
                    state->mode = 0;
                }
            }

            if ( state->mode == 2 )
            {
                if ( !_layout->onTextInput( state->uid, state->str, state->size ) )
                {
                    active = true;
                    state->mode = 0;
                }
                num = atof( state->str );
            }
            _layout->put
            ({
                .uid = &state->uid,
                .style = ( state->mode == 2 ) ? &_theme.scrubber_focus : &_theme.scrubber,
                .single_click = ui::ClickTrigger::Continuous,
                .double_click = ui::ClickTrigger::Immediate,
                .text = state->text_value,
            });
            return active;
        }


        kege::Ref< kege::ui::Layout > _layout;
        kege::Theme _theme;


        std::unordered_map<uint64_t, std::vector<ListElem>> _list_elements;

        std::unordered_map<uint64_t, CollapsableRemovableHeader> _collapsable_removable_headers;
        std::unordered_map<uint64_t, CollapsableHeader> _collapsable_headers;
        std::unordered_map<uint64_t, OptionState> _option_states;
        std::unordered_map<uint64_t, RemovableHeader> _removable_headers;
        std::unordered_map<uint64_t, InputNumeric> _numeric_states;
        std::unordered_map<uint64_t, Scrubber> _scrubbers;
        std::unordered_map<uint64_t, TextFieldState> _input_texts;
        std::unordered_map<uint64_t, SliderState> _slider_states;
        std::unordered_map<uint64_t, SliderUIState> _slider_ui_states;
        std::unordered_map<uint64_t, RadioState> _radio_states;
        std::unordered_map<uint64_t, ScrollContainer> _scroll_containers;

        uint32_t _frame_index;
        //kege::ui::Viewer _viewer;
        //kege::ui::Input _input;
    };

}

#endif /* gui_hpp */
