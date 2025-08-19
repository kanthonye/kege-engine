//
//  viewport-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "viewport-panel.hpp"

namespace kege{

    void ViewportPanel::put( ui::Layout& layout )
    {
        layout.push( _main );
        layout.pop();
    }

    ViewportPanel& ViewportPanel::init( Engine* engine, ui::Layout& layout )
    {
        _engine = engine;
        _main = layout.make
        ({
            .style =
            {
                .background = ui::bgColor(0x171717FF),
                .width = ui::extend(),
                .height = ui::extend(),
                .border_radius = 8,
                .padding = {},
                .gap = {2,2},
            }
        });
        return *this;
    }

    ViewportPanel::ViewportPanel()
    :   _engine( nullptr )
    {}

}
