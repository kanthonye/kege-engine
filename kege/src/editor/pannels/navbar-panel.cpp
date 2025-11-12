//
//  navbar-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "navbar-panel.hpp"

namespace kege{

    NavbarPanel::NavbarPanel( kege::ProjectManager* pm, ui::Layout* l )
    :   kege::ui::Panel( "Navbar", pm, l )
    {
        _main = _layout->make({ .style = _layout->getStyleByName( "navbar" ) });
    }

    void NavbarPanel::update()
    {
        _layout->push( _main );
        if ( 0 <= file.select( *_layout, "File", { "Save", "Open", "Import", "Export" } ) )
        {
            std::cout <<"selection: " << file.index[0] <<"\n";
        }
        _layout->pop();
    }
}
