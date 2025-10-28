//
//  inspector-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef inspector_panel_hpp
#define inspector_panel_hpp

#include "../elements/droplist.hpp"
#include "../elements/ui-numeric3.hpp"
#include "hierarchy-panel.hpp"
#include "editor-panel.hpp"

namespace kege{

    class InspectorPanel : public kege::EditorPanel
    {
    public:

        InspectorPanel( kege::ProjectManager* pm, ui::Layout* l );
        void operator()( const SetSelectedEntity& msg );
        void update();

    public:

        ui::UIElementCreator _ui_element_creator;
        ui::Tree _tree;
        ui::EID _main;


        Entity _selected_entity;
    };
}

#endif /* inspector_panel_hpp */
