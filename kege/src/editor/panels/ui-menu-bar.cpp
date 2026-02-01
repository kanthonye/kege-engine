//
//  navbar-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "ui-menu-bar.hpp"

namespace kege::ui{

    MenuBar::MenuBar( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* e )
    :   kege::ui::Panel( "MenuBar", pm, gui, e )
    {
//        _main = _layout->make({ .style = _layout->getStyleByName( "navbar" ) });
    }

    void MenuBar::update()
    {
//        _layout->push({ .id = &_main });
//        if ( 0 <= file.select( *_layout, "File", { "Save", "Open", "Import", "Export" } ) )
//        {
//            std::cout <<"selection: " << file.index[0] <<"\n";
//        }
//        _layout->pop();
    }
}
