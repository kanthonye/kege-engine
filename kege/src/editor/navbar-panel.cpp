//
//  navbar-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "navbar-panel.hpp"

namespace kege{

    void NavbarPanel::put( ui::Layout& layout )
    {
        layout.push( _main );

        if ( 0 <= file.select( layout, "File", { "Save", "Open", "Import", "Export" } ) )
        {
            std::cout <<"selection: " << file.index[0] <<"\n";
        }

        layout.pop();
    }

    NavbarPanel& NavbarPanel::init( Engine* engine, ui::Layout& layout )
    {
        _engine = engine;
        _main = layout.make({ .style = layout.getStyleByName( "navbar" ) });
        return *this;
    }

    NavbarPanel::NavbarPanel()
    :   _engine( nullptr )
    {}

}
