//
//  hierarchy-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "../../editor-layer.hpp"
#include "../../dock/ui-dock-manager.hpp"
#include "hierarchy-panel.hpp"

#include <unordered_map>
#include <set>
#include <functional>
#include <chrono>

namespace kege::ui{

    void HierarchyPanel::updateLayout()
    {
        kege::ProjectManager* project_manager = _manager->getEditor()->getProjectManager();
        if (!project_manager->empty())
        {
            ref::Scene scene = project_manager->getSceneManager()->getScene();
            if (scene != nullptr)
            {
                if (_scene != scene)
                {
                    _scene = scene;
                }
            }
        }

        _ui->beginScrollContainer( _container );
        {
            handleDeletion();
            handleCreation();

            updateToolbar();
            updateTreeView(_scene->root());
            updateDragAndDrop();
            //updateStatistics();
            updateSelection();
        }
        _ui->endScrollContainer();
    }

    void HierarchyPanel::updateToolbar()
    {
        ui::CornerCurves corner_curves = {4,4,4,4};
        uint32_t hover_color = 0xFFFFFF10;
        uint32_t bg_color = 0xFFFFFF08;

        _ui->beginRow(0);
        _ui->push({.style = &_ui->theme().hierarchy_main_bar});

        _ui->put
        ({
            .color = _ui->mouseover(_create_entity_uid) ? hover_color : bg_color,
            .user_id = _create_entity_uid,
            .rect = {.width = 60.f, .height = 20.f},
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .text = _text_create_entity,
            .border.corner_curves = corner_curves,
        });
        if ( _ui->click(_create_entity_uid) )
        {
            createEntity();
        }

        _ui->put({.style = &_ui->theme().x_seperator});

        _ui->put
        ({
            .color = _ui->mouseover(_delete_uid) ? hover_color : bg_color,
            .user_id = _delete_uid,
            .rect = {.width = 66.f, .height = 20.f},
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = corner_curves,
            .text = _text_delete,
        });
        if ( _ui->click(_delete_uid) )
        {
            deleteSelectedEntities();
        }

        _ui->put({.style = &_ui->theme().x_seperator});

        _ui->put
        ({
            .color = _ui->mouseover(_duplicate_uid) ? hover_color : bg_color,
            .user_id = _duplicate_uid,
            .rect = {.width = 80.f, .height = 20.f},
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = corner_curves,
            .text = _text_duplicate,
        });
        if ( _ui->click(_duplicate_uid) )
        {
            if (_selected_id)
            {
                //duplicateEntity(_selected_id);
            }
        }

        _ui->pop();
        _ui->endRow();
    }

