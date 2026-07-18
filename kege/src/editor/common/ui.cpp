//
//  ui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/8/25.
//

#include "ui.hpp"
#include "ui-builder.hpp"

namespace kege{

    bool UI::input(const ui::ID& user_id, ui::Cursor::InputType type, TextFieldMode& mode, ui::Text& text )
    {
        if (mode == TextFieldMode::Editing)
        {
            text.ptr = getCharBufr();
        }

        ui::WidgetId widget_id = _gui->put
        ({
            .user_id = user_id,
            .rect = {0, 0, 0, text.height},
            .text = text,
            .color = (mode == TextFieldMode::Editing) ? 0x66FFFF35 : (mode == TextFieldMode::Focused) ? 0xFFFFFF18 : 0xFFFFFF05,
            .width = ui::extend(),
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });

        _gui->pushDeferredOp(user_id, widget_id, editTextOp, TextField
        {
            .type = type,
            .mode = mode,
            .str_capacity = CHAR_BUFR_CAPACITY,
            .str_len = _str_len,
            .str = text.ptr,
            .editing = _editing,
            .edited = _edited,
        });

        return _edited;
    }

    bool UI::labelInput(const char* label, const ui::ID& user_id, TextFieldMode& mode, ui::Text& text)
    {
        ui::WidgetDesc desc = {};
        desc.alignment = ui::Alignment
        {
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = ui::AlignDir::RIGHT,
            .items = ui::AlignItem::CENTER,
        };
        desc.color = 0xFFFFFF08;
        desc.gap = {2,2};
        desc.width = ui::extend();
        desc.height = ui::fixed(30);
        _gui->push(desc);

        desc = {};
        desc.text = ui::Text{.ptr = label, .width = 60, .height = 20, .font_size = 24, .color = 0xFFFFFF30};
        desc.single_click = ui::ClickTrigger::OnRelease;
        desc.rect = {0,0, 60, 30};
        desc.color = 0xFFFFFF00;
        desc.padding = {10,5,5,10};
        _gui->put (desc);

        bool modified = input(user_id, ui::Cursor::InputType::Any, mode, text);
        _gui->pop();
        return modified;
    }

    void UI::beginWindow(const ui::ID uid[3], ui::Rect& rect, const char* title, bool& close_window)
    {
        _gui->pushLayer(ui::LAYER_WINDOW);
        _gui->pushRoot
        ({
            .user_id = uid[0],
            .rect = rect,
            .padding = {10,10,10,10},
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .color = 0x18141DFF,
            .position = ui::Positioning::Independent,
            .clip_overflow = true
        });

        _gui->push
        ({
            .user_id = uid[1],
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .items = ui::AlignItem::CENTER,
            },
            .color = 0xFFFFF00,
            .width = ui::extend(),
            .height = ui::fixed(30),
            .single_click = ui::ClickTrigger::Continuous,
            .double_click = ui::ClickTrigger::Immediate,
        });
        {
            float w = rect.width - 50;
            float h = 30;

            kege::ui::Text text;
            text.ptr = title;
            text.width = w;
            text.height = h;
            text.color = 0xFFFFFF30;
            text.font_size = 30;
            _gui->put
            ({
                .rect = {0,0, w,h},
                .text = text,
                .padding = {10,5,5,10},
                .color = 0xFFFFFF00,
                .single_click = ui::ClickTrigger::OnRelease,
            });
            _gui->put({.style = &_theme->x_seperator});
            charButn(uid[2], "x", 7, 0);
        }
        _gui->pop();
        _gui->put({.rect = {0,0,10,10}});

        if( _gui->click(uid[2]) )
        {
            close_window = false;
        }

