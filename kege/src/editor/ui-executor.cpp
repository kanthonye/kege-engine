//
//  ui-executor.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/10/26.
//

#include "ui-executor.hpp"

namespace kege::ui{

    void execute(ui::Command* cmds, size_t count, ui::Context* context)
    {
        for (int i = 0; i < count; i++)
        {
            auto& cmd = cmds[i];

            switch (cmd.type)
            {
                case UICommandType::Push:
                    context->ui->push( context->widget_descriptors[ cmd.index ] );
                    break;

                case UICommandType::Put:
                    context->ui->put( context->widget_descriptors[ cmd.index ] );
                    break;

                case UICommandType::PushLayer:
                    context->ui->pushLayer( cmd.index );
                    break;

                case UICommandType::PopLayer:
                    context->ui->popLayer();
                    break;

                case UICommandType::Pop:
                    context->ui->pop();
                    break;

                case UICommandType::Text:
                    context->ui->text(cmd.text);
                    break;

                case UICommandType::Button:
                    if (context->ui->button(cmd.button.uid[0], cmd.button.text))
                    {
                        applyAction(cmd.button.action);
                    }
                    break;

                case UICommandType::ClickButton:
                    if (context->ui->clickButton(cmd.button.uid[0], cmd.button.text))
                    {
                        applyAction(cmd.button.action);
                    }
                    break;

                case UICommandType::CheckBox:
                    if (context->ui->checkbox(cmd.checkbox.uid[0], cmd.checkbox.text, cmd.checkbox.state))
                    {
                        applyAction(cmd.checkbox.action);
                    }
                    break;

                case UICommandType::LabelOptions:
                    if (context->ui->labelOptions(cmd.options.uid, cmd.options.text, context->text_array_lists[ cmd.index ], cmd.options.selection))
                    {
                        applyAction(cmd.options.action);
                    }
                    break;

                case UICommandType::Options:
                    if (context->ui->options(cmd.options.uid, context->text_array_lists[ cmd.index ], cmd.options.selection))
                    {
                        applyAction(cmd.options.action);
                    }
                    break;

                case UICommandType::LabelScrubber:
                {
                    if (cmd.scrubber.action.bind == nullptr) break;
                    if (cmd.scrubber.action.bind->get == nullptr) break;

                    void* data = nullptr;
                    cmd.scrubber.action.bind->get(data);

                    switch ( cmd.scrubber.action.bind->type )
                    {
                        case VarType::F32:
                            context->ui->labelScrubber<float>
                            (
                                kege::ScrubberState::F32,
                                cmd.scrubber.uid[0], cmd.scrubber.text,
                                *static_cast<float*>( data ),
                                cmd.scrubber.min, cmd.scrubber.max, cmd.scrubber.clamp
                            );
                            break;

                        case VarType::F64:
                            context->ui->labelScrubber<double>
                            (
                                kege::ScrubberState::F64,
                                cmd.scrubber.uid[0], cmd.scrubber.text,
                                *static_cast<double*>( data ),
                                cmd.scrubber.min, cmd.scrubber.max, cmd.scrubber.clamp
                            );
                            break;

                        case VarType::I32:
                            context->ui->labelScrubber<int32_t>
                            (
                                kege::ScrubberState::I32,
                                cmd.scrubber.uid[0], cmd.scrubber.text,
                                *static_cast<int32_t*>( data ),
                                cmd.scrubber.min, cmd.scrubber.max, cmd.scrubber.clamp
                            );
                            break;

                        case VarType::I64:
                            context->ui->labelScrubber<int64_t>
                            (
                                kege::ScrubberState::I64,
                                cmd.scrubber.uid[0], cmd.scrubber.text,
                                *static_cast<int64_t*>( data ),
                                cmd.scrubber.min, cmd.scrubber.max, cmd.scrubber.clamp
                            );
                            break;

                        case VarType::U32:
                            context->ui->labelScrubber<uint32_t>
                            (
                                kege::ScrubberState::U64,
                                cmd.scrubber.uid[0], cmd.scrubber.text,
                                *static_cast<uint32_t*>( data ),
                                cmd.scrubber.min, cmd.scrubber.max, cmd.scrubber.clamp
                            );
                            break;

                        case VarType::U64:
                            context->ui->labelScrubber<uint64_t>
                            (
                                kege::ScrubberState::U64,
                                cmd.scrubber.uid[0], cmd.scrubber.text,
                                *static_cast<uint64_t*>( data ),
                                cmd.scrubber.min, cmd.scrubber.max, cmd.scrubber.clamp
                            );
                            break;

                        default: break;
                    }
                    break;
                }

                case UICommandType::Collapsable:
                    if (context->ui->collapsableHeader(cmd.collapsable.uid[0], cmd.collapsable.expand, cmd.collapsable.text))
                    {
                        applyAction(cmd.collapsable.action);
                    }
                    break;

                case UICommandType::BeginWindow:
                {
                    if (cmd.window.open)
                    {
                        kege::ui::ID id[3] = {cmd.window.uid[0], cmd.window.uid[1], cmd.window.uid[2]};
                        context->ui->beginWindow(id, cmd.window.rect, cmd.window.title, cmd.window.open);
                    }
                    break;
                }

                case UICommandType::EndWindow:
                    if (cmd.end_window.window_cmd_index >= 0 && cmd.end_window.window_cmd_index < count )
                    {
                        Command* c = &cmds[ cmd.end_window.window_cmd_index ];
                        if (c->type == UICommandType::BeginWindow)
                        {
                            if (c->window.open)
                            {
                                context->ui->endWindow();
                            }
                        }
                    }
                    break;

                case UICommandType::BeginScrollContainer:
                {
                    kege::ui::ID id[2] = {cmd.scroll_container.uid[0], cmd.scroll_container.uid[1]};
                    context->ui->beginScrollContainer(id);
                    break;
                }

                case UICommandType::EndScrollContainer:
                    context->ui->endScrollContainer();
                    break;

                case UICommandType::ClickIf:
                    if ( context->ui->click( cmd.click_if.id ) )
                    {
                        applyAction(cmd.click_if.action);
                    }
                    break;

                default: break;
            }
        }
    }

    void applyAction(Action& action)
    {
        if (!action.bind) return;

        switch (action.type)
        {
            case ActionType::Toggle:
            {
                if ( !action.bind->get ) return;
                if (action.bind->type == VarType::Bool)
                {
                    void* stateptr = nullptr;
                    action.bind->get( stateptr );
                    bool& b = *static_cast<bool*>( stateptr );
                    b = !b;
                }
                break;
            }

            case ActionType::SetValue:
            {
                if ( !action.bind->set ) return;
                switch (action.bind->type)
                {
                    case VarType::F32:
                        //action.bind->set( &action.value );
                        break;

                    case VarType::I32:
                        //action.bind->set( &action.value );
                        break;

                    default: break;
                }
                break;
            }

            default: break;
        }
    }

}
