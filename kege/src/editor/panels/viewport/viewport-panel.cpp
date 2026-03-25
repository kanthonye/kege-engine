//
//  viewport-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "../../editor-layer.hpp"
#include "viewport-panel.hpp"

namespace kege{

    ViewportPanel::ViewportPanel( kege::EditorLayer* editor )
    :   kege::ui::Panel( "Viewport", editor )
    {
        _style.height = ui::extend();
        _style.width = ui::extend();
        _style.background = ui::Background( 0x340505FF );
    }

    void ViewportPanel::update()
    {
        _gui->put
        ({
            .layer = 0,
            .style = &_style,
            .mouseover = false,
            .texr_info = ui::TexrInfo{.id = 2, .index = 2},
            .texel = ui::Rect{0.f,0.f,1.f,1.f},
        });
    }

}
