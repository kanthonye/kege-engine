//
//  ui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 12/8/25.
//

#ifndef gui_hpp
#define gui_hpp

#include "../../core/graphics/components/gui/ui-state-manager.hpp"
#include "theme.hpp"

namespace kege{

    template< typename T >
    struct RangeParams
    {
        T  min;
        T  max;
        T* val;
        ui::NodeId node_id;
        float length;
    };

    enum class TextFieldMode
    {
        Idle,       // Not focused
        Focused,    // Focused but not editing
        Editing     // Actively receiving input
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
            ui::NodeId index;
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
            _gui->push
            ({
                .wid = _gui->newElem
                ({
                    .width = ui::extend(),
                    .height = ui::fixed(30),
                    .quad_color = 0xFFFFFF08,
                    .padding = {10,5,10,5},
                    .alignment =
                    {
                        .gap = {2,2},
                        .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                        .direction = {ui::AlignDir::RIGHT},
                        //.items = ui::AlignItem::CENTER,
                    }
                })
            });
            _gui->text(label);
            this->scrubber< T >( type, user_id, value, clamp, min, max );
            _gui->pop();
        }

        bool labelInput(const char* label, const ui::ID& user_id, TextFieldMode& mode, ui::Text& text);

        void beginWindow(const ui::ID uid[3], ui::Rect& rect, const char* title, bool& close_window);
        void endWindow();

        template<typename Params>void pushDeferredOp(const ui::ID& id, ui::NodeId index, ui::DeferredOperation fn, const Params& params)
        {
            _gui->pushDeferredOp< Params >( id, index, fn, params );
        }

        template<typename Params>void pushDeferredOpPtr(const ui::ID& id, ui::NodeId index, ui::DeferredOperation fn, Params* params)
        {
            _gui->pushDeferredOpPtr< Params >( id, index, fn, params );
        }

        template<typename T> uint64_t getAddressAsInt(const T& value)
        {
            return reinterpret_cast<uint64_t>(&value);
        }

        bool submit(ui::ID user_id, const char* label);
        bool clickButton( ui::ID user_id, const ui::Text& text );


        bool button( ui::ID user_id, const ui::Text& text, const ui::Elem* elem = nullptr );
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
            state->text_value.data = state->str;
//            ui::NodeId node_id = _gui->put
//            ({
//                .user_id = id,
//                .text = state->text_value,
//                .style = ( state->mode == TextFieldMode::Editing ) ? &_theme->scrubber_focus : &_theme->scrubber,
//                .single_click = ui::ClickTrigger::Continuous,
//                .double_click = ui::ClickTrigger::Immediate,
//            });
            ui::NodeId node_id = _gui->push
            ({
                .user_id = id,
                .wid = _gui->newElem
                ({
                    .width = ui::extend(),
                    .height = ui::fixed(30),
                    .quad_color = 0xFFFFFF08,
                    .padding = {10,5,10,5},
                    .alignment =
                    {
                        .gap = {2,2},
                        .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                        .direction = {ui::AlignDir::RIGHT},
                        //.items = ui::AlignItem::CENTER,
                    },
                }),
                .text = state->text_value,
                .single_click = ui::ClickTrigger::Continuous,
                .double_click = ui::ClickTrigger::Immediate,
            });