        if( _gui->click(uid[1]) )
        {
            rect.x += _gui->getInputManager()->getMouse()->getDelta().x;
            rect.y += _gui->getInputManager()->getMouse()->getDelta().y;
        }
    }
    
    void UI::endWindow()
    {
        _gui->popRoot();
        _gui->popLayer();
    }


    void UI::editTextOp(kege::GUI* gui, ui::ID user_id, ui::WidgetId widget_id, void* data)
    {
        TextField* text = static_cast<TextField*>(data);
        //TextFieldState* state = text->state;
        text->edited = false;

        if ( gui->click( user_id ) && text->mode != TextFieldMode::Editing )
        {
            text->mode = TextFieldMode::Focused;
        }
        else if ( gui->doubleClick( user_id ) )
        {
            if (text->mode == TextFieldMode::Editing)
            {
                gui->cursor()->selectAll( widget_id, getCharBufr(), text->str_len );
            }
            else
            {
                text->mode = TextFieldMode::Editing;
            }
        }
        else if (!gui->hasFocus( user_id ) && text->mode != TextFieldMode::Idle)
        {
            // Lost focus
            if (text->mode == TextFieldMode::Editing)
            {
                text->edited = true; // Text was being edited
            }
            gui->cursor()->stopEditing();
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

                gui->cursor()->startEditing
                (
                    user_id, widget_id, text->type,
                    getCharBufr(), text->str_len, CHAR_BUFR_CAPACITY
                );
                gui->cursor()->computeCursorPosition( widget_id );
            }

            if (text->editing)
            {
                if (!gui->cursor()->update( widget_id ))
                {
                    text->mode = TextFieldMode::Idle;
                    text->edited = true;
                }
            }
        }
    }





    bool UI::submit( ui::ID user_id, const char* label)
    {
        _gui->put
        ({
            .user_id = user_id,
            .text = ui::Text{
                .ptr = label,
                .width = 50,
                .height = 20,
                .font_size = 24,
                .color = 0xFFFFFF30,
                .align = ui::AlignText::Center,
            },
            .padding = {10,5,5,10},
            .color = 0xFFFFFF08,
            .width = ui::extend(),
            .height = ui::fixed(30),
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        return _gui->click( user_id );
    }

    bool UI::clickButton( ui::ID user_id, const ui::Text& label )
    {
        _gui->put
        ({
            .user_id = user_id,
            .text = label,
            .padding = {10,5,5,10},
            .color = 0xFFFFFF08,
            .width = ui::extend(),
            .height = ui::fixed(30),
            .single_click = ui::ClickTrigger::Immediate,
            .double_click = ui::ClickTrigger::Immediate,
        });
        return _gui->click( user_id );
    }

    bool UI::collapsableHeader( const ui::ID& id, bool& expand, const ui::Text& text )
    {
        _gui->push
        ({
            .user_id = id,
            .padding = {10, 0, 10, 0,},
            .alignment = {
                .direction = ui::AlignDir::DOWN
            },
            .color = 0xFFFFFF06,
            .width = ui::extend(),
            .height = ui::fixed(20),
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        {
            _gui->push
            ({
                .user_id = id,
                .alignment = {
                    .direction = ui::AlignDir::RIGHT
                },
                .gap = {5,0},
                .width = ui::extend(),
                .height = ui::fixed(20),
            });
            {
                _gui->put
                ({
                    .rect = ui::Rect{.width = 10, .height = 10},
                    .text = ui::Text{.ptr = (expand) ?"-" : "+", .width = 10, .height = 10}
                });
                label(text);
            }
            _gui->pop();
        }
        _gui->pop();

        if ( _gui->click( id ) )
        {
            expand = !expand;
        }
        return expand;
    }
    
    bool UI::checkbox( const ui::ID& id, const ui::Text& text,  bool& checked )
    {
        if ( _gui->click( id ) )
        {
            checked = !checked;
        }
        _gui->push
        ({
            .user_id = id,
            .alignment = kege::ui::Alignment
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .items = ui::AlignItem::CENTER
            },
            .color = 0xFFFFFF00,
            .gap {5,5},
            .width = ui::extend(),
            .height = ui::fixed(20),
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _gui->push
        ({
            .user_id = id,
            .rect = kege::ui::Rect{0.f, 0.f, 10, 10},
            .alignment = kege::ui::Alignment
            {
                .origin = {ui::AlignX::CENTER, ui::AlignY::CENTER},
                .direction = ui::AlignDir::CENTER,
            },
            .color = ( checked ) ? 0xf71326ff : 0xFFFFFF30,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
//        if ( checked )
//        {
//            _gui->put
//            ({
//                .layer = layer,
//                .rect = kege::ui::Rect{0.f, 0.f, .width = 6, .height = 6},
//                .color = 0xf71326ff
//            });
//        }
        _gui->pop();
        label(text);
        _gui->pop();
        return checked;
    }

    int UI::collapsableRemovableHeaderInput( const ui::UID& uid, char* str, size_t& size )
    {
        CollapsableRemovableHeader* header = ui::StateManager::make< CollapsableRemovableHeader >( uid );
        const ui::ID Id[4] = {uid[0], uid[1], uid[2], uid[4]};

        _gui->push
        ({
            .user_id = Id[0],
            .style = &_theme->collapsable_header,
            .gap = {10,0},
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        radio( Id[1], header->state );
        ui::WidgetId widget_id = _gui->put
        ({
            .user_id = Id[2],
            .text = {.ptr = str},
            .style = ( header->mode == 2 ) ? &_theme->collapsable_element_hl : &_theme->collapsable_element,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _gui->put /* remove button */
        ({
            .user_id = Id[3],
            .rect = {0, 4, 10, 10},
            .border = {.corner_curves = {8,8,8,8}},
            .color = _gui->mouseover( Id[3] ) ? 0xEE2200FF : 0xEE220050,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _gui->pop();

        if ( _gui->doubleClick( Id[2] ) )
        {
            header->mode = 2;
        }
        if ( header->mode == 2 )
        {
//            if ( !_gui->onTextInput( widget_id, str, size ) )
//            {
//                header->mode = 0;
//            }
        }

        if ( _gui->click(Id[1]) || _gui->click( Id[2] ) )
        {
            header->state = !header->state;
        }
        if ( _gui->click(Id[3]) )
        {
            return -1;
        }
        return header->state;
    }

    int UI::collapsableRemovableHeader( const ui::UID& uid, const ui::Text& text )
    {
        CollapsableRemovableHeader* header = ui::StateManager::make< CollapsableRemovableHeader >( uid );
        const ui::ID Id[3] = {uid[0], uid[1], uid[2]};

        _gui->push
        ({
            .user_id = Id[0],
            .style = &_theme->collapsable_removable_header,
            .gap = {10,0},
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        radio( Id[1], header->state );
        label(text);
        bool state = dotButn( Id[2] );
        _gui->pop();

        if ( _gui->click( Id[0] ) )
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
        CollapsableHeader* header = ui::StateManager::make< CollapsableHeader >( uid[0] );
        const ui::ID Id[3] = {uid[0], uid[1], uid[2]};

        _gui->push
        ({
            .user_id = Id[0],
            .style = &_theme->collapsable_header,
            .gap = {10,0},
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _gui->put
        ({
            .user_id = Id[1],
            .rect = {0, 6, 6, 6},
            .border = {.corner_curves = {5,5,5,5}},
            .color = (header->state == 0) ? 0xFFBB0040 : 0xFFBB00FF,
            .mouseover = false,
        });
        _gui->put
        ({
            .user_id = Id[2],
            .text = text,
            .style = &_theme->label,
            .mouseover = false,
        });
        _gui->pop();

        if ( _gui->click(Id[0]) )
        {
            header->state = !header->state;
        }
        return header->state;
    }

    int UI::removableHeader( const ui::ID id[4], const ui::Text& text )
    {
        RemovableHeader* header = ui::StateManager::make< RemovableHeader >( id[0] );

        _gui->push
        ({
            .user_id = id[0],
            .style = &_theme->collapsable_header,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _gui->put
        ({
            .user_id = id[1],
            .rect = {0 ,6 ,6 ,6 },
            .border = {.corner_curves = {5,5,5,5}},
            .color = ((header->state == 0) ? 0xFFBB0040 : 0xFFBB00FF),
            .mouseover = false,
        });
        _gui->put
        ({
            .user_id = id[2],
            .text = text,
            .style = &_theme->label,
            .mouseover = false,
        });
        _gui->put
        ({
            .user_id = id[3],
            .rect = {0, 4, 10, 10},
            .border = {.corner_curves = {8,8,8,8}},
            .color = _gui->mouseover( id[3] ) ? 0xEE2200FF : 0xEE220050,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _gui->pop();

        if ( _gui->click(id[0]) )
        {
            header->state = !header->state;
        }
        if ( _gui->click(id[3]) )
        {
            return 2;
        }
        return header->state;
    }

    struct OptionParam{ UI::OptionState* state; };
    void repositionOptionListOp(kege::GUI* gui, ui::ID user_id, ui::WidgetId widget_id, void* data)
    {
        OptionParam* state = reinterpret_cast< OptionParam* >(data);
        ui::Widget* main = gui->elem( widget_id );
        if (gui->leftClickDown())
        {
            const kege::dvec2& position = gui->getPointerPosition();
            if (!kege::ui::testPointVsRect(position, main->rect))
            {
               state->state->state = false;
            }
        }
    }

    bool UI::beginDropdown( const ui::ID& id, const ui::Text& label )//103079215104:orient 103079215104:posit
    {
        OptionState* state = ui::StateManager::make< OptionState >( id );
        _gui->push
        ({
            .user_id = id,
            .style = &theme()->card,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _gui->push({ .style = &theme()->row });
        _gui->text( label );
        _gui->pop();
        //_gui->pushDeferredOp<OptionParam>(id, widget_id, repositionOptionListOp, {&state});
        if( _gui->click( id ) )
        {
            state->state = !state->state;
        }
        return state->state;
    }

    void UI::endDropdown()
    {
        _gui->pop();
    }

    bool UI::labelOptions( const ui::UID& uid, const ui::Text& label, std::vector< ui::Text >& list, int& selection, bool vertical )
    {
        OptionState* state = ui::StateManager::make< OptionState >( uid[0] );
//        ui::WidgetId widget_id = _gui->push({ .style = (vertical)? &_theme->column : &_theme->row });
//        _gui->put
//        ({
//            .text = label,
//            .rect = {0,0,label.width, label.height},
//            .color = 0xFFFFFF06,
//            .mouseover = false
//        });
//        bool result = this->options( uid, list, selection );
//        _gui->pop();
//        return result;

        ui::WidgetId widget_id = _gui->push({ .style = &theme()->card });
        _gui->push
        ({
            .user_id = uid[0],
            .style = &theme()->row,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _gui->text( label );
        _gui->text( list[ selection ] );
        _gui->pop();

        if( _gui->click( uid[0] ) )
        {
            state->state = !state->state;
        }
        bool modified = false;
        if( state->state )
        {
            for (int i=0; i<list.size(); ++i)
            {
                ui::ID option_id = uid[i + 3];
                _gui->put
                ({
                    .user_id = option_id,
                    .text = list[i],
                    .style = (_gui->mouseover( option_id ))
                    ? &_theme->option_hot
                    : &_theme->option_element,
                    .single_click = ui::ClickTrigger::OnRelease,
                });

                if( _gui->click( option_id ) && !modified )
                {
                    modified = true;
                    selection = i;
                    state->selection = i;
                    state->state = false;
                }
            }
        }
        _gui->pop();
        _gui->pushDeferredOp<OptionParam>(uid[0], widget_id, repositionOptionListOp, {state});
        return modified;
    }

    bool UI::options( const ui::UID& uid, std::vector< ui::Text >& list, int& selection )
    {
        OptionState* state = ui::StateManager::make< OptionState >( uid[0] );
        bool has_selection = false;

        ui::WidgetId widget_id = _gui->push
        ({
            .user_id = uid[0],
            .style = &_theme->option,
            .gap = {10,4},
            .single_click = ui::ClickTrigger::Immediate,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _gui->put
        ({
            .user_id = uid[1],
            .text = list[ state->selection ],
            .style = &_theme->option_selection,
            .mouseover = false,
        });

        if ( _gui->click( uid[0] ) || _gui->click( uid[1] ) )
        {
            state->state = !state->state;
        }

        if ( state->state )
        {
            state->index = _gui->push({.user_id = uid[2], .style = &_theme->option_list});
            for (int i=0; i<list.size(); ++i)
            {
                ui::ID option_id = uid[i + 3];
                _gui->put
                ({
                    .user_id = option_id,
                    .text = list[i],
                    .style = (_gui->mouseover( option_id ))
                    ? &_theme->option_hot
                    : &_theme->option_element,
                    .single_click = ui::ClickTrigger::OnRelease,
                });

                if( _gui->click( option_id ) && !has_selection )
                {
                    state->selection = i;
                    has_selection = true;
                    state->state = false;
                }
            }
            _gui->pop();
            _gui->pushDeferredOp<OptionParam>(uid[0], widget_id, repositionOptionListOp, {state});
        }
        _gui->pop();
        selection = state->selection;
        return has_selection;
    }

    bool  UI::select( const ui::UID& uid, std::vector< ui::Text >& list, int& selection )
    {
        bool has_selection = false;
        _gui->push({ .style = &_theme->select });
        for (int i=0; i<list.size(); ++i)
        {
            _gui->put
            ({
                .user_id = uid[i],
                .text = list[i],
                .style = (_gui->mouseover( uid[i] ))
                ? &_theme->option_hot
                : &_theme->option_element,
                .single_click = ui::ClickTrigger::OnRelease,

            });

            if( _gui->click( uid[i] ) && !has_selection )
            {
                selection = i;
                has_selection = true;
            }
        }
        _gui->pop();
        return has_selection;
    }

    bool UI::tab( const ui::UID& uid, std::vector< ui::Text >& list, int& selection )
    {
        bool active = false;
        _gui->push({ .style = &_theme->tab });
        for (int i=0; i<list.size(); ++i)
        {
            _gui->put
            ({
                .user_id = uid[i],
                .text = list[i],
                .style = (selection == i) ? &_theme->tab_elem_focus : &_theme->tab_elem,
                .single_click = ui::ClickTrigger::OnRelease,
            });
            if( _gui->click( uid[i] ) )
            {
                selection = i;
                active = true;
            }
        }
        _gui->pop();
        return active;
    }

    void UI::list( std::vector< ui::Text >& list )
    {
        _gui->push({ .style = &_theme->list });
        for (int i=0; i<list.size(); ++i)
        {
            _gui->put
            ({
                .text = list[i],
                .style = &_theme->list_elem
            });
        }
        _gui->pop();
    }

    bool UI::numSlideBar( ui::ID user_id, double* val, double min, double max )
    {
        char snum[16];
        snprintf(snum, 16, "%.6g", *val );

        ui::WidgetId widget_index = _gui->push
        ({
            .user_id = user_id,
            .style = &_theme->slide_bar_track,
            .single_click = ui::ClickTrigger::Continuous,
            .double_click = ui::ClickTrigger::Immediate
        });
        _gui->put({ .user_id = user_id, .style = &_theme->slide_bar });
        _gui->put({ .text = {.ptr = snum}, .style = &_theme->slide_bar_value });
        _gui->pop();

        _gui->pushDeferredOp(user_id, widget_index, slidebarOp<double>, kege::RangeParams<double>{min, max, val, widget_index});
        return _gui->click( user_id );
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

        _gui->put
        ({
            .user_id = user_id,
            .rect = ui::Rect{.x = 0.f, .y = 0.f, .width = 20.f, .height = 20.f},
            .border = {.corner_curves = {10,10,10,10}},
            .text = text,
            .color = ( _gui->mouseover(user_id) ) ? 0xFFFFFF3F : 0xFFFFFF1F,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        return _gui->click(user_id);
    }

    bool UI::dotButn( ui::ID user_id )
    {
        _gui->put
        ({
            .user_id = user_id,
            .rect = {0, 4, 10, 10},
            .border = {.corner_curves = {8,8,8,8}},
            .color = _gui->mouseover( user_id ) ? 0xEE2200FF : 0xEE220050,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        return _gui->click( user_id );
    }

    bool UI::button( const kege::ui::WidgetDesc& desc )
    {
        _gui->put( desc );
        return _gui->click( desc.user_id );
    }

    bool UI::button( ui::ID user_id, const ui::Text& text, const ui::Style* style )
    {
        _gui->put
        ({
            .user_id = user_id,
            .text = text,
            .style = (style == nullptr)? &_theme->button : style,
            .single_click = ui::ClickTrigger::OnRelease,
        });
        return _gui->click( user_id );
    }

    ui::WidgetId UI::fittedLabel( const ui::Text& text, ui::Style* style )
    {
        return _gui->put
        ({
            .text = text,
            .style = (style == nullptr)? &_theme->fitted_label : style,
            .mouseover = false
        });
    }

    ui::WidgetId UI::label( const ui::Text& text, ui::Style* style )
    {
        return _gui->put
        ({
            .text = text,
            .style = (style == nullptr)? &_theme->label : style,
            .mouseover = false
        });
    }

    bool UI::radio( ui::ID user_id, const ui::Text& text, bool& state )
    {
        push({.style = &_theme->row });
        radio( user_id, state );
        fittedLabel(text);
        pop();
        return state;
    }

    bool UI::radio( ui::ID user_id, bool& state )
    {
        _gui->put
        ({
            .user_id = user_id,
            .rect = {0,4,0,0},
            .border = {.corner_curves = {5,5,5,5}},
            .style = (state)? &_theme->radio_active: &_theme->radio,
            .color = (state == 0) ? 0xFFBB0040 : 0xFFBB00FF,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        if ( _gui->click(user_id) )
        {
            state = !state;
        }
        return state;
    }

    bool UI::radio( ui::ID user_id, int16_t layer )
    {
        RadioState* state = ui::StateManager::make< RadioState >( user_id );
        return radio(user_id, state->state);
    }

    ui::WidgetId UI::beginList(int16_t layer)
    {
        return _gui->push({ .style = &_theme->list });
    }
    
    void UI::endList()
    {
        _gui->pop();
    }

    ui::WidgetId UI::beginColumn( ui::Style* style )
    {
        return _gui->push
        ({
            .style = (style == nullptr)? &_theme->column : style
        });
    }
    void UI::endColumn()
    {
        _gui->pop();
    }

    ui::WidgetId UI::beginRow( ui::Style* style )
    {
        return _gui->push
        ({
            .style = (style == nullptr)? &_theme->row : style
        });
    }
    void UI::endRow()
    {
        _gui->pop();
    }


    struct ScrollOp
    {
        ui::WidgetId widget_id;
        float& amount;
    };
    void scrollingOperation(kege::GUI* gui, ui::ID user_id, ui::WidgetId widget_id, void* data)
    {
        ScrollOp* params = reinterpret_cast<ScrollOp*>(data);

        ui::Widget* scrolly = gui->elem(params->widget_id);
        ui::Widget* clipper = gui->elem(widget_id);

        if (kege::ui::testPointVsRect( gui->getPointerPosition(), clipper->rect ))
        {
            float scroll_length = scrolly->rect.height - clipper->rect.height;
//            if ( 0 < scroll_length)
//            {
                params->amount += gui->getScrollOffset().y;
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
        ScrollContainer* state =  ui::StateManager::get< ScrollContainer >( id[0] );
        if (state == nullptr)
        {
            state =  ui::StateManager::insert< ScrollContainer >( id[0] );
            state->scroll_amount = 0.f;
            state->frame_id = _frame_index;
        }

        ui::WidgetId clipper = _gui->push /* the clipper is the container the scroller is clipped against */
        ({
            .user_id = id[0],
            .style = &_theme->scroll_container,
            .clip_overflow = true
        });

        ui::WidgetId scroller = _gui->push /* the scroller is the container that is scrolled */
        ({
            .user_id = id[1],
            .rect = {0, state->scroll_amount, 0, 0},
            .style = &_theme->column,
            .mouseover = false,
        });

        _gui->pushDeferredOp(id[0], clipper, scrollingOperation, ScrollOp{
            .widget_id = scroller,
            .amount = state->scroll_amount
        });
    }
    
    void UI::endScrollContainer()
    {
        _gui->pop(); /* pop the scroller container */
        _gui->pop(); /* pop the clipper container */
    }

    ui::WidgetId UI::pushRoot( const ui::WidgetDesc& desc )
    {
        return _gui->pushRoot(desc);
    }

    ui::WidgetId UI::putRoot( const ui::WidgetDesc& desc )
    {
        return _gui->putRoot(desc);
    }
    
    void UI::popRoot()
    {
        _gui->popRoot();
    }

    ui::WidgetId UI::push( const kege::ui::WidgetDesc& desc )
    {
        return _gui->push( desc );
    }

    ui::WidgetId UI::put( const kege::ui::WidgetDesc& desc )
    {
        return _gui->put( desc );
    }

    ui::WidgetId UI::text( const kege::ui::Text& text )
    {
        return _gui->text( text );
    }

    void UI::pop()
    {
        _gui->pop();
    }
    void UI::pushLayer( uint32_t index )
    {
        _gui->pushLayer( index );
    }

    bool UI::popLayer()
    {
        return _gui->popLayer();
    }

    const bool UI::pointerDragging() const
    {
        return _gui->isPointerDragging();
    }

    bool UI::mouseover( const ui::ID& id )
    {
        return _gui->mouseover( id );
    }

    bool UI::click( const ui::ID& id )
    {
        return _gui->click( id );
    }

    bool UI::hot( const ui::ID& id )
    {
        return _gui->mouseover( id );
    }

    bool UI::leftClickDown()const
    {
        return _gui->leftClickDown();
    }

    bool UI::dragging()const
    {
        return _gui->isPointerDragging();
    }

    ui::Widget* UI::get( ui::WidgetId index )
    {
        return _gui->elem( index );
    }

    bool UI::initialize(kege::GUI* gui)
    {
        _gui = gui;
        return true;
    }

//    void UI::createLayers( uint32_t quantity )
//    {
//        _gui->createLayers( quantity );
//    }

    vec2d UI::deltaPointer()const
    {
        return _gui->getPointerDelta();
    }

    vec2d UI::pointer()const
    {
        return _gui->getPointerPosition();
    }

    void UI::begin( double dms )
    {
        ++_frame_index;

        // reset layout state
        _gui->begin( dms );
    }

    void UI::end()
    {
        _gui->end();

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

    void UI::init(kege::GUI* gui, kege::Theme* theme)
    {
        _theme = theme;
        _gui = gui;
    }
    
    UI::~UI()
    {}

    UI::UI()
    :   _frame_index(0)
    ,   _str_len(0)
    ,   _gui( nullptr )
    {
    }
}
