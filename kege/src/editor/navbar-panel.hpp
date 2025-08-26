//
//  navbar-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef navbar_panel_hpp
#define navbar_panel_hpp

#include "ui-elements/ui-menu.hpp"
#include "ui-elements/ui-options.hpp"
#include "ui-elements/ui-button.hpp"

namespace kege{

    class NavbarPanel
    {
    public:

        NavbarPanel& init( Engine* engine, ui::Layout& layout );
        void put( ui::Layout& layout );
        NavbarPanel();

    public:

        ui::Options file;
        Engine* _engine;
        ui::EID _main;
    };
}

#endif /* navbar_panel_hpp */
