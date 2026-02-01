//
//  ui-dock-context.cpp
//  editor
//
//  Created by Kenneth Esdaile on 1/18/26.
//

#include "ui-dock-context.hpp"
#include "hierarchy-panel.hpp"
#include "inspector-panel.hpp"
#include "viewport-panel.hpp"
#include "ui-file-browser.hpp"
#include "ui-asset-manager.hpp"
#include "ui-console.hpp"
#include "ui-menu-bar.hpp"

namespace kege::ui{

    DockContext::DockContext(kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* ecs)
    :   ghost{}
    ,   gui(gui)
    {
        style_dock.width = ui::extend();
        style_dock.height = ui::extend();
        style_dock.padding = {};
        style_dock.background = ui::Background(0x0b090fFF);
        style_dock.alignment.direction = ui::AlignDir::VERTICAL;
        style_dock.alignment.flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS};
        style_dock.alignment.origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP};
        style_dock.alignment.content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP};

        style_tab = kege::ui::Style
        {
            .background = ui::Background(0x0B090F00),
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {4,0},
            .alignment =
            {
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };

        style_tab_elem = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::flexible(),
            .background = ui::Background(0x171420FF),
            .align_text =  ui::AlignText::Center,
            .padding = {10,4,10,4},
            .gap = {20,0}
        };

        style_tab_label = kege::ui::Style
        {
            .height = ui::fixed(20),
            .width = ui::flexible(),
            .background = ui::Background(0xFFFFFF00),
            .align_text =  ui::AlignText::Left,
            .text_color = 0xBBA0FFFF,
            .font_size = 20,
        };

        style_ghost = kege::ui::Style
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

        panels.push_back(new ui::HierarchyPanel( pm, gui, ecs ));
        panels.push_back(new kege::InspectorPanel( pm, gui, ecs ));
        panels.push_back(new kege::ViewportPanel( pm, gui, ecs ));
        panels.push_back(new ui::FileBrowser( pm, gui, ecs, "/Users/kae/Developer/vscode/kege-engine/kege/assets" ));
        panels.push_back(new ui::AssetManagerUI( pm, gui, ecs ));
        panels.push_back(new ui::Console( pm, gui, ecs ));
        panels.push_back(new ui::MenuBar( pm, gui, ecs ));
    }

}
