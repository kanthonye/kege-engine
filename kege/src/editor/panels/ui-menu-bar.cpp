//
//  navbar-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "../editor-layer.hpp"
#include "ui-menu-bar.hpp"

namespace kege::ui{

    MenuBar::MenuBar( kege::ui::DockManager* dm )
    :   kege::ui::Panel( "MenuBar", dm )
    {
//        _main = _layout->make({ .style = _layout->getStyleByName( "navbar" ) });
    }

    void MenuBar::updateLayout()
    {
//        _layout->push({ .id = &_main });
//        if ( 0 <= file.select( *_layout, "File", { "Save", "Open", "Import", "Export" } ) )
//        {
//            std::cout <<"selection: " << file.index[0] <<"\n";
//        }
//        _layout->pop();
    }
}
