//
//  ui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/8/25.
//

#include "ui.hpp"

namespace kege{

    bool UI::input(const ui::ID& user_id, ui::Cursor::InputType type, TextFieldMode& mode, ui::Text& text )
    {
        if (mode == TextFieldMode::Editing)
        {
            text.ptr = getCharBufr();
        }

        ui::WidgetId widget_id = _layout->put
        ({
            .user_id = user_id,
            .text = text,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .rect = {0, 0, 0, text.height},
            .color = (mode == TextFieldMode::Editing) ? 0x66FFFF35 : (mode == TextFieldMode::Focused) ? 0xFFFFFF18 : 0xFFFFFF05,
            .width = ui::extend(),
        });

        _layout->pushDeferredOp(user_id, widget_id, editTextOp, TextField
        {
            .type = type,
            .mode = mode,
            .edited = _edited,
            .editing = _editing,
            .str = text.ptr,
            .str_len = _str_len,
            .str_capacity = CHAR_BUFR_CAPACITY
        });

        return _edited;
    }

    bool UI::labelInput(const char* label, const ui::ID& user_id, TextFieldMode& mode, ui::Text& text)
    {
        _layout->push
        ({
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
        _layout->put
        ({
            .text = ui::Text{.width = 60, .height = 20, .color = 0xFFFFFF30, .font_size = 24, .ptr = label},
            .single_click = ui::ClickTrigger::OnRelease,
            .rect = {0,0, 60, 30},
            .color = 0xFFFFFF00,
            .padding = {10,5,5,10},
        });
        bool modified = input(user_id, ui::Cursor::InputType::Any, mode, text);
        _layout->pop();
        return modified;
    }

    void UI::beginWindow(const ui::ID uid[3], ui::Rect& rect, const char* title, bool& close_window)
    {
        _layout->pushLayer(ui::LAYER_WINDOW);
        _layout->push
        ({
            .user_id = uid[0],
            .rect = rect,
            .position = ui::Positioning::Independent,
            .padding = {10,10,10,10},
            .color = 0x18141DFF,
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .clip_overflow = true
        });

        _layout->push
        ({
            .user_id = uid[1],
            .width = ui::extend(),
            .height = ui::fixed(30),
            .color = 0xFFFFF00,
            .single_click = ui::ClickTrigger::Continuous,
            .double_click = ui::ClickTrigger::Immediate,
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .items = ui::AlignItem::CENTER,
            }
        });
        {
            float w = rect.width - 50;
            float h = 30;
            _layout->put // the title
            ({
                .text = ui::Text
                {
                    .width = w,
                    .height = h,
                    .color = 0xFFFFFF30,
                    .font_size = 30,
                    .ptr = title
                },
                .single_click = ui::ClickTrigger::OnRelease,
                .rect = {0,0, w,h},
                .color = 0xFFFFFF00,
                .padding = {10,5,5,10},
            });
            _layout->put({.style = &_theme.x_seperator});
            charButn(uid[2], "x", 7, 0);
        }
        _layout->pop();
        _layout->put({.rect = {0,0,10,10}});

        if( _layout->click(uid[2]) )
        {
            close_window = false;
        }

        if( _layout->click(uid[1]) )
        {
            rect.x += _layout->inputManager()->getMouse()->getDelta().x;
            rect.y += _layout->inputManager()->getMouse()->getDelta().y;
        }
    }
    
    void UI::endWindow()
    {
        _layout->popLayer();
    }


    void UI::editTextOp(ui::Layout* layout, ui::ID user_id, ui::WidgetId widget_id, void* data)
    {
        TextField* text = static_cast<TextField*>(data);
        //TextFieldState* state = text->state;
        text->edited = false;

        if ( layout->click( user_id ) && text->mode != TextFieldMode::Editing )
        {
            text->mode = TextFieldMode::Focused;
        }
        else if ( layout->doubleClick( user_id ) )
        {
            if (text->mode == TextFieldMode::Editing)
            {
                layout->cursor()->selectAll( widget_id, getCharBufr(), text->str_len );
            }
            else
            {
                text->mode = TextFieldMode::Editing;
            }
        }
        else if (!layout->hasFocus( user_id ) && text->mode != TextFieldMode::Idle)
        {
            // Lost focus
            if (text->mode == TextFieldMode::Editing)
            {
                text->edited = true; // Text was being edited
            }
            layout->cursor()->stopEditing();
            text->editing = false;

            text->mode = TextFieldMode::Idle;
        }

        if (text->mode == TextFieldMode::Editing)
        {
            // begin editing
            if (!text->editing)
            {
                text->editing = true;
                text->str_len = strlen(text->str);
                memcpy(getCharBufr(), text->str, text->str_len);

                layout->cursor()->startEditing
                (
                    user_id, widget_id, text->type,
                    getCharBufr(), text->str_len, CHAR_BUFR_CAPACITY
                );
                layout->cursor()->computeCursorPosition( widget_id );
            }

            if (text->editing)
            {
                if (!layout->cursor()->update( widget_id ))
                {
                    text->mode = TextFieldMode::Idle;
                    text->edited = true;
                }
            }
        }
    }





    bool UI::submit( ui::ID user_id, const char* label)
    {
        _layout->put
        ({
            .user_id = user_id,
            .text = ui::Text{
                .width = 50,
                .height = 20,
                .color = 0xFFFFFF30,
                .font_size = 24,
                .align = ui::AlignText::Center,
                .ptr = label
            },
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .color = 0xFFFFFF08,
            .padding = {10,5,5,10},
            .width = ui::extend(),
            .height = ui::fixed(30),
        });
        return _layout->click( user_id );
    }

    bool UI::clickButton( ui::ID user_id, const ui::Text& label )
    {
        _layout->put
        ({
            .user_id = user_id,
            .text = label,
            .single_click = ui::ClickTrigger::Immediate,
            .double_click = ui::ClickTrigger::Immediate,
            .color = 0xFFFFFF08,
            .padding = {10,5,5,10},
            .width = ui::extend(),
            .height = ui::fixed(30),
        });
        return _layout->click( user_id );
    }

    bool UI::collapsableHeader( const ui::ID& id, bool& expand, const ui::Text& text )
    {
        _layout->push
        ({
            .user_id = id,
            .width = ui::extend(),
            .height = ui::fixed(20),
            .padding = {10, 0, 10, 0,},
            .alignment = {
                .direction = ui::AlignDir::DOWN
            },
            //.style = &_theme.collapsable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .color = 0xFFFFFF06,
        });
        {
            _layout->push
            ({
                .user_id = id,
                .width = ui::extend(),
                .height = ui::fixed(20),
                .alignment = {
                    .direction = ui::AlignDir::RIGHT
                },
                .gap.width = 5
            });
            {
                _layout->put
                ({
                    .rect = ui::Rect{.width = 10, .height = 10},
                    .text = ui::Text{.width = 10, .height = 10, .ptr = (expand) ?"-" : "+"}
                });
                label(text);
            }
            _layout->pop();
        }
        _layout->pop();

        if ( _layout->click( id ) )
        {
            expand = !expand;
        }
        return expand;
    }
    bool UI::checkbox( const ui::ID& id, const ui::Text& text,  bool& checked )
    {
        if ( _layout->click( id ) )
        {
            checked = !checked;
        }
        _layout->push
        ({
            .user_id = id,
            .width = ui::extend(),
            .height = ui::fixed(20),
            .alignment = kege::ui::Alignment
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .items = ui::AlignItem::CENTER
            },
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .color = 0xFFFFFF00,
            .gap {5,5}
        });
        _layout->push
        ({
            .user_id = id,
            .rect = kege::ui::Rect{0.f, 0.f, .width = 10, .height = 10},
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .alignment = kege::ui::Alignment
            {
                .origin = {ui::AlignX::CENTER, ui::AlignY::CENTER},
                .direction = ui::AlignDir::CENTER,
            },
            .color = ( checked ) ? 0xf71326ff : 0xFFFFFF30
        });
//        if ( checked )
//        {
//            _layout->put
//            ({
//                .layer = layer,
//                .rect = kege::ui::Rect{0.f, 0.f, .width = 6, .height = 6},
//                .color = 0xf71326ff
//            });
//        }
        _layout->pop();
        label(text);
        _layout->pop();
        return checked;
    }

