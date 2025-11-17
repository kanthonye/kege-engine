//
//  hierarchy-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef hierarchy_panel_hpp
#define hierarchy_panel_hpp

#include "../elements/ui-panel.hpp"
#include "../elements/ui-text-field.hpp"

namespace kege::ui{

    struct HierarchyDroplist
    {
        ui::Style spacer_style;

        //ui::Elem text_field;
        //ui::Elem text_field;

        ui::TextField text_field;

        ui::Elem expand_toggle;
        ui::Elem delete_button;
        ui::Elem container;
        ui::Elem content;
        ui::Elem field;
        ui::Elem spacer;

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

        ui::HierarchyDroplist* makeEntityUI( Entity& entity, int space );
        void buildHierarchy( Entity& root, int spacer = 0 );
        bool clicked( ui::HierarchyDroplist* list );

    public:

        std::map< uint32_t, ui::HierarchyDroplist > _hierarchy;

        ref::Scene _scene;

        kege::Entity _selected_entity;
        ui::Elem _create_entity;
        ui::Elem _panel;
        bool _butn_down;
    };
}

#endif /* hierarchy_panel_hpp */
