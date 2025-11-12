//
//  ui-docking-manager.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#ifndef ui_docking_manager_hpp
#define ui_docking_manager_hpp

#include "ui-tab.hpp"
#include "ui-panel.hpp"

namespace kege{
    class EditorLayer;
}
namespace kege::ui{

    struct DockNode : public kege::RefCounter
    {
        void update(EditorLayer* mgr);
        DockNode(ui::Layout* layout, const std::string& split);

        ui::EID _main_container;
        ui::Tabs _tabs;
        uint32_t _index[2];

        ui::Layout* _layout;
        std::vector< std::string > _tab_names;
        std::vector< Ref< ui::Panel > > _panels;
        std::vector< Ref< ui::DockNode > > _children;
    };

}
#endif /* ui_docking_manager_hpp */