    int UI::collapsableRemovableHeaderInput( const ui::UID& uid, char* str, size_t& size )
    {
        CollapsableRemovableHeader* header = State<CollapsableRemovableHeader>::get( uid );
        const ui::ID Id[4] = {uid[0], uid[1], uid[2], uid[4]};

        _layout->push
        ({
            .user_id = Id[0],
            .style = &_theme.collapsable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        radio( Id[1], header->state );
        ui::WidgetId widget_id = _layout->put
        ({
            .user_id = Id[2],
            .style = ( header->mode == 2 ) ? &_theme.collapsable_element_hl : &_theme.collapsable_element,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .text.ptr = str,
        });
        _layout->put /* remove button */
        ({
            .user_id = Id[3],
            .rect.height = 10,
            .rect.width = 10,
            .rect.y = 4,
            .color = _layout->mouseover( Id[3] ) ? 0xEE2200FF : 0xEE220050,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = {8,8,8,8}
        });
        _layout->pop();

        if ( _layout->doubleClick( Id[2] ) )
        {
            header->mode = 2;
        }
        if ( header->mode == 2 )
        {
            if ( !_layout->onTextInput( widget_id, str, size ) )
            {
                header->mode = 0;
            }
        }

        if ( _layout->click(Id[1]) || _layout->click( Id[2] ) )
        {
            header->state = !header->state;
        }
        if ( _layout->click(Id[3]) )
        {
            return -1;
        }
        return header->state;
    }

    int UI::collapsableRemovableHeader( const ui::UID& uid, const ui::Text& text )
    {
        CollapsableRemovableHeader* header = State<CollapsableRemovableHeader>::get( uid );
        const ui::ID Id[3] = {uid[0], uid[1], uid[2]};

        _layout->push
        ({
            .user_id = Id[0],
            .style = &_theme.collapsable_removable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        radio( Id[1], header->state );
        label(text);
        bool state = dotButn( Id[2] );
        _layout->pop();

        if ( _layout->click( Id[0] ) )
        {
            return 2;
        }
        if ( state )
        {
            return -1;
        }
        return header->state;
    }


    bool UI::collapsableHeader( const ui::UID& uid, const ui::Text& text )
    {
        CollapsableHeader& header = _collapsable_headers[ uid[0] ];
        const ui::ID Id[3] = {uid[0], uid[1], uid[2]};

        _layout->push
        ({
            .user_id = Id[0],
            .style = &_theme.collapsable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        _layout->put
        ({
            .user_id = Id[1],
            .rect.height = 6,
            .rect.width = 6,
            .rect.y = 6,
            .mouseover = false,
            .color = (header.state == 0) ? 0xFFBB0040 : 0xFFBB00FF,
            .border.corner_curves = {5,5,5,5}
        });
        _layout->put
        ({
            .user_id = Id[2],
            .style = &_theme.label,
            .text = text,
            .mouseover = false,
        });
        _layout->pop();

        if ( _layout->click(Id[0]) )
        {
            header.state = !header.state;
        }
        return header.state;
    }

    int UI::removableHeader( const ui::ID id[4], const ui::Text& text )
    {
        RemovableHeader& header = _removable_headers[ id[0] ];

        _layout->push
        ({
            .user_id = id[0],
            .style = &_theme.collapsable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _layout->put
        ({
            .user_id = id[1],
            .rect.height = 6,
            .rect.width = 6,
            .rect.y = 6,
            .mouseover = false,
            .color = (header.state == 0) ? 0xFFBB0040 : 0xFFBB00FF,
            .border.corner_curves = {5,5,5,5}
        });
        _layout->put
        ({
            .user_id = id[2],
            .style = &_theme.label,
            .text = text,
            .mouseover = false,
        });
        _layout->put
        ({
            .user_id = id[3],
            .rect.height = 10,
            .rect.width = 10,
            .rect.y = 4,
            .color = _layout->mouseover( id[3] ) ? 0xEE2200FF : 0xEE220050,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = {8,8,8,8}
        });
        _layout->pop();

        if ( _layout->click(id[0]) )
        {
            header.state = !header.state;
        }
        if ( _layout->click(id[3]) )
        {
            return 2;
        }
        return header.state;
    }

    struct OptionParam{ UI::OptionState* state; };
    void repositionOptionListOp(ui::Layout* layout, ui::ID user_id, ui::WidgetId widget_id, void* data)
    {
        OptionParam* state = reinterpret_cast< OptionParam* >(data);
        ui::Widget* main = layout->elem( widget_id );
        if (layout->leftClickDown())
        {
            const kege::dvec2& position = layout->getPointerPosition();
            if (!layout->testPointVsRect(position, main->rect))
            {
               state->state->state = false;
            }
        }
    }

    bool UI::labelOptions( const ui::UID& uid, const ui::Text& label, std::vector< ui::Text >& list, int& selection, bool vertical )
    {
        OptionState& state = _option_states[ uid[0] ];
//        ui::WidgetId widget_id = _layout->push({ .style = (vertical)? &_theme.column : &_theme.row });
//        _layout->put
//        ({
//            .text = label,
//            .rect = {0,0,label.width, label.height},
//            .color = 0xFFFFFF06,
//            .mouseover = false
//        });
//        bool result = this->options( uid, list, selection );
//        _layout->pop();
//        return result;

        ui::WidgetId widget_id = _layout->push({ .style = &theme().card });
        _layout->push
        ({
            .user_id = uid[0],
            .style = &theme().row,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _layout->text( label );
        _layout->text( list[ selection ] );
        _layout->pop();

        if( _layout->click( uid[0] ) )
        {
            state.state = !state.state;
        }
        bool modified = false;
        if( state.state )
        {
            for (int i=0; i<list.size(); ++i)
            {
                ui::ID option_id = uid[i + 3];
                _layout->put
                ({
                    .user_id = option_id,
                    .text = list[i],
                    .single_click = ui::ClickTrigger::OnRelease,
                    .style = (_layout->mouseover( option_id ))
                    ? &_theme.option_hot
                    : &_theme.option_element
                });

                if( _layout->click( option_id ) && !modified )
                {
                    modified = true;
                    selection = i;
                    state.selection = i;
                    state.state = false;
                }
            }
        }
        _layout->pop();
        _layout->pushDeferredOp<OptionParam>(uid[0], widget_id, repositionOptionListOp, {&state});
        return modified;
    }
    
    bool UI::options( const ui::UID& uid, std::vector< ui::Text >& list, int& selection )
    {
        OptionState& state = _option_states[ uid[0] ];
        bool has_selection = false;

        ui::WidgetId widget_id = _layout->push
        ({
            .user_id = uid[0],
            .style = &_theme.option,
            .single_click = ui::ClickTrigger::Immediate,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,4},
        });
        _layout->put
        ({
            .user_id = uid[1],
            .style = &_theme.option_selection,
            .text = list[ state.selection ],
            .mouseover = false,
        });

        if ( _layout->click( uid[0] ) || _layout->click( uid[1] ) )
        {
            state.state = !state.state;
        }

        if ( state.state )
        {
            state.index = _layout->push({.user_id = uid[2], .style = &_theme.option_list});
            for (int i=0; i<list.size(); ++i)
            {
                ui::ID option_id = uid[i + 3];
                _layout->put
                ({
                    .user_id = option_id,
                    .text = list[i],
                    .single_click = ui::ClickTrigger::OnRelease,
                    .style = (_layout->mouseover( option_id ))
                    ? &_theme.option_hot
                    : &_theme.option_element
                });

                if( _layout->click( option_id ) && !has_selection )
                {
                    state.selection = i;
                    has_selection = true;
                    state.state = false;
                }
            }
            _layout->pop();
            _layout->pushDeferredOp<OptionParam>(uid[0], widget_id, repositionOptionListOp, {&state});
        }
        _layout->pop();
        selection = state.selection;
        return has_selection;
    }

    bool  UI::select( const ui::UID& uid, std::vector< ui::Text >& list, int& selection )
    {
        bool has_selection = false;
        _layout->push({ .style = &_theme.select });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put
            ({
                .user_id = uid[i],
                .text = list[i],
                .single_click = ui::ClickTrigger::OnRelease,
                .style = (_layout->mouseover( uid[i] ))
                ? &_theme.option_hot
                : &_theme.option_element

            });

            if( _layout->click( uid[i] ) && !has_selection )
            {
                selection = i;
                has_selection = true;
            }
        }
        _layout->pop();
        return has_selection;
    }

    bool UI::tab( const ui::UID& uid, std::vector< ui::Text >& list, int& selection )
    {
        bool active = false;
        _layout->push({ .style = &_theme.tab });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put
            ({
                .user_id = uid[i],
                .text = list[i],
                .single_click = ui::ClickTrigger::OnRelease,
                .style = (selection == i) ? &_theme.tab_elem_focus : &_theme.tab_elem
            });
            if( _layout->click( uid[i] ) )
            {
                selection = i;
                active = true;
            }
        }
        _layout->pop();
        return active;
    }

    void UI::list( std::vector< ui::Text >& list )
    {
        _layout->push({ .style = &_theme.list });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put
            ({
                .text = list[i],
                .style = &_theme.list_elem
            });
        }
        _layout->pop();
    }

    bool UI::numSlideBar( ui::ID user_id, double* val, double min, double max )
    {
        char snum[16];
        snprintf(snum, 16, "%.6g", *val );

        ui::WidgetId widget_index = _layout->push
        ({
            .user_id = user_id,
            .style = &_theme.slide_bar_track,
            .single_click = ui::ClickTrigger::Continuous,
            .double_click = ui::ClickTrigger::Immediate
        });
        _layout->put({ .user_id = user_id, .style = &_theme.slide_bar });
        _layout->put({ .style = &_theme.slide_bar_value, .text = snum });
        _layout->pop();

        _layout->pushDeferredOp(user_id, widget_index, slidebarOp<double>, kege::RangeParams<double>{min, max, val, widget_index});
        return _layout->click( user_id );
    }

    bool UI::charButn(ui::ID user_id, const char* label, float x, float y)
    {
        // Back button
        ui::Text text;
        text.width = 10;
        text.height = 20;
        text.color = 0xFFFFFFFF;
        text.font_size = 20;
        text.ptr = label;
        text.x = x;
        text.y = y;

        _layout->put
        ({
            .user_id = user_id,
            .rect = ui::Rect{.width = 20.f, .height = 20.f, .x = 0.f, .y = 0.f },
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = {10,10,10,10},
            .color = 0xFFFFFF0F,
            .text = text,
        });
        return _layout->click(user_id);
    }

    bool UI::dotButn( ui::ID user_id )
    {
        _layout->put
        ({
            .user_id = user_id,
            .rect.height = 10,
            .rect.width = 10,
            .rect.y = 4,
            .color = _layout->mouseover( user_id ) ? 0xEE2200FF : 0xEE220050,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = {8,8,8,8}
        });
        return _layout->click( user_id );
    }

    bool UI::button( const kege::ui::WidgetDesc& desc )
    {
        _layout->put( desc );
        return _layout->click( desc.user_id );
    }

    bool UI::button( ui::ID user_id, const ui::Text& text, const ui::Style* style )
    {
        _layout->put
        ({
            .user_id = user_id,
            .text = text,
            .style = (style == nullptr)? &_theme.button : style,
            .single_click = ui::ClickTrigger::OnRelease,
        });
        return _layout->click( user_id );
    }

    ui::WidgetId UI::fittedLabel( const ui::Text& text, ui::Style* style )
    {
        return _layout->put
        ({
            .text = text,
            .style = (style == nullptr)? &_theme.fitted_label : style,
            .mouseover = false
        });
    }

    ui::WidgetId UI::label( const ui::Text& text, ui::Style* style )
    {
        return _layout->put
        ({
            .text = text,
            .style = (style == nullptr)? &_theme.label : style,
            .mouseover = false
        });
    }

    bool UI::radio( ui::ID user_id, const ui::Text& text, bool& state )
    {
        push({.style = &_theme.row });
        radio( user_id, state );
        fittedLabel(text);
        pop();
        return state;
    }

    bool UI::radio( ui::ID user_id, bool& state )
    {
        _layout->put
        ({
            .style = (state)? &_theme.radio_active: &_theme.radio,
            .user_id = user_id,
            .rect.y = 4,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .color = (state == 0) ? 0xFFBB0040 : 0xFFBB00FF,
            .border.corner_curves = {5,5,5,5}
        });
        if ( _layout->click(user_id) )
        {
            state = !state;
        }
        return state;
    }

    bool UI::radio( ui::ID user_id, int16_t layer )
    {
        RadioState& state = _radio_states[ user_id ];
        return radio(user_id, state.state);
    }

    ui::WidgetId UI::beginList(int16_t layer)
    {
        return _layout->push({ .style = &_theme.list });
    }
    
    void UI::endList()
    {
        _layout->pop();
    }

    ui::WidgetId UI::beginColumn( ui::Style* style )
    {
        return _layout->push
        ({
            .style = (style == nullptr)? &_theme.column : style
        });
    }
    void UI::endColumn()
    {
        _layout->pop();
    }

    ui::WidgetId UI::beginRow( ui::Style* style )
    {
        return _layout->push
        ({
            .style = (style == nullptr)? &_theme.row : style
        });
    }
    void UI::endRow()
    {
        _layout->pop();
    }


    struct ScrollOp
    {
        ui::WidgetId widget_id;
        float& amount;
    };
    void scrollingOperation(ui::Layout* layout, ui::ID user_id, ui::WidgetId widget_id, void* data)
    {
        ScrollOp* params = reinterpret_cast<ScrollOp*>(data);

        ui::Widget* scrolly = layout->elem(params->widget_id);
        ui::Widget* clipper = layout->elem(widget_id);

        if (layout->testPointVsRect( layout->getPointerPosition(), clipper->rect ))
        {
            float scroll_length = scrolly->rect.height - clipper->rect.height;
//            if ( 0 < scroll_length)
//            {
                params->amount += layout->getScrollOffset().y;
                params->amount = kege::min(0.f, kege::max(params->amount, -scroll_length));
                std::cout << params->amount <<"\n";
//            }
//            else
//            {
//                params->amount = 0.f;
//            }
        }
        scrolly->rect.y = clipper->rect.y + params->amount;
    }
    
    void UI::beginScrollContainer( const ui::ID id[2] )
    {
        ScrollContainer* state;
        auto i = _scroll_containers.find( id[0] );
        if (i == _scroll_containers.end())
        {
            state = &_scroll_containers[ id[0] ];
            state->scroll_amount = 0.f;
            state->frame_id = _frame_index;
        }
        else state = &i->second;

        ui::WidgetId clipper = _layout->push /* the clipper is the container the scroller is clipped against */
        ({
            .user_id = id[0],
            .style = &_theme.scroll_container,
            .clip_overflow = true
        });

        ui::WidgetId scroller = _layout->push /* the scroller is the container that is scrolled */
        ({
            .user_id = id[1],
            .style = &_theme.column,
            .rect.y = state->scroll_amount,
            .mouseover = false,
        });

        _layout->pushDeferredOp(id[0], clipper, scrollingOperation, ScrollOp{
            .widget_id = scroller,
            .amount = state->scroll_amount
        });
    }
    
    void UI::endScrollContainer()
    {
        _layout->pop(); /* pop the scroller container */
        _layout->pop(); /* pop the clipper container */
    }

    ui::WidgetId UI::pushRoot( const ui::WidgetDesc& desc )
    {
        return _layout->pushRoot(desc);
    }

    ui::WidgetId UI::putRoot( const ui::WidgetDesc& desc )
    {
        return _layout->putRoot(desc);
    }
    
    void UI::popRoot()
    {
        _layout->popRoot();
    }

    ui::WidgetId UI::push( const kege::ui::WidgetDesc& desc )
    {
        return _layout->push( desc );
    }

    ui::WidgetId UI::put( const kege::ui::WidgetDesc& desc )
    {
        return _layout->put( desc );
    }

    ui::WidgetId UI::text( const kege::ui::Text& text )
    {
        return _layout->text( text );
    }

    void UI::pop()
    {
        _layout->pop();
    }
    void UI::pushLayer( uint32_t index )
    {
        _layout->pushLayer( index );
    }

    bool UI::popLayer()
    {
        return _layout->popLayer();
    }

    const bool UI::pointerDragging() const
    {
        return _layout->isPointerDragging();
    }

    bool UI::mouseover( const ui::ID& id )
    {
        return _layout->mouseover( id );
    }

    bool UI::click( const ui::ID& id )
    {
        return _layout->click( id );
    }

    bool UI::hot( const ui::ID& id )
    {
        return _layout->mouseover( id );
    }

    bool UI::leftClickDown()const
    {
        return _layout->leftClickDown();
    }

    bool UI::dragging()const
    {
        return _layout->isPointerDragging();
    }

    ui::Widget* UI::get( ui::WidgetId index )
    {
        return _layout->elem( index );
    }

    bool UI::initialize(Ref< ui::Layout > layout)
    {
        _layout = layout;
        return true;
    }

    void UI::createLayers( uint32_t quantity )
    {
        _layout->createLayers( quantity );
    }

    vec2d UI::deltaPointer()const
    {
        return _layout->getPointerDelta();
    }

    vec2d UI::pointer()const
    {
        return _layout->getPointerPosition();
    }

    void UI::begin( double dms )
    {
        ++_frame_index;

        // reset layout state
        _layout->begin( dms );
    }

    void UI::end()
    {
        _layout->end();

        // cleanup dead states
//        for (auto it = _collapsable_headers.begin(); it != _collapsable_headers.end(); )
//        {
//            if (_frame_index - it->second.last_frame_value > 2)
//                it = _collapsable_headers.erase(it);
//            else
//                ++it;
//        }
//        for (auto it = _removable_headers.begin(); it != _removable_headers.end(); )
//        {
//            if (_frame_index - it->second.last_frame_value > 2)
//                it = _removable_headers.erase(it);
//            else
//                ++it;
//        }
//        for (auto it = _numeric_states.begin(); it != _numeric_states.end(); )
//        {
//            if (_frame_index - it->second.last_frame_value > 2)
//                it = _numeric_states.erase(it);
//            else
//                ++it;
//        }
//        for (auto it = _scrubbers.begin(); it != _scrubbers.end(); )
//        {
//            if (_frame_index - it->second.last_frame_value > 2)
//                it = _scrubbers.erase(it);
//            else
//                ++it;
//        }
//        for (auto it = _input_texts.begin(); it != _input_texts.end(); )
//        {
//            if (_frame_index - it->second.last_frame_value > 2)
//                it = _input_texts.erase(it);
//            else
//                ++it;
//        }
    }

    UI::~UI()
    {}

    UI::UI()
    :   _frame_index(0)
    ,   _str_len(0)
    {
    }


    Theme::Theme()
    {
        panel = kege::ui::Style
        {
            .background = ui::Background(0x171420FF),
            //.border.corner_curves = {10,10,10,10},
            .height = ui::extend(),
            .width = ui::extend(),
            .padding = {10,10,10,10},
            .gap = {5,5},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN
            }
        };
        Theme::padded_list = kege::ui::Style
        {
            //.border.corner_curves = {20,20,20,20},
            .background = ui::Background(0xFFFFFF00),
            .padding = {10,10,10,10},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };
        Theme::panel_transparnt = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF00),
            .height = ui::extend(),
            .width = ui::extend(),
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };

        Theme::scroll_container = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF00),
            .height = ui::extend(),
            .width = ui::extend(),
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };

        Theme::group = kege::ui::Style
        {
            //.border.corner_curves = {20,20,20,20},
            .background = ui::Background(0xFFFFFF00),
            .padding = {10, 0, 10, 0},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };

        dock.width = ui::extend();
        dock.height = ui::extend();
        dock.padding = {};
        dock.background = ui::Background(0x0b090fFF);
        dock.alignment = ui::Alignment
        {
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = ui::AlignDir::DOWN,
        };

        tab = kege::ui::Style
        {
            .background = ui::Background(0x0B090F00),
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {4,0},
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            }
        };

        tab_elem = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::flexible(),
            .background = ui::Background(0x171420FF),
            .align_text =  ui::AlignText::Center,
            .padding = {10,4,10,4},
            .gap = {20,0}
        };

