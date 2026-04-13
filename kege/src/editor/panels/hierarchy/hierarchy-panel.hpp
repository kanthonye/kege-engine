//
//  hierarchy-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef hierarchy_panel_hpp
#define hierarchy_panel_hpp

#include "../../common/ui-description.hpp"

namespace kege::ui{
    
    struct SetSelectedEntity
    {
        ecs::Entity entity;
    };

    struct SetSelectedEntities
    {
        std::vector<ecs::Entity> entities;
    };

    struct EntityRenamed
    {
        ecs::Entity entity;
        std::string new_name;
    };

    struct EntityReparented
    {
        ecs::Entity entity;
        ecs::Entity new_parent;
        ecs::Entity old_parent;
    };



    class HierarchyPanel : public kege::ui::Panel
    {
    public:

        struct EntityNode : kege::RefCounter
        {
            ecs::Entity entity;
            uint64_t entity_id;

            ui::UID uid;

            bool expand;
            bool visible;
            bool selected;

            ui::Text text_name;
            std::string str_name;
        };

    public:

        void updateLayout();
        ecs::Entity getSelectedEntity();
        HierarchyPanel( kege::ui::DockManager* dm );

    public:

        void buildUI(EntityNode* node, int depth);

        EntityNode* createNode(uint64_t entity_id, ecs::Entity entity, EntityNode* parent = nullptr);
        ecs::Entity createEntity(const std::string& name, EntityNode* parent);
        EntityNode* getEntityNode(ecs::Entity entity);
        void deleteNode(EntityNode* node);
        void deleteSelectedEntities();

        void deleteEntity(uint64_t entity_id);
        void createEntity();
//        EntityNode* findNode(ecs::Entity entity) const;
//        EntityNode* findNodeByUID(ui::WidgetHandle uid) const;
//
//        // UI rendering
        void updateToolbar();
        void updateTreeView( ecs::Entity entity, int depth );
        void updateTreeView( ecs::Entity entity );
        void updateDragAndDrop();
        void updateSelection();
        void clearSelection();
        void handleDeletion();
        void handleCreation();
//        void updateEntityNode(EntityNode* node);
//        void updateContextMenu();
//        void updateStatistics();
        void rebuildTree();
        
        EntityNode* makeEntityUI( ecs::Entity& entity, int space );

    public:

        std::map< uint64_t, Ref<EntityNode> > _entity_map;
        std::vector<uint64_t> _deletables;

        uint32_t _creation_count;
        std::vector<size_t> _selected_nodes;

        ref::Scene _scene;

        uint64_t _selected_id;
        ui::WidgetHandle _uid_create_entity;
        ui::Text text_create;

        ui::UID _uid_root;

        ui::ID _container[2];

        ui::ID _create_entity_uid;
        ui::ID _create_folder_uid;
        ui::ID _delete_uid;
        ui::ID _duplicate_uid;
        ui::ID _expand_all_uid;
        ui::ID _collapse_all_uid;
        ui::ID _filter_field_uid;
        ui::ID _tree_container_uid;
        ui::ID _focus_button_uid;

        ui::Text _text_create_entity;
        ui::Text _text_create_folder;
        ui::Text _text_delete;
        ui::Text _text_duplicate;
        ui::Text _text_expand_all;
        ui::Text _text_collapse_all;
        ui::Text _text_focus;

        ///char _name_bufr[128];
    };

}

#endif /* hierarchy_panel_hpp */
