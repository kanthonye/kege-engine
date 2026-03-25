//
//  gui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/8/25.
//

#include "gui.hpp"

namespace kege{

    bool GUI::submit( ui::UserId user_id, const char* label)
    {
        _layout->put
        ({
            .layer = 2,
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
            .color = 0xFFFFFF08,
            .padding = {10,5,5,10},
            .width = ui::extend(),
            .height = ui::fixed(30),
        });
        return _layout->click( user_id );
    }

    void GUI::text(const char* label, char* input, size_t& size, size_t capacity)
    {
        _layout->push
        ({
            .layer = 2,
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
            .height = ui::fixed(40),
        });
        _layout->put
        ({
            .layer = 2,
            .text = ui::Text{.width = 60, .height = 20, .color = 0xFFFFFF30, .font_size = 24, .ptr = label},
            .single_click = ui::ClickTrigger::OnRelease,
            .rect = {0,0, 60, 30},
            .color = 0xFFFFFF00,
            .padding = {10,5,5,10},
        });
        textField(2, getAddressAsInt(input), input, capacity, size);
        _layout->pop();
    }

    void GUI::input( ui::UserId user_id, const char* label, double& value)
    {
        _layout->push
        ({
            .layer = 2,
            .padding = {10,5,10,5},
            //.border.corner_curves = {border_radius,0,0,border_radius},
            //.rect = {0,0,200,30},
            .color = 0xFFFFFF08,
            .gap = {2,2},
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .items = ui::AlignItem::CENTER,
            },
            .width = ui::extend(),
            .height = ui::fixed(40),
        });
        _layout->put
        ({
            .layer = 2,
            .text = ui::Text{.width = 60, .height = 24, .color = 0xFFFFFF30, .font_size = 24, .ptr = label},
            .single_click = ui::ClickTrigger::OnRelease,
            .rect = {0,0, 60, 24},
            .color = 0xFFFFFF00,
            //.padding = {10,5,5,10},
        });
        scrubber(user_id, 2, value);
        _layout->pop();
    }


    int GUI::collapsableRemovableHeaderInput( ui::UserId user_id, int16_t layer, char* str, size_t& size )
    {
        CollapsableRemovableHeader* header = State<CollapsableRemovableHeader>::get(user_id);
        //CollapsableRemovableHeader& header = _collapsable_removable_headers[ user_id ];
        const ui::UserId Id[4] = {user_id, user_id + 1, user_id + 2, user_id + 3};

        _layout->push
        ({
            .layer = layer,
            .user_id = Id[0],
            .style = &_theme.collapsable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        radio( Id[1], layer, header->state );
        ui::WidgetId widget_id = _layout->put
        ({
            .layer = layer,
            .user_id = Id[2],
            .style = ( header->mode == 2 ) ? &_theme.collapsable_element_hl : &_theme.collapsable_element,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .text.ptr = str,
        });
        _layout->put /* remove button */
        ({
            .layer = layer,
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

    int GUI::collapsableRemovableHeader( ui::UserId user_id, int16_t layer, const ui::Text& text )
    {
        CollapsableRemovableHeader& header = _collapsable_removable_headers[ user_id ];
        const ui::UserId Id[3] = {user_id, user_id + 1, user_id + 2};

        _layout->push
        ({
            .layer = layer,
            .user_id = Id[0],
            .style = &_theme.collapsable_removable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        radio( Id[1], layer, header.state );
        label(layer, text);
        bool state = dotButn( Id[2], layer );
        _layout->pop();

        if ( _layout->click( Id[0] ) )
        {
            return 2;
        }
        if ( state )
        {
            return -1;
        }
        return header.state;
    }


    bool GUI::collapsableHeader( ui::UserId user_id, int16_t layer, const ui::Text& text )
    {
        CollapsableHeader& header = _collapsable_headers[ user_id ];
        const uint64_t Id[3] = {user_id, user_id + 1, user_id + 2};

        _layout->push
        ({
            .layer = layer,
            .user_id = Id[0],
            .style = &_theme.collapsable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        _layout->put
        ({
            .layer = layer,
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
            .layer = layer,
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

    int GUI::removableHeader( ui::UserId user_id, int16_t layer, const ui::Text& text )
    {
        RemovableHeader& header = _removable_headers[ user_id ];
        const ui::UserId Id[4] = {user_id, user_id + 1, user_id + 2, user_id + 3};

        _layout->push
        ({
            .layer = layer,
            .user_id = Id[0],
            .style = &_theme.collapsable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _layout->put
        ({
            .layer = layer,
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
            .layer = layer,
            .user_id = Id[2],
            .style = &_theme.label,
            .text = text,
            .mouseover = false,
        });
        _layout->put
        ({
            .layer = layer,
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

        if ( _layout->click(Id[0]) )
        {
            header.state = !header.state;
        }
        if ( _layout->click(Id[3]) )
        {
            return 2;
        }
        return header.state;
    }


    void GUI::textFieldOp(ui::Layout* layout, ui::UserId user_id, ui::WidgetId widget_id, void* data)
    {
        TextField* text = static_cast<TextField*>(data);
        //TextFieldState* state = text->state;

        text->edited = false;

        if (layout->click( user_id ))
        {
            // Handle single click
            if (text->mode != TextFieldMode::Editing)
            {
                text->mode = TextFieldMode::Focused;
                layout->cursor()->startEditing
                (
                    user_id,
                    widget_id,
                    text->type,
                    text->buffer,
                    text->current_size,
                    text->buffer_capacity
                );
            }
        }
        else if (layout->doubleClick( user_id ))
        {
            layout->cursor()->startEditing
            (
                user_id,
                widget_id,
                ui::Cursor::InputType::Any,
                text->buffer,
                text->current_size,
                text->buffer_capacity
            );
            
            layout->cursor()->computeCursorPosition();

            // Select all text
            if (text->mode == TextFieldMode::Editing)
            {
                layout->cursor()->selectAll(text->buffer, text->current_size);
            }
            text->mode = TextFieldMode::Editing;
        }
        else if (!layout->hasFocus( user_id ) && text->mode != TextFieldMode::Idle)
        {
            // Lost focus
            if (text->mode == TextFieldMode::Editing)
            {
                text->edited = true; // Text was being edited
            }
            layout->cursor()->stopEditing();
            text->mode = TextFieldMode::Idle;
        }

        // Handle text input if editing
        if (text->mode == TextFieldMode::Editing)
        {
            if (!layout->cursor()->update())
            {
                text->mode = TextFieldMode::Idle;
                text->edited = true;
            }
        }
    }

    bool GUI::textField( ui::UserId user_id, int16_t layer, char* buffer, size_t buffer_capacity, size_t& current_size )
    {
        TextFieldState& state = _input_texts[ user_id ];

        ui::Style* style = nullptr;
        switch (state.mode)
        {
            case TextFieldMode::Editing:
                style = &_theme.text_field_editing;
                break;
            case TextFieldMode::Focused:
                style = &_theme.text_field_focus;
                break;
            default: style = &_theme.text_field_idle;
        }

        ui::WidgetId widget_index = _layout->put
        ({
            .layer = layer,
            .user_id = user_id,
            .style = style,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .text.ptr = buffer,
        });

        _layout->pushDeferredOp(user_id, widget_index, textFieldOp, TextField
        {
            .type = ui::Cursor::InputType::Any,
            .mode = state.mode,
            .edited = state.edited,
            .buffer = buffer,
            .buffer_capacity = buffer_capacity,
            .current_size = current_size
        });

        return state.edited;
    }

    bool GUI::scrubber( ui::UserId user_id, int16_t layer, double& num )
    {
        ScrubberState* state;
        auto i = _scrubbers.find( user_id );
        if (i == _scrubbers.end())
        {
            state = &_scrubbers[user_id];
            snprintf(state->str, 16, "%.3f", num);
            state->text_value = _layout->text(state->str, 20);
            state->text_value.color = 0xFFFFFFFF;
            state->sensitivity = 0.002;
            state->type = ScrubberState::DOUBLE;
        }
        else state = &i->second;
        return scrubber<double>( user_id, layer, num, state );
    }

    bool GUI::scrubber( ui::UserId user_id, int16_t layer, float& num )
    {
        ScrubberState* state;
        auto i = _scrubbers.find( user_id );
        if (i == _scrubbers.end())
        {
            state = &_scrubbers[user_id];
            snprintf(state->str, 16, "%.3f", num);
            state->text_value = _layout->text(state->str, 20);
            state->text_value.color = 0xFFFFFFFF;
            state->sensitivity = 0.002;
            state->type = ScrubberState::FLOAT;
        }
        else state = &i->second;
        return scrubber<float>( user_id, layer, num, state );
    }

    bool GUI::scrubber( ui::UserId user_id, int16_t layer, int& num )
    {
        State<ScrubberState>::get(user_id);
        ScrubberState* state;
        auto i = _scrubbers.find( user_id );
        if (i == _scrubbers.end())
        {
            state = &_scrubbers[user_id];
            snprintf(state->str, 16, "%i", num);
            state->text_value = _layout->text(state->str, 20);
            state->text_value.color = 0xFFFFFFFF;
            state->sensitivity = 0.002;
            state->type = ScrubberState::INT32;
        }
        else state = &i->second;
        return scrubber<int>( user_id, layer, num, state );
    }

//    bool GUI::scrubber( int16_t layer, const ui::Text& name, int& num )
//    {
//        _layout->push({ .layer = layer, .style = &_theme.scrubber_row });
//        fittedLabel(layer, name);
//        scrubber( layer, num );
//        _layout->pop();
//        return false;
//    }

    bool GUI::scrubber( ui::UserId user_id, int16_t layer, const ui::Text& name, float& num )
    {
        _layout->push({ .layer = layer, .style = &_theme.scrubber_row });
        fittedLabel(layer, name);
        scrubber( user_id, layer, num );
        _layout->pop();
        return false;
    }

    bool GUI::scrubber( ui::UserId user_id, int16_t layer, const ui::Text& name, double& num )
    {
        _layout->push({ .layer = layer, .style = &_theme.scrubber_row });
        fittedLabel(layer, name);
        scrubber( user_id, layer, num );
        _layout->pop();
        return false;
    }

    bool GUI::scrubber4( ui::UserId user_id, int16_t layer, const ui::Text& name, float& x, float& y, float& z, float& w )
    {
        bool modified[3] = {};

        beginColumn(layer);
        {
            label(layer, name);

            beginColumn(layer);
            beginRow(layer);
            ui::Text x_label{.ptr = "x:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[0] = scrubber(user_id + 1, layer, x_label, x);

            ui::Text y_label{.ptr = "y:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[1] = scrubber(user_id + 2, layer, y_label, y);

            ui::Text z_label{.ptr = "z:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[2] = scrubber(user_id + 3, layer, z_label, z);
            endRow();

            ui::Text w_label{.ptr = "w:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[2] = scrubber(user_id + 4, layer, w_label, w);
            endColumn();
        }
        endColumn();

        return modified[0] || modified[1] || modified[2];
    }

    bool GUI::scrubber3
    (
     ui::UserId user_id, int16_t layer, const ui::Text& name,
     float& x, float& y, float& z
    )
    {
        bool modified[3] = {};

        beginColumn(layer);
        {
            label(layer, name);
            beginRow(layer);
            ui::Text x_label{.ptr = "x:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[0] = scrubber(user_id + 1, layer, x_label, x);

            ui::Text y_label{.ptr = "y:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[1] = scrubber(user_id + 2, layer, y_label, y);

            ui::Text z_label{.ptr = "z:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[2] = scrubber(user_id + 3, layer, z_label, z);
            endRow();
        }
        endColumn();

        return modified[0] || modified[1] || modified[2];
    }


    bool GUI::numeric( int16_t layer, ui::UserId user_id, double& num )
    {
        const uint64_t Id[3] = {user_id, user_id+1};
        InputNumeric* state;
        auto i = _numeric_states.find( user_id );
        if (i == _numeric_states.end())
        {
            state = &_numeric_states[user_id];
            state->mode = 0;
            state->last_frame_value = 0;
            snprintf(state->str, state->size, "%.6g", num);
        }
        else state = &i->second;

        _layout->push(kege::ui::WidgetDesc{
            .user_id = Id[0],
            .layer = layer,
            .style = &_theme.numeric,
        });
            ui::WidgetId widget_id = _layout->put
            ({
                .layer = layer,
                .user_id = Id[1],
                .style = (( state->mode == 2 )? &_theme.numeric_focus : &_theme.numeric_value ),
                .single_click = ui::ClickTrigger::OnRelease,
                .double_click = ui::ClickTrigger::Immediate,
                .text.ptr = state->str
            });
        _layout->pop();

        bool clicked = false;

        if ( _layout->click( Id[1] ) )
        {
            clicked = true;
            state->mode += 1;
        }
        else if ( _layout->doubleClick( Id[1] ) )
        {
            state->mode = 2;
        }
        else if ( !_layout->hasFocus( Id[1] ) && state->mode > 0 )
        {
            state->mode = 0;
        }

        if ( state->mode == 2 )
        {
            if ( !_layout->onTextInput( widget_id, state->str, state->size ) )
            {
                state->mode = 0;
            }
            num = atof( state->str );
        }

        return clicked;
    }


    struct OptionParam{ GUI::OptionState* state; };
    void repositionOptionListOp(ui::Layout* layout, ui::UserId user_id, ui::WidgetId widget_id, void* data)
    {
        OptionParam* state = reinterpret_cast< OptionParam* >(data);
        ui::Widget* main = layout->elem( widget_id );
        ui::Widget* list = layout->elem( state->state->index );
        if ( list )
        {
            list->rect.x = main->rect.x;
            list->rect.y = main->rect.y + main->rect.height;
            list->rect.width = main->rect.width;
            list->rect.height = 0.f;
        }

        for (uint32_t eid = layout->head( widget_id.index ); eid != 0 ; eid = layout->next( eid ) )
        {
            layout->operator[](eid)->rect.x = 0.f;
            layout->operator[](eid)->rect.y = 0.f;
        }
        ui::Resizer::resize(*layout, widget_id.index);
        ui::Aligner::align(*layout, widget_id.index);

        if (layout->leftClickDown())
        {
            const kege::dvec2& position = layout->getPointerPosition();
            if (!layout->testPointVsRect(position, main->rect) && !layout->testPointVsRect(position, list->rect))
            {
                state->state->state = false;
            }
        }
    }

    bool GUI::options( ui::UserId user_id, int16_t layer,std::vector<ListElem>& list, int& selection )
    {
        OptionState& state = _option_states[ user_id ];
        bool has_selection = false;

        ui::WidgetId widget_id = _layout->push
        ({
            .layer = layer,
            .user_id = user_id,
            .style = &_theme.option,
            .single_click = ui::ClickTrigger::Immediate,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        _layout->put
        ({
            .layer = layer,
            .style = &_theme.option_selection,
            .text = list[ state.selection ].text,
            .mouseover = false,
        });

        if ( _layout->click(user_id) )
        {
            state.state = !state.state;
        }

        if ( state.state )
        {
            _layout->pushRoot({.layer = int16_t(layer + 1), .user_id = user_id + 1, .style = &_theme.option_list});
            for (int i=0; i<list.size(); ++i)
            {
                _layout->put
                ({
                    .layer = layer,
                    .user_id = list[i].user_id,
                    .text = list[i].text,
                    .single_click = ui::ClickTrigger::OnRelease,
                    .style = (_layout->mouseover( list[i].user_id )) ? &_theme.option_hot : &_theme.option_element
                });

                if( _layout->click( list[i].user_id ) && !has_selection )
                {
                    state.selection = i;
                    has_selection = true;
                    state.state = false;
                }
            }
            _layout->popRoot();
            _layout->pushDeferredOp<OptionParam>(user_id, widget_id, repositionOptionListOp, {&state});
        }
        _layout->pop();
        selection = state.selection;
        return has_selection;
    }

    bool  GUI::select( int16_t layer, std::vector<ListElem>& list, int& selection )
    {
        bool has_selection = false;
        _layout->push
        ({
            .layer = layer,
            .style = &_theme.select
        });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put
            ({
                .layer = layer,
                .user_id = list[i].user_id,
                .text = list[i].text,
                .single_click = ui::ClickTrigger::OnRelease,
                .style = (_layout->mouseover( list[i].user_id )) ? &_theme.select_elem_focus : &_theme.select_elem

            });

            if( _layout->click( list[i].user_id ) && !has_selection )
            {
                selection = i;
                has_selection = true;
            }
        }
        _layout->pop();
        return has_selection;
    }

    void GUI::list( int16_t layer, std::vector<ListElem>& list, int selection )
    {
        _layout->push({ .layer = layer, .style = &_theme.list });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put
            ({
                .layer = layer,
                .user_id = list[i].user_id,
                .text = list[i].text,
                .style = &_theme.list_elem
            });
        }
        _layout->pop();
    }

    bool GUI::tab( int16_t layer, std::vector<ListElem>& list, int& selection )
    {
        bool active = false;
        _layout->push({ .layer = layer, .style = &_theme.tab });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put({
                .layer = layer,
                .user_id = list[i].user_id,
                .text = list[i].text,
                .single_click = ui::ClickTrigger::OnRelease,
                .style = (selection == i)
                    ? &_theme.tab_elem_focus
                    : &_theme.tab_elem
            });
            if( _layout->click( list[i].user_id ) )
            {
                selection = i;
                active = true;
            }
        }
        _layout->pop();
        return active;
    }

    bool GUI::numSlideBar( ui::UserId user_id, int16_t layer, double* val, double min, double max )
    {
        char snum[16];
        snprintf(snum, 16, "%.6g", *val );

        ui::WidgetId widget_index = _layout->push
        ({
            .layer = layer,
            .user_id = user_id,
            .style = &_theme.slide_bar_track,
            .single_click = ui::ClickTrigger::Continuous,
            .double_click = ui::ClickTrigger::Immediate
        });
        _layout->put({ .layer = layer, .user_id = user_id, .style = &_theme.slide_bar });
        _layout->put({ .layer = layer, .style = &_theme.slide_bar_value, .text = snum });
        _layout->pop();

        _layout->pushDeferredOp(user_id, widget_index, slidebarOp<double>, RangeParams{min,max,val,widget_index});

        return _layout->click( user_id );
    }

    bool GUI::charButn(ui::UserId user_id, const char* label, float x, float y)
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

    bool GUI::dotButn( ui::UserId user_id, int16_t layer )
    {
        _layout->put
        ({
            .layer = layer,
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

    bool GUI::button( const kege::ui::WidgetDesc& desc )
    {
        _layout->put( desc );
        return _layout->click( desc.user_id );
    }

    bool GUI::button( ui::UserId user_id, int16_t layer, const ui::Text& text, ui::Style* style )
    {
        _layout->put
        ({
            .layer = layer,
            .user_id = user_id,
            .text = text,
            .style = (style == nullptr)? &_theme.button : style,
            .single_click = ui::ClickTrigger::OnRelease,
        });
        return _layout->click( user_id );
    }

    ui::WidgetId GUI::fittedLabel( int16_t layer, const ui::Text& text, ui::Style* style )
    {
        return _layout->put
        ({
            .layer = layer,
            .text = text,
            .style = (style == nullptr)? &_theme.fitted_label : style,
            .mouseover = false
        });
    }

    ui::WidgetId GUI::label( int16_t layer, const ui::Text& text, ui::Style* style )
    {
        return _layout->put
        ({
            .layer = layer,
            .text = text,
            .style = (style == nullptr)? &_theme.label : style,
            .mouseover = false
        });
    }

    bool GUI::radio( ui::UserId user_id, int16_t layer, const ui::Text& text, bool& state )
    {
        push({.layer = layer, .style = &_theme.row });
        radio( user_id, layer, state );
        fittedLabel(layer, text);
        pop();
        return state;
    }

    bool GUI::radio( ui::UserId user_id, int16_t layer, bool& state )
    {
        _layout->put
        ({
            .style = (state)? &_theme.radio_active: &_theme.radio,
            .layer = layer,
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

    bool GUI::radio( ui::UserId user_id, int16_t layer )
    {
        RadioState& state = _radio_states[ user_id ];
        return radio(user_id, layer, state.state);
    }

    ui::WidgetId GUI::beginList(int16_t layer)
    {
        return _layout->push({ .layer = layer, .style = &_theme.list });
    }
    
    void GUI::endList()
    {
        _layout->pop();
    }

    ui::WidgetId GUI::beginColumn( int16_t layer, ui::Style* style )
    {
        return _layout->push
        ({
            .layer = layer,
            .style = (style == nullptr)? &_theme.column : style
        });
    }
    void GUI::endColumn()
    {
        _layout->pop();
    }

    ui::WidgetId GUI::beginRow( int16_t layer, ui::Style* style )
    {
        return _layout->push
        ({
            .layer = layer,
            .style = (style == nullptr)? &_theme.row : style
        });
    }
    void GUI::endRow()
    {
        _layout->pop();
    }


    struct ScrollOp
    {
        ui::WidgetId widget_id;
        float& amount;
    };
    void scrollingOperation(ui::Layout* layout, ui::UserId user_id, ui::WidgetId widget_id, void* data)
    {
        ScrollOp* params = reinterpret_cast<ScrollOp*>(data);

        ui::Widget* scrolly = layout->elem(params->widget_id);
        ui::Widget* clipper = layout->elem(widget_id);

        if (layout->testPointVsRect(layout->getPointerPosition(), clipper->rect))
        {
            float scroll_length = scrolly->rect.height - clipper->rect.height;
            if ( 0 < scroll_length)
            {
                params->amount += layout->getScrollOffset().y;
                params->amount = kege::min(0.f, kege::max(params->amount, -scroll_length));
                scrolly->rect.y = params->amount;
            }
            else
            {
                params->amount = 0.f;
            }
        }
    }
    
    void GUI::beginScrollContainer( ui::UserId user_id, int16_t layer )
    {
        ScrollContainer* state;
        auto i = _scroll_containers.find( user_id );
        if (i == _scroll_containers.end())
        {
            state = &_scroll_containers[user_id];
            state->scroll_amount = 0.f;
            state->frame_id = _frame_index;
        }
        else state = &i->second;

        ui::WidgetId widget_id = _layout->push /* the clipper is the container the scroller is clipped against */
        ({
            .layer = layer,
            .user_id = state->clipper,
            .style = &_theme.panel_transparnt,
            .clip_overflow = true
        });

        ui::WidgetId scroller = _layout->push /* the scroller is the container that is scrolled */
        ({
            .layer = layer,
            .user_id = state->scroller,
            .style = &_theme.column,
            .rect.y = state->scroll_amount,
            .mouseover = false
        });

        _layout->pushDeferredOp(state->clipper, widget_id, scrollingOperation, ScrollOp{
            .widget_id = scroller,
            .amount = state->scroll_amount
        });
    }
    
    void GUI::endScrollContainer()
    {
        _layout->pop(); /* pop the scroller container */
        _layout->pop(); /* pop the clipper container */
    }

    ui::WidgetId GUI::pushRoot( const ui::WidgetDesc& desc )
    {
        return _layout->pushRoot(desc);
    }

    ui::WidgetId GUI::putRoot( const ui::WidgetDesc& desc )
    {
        return _layout->putRoot(desc);
    }
    
    void GUI::popRoot()
    {
        _layout->popRoot();
    }

    ui::WidgetId GUI::push( const kege::ui::WidgetDesc& desc )
    {
        return _layout->push( desc );
    }

    ui::WidgetId GUI::put( const kege::ui::WidgetDesc& desc )
    {
        return _layout->put( desc );
    }

    void GUI::pop()
    {
        _layout->pop();
    }

    const bool GUI::pointerDragging() const
    {
        return _layout->isPointerDragging();
    }

    bool GUI::mouseover( uint64_t user_id )
    {
        return _layout->mouseover( user_id );
    }

    bool GUI::click( uint64_t user_id )
    {
        return _layout->click( user_id );
    }

    bool GUI::hot( uint64_t user_id )
    {
        return _layout->mouseover( user_id );
    }

    bool GUI::leftClickDown()const
    {
        return _layout->leftClickDown();
    }

    bool GUI::dragging()const
    {
        return _layout->isPointerDragging();
    }

    ui::Widget* GUI::get( ui::WidgetId index )
    {
        return _layout->elem( index );
    }

    bool GUI::initialize(Ref< ui::Layout > layout)
    {
        _layout = layout;
        return true;
    }

    void GUI::createLayers( uint32_t quantity )
    {
        _layout->createLayers( quantity );
    }

    vec2d GUI::deltaPointer()const
    {
        return _layout->getPointerDelta();
    }

    vec2d GUI::pointer()const
    {
        return _layout->getPointerPosition();
    }

    void GUI::begin( double dms )
    {
        ++_frame_index;

        // reset layout state
        _layout->begin( dms );
    }

    void GUI::end()
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

    GUI::~GUI()
    {}

    GUI::GUI()
    :   _frame_index(0)
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
            .background = ui::Background(0xFFFFFF00),
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
            .background = ui::Background( 0xFFFFFF00 ),
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
            .background = ui::Background(0x222222FF),
            .padding = {5,5,5,5},
            .height = ui::flexible(),
            .width = ui::fixed(250),
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
            .background = 0xFFFFFF00,
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