            bool active = false;
            if ( _gui->click( id ) && state->mode != TextFieldMode::Editing)
            {
                const kege::Mouse* mouse = _gui->getInputManager()->getMouse();
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
                    state->text_value = _gui->text(state->str, 20);
                    state->size = strlen(state->str);
                }
            }
            else
            {
                _gui->pushDeferredOp(id, node_id, editTextOp, TextField
                {
                    .type = ui::Cursor::InputType::Numeric,
                    .mode = state->mode,
                    .str_capacity = 15,
                    .str_len = state->size,
                    .str = state->str,
                    .editing = state->editing,
                    .edited = state->edited,
                });
            }

            return active || state->edited;
        }

        template< typename T >
        bool scrubber( ScrubberState::Type type, const ui::ID& id, T& num, bool clamp = false, const T& min = 0, const T& max = 1 )
        {
            ScrubberState* state = kege::ui::StateManager::get< ScrubberState >( id );
            if (state == nullptr)
            {
                state = kege::ui::StateManager::insert< ScrubberState >( id );
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
                state->text_value = _gui->text(state->str, 20);
                state->text_value.color = 0xFFFFFFFF;
                state->sensitivity = 0.002;
                state->type = type;
                state->clamp = clamp;
            }

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
            _gui->push
            ({
                .user_id = id,
                .wid = _gui->newElem( _theme->scrubber_row ),
                .text = name
            });

            fittedLabel(name);
            scrubber< T >( type, id, num, 0, 0, false );
            _gui->pop();
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
                ui::Text x_label{.width = 10, .font_size = 20, .color = 0xFFFFFFFF, .data = "x:"};
                modified[0] = scrubber(type, uid[0], x_label, x);

                ui::Text y_label{.width = 10, .font_size = 20, .color = 0xFFFFFFFF, .data = "y:"};
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
                ui::Text x_label{.width = 10, .font_size = 20, .color = 0xFFFFFFFF, .data = "x:"};
                modified[0] = scrubber(type, id[0], x_label, x);

                ui::Text y_label{.width = 10, .font_size = 20, .color = 0xFFFFFFFF, .data = "y:"};
                modified[1] = scrubber(type, id[1], y_label, y);

                ui::Text z_label{.width = 10, .font_size = 20, .color = 0xFFFFFFFF, .data = "z:"};
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
                ui::Text x_label{.width = 10, .font_size = 20, .color = 0xFFFFFFFF, .data = "x:"};
                modified[0] = scrubber(type, uid[0], x_label, x);

                ui::Text y_label{.width = 10, .font_size = 20, .color = 0xFFFFFFFF, .data = "y:"};
                modified[1] = scrubber(type, uid[1], y_label, y);

                ui::Text z_label{.width = 10, .font_size = 20, .color = 0xFFFFFFFF, .data = "z:"};
                modified[2] = scrubber(type, uid[2], z_label, z);

                ui::Text w_label{.width = 10, .font_size = 20, .color = 0xFFFFFFFF, .data = "w:"};
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
        static void slidebarOp(kege::GUI* gui, ui::ID user_id, ui::NodeId node_id, void* data)
        {
            //gui->elemParent(id)->uid
            RangeParams<T>* params = reinterpret_cast<RangeParams<T>*>(data);
            float length = gui->at(node_id)->quad.width;
            float& w = gui->at(params->node_id)->quad.width;

            // calculate where the slider should be (base on the numeric value) before moving it.
            w = (length * (*params->val - params->min)) / (params->max - params->min);

            if( gui->click( user_id ) )
            {
                // update slide bar width
                w += gui->getPointerDelta().x;
                // clamp slide bar width
                w = (w < 0)? 0.f : (w > length) ? length: w;
                // calculate numeric value base on slide bar width
                *params->val = (params->max - params->min) * (w / length) + params->min;
            }
            else if( gui->doubleClick( user_id ) )
            {
                // update slide bar width
                w = gui->getClickPosition(kege::MouseButtonCode::Left).x - gui->at(node_id)->quad.x;
                // clamp slide bar width
                w = (w < 0)? 0.f : (w > length) ? length: w;
                // calculate numeric value base on slide bar width
                *params->val = (params->max - params->min) * (w / length) + params->min;
            }
        }

        template< typename T >
        bool slidebar( const ui::ID uid[2], T& val, const T& min, const T& max )
        {
//            ui::NodeId widget_index = _gui->push
//            ({
//                .user_id = uid[0],
//                .style = &_theme->slide_bar_track,
//                .single_click = ui::ClickTrigger::Continuous
//            });
//            ui::NodeId node_id = _gui->put({ .user_id = uid[1], .style = &_theme->slide_bar });
//            _gui->pop();
//
//            _gui->pushDeferredOp(uid[1], widget_index, slidebarOp<T>, RangeParams<T>{min, max, &val, node_id});

            return _gui->click( uid[0] );
        }


        bool beginDropdown( const ui::ID& id, const ui::Text& label );
        void endDropdown();



        
        bool labelOptions( const ui::UID& uid, const ui::Text& label, std::vector< ui::Text >& list, int& selection, bool vertical = false );
        bool options( const ui::UID& uid, std::vector< ui::Text >& list, int& selection );

        bool select( const ui::UID& uid, std::vector< ui::Text >& list, int& selection );
        bool tab( const ui::UID& uid, std::vector< ui::Text>& list, int& selection );
        void list( std::vector< ui::Text>& list );

        bool numSlideBar( ui::ID user_id, double* val, double min, double max );

        template< typename T >
        static void sliderOp(kege::GUI* gui, ui::ID user_id, ui::NodeId node_id, void* data)
        {
//            RangeParams<T>* params = reinterpret_cast<RangeParams<T>*>(data);
//            const ui::Elem* track = gui->elem( gui->at(node_id)->parent );
//            ui::Elem* knob = gui->elem(node_id);
//
//            float length = track->traits.rect.width - knob->traits.rect.width;
//
//            // calculate where the slider should be (base on the numeric value) before moving it.
//            float pos = (length * (*params->val - params->min)) / (params->max - params->min);
//
//            if( gui->click( user_id ) )
//            {
//                // update slider position
//                pos += gui->getPointerDelta().x;
//                // keep the slider with in bound
//                //pos = kege::min(length, kege::min(0.f, pos));
//                pos = kege::clamp(pos, 0.f, length);
//                // calculate the new numeric value base on the updated position
//                *params->val = (params->max - params->min) * (pos / length) + params->min;
//            }
//
//            knob->traits.rect.x += pos;
        }

        template< typename T >
        bool slider( const ui::ID uid[2], T& val, const T& min, const T& max )
        {
//            _gui->push({ .user_id = uid[0], .style = &_theme->slider_track });
//            ui::NodeId widget_index = _gui->put
//            ({
//                .user_id = uid[1],
//                .style = &_theme->slider_knob,
//                .single_click = ui::ClickTrigger::Continuous
//            });
//            _gui->pop();
//            _gui->pushDeferredOp( uid[1], widget_index, sliderOp<T>, RangeParams<T>{min,max,&val} );
            return _gui->click( uid[1] );
        }

        bool charButn( ui::ID user_id, const char* label, float x, float y );
        bool dotButn( ui::ID user_id );

        ui::NodeId fittedLabel( const ui::Text& text, ui::Elem* elem = nullptr );
        ui::NodeId label( const ui::Text& text, ui::Elem* elem = nullptr );

        bool radio( ui::ID user_id, const ui::Text& text, bool& state );
        bool radio( ui::ID user_id, bool& state );
        bool radio( ui::ID user_id, int16_t layer );

        ui::NodeId beginList(int16_t layer);
        void endList();

        ui::NodeId beginColumn( ui::Elem* elem = nullptr );
        void endColumn();

        ui::NodeId beginRow( ui::Elem* elem = nullptr );
        void endRow();

        void beginScrollContainer( const ui::ID id[2] );
        void endScrollContainer();

        kege::ui::Node* newNode( const kege::ui::NodeDesc& desc );
        kege::ui::WID newElem( const kege::ui::Elem& desc );

        void beginRoot();
        void endRoot();

        ui::NodeId push( const kege::ui::NodeDesc& desc );
        ui::NodeId put( const kege::ui::NodeDesc& desc );
        
        ui::NodeId text( const kege::ui::Text& text );
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

        kege::GUI* gui()
        {
            return _gui;
        }


        /**
         * Checks if geven point and rectangular shape intersects.
         *
         * @param p The given point to test the rectangle against.
         * @param rect The rectangle to test the point against.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        //inline bool testPointVsRect( const kege::dvec2& p, const ui::Rect& rect )const
        //{
        //    return ui:testPointVsRect( p, rect );
        //}


        inline bool intersect(const ui::Rect& m, const ui::Rect& n)const
        {
            return checkOverlap( m, n );
        }

        ui::Node* get( ui::NodeId index );
        ui::Elem* elem( ui::NodeId index );
        //kege::ui::Style* getStyle( int index );
        bool initialize(kege::GUI* layout);


        ///void createLayers( uint32_t quantity );
        
        vec2d deltaPointer()const;
        vec2d pointer()const;


        const kege::Theme* theme()const{ return _theme; };

        void begin( double dms );
        void end();


        void init(kege::GUI* gui, kege::Theme* theme);

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

        static void editTextOp(kege::GUI* gui, ui::ID user_id, ui::NodeId index, void* data);

    private:

        kege::Theme* _theme;
        kege::GUI* _gui;

        uint32_t _frame_index;

        size_t _str_len;
        bool _editing;
        bool _edited;
    };

}

#endif /* gui_hpp */
