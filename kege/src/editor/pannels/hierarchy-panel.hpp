//
//  hierarchy-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef hierarchy_panel_hpp
#define hierarchy_panel_hpp

#include "../elements/ui-panel.hpp"

namespace kege::ui{

    struct HierarchyDroplist
    {
        ui::Style spacer_style;

        ui::EID container;
        ui::EID content;
        ui::EID field;
        ui::EID label;
        ui::EID icon;
        ui::EID spacer;

        bool open[2] = {false, false};
    };
}

namespace kege{

    struct SetSelectedEntity
    {
        Entity entity;
    };

    class HierarchyPanel : public kege::ui::Panel
    {
    public:

        void update();
        Entity getSelectedEntity();
        HierarchyPanel( kege::ProjectManager* pm, ui::Layout* l );

    public:

        ui::HierarchyDroplist* makeEntityUI( const Entity& entity, int space );
        void buildHierarchy( const Entity& root, int spacer = 0 );
        bool clicked( ui::HierarchyDroplist* list );

    public:

        std::map< uint32_t, ui::HierarchyDroplist > _hierarchy;

        ref::Scene _scene;

        kege::Entity _selected_entity;
        ui::EID _create_entity;
        ui::EID _panel;
    };
}

#endif /* hierarchy_panel_hpp */
