//
//  floating-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef floating_panel_hpp
#define floating_panel_hpp

#include "editor-panel.hpp"

namespace kege{

    class FloatingPanel : public kege::EditorPanel
    {
    public:


        FloatingPanel( kege::ProjectManager* pm, ui::Layout* l );
        void update();

    public:

        std::vector< ref::EditorPanel > _panels;
        ui::EID _main;
    };
}

#endif /* floating_panel_hpp */
