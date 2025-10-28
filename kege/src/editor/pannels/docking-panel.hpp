//
//  docking-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef docking_panel_hpp
#define docking_panel_hpp

#include "editor-panel.hpp"

namespace kege{ class DockingPanel; }
namespace kege::ref{ typedef kege::Ref< kege::DockingPanel > DockingPanel; }

namespace kege{

    class DockingPanel : public kege::EditorPanel
    {
    public:

        DockingPanel( kege::ProjectManager* pm, ui::Layout* l );
        void add( ref::EditorPanel panel );
        void update();

    public:

        std::vector< ref::EditorPanel > _panels;
        ui::EID _main;
    };
}

#endif /* docking_panel_hpp */
