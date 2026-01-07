//
//  viewport-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "viewport-panel.hpp"

namespace kege{

    ViewportPanel::ViewportPanel( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* ecs )
    :   kege::ui::Panel( "Viewport", pm, gui, ecs )
    {
        _style = kege::ui::Style
        {
            .background = ui::Background(0x171420FF),
            .height = ui::extend(),
            .width = ui::extend(),
            .padding = {10,10,10,10},
            .gap = {2,2},
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };
//        _main = _layout->make
//        ({
//            .visible    = true,
//            .mouseover  = false,
//            .texr       = ui::TexrID
//            {
//                .id     = 2,
//                .x      = 0.f,
//                .y      = 0.f,
//                .width  = 1.f,
//                .height = 1.f,
//            },
//            .style      = _layout->getStyleByName( "panel" )
//        });
    }

    void ViewportPanel::update()
    {
        _gui->put({ .layer = 0, .style = &_style, .enabled = false, .text = "viewport" });
//        _gui->pop();
    }

}
