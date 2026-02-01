//
//  navbar-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef navbar_panel_hpp
#define navbar_panel_hpp

#include "ui-panel.hpp"

namespace kege::ui{

    class MenuBar : public kege::ui::Panel
    {
    public:

        MenuBar( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* ecs );
        void update();

    public:

        ui::UID _main;
    };
}

#endif /* navbar_panel_hpp */
