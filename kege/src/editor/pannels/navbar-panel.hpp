//
//  navbar-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef navbar_panel_hpp
#define navbar_panel_hpp

#include "../elements/ui-menu.hpp"
#include "../elements/ui-options.hpp"
#include "../elements/ui-button.hpp"
#include "ui-panel.hpp"

namespace kege{

    class NavbarPanel : public kege::ui::Panel
    {
    public:

        NavbarPanel( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* ecs );
        void update();

    public:

        ui::Options file;
        ui::UID _main;
    };
}

#endif /* navbar_panel_hpp */
