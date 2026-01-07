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

            ui::UID expand_toggle;
            ui::UID delete_button;
            ui::UID container;

            bool open[2] = {false, false};

            ui::Text text_delete;
            ui::Text text_expand;
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
        ui::UID _create_entity;
        ui::Text text_create;
        kege::ui::Style _styles[COUNT];
    };
}

#endif /* hierarchy_panel_hpp */