    void HierarchyPanel::buildUI(EntityNode* node, int depth)
    {
        uint32_t small_butn_colr = 0xFFFFFF03;
        uint32_t small_butn_hot_colr = 0xFFFFFF10;

        bool highlight = node->selected || _ui->mouseover(node->uid[0]);

        _ui->push
        ({
            .user_id = node->uid[0],
            .style = highlight ? &_ui->theme().hierarchy_entity_selected : &_ui->theme().hierarchy_entity,
            .single_click = ui::ClickTrigger::Immediate,
            .double_click = ui::ClickTrigger::Immediate,
        });

        _ui->put
        ({
            .user_id = node->uid[1],
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .rect = {.width = 15,.height = 15},
            .color = _ui->mouseover(node->uid[1]) ? small_butn_hot_colr : small_butn_colr,
            .text = ui::Text
            {
                .ptr = node->expand ? ">" : "v",
                .width = 6.f,
                .height = 20.f,
                .color = 0xFFFFFFFF,
                .font_size = 20,
                .align = ui::AlignText::Center
            },
        });
        if ( _ui->click(node->uid[1]) )
        {
            node->expand = !node->expand;
        }

        _ui->label(node->text_name);

        _ui->put
        ({
            .style = &_ui->theme().x_seperator
        });

        _ui->put
        ({
            .user_id = node->uid[2],
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .rect = {.width = 15,.height = 15},
            .color = _ui->mouseover(node->uid[2]) ? small_butn_hot_colr : small_butn_colr,
            .text = ui::Text
            {
                .ptr = node->visible ? "o" : "-",
                .width = node->visible ? 6.f : 5.f,
                .height = 18.f,
                .color = 0xFFFFFFFF,
                .font_size = 20,
                .align = ui::AlignText::Center
            },
        });
        if ( _ui->click(node->uid[2]) )
        {
            node->visible = !node->visible;
        }

        _ui->put
        ({
            .user_id = node->uid[3],
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .rect = {.width = 15,.height = 15},
            .color = _ui->mouseover(node->uid[3]) ? small_butn_hot_colr : small_butn_colr,
            .text = ui::Text
            {
                .ptr = "x",
                .width = 5,
                .height = 18.f,
                .color = 0xFFFFFFFF,
                .font_size = 20,
                .align = ui::AlignText::Center
            },
        });
        if ( _ui->click(node->uid[3]) )
        {
            _deletables.push_back( node->entity_id );
            node->visible = !node->visible;
        }

        _ui->pop();

        if ( _ui->click(node->uid[0]) )
        {
            _selected_id = node->entity_id;
            Communication::broadcast< const SetSelectedEntity& >({ node->entity });
            clearSelection();
            _selected_nodes.push_back( node->entity_id );
        }
    }

    void HierarchyPanel::updateTreeView( ecs::Entity entity, int depth )
    {
        kege::ECS* ecs = _manager->getEditor()->getECS();
        EntityNode* node = getEntityNode( entity );
        buildUI(node, depth);
        for (ecs::Entity child = ecs->begin(entity); child != 0; child = ecs->next(child))
        {
            updateTreeView(child, depth + 1);
        }
    }

    void HierarchyPanel::updateTreeView( ecs::Entity entity )
    {
        kege::ECS* ecs = _manager->getEditor()->getECS();
        _ui->push({.style = &_ui->theme().column});
        for (ecs::Entity child = ecs->begin(entity); child != 0; child = ecs->next(child))
        {
            updateTreeView(child, 0);
        }
        _ui->pop();
    }

    void HierarchyPanel::updateDragAndDrop()
    {

    }

    void HierarchyPanel::deleteEntity(uint64_t entity_id)
    {
        _deletables.push_back( entity_id );
    }

    void HierarchyPanel::deleteSelectedEntities()
    {
        for (int i=0; i<_selected_nodes.size(); ++i)
        {
            _deletables.push_back( _selected_nodes[i] );
        }
    }

    HierarchyPanel::EntityNode* HierarchyPanel::createNode(uint64_t entity_id, ecs::Entity entity, EntityNode* parent)
    {
        EntityNode* node = new EntityNode;
        node->entity_id = entity_id;
        node->entity = entity;
        node->visible = true;
        node->expand = true;
        node->str_name = "Entity_" + std::to_string(entity_id);
        node->text_name = _ui->layout()->text(node->str_name.c_str(), 20);

        _entity_map[ entity_id ] = node;

        clearSelection();
        _selected_id = entity_id;
        _selected_nodes.push_back( node->entity_id );
        return node;
    }

    ecs::Entity HierarchyPanel::createEntity(const std::string& name, EntityNode* parent)
    {
        if (!_scene) return ecs::Entity{0};

        // Create entity in ECS
        ecs::Entity entity = _manager->getEditor()->getECS()->create();

        // Add to scene
        _scene->insert(entity);

        // Create node
        //EntityNode* parent_node = parent ? parent : _root_node;
        uint64_t entity_id = ecs::to_uint64(entity);
        EntityNode* node = createNode(entity_id, entity);
        node->str_name = name;
        node->text_name = _ui->layout()->text(node->str_name.c_str(), 20);

        // Select the new entity
        Communication::broadcast< const SetSelectedEntity& >({ entity });
        //_selected_nodes.push_back(entity_id);

        return entity;
    }

