//
//  viewport-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "viewport-panel.hpp"

namespace kege{

    ViewportPanel::ViewportPanel( kege::ProjectManager* pm, ui::Layout* l )
    :   kege::EditorPanel( pm, l )
    {
        _main = _layout->make
        ({
            .visible    = true,
            .mouseover  = false,
            .texr       = ui::TexrID
            {
                .id     = 2,
                .x      = 0.f,
                .y      = 0.f,
                .width  = 1.f,
                .height = 1.f,
            },
            .style      = _layout->getStyleByName( "viewport" )
        });
    }

    void ViewportPanel::update()
    {
        _layout->push( _main );
        _layout->pop();
    }

}
