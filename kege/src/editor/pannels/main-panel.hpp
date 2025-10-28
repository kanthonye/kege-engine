//
//  main-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef main_panel_hpp
#define main_panel_hpp

#include "editor-panel.hpp"

namespace kege{

    class MainPanel : public kege::EditorPanel
    {
    public:


        MainPanel( kege::ProjectManager* pm, ui::Layout* l );
        void update();

    public:

        std::vector< ref::EditorPanel > _panels;
        ui::EID _main;
    };
}

#endif /* main_panel_hpp */