    void HierarchyPanel::createEntity()
    {
        _creation_count += 1;
    }

    HierarchyPanel::EntityNode* HierarchyPanel::getEntityNode(ecs::Entity entity)
    {
        EntityNode* node;
        uint64_t entity_id = ecs::to_uint64(entity);
        auto itr = _entity_map.find( entity_id );
        if ( itr == _entity_map.end())
        {
            node = createNode( entity_id, entity );
        }
        else
        {
            node = itr->second.ref();
        }
        return node;
    }

    void HierarchyPanel::updateSelection()
    {
        for (uint64_t entity_id : _selected_nodes)
        {
            EntityNode* node = _entity_map[ entity_id ].ref();
            if ( node )
            {
                node->selected = true;
                //_ui->get( node->uid )->color = 0x063F47FF;
            }
        }

        auto itr = _entity_map.find( _selected_id );
        if (itr != _entity_map.end())
        {
            itr->second->selected = true;
            //_ui->get( itr->second->uid )->color = 0x063F47FF;
        }
    }

    void HierarchyPanel::handleDeletion()
    {
        for (uint64_t entity_id : _deletables)
        {
            auto itr = _entity_map.find( entity_id );
            if (itr != _entity_map.end())
            {
                _scene->remove(itr->second->entity);
                _manager->getEditor()->getECS()->destroy(itr->second->entity);
                itr->second.clear();
                _entity_map.erase( itr );

                if (_selected_id == entity_id )
                {
                    _selected_id = 0;
                }
            }
        }
    }

    void HierarchyPanel::handleCreation()
    {
        while (0 < _creation_count)
        {
            createEntity("Entity", nullptr);
            _creation_count -= 1;
        }
    }

    ecs::Entity HierarchyPanel::getSelectedEntity()
    {
        return {};
    }

    void HierarchyPanel::clearSelection()
    {
        for (uint64_t entity_id : _selected_nodes)
        {
            EntityNode* node = _entity_map[ entity_id ].ref();
            if ( node )
            {
                node->selected = false;
            }
        }
        _selected_nodes.clear();
    }

    HierarchyPanel::HierarchyPanel( kege::ui::DockManager* dm )
    :   kege::ui::Panel( "Hierarchy", dm )
    ,   _creation_count(0)
    {
        text_create = _ui->layout()->text("create", 20);
        _text_create_entity = _ui->layout()->text("Create", 20);
        _text_create_entity.align = ui::AlignText::Center;
        _text_create_entity.color = 0xFFFFFF6F;

        _text_create_folder = _ui->layout()->text("+ Folder", 20);

        _text_delete = _ui->layout()->text("Delete", 20);
        _text_delete.align = ui::AlignText::Center;
        _text_delete.color = 0xFFFFFF6F;

        _text_duplicate = _ui->layout()->text("Duplicate", 20);
        _text_duplicate.align = ui::AlignText::Center;
        _text_duplicate.color = 0xFFFFFF6F;

        _text_expand_all = _ui->layout()->text("Expand All", 20);
        _text_collapse_all = _ui->layout()->text("Collapse All", 20);
        _text_focus = _ui->layout()->text("Focus", 20);


        _container[0]       = _uid_root[0];
        _container[1]       = _uid_root[1];
        _create_entity_uid  = _uid_root[2];
        _create_folder_uid  = _uid_root[3];
        _delete_uid         = _uid_root[4];
        _duplicate_uid      = _uid_root[5];
        _expand_all_uid     = _uid_root[6];
        _collapse_all_uid   = _uid_root[7];
        _filter_field_uid   = _uid_root[8];
        _tree_container_uid = _uid_root[9];
        _focus_button_uid   = _uid_root[10];
    }

}