        tab_label = kege::ui::Style
        {
            .height = ui::fixed(20),
            .width = ui::flexible(),
            .background = ui::Background(0xFFFFFF00),
            .align_text =  ui::AlignText::Left,
            .text_color = 0xBBA0FFFF,
            .font_size = 20,
        };

        ghost = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF20),
            .align_text =  ui::AlignText::Center,
            .position = ui::Positioning::Absolute,
            .text_color = 0xBBA0FFFF,
            .height = ui::fixed(40),
            .width = ui::fixed(100),
            .font_size = 20,
            .border.corner_curves = {8,8,8,8},
        };



        Theme::y_seperator = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .height = ui::extend(),
            .width = ui::fixed(0),
        };

        Theme::x_seperator = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .height = ui::fixed(0),
            .width = ui::extend(),
        };

        Theme::label = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {2,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
        };

        Theme::fitted_label = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {2,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::flexible(),
            .font_size = 20,
        };

        row = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0xFFFFFF00),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };

        Theme::row_wrap = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0xFFFFFF00),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .wrap = {true, ui::AlignDir::DOWN},
            },
            .gap = {8, 8},
        };

        column = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF08),
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };



        Theme::slider_container = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0xFFFFFF0B),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            },
            .border.corner_curves = {6,6,6,6},
            .padding = {6,5,6,5},
            .gap.width = 6,
        };
        Theme::slider_inner_container = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF0B),
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };

        Theme::slider_row = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0xFFFFFF00),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            },
            .gap = {6, 6},
            .border.corner_curves = {6,6,6,6},
        };
        Theme::slider_name = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {2,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::flexible(),
            .font_size = 20,
        };
        Theme::slider_value = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {2,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::flexible(),
            .font_size = 20,
        };
        Theme::slider_range = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {2,0,0,0},
            .text_color = 0xFFFFFF50,
            .height = ui::fixed(18),
            .width = ui::flexible(),
            .font_size = 20,
        };



        Theme::radio = kege::ui::Style
        {
            .height = ui::fixed(10),
            .width = ui::fixed(10),
            .background = 0xFFBB0040,
            .border.corner_curves = {5,5,5,5},
        };
        Theme::radio_active = kege::ui::Style
        {
            .height = ui::fixed(10),
            .width = ui::fixed(10),
            .background = 0xFFBB00FF,
            .border.corner_curves = {5,5,5,5},
        };

        Theme::button = kege::ui::Style
        {
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF20,
            .align_text =  ui::AlignText::Center,
            .text_color = 0xFFFFFFFF
        };
        disabled_button = kege::ui::Style
        {
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF10,
            .align_text =  ui::AlignText::Center,
            .text_color = 0xFFFFFFF0
        };

        Theme::collapsable_removable_header = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .border.corner_curves = {10,10,10,10},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .padding = {6,0,6,0},
            .gap = {4,0}
        };

        container_header = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .border.corner_curves = {0,0,0,0},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .padding = {0,0,0,0},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4,0}
        };
        container_body = kege::ui::Style{
            .background = ui::Background(0x23232300),
            .border.corner_curves = {0,0,0,0},
            .height = ui::flexible(),
            .width = ui::extend(),
            .padding = {4,4,4,4},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4,4}
        };



        Theme::scrubber_row = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0xFFFFFF10),
            .padding = {8,0,8,0},
            .border.corner_curves = {6,6,6,6},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };
        Theme::scrubber = kege::ui::Style
        {
            .background = ui::Background( 0xFFFFFF06 ),
            .height = ui::fixed(20),
            .width = ui::extend(),
            //.padding = {4,0,4,0},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            },
            .align_text = ui::AlignText::Right,
            .text_color = 0xFFFFFFFF,
        };
        Theme::scrubber_focus = kege::ui::Style
        {
            .background = ui::Background( 0xFFFFFF1B ),
            .height = ui::fixed(20),
            .width = ui::extend(),
            .padding = {8,0,8,0},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            },
            .align_text = ui::AlignText::Right,
            .text_color = 0xFFFFFFFF,
        };
        Theme::scrubber_label = kege::ui::Style
        {
            .background = ui::Background( 0xFFFFFF00 ),
            .height = ui::fixed(20),
            .width = ui::fixed(20),
            //.padding = {8,0,8,0},
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
        };


        Theme::card = kege::ui::Style
        {
            .background = ui::Background( 0xFFFFFF0B ),
            .border.corner_curves = {6,6,6,6},
            .height = ui::flexible(),
            .width = ui::extend(),
            .padding = {10, 10, 10, 10},
            .alignment =
            {
                .direction = ui::AlignDir::DOWN
            },
            .gap = {4, 4},
        };


        card2 = kege::ui::Style
        {
            .background = ui::Background( 0xFFFFFF0B ),
            .border.corner_curves = {6,6,6,6},
            .height = ui::flexible(),
            .width = ui::extend(),
            .padding = {10, 10, 10, 10},
            .alignment =
            {
                .direction = ui::AlignDir::RIGHT
            },
            .gap = {4, 4},
        };



        Theme::navbar = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF0F),
            .border.corner_curves = {20,20,20,20},
            .padding = {6,6,6,6},
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {6,6},
            .alignment =
            {
                .direction = ui::AlignDir::RIGHT
            },
        };
        Theme::broswer_file_path = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF0F),
            .border.corner_curves = {10,10,10,10},
            .padding = {10,0,10,0},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .text_color = 0xFFFFFFFF,
            .align_text = ui::AlignText::Left,
        };
        Theme::filter_container = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF00),
            .height = ui::fixed(20),
            .width = ui::percent(30),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };

        Theme::filter_input = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF0D),
            .border.corner_curves = {10,10,10,10},
            .padding = {10,0,10,0},
            .height = ui::fixed(20),
            .width = ui::extend(),
        };

        Theme::file_container = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF00),
            .padding = {10,10,10,10},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .wrap = {true, ui::AlignDir::DOWN},
            },
            .gap = {8, 8},
        };


        option = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF06),
            .border.corner_curves = {6,6,6,6},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };
        option_list = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF10),
            .padding = {5,5,5,5},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };
        option_selection = kege::ui::Style
        {
            .background = 0xFFFFFF40,
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };
        option_hot = kege::ui::Style
        {
            .background = 0xFFFFFF30,
            .border.corner_curves = {4,4,4,4},
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };
        option_element = kege::ui::Style
        {
            .background = 0xFFFFFF10,
            .border.corner_curves = {4,4,4,4},
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };





        text_field_editing = kege::ui::Style
        {
            .background = 0x456E66FA,
            .border.corner_curves = {4,4,4,4},
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };
        text_field_focus = kege::ui::Style
        {
            .background = 0xFFFFFF0A,
            .border.corner_curves = {4,4,4,4},
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };
        text_field_idle = kege::ui::Style
        {
            .background = 0xFFFFFF05,
            .border.corner_curves = {4,4,4,4},
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };

        hierarchy_main_bar  = kege::ui::Style
        {
            .background = ui::Background(0x222222FF),
            .padding = {5,5,5,5},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };
        hierarchy_entity = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF10),
            .border.corner_curves = {2,2,2,2},
            .padding = {4,4,4,4},
            .height = ui::fixed(24),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };
        hierarchy_entity = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF03),
            .border.corner_curves = {4,4,4,4},
            .padding = {4,4,4,4},
            .height = ui::fixed(24),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };
        hierarchy_entity_selected = hierarchy_entity;
        hierarchy_entity_selected.background = ui::Background(0xFFFFFF08);



        close_butn = kege::ui::Style{
            .height = ui::fixed(18),
            .width = ui::fixed(18),
            .background = 0xFFFFFF20,
            .align_text =  ui::AlignText::Center,
            .font_size = 20,
            .border.corner_curves = {2,2,2,2},
        };

        slider_knob = kege::ui::Style{
            .height = ui::fixed(15),
            .width = ui::fixed(15),
            .background = 0xFF22FFFF,
            .align_text =  ui::AlignText::Center,
        };
        slider_track = kege::ui::Style{
            .height = ui::fixed(15),
            .width = ui::extend(),
            .background = 0xFFFFFF0B,
            .align_text = ui::AlignText::Center,
        };


        slide_bar = kege::ui::Style{
            .background = 0xFF22FFFF,
            .position = ui::Positioning::Absolute,
            .height = ui::fixed(14),
            .width = ui::fixed(14),
        };
        slide_bar = kege::ui::Style{
            .background = 0xFFFFFF20,
            .height = ui::fixed(14),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            }
        };
        slide_bar_value = kege::ui::Style{
            .position = ui::Positioning::Absolute,
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Center,
            .height = ui::fixed(14),
            .width = ui::percent(100),
            .font_size = 20,
        };


        collapsable_header = kege::ui::Style{
            .background = ui::Background(0x23232300),
            .height = ui::fixed(20),
            .width = ui::extend(),
            .padding = {6,0,6,0},
            .gap = {4,0}
        };

        list = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {2,2},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            }
        };
        list_elem = kege::ui::Style{
            .border.corner_curves = {4,4,4,4},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF10,
            .font_size = 20,
            .text_color = 0xFFFFFFFF,
        };
        list_elem_focus = kege::ui::Style{
            .border.corner_curves = {4,4,4,4},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF30,
            .font_size = 20,
            .text_color = 0xFFFFFFFF,
        };


        select = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {2,2},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            }
        };
        select_elem = kege::ui::Style{
            .border.corner_curves = {4,4,4,4},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF10,
            .font_size = 20,
            .text_color = 0xFFFFFFFF,
        };
        select_elem_focus = kege::ui::Style{
            .border.corner_curves = {4,4,4,4},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF30,
            .font_size = 20,
            .text_color = 0xFFFFFFFF,
        };


        tab = kege::ui::Style{
            .background = ui::Background(0x10101000),
            .height = ui::fixed(20),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            }
        };
        tab_elem = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::fixed(120),
            .background = 0x171420FF,
            .align_text = ui::AlignText::Center,
            .text_color = 0xBBA0FFFF,
            .font_size = 20,
        };
        tab_elem_focus = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::fixed(120),
            .background = ui::Background(0x171420FF),
            .align_text = ui::AlignText::Center,
            .border.corner_curves = {4,4,4,4},
            .font_size = 20,
        };

        hierarchy_panel = kege::ui::Style{
            .background = ui::Background(0x433333FF),
            .height = ui::extend(),
            .width = ui::fixed(400),
            //.padding = {10,10,10,10},
            .alignment =
            {
                .content = {ui::AlignX::CENTER, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN
            }
        };
        viewport_panel = kege::ui::Style{
            .height = ui::extend(),
            .width = ui::extend(),
            .background = ui::Background(0x222222FF),
            //.padding = {10,10,10,10},
            .alignment =
            {
                .content = {ui::AlignX::RIGHT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            }
        };


        numeric = kege::ui::Style{
            .background = ui::Background(0xFFFFFF0B),
            .height = ui::fixed(18),
            .width = ui::extend(),
            .padding = {8,2,8,2},
            .gap = {2,2},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            }
        };
        numeric_label = kege::ui::Style{
            .background = ui::Background(0xFFFFFF00),
            .align_text = ui::AlignText::Right,
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::fixed(100),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            }
        };
        numeric_value = kege::ui::Style{
            .background = ui::Background(0xFFFFFF00),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            }
        };
        numeric_focus = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            }
        };


        folder_icon = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(40),
            .width = ui::fixed(60),
            .font_size = 20,
        };
        file_icon = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(40),
            .width = ui::fixed(20),
            .font_size = 20,
        };
        selected_item = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(40),
            .width = ui::fixed(40),
            .font_size = 20,
        };
        selection_box = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(40),
            .width = ui::fixed(40),
            .font_size = 20,
        };
        tooltip = kege::ui::Style{
            .background = ui::Background(0xCC7F41FF),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .padding = {10,10,10,10},
            .height = ui::fixed(150),
            .width = ui::fixed(250),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT,
                .wrap = {true, ui::AlignDir::DOWN},
            }
        };
    }
}
