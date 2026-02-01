//
//  gui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/8/25.
//

#include "gui.hpp"

namespace kege{

    int GUI::collapsableRemovableHeaderInput( int16_t layer, uint64_t id, char* str, size_t& size )
    {
        CollapsableRemovableHeader& header = _collapsable_removable_headers[ id ];

        _layout->push
        ({
            .layer = layer,
            .uid = &header.uid[0],
            .style = getStyle( Theme::CollapsableHeader ),
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        radio( layer, header.uid[1], header.state );
//        _layout->put/* collapse button */
//        ({
//            .layer = layer,
//            .uid = &header.uid[1],
//            .rect.height = 10,
//            .rect.width = 10,
//            .rect.y = 4,
//            .color = _layout->mouseover( header.uid[3] ) ? 0xEE2200FF : 0xEE220050,
//            .single_click = ui::ClickTrigger::OnRelease,
//            .double_click = ui::ClickTrigger::Immediate,
//            .border.corner_curves = {8,8,8,8}
//        });
        _layout->put
        ({
            .layer = layer,
            .uid = &header.uid[2],
            .style = ( header.mode == 2 ) ? getStyle( Theme::NumericFocus ) : getStyle( Theme::NumericValue ),
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .text.ptr = str,
        });
        _layout->put /* remove button */
        ({
            .layer = layer,
            .uid = &header.uid[3],
            .rect.height = 10,
            .rect.width = 10,
            .rect.y = 4,
            .color = _layout->mouseover( header.uid[3] ) ? 0xEE2200FF : 0xEE220050,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = {8,8,8,8}
        });
        _layout->pop();

        if ( _layout->doubleClick( header.uid[2] ) )
        {
            header.mode = 2;
        }
        if ( header.mode == 2 )
        {
            if ( !_layout->onTextInput( header.uid[2], str, size ) )
            {
                header.mode = 0;
            }
        }

        if ( _layout->click(header.uid[1]) || _layout->click( header.uid[2] ) )
        {
            header.state = !header.state;
        }
        if ( _layout->click(header.uid[3]) )
        {
            return -1;
        }
        return header.state;
    }

    int GUI::collapsableRemovableHeader( int16_t layer, uint64_t id, const ui::Text& text )
    {
        CollapsableRemovableHeader& header = _collapsable_removable_headers[ id ];

        _layout->push
        ({
            .layer = layer,
            .uid = &header.uid[0],
            .style = &_theme.collapsable_removable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        radio( layer, header.uid[1], header.state );
        label(layer, text);
        bool state = circleButton( layer, header.uid[3] );
        _layout->pop();

        if ( _layout->click( header.uid[0] ) )
        {
            return 2;
        }
        if ( state )
        {
            return -1;
        }
        return header.state;
    }


    bool GUI::collapsableHeader( int16_t layer, uint64_t id, const ui::Text& text )
    {
        CollapsableHeader& header = _collapsable_headers[ id ];

        _layout->push
        ({
            .layer = layer,
            .uid = &header.uid[0],
            .style = getStyle( Theme::CollapsableHeader ),
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        _layout->put
        ({
            .layer = layer,
            .uid = &header.uid[1],
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
            .uid = &header.uid[2],
            .style = &_theme.label,
            .text = text,
            .mouseover = false,
        });
        _layout->pop();

        if ( _layout->click(header.uid[0]) )
        {
            header.state = !header.state;
        }
        return header.state;
    }

    int GUI::removableHeader( int16_t layer, uint64_t id, const ui::Text& text )
    {
        RemovableHeader& header = _removable_headers[ id ];

        _layout->push
        ({
            .layer = layer,
            .uid = &header.uid[0],
            .style = getStyle( Theme::CollapsableHeader ),
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _layout->put
        ({
            .layer = layer,
            .uid = &header.uid[1],
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
            .uid = &header.uid[2],
            .style = &_theme.label,
            .text = text,
            .mouseover = false,
        });
        _layout->put
        ({
            .layer = layer,
            .uid = &header.uid[3],
            .rect.height = 10,
            .rect.width = 10,
            .rect.y = 4,
            .color = _layout->mouseover( header.uid[3] ) ? 0xEE2200FF : 0xEE220050,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = {8,8,8,8}
        });
        _layout->pop();

        if ( _layout->click(header.uid[0]) )
        {
            header.state = !header.state;
        }
        if ( _layout->click(header.uid[3]) )
        {
            return 2;
        }
        return header.state;
    }


    struct TextField
    {
        GUI::TextFieldState* state;
        char* buffer;
        size_t& buffer_capacity;
        size_t& current_size;
    };

    void textFieldOp(ui::Layout* layout, const ui::UID* uid, void* data)
    {
        TextField* text = static_cast<TextField*>(data);
        GUI::TextFieldState* state = text->state;

        state->edited = false;

        if (layout->click( state->field ))
        {
            // Handle single click
            if (state->mode != TextFieldMode::Editing)
            {
                state->mode = TextFieldMode::Focused;
                layout->cursor()->startEditing
                (
                    ui::Cursor::InputType::Any,
                    state->field,
                    text->buffer,
                    text->current_size,
                    text->buffer_capacity
                );
            }
        }
        else if (layout->doubleClick( state->field ))
        {
            layout->cursor()->startEditing
            (
                ui::Cursor::InputType::Any,
                state->field,
                text->buffer,
                text->current_size,
                text->buffer_capacity
            );
            
            layout->cursor()->computeCursorPosition();

            // Select all text
            if (state->mode == TextFieldMode::Editing)
            {
                layout->cursor()->selectAll(text->buffer, text->current_size);
            }
            state->mode = TextFieldMode::Editing;
        }
        else if (!layout->hasFocus( state->field ) && state->mode != TextFieldMode::Idle)
        {
            // Lost focus
            if (state->mode == TextFieldMode::Editing)
            {
                state->edited = true; // Text was being edited
            }
            layout->cursor()->stopEditing();
            state->mode = TextFieldMode::Idle;
        }

        // Handle text input if editing
        if (state->mode == TextFieldMode::Editing)
        {
            if (!layout->cursor()->update())
            {
                state->mode = TextFieldMode::Idle;
                state->edited = true;
            }
        }
    }

    bool GUI::textField( int16_t layer, uint64_t id, char* buffer, size_t buffer_capacity, size_t& current_size )
    {
        TextFieldState& state = _input_texts[ id ];

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

        _layout->put
        ({
            .layer = layer,
            .uid = &state.field,
            .style = style,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .text.ptr = buffer,
        });

        _layout->pushDeferredOp(&state.field, textFieldOp, TextField
        {
            .state = &state,
            .buffer = buffer,
            .buffer_capacity = buffer_capacity,
            .current_size = current_size
        });

        return state.edited;
    }

//    bool GUI::scrubber( int16_t layer, Scrubber* state, float& num )
//    {
//        bool active = false;
//        if ( _layout->buttonDown() )
//        {
//            if ( _layout->click( state->uid ) )
//            {
//                if ( _layout->input()->pointerDragging() )
//                {
//                    if( 0.0 != _layout->input()->deltaPosition().x )
//                        num += _layout->input()->deltaPosition().x * state->sensitivity;
//
//                    if( 0.0 != _layout->input()->deltaPosition().y )
//                        num -= _layout->input()->deltaPosition().y * state->sensitivity;
//                    active = true;
//
//                    snprintf(state->str, 16, "%.3f", num);
//                    state->text_value = _layout->text(state->str, 20);
//                }
//            }
//            else if ( _layout->doubleClick( state->uid ) )
//            {
//                state->mode = 2;
//            }
//            else
//            {
//                state->mode = 0;
//            }
//        }
//
//        if ( state->mode == 2 )
//        {
//            if ( !_layout->onTextInput( state->uid, state->str, state->size ) )
//            {
//                active = true;
//                state->mode = 0;
//            }
//            num = atof( state->str );
//        }
//        _layout->put
//        ({
//            .uid = &state->uid,
//            .style = ( state->mode == 2 ) ? &_theme.scrubber_focus : &_theme.scrubber,
//            .single_click = ui::ClickTrigger::Continuous,
//            .double_click = ui::ClickTrigger::Immediate,
//            .text = state->text_value,
//        });
//        return active;
//    }

    bool GUI::scrubber( int16_t layer, uint64_t id, double& num )
    {
        Scrubber* state;
        auto i = _scrubbers.find( id );
        if (i == _scrubbers.end())
        {
            state = &_scrubbers[id];
            snprintf(state->str, 16, "%.3f", num);
            state->text_value = _layout->text(state->str, 20);
            state->text_value.color = 0xFFFFFFFF;
            state->sensitivity = 0.002;
            state->type = Scrubber::DOUBLE;
        }
        else state = &i->second;
        return scrubber<double>( layer, state, num );
    }

    bool GUI::scrubber( int16_t layer, uint64_t id, float& num )
    {
        Scrubber* state;
        auto i = _scrubbers.find( id );
        if (i == _scrubbers.end())
        {
            state = &_scrubbers[id];
            snprintf(state->str, 16, "%.3f", num);
            state->text_value = _layout->text(state->str, 20);
            state->text_value.color = 0xFFFFFFFF;
            state->sensitivity = 0.002;
            state->type = Scrubber::FLOAT;
        }
        else state = &i->second;
        return scrubber<float>( layer, state, num );
    }

    bool GUI::scrubber( int16_t layer, uint64_t id, const ui::Text& name, double& num )
    {
        _layout->push({ .layer = layer, .style = &_theme.scrubber_row });
        fittedLabel(layer, name);
        scrubber( layer, id, num );
        _layout->pop();
        return false;
    }

    bool GUI::scrubber( int16_t layer, uint64_t id, const ui::Text& name, float& num )
    {
        _layout->push({ .layer = layer, .style = &_theme.scrubber_row });
        fittedLabel(layer, name);
        scrubber( layer, id, num );
        _layout->pop();
        return false;
    }

    bool GUI::scrubber4( int16_t layer, uint64_t id, const ui::Text& name, float& x, float& y, float& z, float& w )
    {
        bool modified[3] = {};

        beginColumn(layer);
        {
            label(layer, name);

            beginColumn(layer);
            beginRow(layer);
            ui::Text x_label{.ptr = "x:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[0] = scrubber(layer, getAddressAsInt(x), x_label, x);

            ui::Text y_label{.ptr = "y:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[1] = scrubber(layer, getAddressAsInt(y), y_label, y);

            ui::Text z_label{.ptr = "z:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[2] = scrubber(layer, getAddressAsInt(z), z_label, z);
            endRow();

            ui::Text w_label{.ptr = "w:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[2] = scrubber(layer, getAddressAsInt(w), w_label, w);
            endColumn();
        }
        endColumn();

        return modified[0] || modified[1] || modified[2];
    }

    bool GUI::scrubber3( int16_t layer, uint64_t id, const ui::Text& name, float& x, float& y, float& z )
    {
        bool modified[3] = {};

        beginColumn(layer);
        {
            label(layer, name);
            beginRow(layer);
            ui::Text x_label{.ptr = "x:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[0] = scrubber(layer, getAddressAsInt(x), x_label, x);

            ui::Text y_label{.ptr = "y:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[1] = scrubber(layer, getAddressAsInt(y), y_label, y);

            ui::Text z_label{.ptr = "z:", .width = 10, .font_size = 20, .height = 15, .color = 0xFFFFFFFF};
            modified[2] = scrubber(layer, getAddressAsInt(z), z_label, z);
            endRow();
        }
        endColumn();

        return modified[0] || modified[1] || modified[2];
    }


    bool GUI::numeric( int16_t layer, uint64_t id, double& num )
    {
        InputNumeric* state;
        auto i = _numeric_states.find( id );
        if (i == _numeric_states.end())
        {
            state = &_numeric_states[id];
            state->mode = 0;
            state->last_frame_value = 0;
            snprintf(state->str, state->size, "%.6g", num);
        }
        else state = &i->second;

        _layout->push(kege::ui::WidgetDesc{
            .uid = &state->uid[0],
            .layer = layer,
            .style = getStyle( Theme::Numeric ),
        });
        {
            _layout->put
            ({
                .layer = layer,
                .uid = &state->uid[2],
                .style = ( state->mode == 2 )? getStyle( Theme::NumericFocus ) : getStyle( Theme::NumericValue ),
                .single_click = ui::ClickTrigger::OnRelease,
                .double_click = ui::ClickTrigger::Immediate,
                .text.ptr = state->str
            });
        }
        _layout->pop();

        bool clicked = false;

        if ( _layout->click( state->uid[2] ) )
        {
            clicked = true;
            state->mode += 1;
        }
        else if ( _layout->doubleClick( state->uid[2] ) )
        {
            state->mode = 2;
        }
        else if ( !_layout->hasFocus( state->uid[2] ) && state->mode > 0 )
        {
            state->mode = 0;
        }

        if ( state->mode == 2 )
        {
            if ( !_layout->onTextInput( state->uid[2], state->str, state->size ) )
            {
                state->mode = 0;
            }
            num = atof( state->str );
        }

        return clicked;
    }


    struct OptionParam{ GUI::OptionState* state; };
    void repositionOptionListOp(ui::Layout* layout, const ui::UID* uid, void* data)
    {
        OptionParam* state = reinterpret_cast< OptionParam* >(data);
        ui::Widget* main = layout->elem( *uid );
        ui::Widget* list = layout->elem( state->state->uid[2] );
        if ( list )
        {
            list->rect.x = main->rect.x;
            list->rect.y = main->rect.y + main->rect.height;
            list->rect.width = main->rect.width;
            list->rect.height = 0.f;
        }

        int pid = list->index;
        for (uint32_t eid = layout->head( pid ); eid != 0 ; eid = layout->next( eid ) )
        {
            layout->operator[](eid)->rect.x = 0.f;
            layout->operator[](eid)->rect.y = 0.f;
        }
        ui::Resizer::resize(*layout, list->index);
        ui::Aligner::align(*layout, list->index);

        if (layout->buttonDown())
        {
            const kege::dvec2& position = layout->getPointerPosition();
            if (!layout->testPointVsRect(position, main->rect) && !layout->testPointVsRect(position, list->rect))
            {
                state->state->state = false;
            }
        }
    }

    bool GUI::options( int16_t layer, uint64_t id, std::vector<ListElem>& list, int& selection )
    {
        OptionState& state = _option_states[ id ];
        bool has_selection = false;

        _layout->push
        ({
            .layer = layer,
            .uid = &state.uid[0],
            .style = &_theme.option,
            .single_click = ui::ClickTrigger::Immediate,
            .double_click = ui::ClickTrigger::Immediate,
            .gap = {10,0},
        });
        _layout->put
        ({
            .layer = layer,
            .uid = &state.uid[1],
            .style = &_theme.option_selection,
            .text = list[ state.selection ].text,
            .mouseover = false,
        });

        if ( _layout->click(state.uid[0]) )
        {
            state.state = !state.state;
        }

        if ( state.state )
        {
            _layout->pushRoot({.layer = int16_t(layer + 1), .uid = &state.uid[2], .style = &_theme.option_list});
            for (int i=0; i<list.size(); ++i)
            {
                _layout->put
                ({
                    .layer = layer,
                    .uid = &list[i].uid,
                    .text = list[i].text,
                    .single_click = ui::ClickTrigger::OnRelease,
                    .style = (_layout->mouseover( list[i].uid )) ? getStyle( Theme::ListSelection ) : getStyle( Theme::ListElem )
                });

                if( _layout->click( list[i].uid ) && !has_selection )
                {
                    state.selection = i;
                    has_selection = true;
                    state.state = false;
                }
            }
            _layout->popRoot();
            _layout->pushDeferredOp<OptionParam>(&state.uid[0], repositionOptionListOp, {&state});
        }
        _layout->pop();
        selection = state.selection;
        return has_selection;
    }

    bool  GUI::select( int16_t layer, ui::Style* style, std::vector<ListElem>& list, int& selection )
    {
        bool has_selection = false;
        _layout->push
        ({
            .layer = layer,
            .style = style
        });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put
            ({
                .layer = layer,
                .uid = &list[i].uid,
                .text = list[i].text,
                .single_click = ui::ClickTrigger::OnRelease,
                .style = (_layout->mouseover( list[i].uid )) ? getStyle( Theme::ListSelection ) : getStyle( Theme::ListElem )
            });

            if( _layout->click( list[i].uid ) && !has_selection )
            {
                selection = i;
                has_selection = true;
            }
        }
        _layout->pop();
        return has_selection;
    }
    
    bool GUI::select( int16_t layer, std::vector<ListElem>& list, int& selection )
    {
        return select( layer, getStyle( Theme::List ), list, selection);
    }

    void GUI::list( int16_t layer, std::vector<ListElem>& list, int selection )
    {
        _layout->push({ .layer = layer, .style = getStyle( Theme::List ) });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put
            ({
                .layer = layer,
                .uid = &list[i].uid,
                .text = list[i].text,
                .style = getStyle( Theme::ListElem )
            });
        }
        _layout->pop();
    }

    bool GUI::tab( int16_t layer, std::vector<ListElem>& list, int& selection )
    {
        bool active = false;
        _layout->push({ .layer = layer, .style = getStyle( Theme::Tab ) });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put({
                .layer = layer,
                .uid = &list[i].uid,
                .text = list[i].text,
                .single_click = ui::ClickTrigger::OnRelease,
                .style = (selection == i)
                    ? getStyle( Theme::TabSelection )
                    : getStyle( Theme::TabElem )
            });
            if( _layout->click( list[i].uid ) )
            {
                selection = i;
                active = true;
            }
        }
        _layout->pop();
        return active;
    }

    void slidebarOp(ui::Layout* layout, const ui::UID* id, void* data)
    {
        //layout->elemParent(id)->uid
        RangeParams* params = reinterpret_cast<RangeParams*>(data);
        float length = layout->elem(*id)->rect.width;
        float& w = layout->elem(*params->id)->rect.width;

        // calculate where the slider should be (base on the numeric value) before moving it.
        w = (length * (*params->val - params->min)) / (params->max - params->min);

        if( layout->click( *id ) )
        {
            // update slide bar width
            w += layout->getPointerDelta().x;
            // clamp slide bar width
            w = (w < 0)? 0.f : (w > length) ? length: w;
            // calculate numeric value base on slide bar width
            *params->val = (params->max - params->min) * (w / length) + params->min;
        }
        else if( layout->doubleClick( *id ) )
        {
            // update slide bar width
            w = layout->getClickPosition(kege::MouseButtonCode::Left).x - layout->elem(*id)->rect.x;
            // clamp slide bar width
            w = (w < 0)? 0.f : (w > length) ? length: w;
            // calculate numeric value base on slide bar width
            *params->val = (params->max - params->min) * (w / length) + params->min;
        }
    }

    bool GUI::numSlideBar( int16_t layer, ui::UID (&id)[2], double* val, double min, double max )
    {
        char snum[16];
        snprintf(snum, 16, "%.6g", *val );

        _layout->push
        ({
            .layer = layer,
            .uid = &id[1],
            .style = getStyle( Theme::SlideBarTrack ),
            .single_click = ui::ClickTrigger::Continuous,
            .double_click = ui::ClickTrigger::Immediate
        });
        _layout->put({ .layer = layer, .uid = &id[0], .style = getStyle( Theme::SlideBar ) });
        _layout->put({ .layer = layer, .style = getStyle( Theme::SlideBarNumber ), .text = snum });
        _layout->pop();

        _layout->pushDeferredOp(&id[1], slidebarOp, RangeParams{min,max,val,&id[0]});

        return _layout->click( id[1] );
    }

    bool GUI::slidebar( int16_t layer, ui::UID (&id)[2], double* val, double min, double max )
    {
        _layout->push({ .layer = layer, .uid = &id[1], .style = getStyle( Theme::SliderTrack ), .single_click = ui::ClickTrigger::Continuous });
        _layout->put({ .layer = layer, .uid = &id[0], .style = getStyle( Theme::SlideBar ) });
        _layout->pop();

        _layout->pushDeferredOp(&id[1], slidebarOp, RangeParams{min,max,val,&id[0]});

        return _layout->click( id[1] );
    }

    void sliderOp(ui::Layout* layout, const ui::UID* id, void* data)
    {
        RangeParams* params = reinterpret_cast<RangeParams*>(data);
        const ui::Widget* track = layout->elemParent(*id);
        ui::Widget* knob = layout->elem(*id);

        float length = track->rect.width - knob->rect.width;

        // calculate where the slider should be (base on the numeric value) before moving it.
        float pos = (length * (*params->val - params->min)) / (params->max - params->min);

        if( layout->click( *id ) )
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

    bool GUI::slider( int16_t layer, ui::UID (&id)[2], double* val, double min, double max )
    {
        _layout->push({ .layer = layer, .uid = &id[1], .style = getStyle( Theme::SliderTrack ) });
        _layout->put
        ({
            .layer = layer, .uid = &id[0],
            .style = getStyle( Theme::Slider ),
            .single_click = ui::ClickTrigger::Continuous
        });
        _layout->pop();

        _layout->pushDeferredOp(&id[0], sliderOp, RangeParams{min,max,val});
        return _layout->click( id[0] );
    }

    bool GUI::slider( int16_t layer, uint64_t id, double* val, double min, double max )
    {
        SliderState* state = &_slider_states[ id ];
        return slider( layer, state->uid, val, min, max );
    }

    bool GUI::sliderUI( int16_t layer, const ui::Text& text, uint64_t id, double* val, double min, double max )
    {
        SliderUIState* state;

        auto i = _slider_ui_states.find( id );
        if (i == _slider_ui_states.end())
        {
            state = &_slider_ui_states[id];
            snprintf(state->str[0], 15, "%.3g", min);
            snprintf(state->str[1], 15, "%.3g", max);
            snprintf(state->str[2], 15, "%.3f", *val);
            state->text[0] = _layout->text(state->str[0], 20);
            state->text[1] = _layout->text(state->str[1], 20);
            state->text[2] = _layout->text(state->str[2], 20);
        }
        else state = &i->second;

        bool active;
        ui::Widget* w = beginColumn(layer, &_theme.slider_inner_container);
        {
            w->padding = {10,10,10,10};
            w->border.corner_curves = {10,10,10,10};
            beginRow(layer, &_theme.slider_row);
            fittedLabel(layer, text, &_theme.slider_name);
            _layout->put({.layer = layer, .style = &_theme.x_seperator});
            fittedLabel(layer, state->text[2], &_theme.slider_value);
            endRow();

            beginRow(layer, &_theme.slider_container);
            {
                fittedLabel(layer, state->text[0], &_theme.slider_range);
                active = slider( layer, state->uid, val, min, max );
                if (active)
                {
                    snprintf(state->str[2], 15, "%.3f", *val);
                    state->text[2] = _layout->text(state->str[2], 20);
                }
                fittedLabel(layer, state->text[1], &_theme.slider_range);
            }
            endRow();
        }
        endColumn();

        return active;
    }

    bool GUI::circleButton( int16_t layer, ui::UID& uid )
    {
        _layout->put
        ({
            .layer = layer,
            .uid = &uid,
            .rect.height = 10,
            .rect.width = 10,
            .rect.y = 4,
            .color = _layout->mouseover( uid ) ? 0xEE2200FF : 0xEE220050,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = {8,8,8,8}
        });
        return _layout->click( uid );
    }

    bool GUI::button( const kege::ui::WidgetDesc& desc )
    {
        _layout->put( desc );
        return _layout->click( *desc.uid );
    }

    bool GUI::button( int16_t layer, ui::UID& id, const ui::Text& text, ui::Style* style )
    {
        _layout->put
        ({
            .layer = layer,
            .uid = &id,
            .text = text,
            .style = (style == nullptr)? &_theme.button : style,
            .single_click = ui::ClickTrigger::OnRelease,
        });
        return _layout->click( id );
    }

    ui::Widget* GUI::fittedLabel( int16_t layer, const ui::Text& text, ui::Style* style )
    {
        return _layout->put
        ({
            .layer = layer,
            .text = text,
            .style = (style == nullptr)? &_theme.fitted_label : style,
            .mouseover = false
        });
    }

    ui::Widget* GUI::label( int16_t layer, const ui::Text& text, ui::Style* style )
    {
        return _layout->put
        ({
            .layer = layer,
            .text = text,
            .style = (style == nullptr)? &_theme.label : style,
            .mouseover = false
        });
    }

    bool GUI::radio( int16_t layer, ui::UID& uid, const ui::Text& text, bool& state )
    {
        push({.layer = layer, .style = &_theme.row });
        radio( layer, uid, state );
        fittedLabel(layer, text);
        pop();
        return state;
    }

    bool GUI::radio( int16_t layer, ui::UID& uid, bool& state )
    {
        _layout->put
        ({
            .style = (state)? &_theme.radio_active: &_theme.radio,
            .layer = layer,
            .uid = &uid,
            .rect.y = 4,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .color = (state == 0) ? 0xFFBB0040 : 0xFFBB00FF,
            .border.corner_curves = {5,5,5,5}
        });
        if ( _layout->click(uid) )
        {
            state = !state;
        }
        return state;
    }

    bool GUI::radio( int16_t layer, uint64_t id )
    {
        RadioState& state = _radio_states[ id ];
        return radio(layer, state.uid, state.state);
    }

    ui::Widget* GUI::beginList(int16_t layer)
    {
        return _layout->push({ .layer = layer, .style = getStyle( Theme::List ) });
    }
    
    void GUI::endList()
    {
        _layout->pop();
    }

    ui::Widget* GUI::beginColumn( int16_t layer, ui::Style* style )
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

    ui::Widget* GUI::beginRow( int16_t layer, ui::Style* style )
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
        ui::UID* uid;
        float& amount;
    };
    void scrollingOperation(ui::Layout* layout, const ui::UID* id, void* data)
    {
        ScrollOp* params = reinterpret_cast<ScrollOp*>(data);

        ui::Widget* scrolly = layout->elem(*params->uid);
        ui::Widget* clipper = layout->elem(*id);

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
    
    void GUI::beginScrollContainer( int16_t layer, uint64_t id )
    {
        ScrollContainer* state;
        auto i = _scroll_containers.find( id );
        if (i == _scroll_containers.end())
        {
            state = &_scroll_containers[id];
            state->scroll_amount = 0.f;
            state->frame_id = _frame_index;
        }
        else state = &i->second;

        _layout->pushDeferredOp(&state->clipper, scrollingOperation, ScrollOp{
            .uid = &state->scroller,
            .amount = state->scroll_amount
        });

        _layout->push /* the clipper is the container the scroller is clipped against */
        ({
            .layer = layer,
            .uid = &state->clipper,
            .style = &_theme.panel_transparnt,
            .clip_overflow = true
        });

        _layout->push /* the scroller is the container that is scrolled */
        ({
            .layer = layer,
            .uid = &state->scroller,
            .style = &_theme.column,
            .rect.y = state->scroll_amount,
            .mouseover = false
        });
    }
    
    void GUI::endScrollContainer()
    {
        _layout->pop(); /* pop the scroller container */
        _layout->pop(); /* pop the clipper container */
    }

    kege::ui::Widget* GUI::pushRoot( const ui::WidgetDesc& desc )
    {
        return _layout->pushRoot(desc);
    }
    kege::ui::Widget* GUI::putRoot( const ui::WidgetDesc& desc )
    {
        return _layout->putRoot(desc);
    }
    void GUI::popRoot()
    {
        _layout->popRoot();
    }

    void GUI::pushHPanel( int16_t layer )
    {
        _layout->push
        ({
            .layer = layer,
            .style = getStyle( Theme::Row )
        });
    }

    void GUI::pushVPanel( int16_t layer )
    {
        _layout->push({ .layer = layer, .style = getStyle( Theme::VPanel ) });
    }

    void GUI::push( const kege::ui::WidgetDesc& desc )
    {
        _layout->push( desc );
    }

    void GUI::put( const kege::ui::WidgetDesc& desc )
    {
        _layout->put( desc );
    }

    void GUI::pop()
    {
        _layout->pop();
    }

    const bool GUI::pointerDragging() const
    {
        return _layout->isPointerDragging();
    }

    bool GUI::mouseover( ui::UID& id )
    {
        return _layout->mouseover( id );
    }

    bool GUI::click( ui::UID& id )
    {
        return _layout->click( id );
    }

    bool GUI::hot( ui::UID& id )
    {
        return _layout->mouseover( id );
    }

    bool GUI::buttonDown()const
    {
        return _layout->buttonDown();
    }

    bool GUI::dragging()const
    {
        return _layout->isPointerDragging();
    }

    ui::Widget* GUI::get( ui::UID& id )
    {
        return _layout->elem( id );
    }

    kege::ui::Style* GUI::getStyle( int index )
    {
        return &_theme.styles[index];
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
            .height = ui::extend(),
            .width = ui::extend(),
            .padding = {10,10,10,10},
            .gap = {5,5},
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
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
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::VERTICAL,
                .wrap_around = false,
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
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::VERTICAL,
                .wrap_around = false,
            },
            .gap = {4, 4},
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
            .background = 0xFFFFFF10,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL,
                .wrap_around = false,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::HORIZONTAL,
                .wrap_around = true,
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
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::VERTICAL,
                .wrap_around = false,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
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
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::VERTICAL,
                .wrap_around = false,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL,
                .wrap_around = false,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL,
                .wrap_around = false,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL,
                .wrap_around = false,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL
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
            .padding = {0,0,0,0},
            .alignment =
            {
                .direction = ui::AlignDir::VERTICAL
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
                .direction = ui::AlignDir::HORIZONTAL
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL,
                .wrap_around = false,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::HORIZONTAL,
                .wrap_around = true,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::VERTICAL,
                .wrap_around = false,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::VERTICAL,
                .wrap_around = false,
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
        };;





        text_field_editing = kege::ui::Style
        {
            .background = 0x456E66FA,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::HORIZONTAL,
                .wrap_around = false,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::HORIZONTAL,
                .wrap_around = false,
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .direction = ui::AlignDir::HORIZONTAL,
                .wrap_around = false,
            },
            .gap = {4, 4},
        };
        hierarchy_entity_selected = hierarchy_entity;
        hierarchy_entity_selected.background = ui::Background(0xFFFFFF08);



        styles[CloseButn] = kege::ui::Style{
            .height = ui::fixed(18),
            .width = ui::fixed(18),
            .background = 0xFFFFFF20,
            .align_text =  ui::AlignText::Center,
            .font_size = 20,
            .border.corner_curves = {2,2,2,2},
        };

        styles[Theme::Slider] = kege::ui::Style{
            .height = ui::fixed(15),
            .width = ui::fixed(15),
            .background = 0xFF22FFFF,
            .align_text =  ui::AlignText::Center,
        };
        styles[SliderTrack] = kege::ui::Style{
            .height = ui::fixed(15),
            .width = ui::extend(),
            .background = 0xFFFFFF0B,
            .align_text = ui::AlignText::Center,
        };


        styles[SlideBar] = kege::ui::Style{
            .background = 0xFF22FFFF,
            .position = ui::Positioning::Absolute,
            .height = ui::fixed(14),
            .width = ui::fixed(14),
        };
        styles[SlideBarTrack] = kege::ui::Style{
            .background = 0xFFFFFF20,
            .height = ui::fixed(14),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[SlideBarNumber] = kege::ui::Style{
            .position = ui::Positioning::Absolute,
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Center,
            .height = ui::fixed(14),
            .width = ui::percent(100),
            .font_size = 20,
        };


        styles[CollapsableHeader] = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .height = ui::fixed(20),
            .width = ui::extend(),
            .padding = {6,0,6,0},
            .gap = {4,0}
        };

        styles[List] = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {2,2},
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL,
                .wrap_around = false
            }
        };
        styles[ListElem] = kege::ui::Style{
            .border.corner_curves = {4,4,4,4},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF10,
            .font_size = 20,
            .text_color = 0xFFFFFFFF,
        };
        styles[ListSelection] = kege::ui::Style{
            .border.corner_curves = {4,4,4,4},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF30,
            .font_size = 20,
            .text_color = 0xFFFFFFFF,
        };


        styles[Tab] = kege::ui::Style{
            .background = ui::Background(0x10101000),
            .height = ui::fixed(20),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[TabElem] = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::fixed(120),
            .background = 0x171420FF,
            .align_text = ui::AlignText::Center,
            .text_color = 0xBBA0FFFF,
            .font_size = 20,
        };
        styles[TabSelection] = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::fixed(120),
            .background = ui::Background(0x171420FF),
            .align_text = ui::AlignText::Center,
            .border.corner_curves = {4,4,4,4},
            .font_size = 20,
        };


        styles[FlexH] = kege::ui::Style{
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0x333333FF),
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };
        styles[FlexW] = kege::ui::Style{
            .height = ui::extend(),
            .width = ui::flexible(),
            .background = ui::Background(0x333333FF),
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };

        styles[VPanel] = kege::ui::Style{
            .height = ui::extend(),
            .width = ui::extend(),
            .background = ui::Background(0x334433FF),
            //.padding = {10,10,10,10},
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL,
                .wrap_around = true,
            }
        };


        styles[Hierarchy] = kege::ui::Style{
            .background = ui::Background(0x433333FF),
            .height = ui::extend(),
            .width = ui::fixed(400),
            //.padding = {10,10,10,10},
            .alignment =
            {
                .content = {ui::AlignPosX::CENTER, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };
        styles[Viewport] = kege::ui::Style{
            .height = ui::extend(),
            .width = ui::extend(),
            .background = ui::Background(0x222222FF),
            //.padding = {10,10,10,10},
            .alignment =
            {
                .content = {ui::AlignPosX::RIGHT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };


        styles[Numeric] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF0B),
            .height = ui::fixed(18),
            .width = ui::extend(),
            .padding = {8,2,8,2},
            .gap = {2,2},
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[NumericLabel] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF00),
            .align_text = ui::AlignText::Right,
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::fixed(100),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[NumericValue] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF00),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[NumericFocus] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL
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
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL,
                .wrap_around = true,
            }
        };
    }
}
