//
//  ui-executor.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/10/26.
//

#ifndef ui_executor_hpp
#define ui_executor_hpp

#include "ui.hpp"
#include <unordered_map>

namespace kege::ui{

    enum class UICommandType {
        PushLayer,
        PopLayer,
        Push,
        Pop,
        Put,
        Text,
        ClickIf,

        Row,
        Column,
        RowPadded,
        ColumnPadded,

        Button,
        ClickButton,
        CheckBox,
        DragNum,
        Slider,
        DragBar,
        Input,

        Options,
        LabelOptions,
        Scrubber,
        LabelScrubber,
        Collapsable,
        RemovableHeader,
        BeginWindow,
        EndWindow,
        BeginScrollContainer,
        EndScrollContainer,
    };

    enum class VarType {
        F32,
        F64,
        I32,
        I64,
        U32,
        U64,
        Bool,
        String,
        Vec2,
        Vec3,
        Vec4,
        Quat,
        Mat22,
        Mat33,
        Mat44
    };

    struct Binding
    {
        std::function<void(void*&)> set;
        std::function<void(void*&)> get;
        VarType type;
    };

    enum class ActionType {
        None,
        Toggle,
        SetValue
    };

    struct Action
    {
        ActionType type;
        Binding* bind;
        double value;
    };

    struct Button
    {
        kege::ui::UID uid;
        kege::ui::Text text;
        Action action;
    };

    struct Scrubber {
        kege::ui::UID uid;
        kege::ui::Text text;
        Action action;
        double min, max;
        bool clamp;
    };

    struct CheckBox {
        kege::ui::UID uid;
        kege::ui::Text text;
        Action action;
        bool state;
    };

    struct Options {
        kege::ui::UID uid;
        kege::ui::Text text;
        Action action;
        int selection;
    };

    struct Collapsable {
        kege::ui::UID uid;
        kege::ui::Text text;
        Action action;
        bool expand;
    };

    struct ClickIf
    {
        kege::ui::ID id;
        Action action;
    };

    struct Window
    {
        kege::ui::UID uid;
        kege::ui::Rect rect;
        bool open;
        const char *title;
    };
    struct EndWindow
    {
        int32_t window_cmd_index = -1;
    };

    struct ScrollContainer
    {
        kege::ui::UID uid;
        uint64_t clipper;
        uint64_t scroller;
        float scroll_amount;
        int frame_id;
    };


    struct Command
    {
        Command& operator =(const kege::ui::Command& cmd){
            switch (cmd.type)
            {
                case UICommandType::Push:
                case UICommandType::Put:
                case UICommandType::PushLayer:
                case UICommandType::PopLayer:
                case UICommandType::Pop:
                    index = cmd.index;
                    break;

                case UICommandType::Text:
                    text = cmd.text;
                    break;

                case UICommandType::ClickButton:
                case UICommandType::Button:
                    button = cmd.button;
                    break;

                case UICommandType::CheckBox:
                    checkbox = cmd.checkbox;
                    break;

                case UICommandType::LabelOptions:
                case UICommandType::Options:
                    options = cmd.options;
                    break;

                case UICommandType::LabelScrubber:
                    scrubber = cmd.scrubber;
                    break;

                case UICommandType::Collapsable:
                    collapsable = cmd.collapsable;
                    break;

                case UICommandType::BeginWindow:
                    window = cmd.window;
                    break;

                case UICommandType::EndWindow:
                    end_window = cmd.end_window;
                    break;

                    break;

                case UICommandType::BeginScrollContainer:
                case UICommandType::EndScrollContainer:
                    scroll_container = cmd.scroll_container;
                    break;

                case UICommandType::ClickIf:
                    click_if = cmd.click_if;
                    break;

                default: break;
            }
            return *this;
        }
        Command(const kege::ui::Command& cmd)
        {
            operator =(cmd);
        }
        
        ~Command(){}
        Command(){}

        UICommandType type;

        union
        {
            Button button;
            CheckBox checkbox;
            Options options;
            Scrubber scrubber;
            Collapsable collapsable;
            kege::ui::Text text;
            ClickIf click_if;
            Window window;
            EndWindow end_window;
            ScrollContainer scroll_container;
            // etc
        };

        int index;
    };


    struct BindingContext
    {
        std::unordered_map<std::string, Binding> table;

        void add(const std::string& id, const Binding& binding)
        {
            table[id] = binding;
        }

        void remove(const std::string& id)
        {
            std::unordered_map<std::string, Binding>::iterator it = table.find(id);
            if (it != table.end()) table.erase( it );
        }

        Binding* get(const std::string& id)
        {
            auto it = table.find(id);
            return (it != table.end()) ? &it->second : nullptr;
        }

        void clear()
        {
            table.clear();
        }
    };

    struct Context
    {
        std::vector< std::vector< kege::ui::Text > > text_array_lists;
        std::vector<kege::ui::WidgetDesc> widget_descriptors;
        std::vector< kege::ui::Text > textlist;
        BindingContext bindings;
        kege::UI* ui;
    };

    void execute(ui::Command* cmds, size_t count, ui::Context* context);
    void applyAction(ui::Action& action);
}
#endif /* ui_executor_hpp */
