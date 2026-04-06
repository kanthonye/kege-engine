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
        _style.height = ui::extend();
        _style.width = ui::extend();
        _style.background = ui::Background( 0x340505FF );
    }

    void ViewportPanel::updateLayout( int16_t layer )
    {
        _gui->put
        ({
            .user_id = _uid[0],
            .layer = layer,
            .style = &_style,
            .mouseover = false,
            .texr_info = ui::TexrInfo{.id = 2, .index = 2},
            .texel = ui::Rect{0.f,0.f,1.f,1.f},
        });
    }

}
