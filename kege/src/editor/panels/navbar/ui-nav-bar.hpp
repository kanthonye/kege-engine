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

    struct MenuElem
    {
        ui::Text text;
        std::vector< MenuElem > elements;
        kege::ui::UID uid;
        bool hot = false;
    };

    class NavBar : public kege::RefCounter
    {
    public:

        void update();

        NavBar
        (
            const kege::ui::Quad& quad,
            kege::GUI* gui,
            kege::UI* ui,
            kege::ProjectManager* pm,
            kege::ECS* ecs
        );
    private:

        void updateMenu();
        void display( MenuElem& menu );

    private:

        kege::ui::Quad _quad;
        ui::WidgetHandle _main;
        kege::GUI* _gui;
        kege::UI* _ui;
        kege::ui::UID _uid;
        std::vector<ui::Text> _tabs;
        std::vector<ui::MenuElem> _menus;
        int _selected;

        bool _expand;
    };
}

#endif /* navbar_panel_hpp */
