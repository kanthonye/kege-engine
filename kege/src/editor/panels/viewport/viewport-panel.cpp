//
//  viewport-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "../../editor-layer.hpp"
#include "viewport-panel.hpp"

namespace kege{

    ViewportPanel::ViewportPanel( kege::ui::DockManager* dm )
    :   kege::ui::Panel( "Viewport", dm )
    {
    }

    void ViewportPanel::updateLayout()
    {
        _ui->put
        ({
            .user_id = _uid[0],
            .wid = _ui->newElem
            ({
                .width = ui::extend(),
                .height = ui::extend(),
                .quad_color = 0x340505FF,
                .texr_info = ui::Rect{0.f, 0.f, 1.f, 1.f},
                .texr_id = ui::TexrId{.id = 2, .index = 2},
            }),
            .mouseover = false,
        });
    }

}

