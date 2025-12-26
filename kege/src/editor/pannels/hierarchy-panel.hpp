//
//  hierarchy-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef hierarchy_panel_hpp
#define hierarchy_panel_hpp

#include "ui-panel.hpp"
#include "../elements/ui-text-field.hpp"

namespace kege{

    struct SetSelectedEntity
    {
        ecs::Entity entity;
    };

    class HierarchyPanel : public kege::ui::Panel
    {
    public:

        struct EntityUI
        {
            ui::Style spacer_style;

            UID expand_toggle;
            UID delete_button;
            UID container;

            bool open[2] = {false, false};
        };

    public:

        void update();
        ecs::Entity getSelectedEntity();
        HierarchyPanel( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* ecs );

    public:

        EntityUI* makeEntityUI( ecs::Entity& entity, int space );
        bool expand( EntityUI* list );

    public:

        enum{PANEL, CONTINER, ENTITY, ENTITY_BUTON, ENTITY_CONTENT, COUNT};
        std::map< uint64_t, EntityUI > _hierarchy;

        ref::Scene _scene;

        ecs::Entity _selected_entity;
        UID _create_entity;

        kege::ui::Style _styles[COUNT];
    };
}

#endif /* hierarchy_panel_hpp */
