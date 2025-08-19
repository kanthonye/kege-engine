//
//  navbar-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef navbar_panel_hpp
#define navbar_panel_hpp

#include "../core/engine/engine.hpp"
#include "../core/gui/ui-input.hpp"
#include "../core/gui/ui-viewer.hpp"
#include "../core/gui/ui-layout.hpp"
#include "../core/gui/ui-font-creator.hpp"

namespace kege{

    class NavbarPanel
    {
    public:

        NavbarPanel& init( Engine* engine, ui::Layout& layout );
        void put( ui::Layout& layout );
        NavbarPanel();

    public:

        Engine* _engine;
        ui::EID _main;
    };
}

#endif /* navbar_panel_hpp */
