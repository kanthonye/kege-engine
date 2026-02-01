//
//  hierarchy-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "hierarchy-panel.hpp"

#include <unordered_map>
#include <set>
#include <functional>
#include <chrono>

namespace kege::ui{

    void HierarchyPanel::update()
    {
        if (!_project_manager->empty())
        {
            ref::Scene scene = _project_manager->getSceneManager()->getScene();
            if (scene != nullptr)
            {
                if (_scene != scene)
                {
                    _scene = scene;
                }
            }
        }

        const int layer = 0;
        _gui->push({ .layer = layer, .style = &_gui->_theme.panel});
        _gui->beginScrollContainer(layer, _gui->getAddressAsInt(this));
        handleDeletion();
        handleCreation();

        updateToolbar();
        updateTreeView(_scene->root());
        updateDragAndDrop();
        //updateStatistics();
        updateSelection();
        _gui->endScrollContainer();
        _gui->pop();
    }

    void HierarchyPanel::updateToolbar()
    {
        ui::CornerCurves corner_curves = {4,4,4,4};
        uint32_t hover_color = 0xFFFFFF10;
        uint32_t bg_color = 0xFFFFFF08;

        _gui->beginRow(0);
        _gui->push({.layer = 0, .style = &_gui->_theme.hierarchy_main_bar});

        _gui->put
        ({
            .layer = 0,
            .color = _gui->mouseover(_create_entity_uid) ? hover_color : bg_color,
            .uid = &_create_entity_uid,
            .rect = {.width = 60.f, .height = 20.f},
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .text = _text_create_entity,
            .border.corner_curves = corner_curves,
        });
        if ( _gui->click(_create_entity_uid) )
        {
            createEntity();
        }

        _gui->put({.layer = 0, .style = &_gui->_theme.x_seperator});

        _gui->put
        ({
            .layer = 0,
            .color = _gui->mouseover(_delete_uid) ? hover_color : bg_color,
            .uid = &_delete_uid,
            .rect = {.width = 66.f, .height = 20.f},
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = corner_curves,
            .text = _text_delete,
        });
        if ( _gui->click(_delete_uid) )
        {
            deleteSelectedEntities();
        }

        _gui->put({.layer = 0, .style = &_gui->_theme.x_seperator});

        _gui->put
        ({
            .layer = 0,
            .color = _gui->mouseover(_duplicate_uid) ? hover_color : bg_color,
            .uid = &_duplicate_uid,
            .rect = {.width = 80.f, .height = 20.f},
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = corner_curves,
            .text = _text_duplicate,
        });
        if ( _gui->click(_duplicate_uid) )
        {
            if (_selected_id)
            {
                //duplicateEntity(_selected_id);
            }
        }



//        if (_gui->button(0, _create_entity_uid, _text_create_entity, &_gui->_theme.button)) {
//            createEntity("Entity",nullptr);
//        }
//        /*
//        // Create folder button
//        if (_gui->button(0, _create_folder_uid, _text_create_folder, &_gui->_theme.button)) {
//            createFolder();
//        }
//         */
        // Delete button
//        if (_gui->button(0, _delete_uid, _text_delete, &_gui->_theme.button)) {
//            deleteSelectedEntities();
//        }

        // Duplicate button
//        if (_gui->button(0, _duplicate_uid, _text_duplicate, &_gui->_theme.button))
//        {
//            if (_primary_selected)
//            {
//                duplicateEntity(_primary_selected);
//            }
//        }
//        /*
//        // Expand all button
//        if (_gui->button(0, _expand_all_uid, _text_expand_all, &_gui->_theme.button))
//        {
//            expandAll();
//        }
//
//        // Collapse all button
//        if (_gui->button(0, _collapse_all_uid, _text_collapse_all, &_gui->_theme.button))
//        {
//            collapseAll();
//        }
//
//        // Focus button
//        if (_gui->button(0, _focus_button_uid, _text_focus, &_gui->_theme.button))
//        {
//            focusSelectedEntity();
//        }
//         */
//
//        _gui->endRow(0);
//
//        // Search/filter field
//        _gui->beginRow(0);
//
//        char filter_buffer[256] = "";
//        strncpy(filter_buffer, _filter_text.c_str(), sizeof(filter_buffer));
//        size_t filter_size = _filter_text.size() + 1;
//
//        if (_gui->textField(0, _filter_field_uid.global.id, filter_buffer, filter_size))
//        {
//            _filter_text = filter_buffer;
//            applyFilter(_filter_text);
//        }

        _gui->pop();
        _gui->endRow();
    }

    void HierarchyPanel::buildUI(EntityNode* node, int depth)
    {
        uint32_t small_butn_colr = 0xFFFFFF03;
        uint32_t small_butn_hot_colr = 0xFFFFFF10;

        bool highlight = node->selected || _gui->mouseover(node->uid);

        _gui->push
        ({
            .layer = 0,
            .uid = &node->uid,
            .style = highlight ? &_gui->_theme.hierarchy_entity_selected : &_gui->_theme.hierarchy_entity,
            .single_click = ui::ClickTrigger::Immediate,
            .double_click = ui::ClickTrigger::Immediate,
        });

        _gui->put
        ({
            .layer = 0,
            .uid = &node->uid_expand,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .rect = {.width = 15,.height = 15},
            .color = _gui->mouseover(node->uid_expand) ? small_butn_hot_colr : small_butn_colr,
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
        if ( _gui->click(node->uid_expand) )
        {
            node->expand = !node->expand;
        }

        _gui->label(0, node->text_name);

        _gui->put
        ({
            .layer = 0,
            .style = &_gui->_theme.x_seperator
        });

        _gui->put
        ({
            .layer = 0,
            .uid = &node->uid_visible,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .rect = {.width = 15,.height = 15},
            .color = _gui->mouseover(node->uid_visible) ? small_butn_hot_colr : small_butn_colr,
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
        if ( _gui->click(node->uid_visible) )
        {
            node->visible = !node->visible;
        }

        _gui->put
        ({
            .layer = 0,
            .uid = &node->uid_delete,
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .rect = {.width = 15,.height = 15},
            .color = _gui->mouseover(node->uid_delete) ? small_butn_hot_colr : small_butn_colr,
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
        if ( _gui->click(node->uid_delete) )
        {
            _deletables.push_back( node->entity_id );
            node->visible = !node->visible;
        }

        _gui->pop();

        if ( _gui->click(node->uid) )
        {
            _selected_id = node->entity_id;
            Communication::broadcast< const SetSelectedEntity& >({ node->entity });
            clearSelection();
            _selected_nodes.push_back( node->entity_id );
        }
    }

    void HierarchyPanel::updateTreeView( ecs::Entity entity, int depth )
    {
        EntityNode* node = getEntityNode( entity );
        buildUI(node, depth);
        for (ecs::Entity child = _ecs->begin(entity); child != 0; child = _ecs->next(child))
        {
            updateTreeView(child, depth + 1);
        }
    }

    void HierarchyPanel::updateTreeView( ecs::Entity entity )
    {
        _gui->push({.layer = 0, .style = &_gui->_theme.column});
        for (ecs::Entity child = _ecs->begin(entity); child != 0; child = _ecs->next(child))
        {
            updateTreeView(child, 0);
        }
        _gui->pop();
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
        node->text_name = _gui->layout()->text(node->str_name.c_str(), 20);

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
        ecs::Entity entity = _ecs->create();

        // Add to scene
        _scene->insert(entity);

        // Create node
        //EntityNode* parent_node = parent ? parent : _root_node;
        uint64_t entity_id = ecs::to_uint64(entity);
        EntityNode* node = createNode(entity_id, entity);
        node->str_name = name;
        node->text_name = _gui->layout()->text(node->str_name.c_str(), 20);

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
                //_gui->get( node->uid )->color = 0x063F47FF;
            }
        }

        auto itr = _entity_map.find( _selected_id );
        if (itr != _entity_map.end())
        {
            itr->second->selected = true;
            //_gui->get( itr->second->uid )->color = 0x063F47FF;
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
                _ecs->destroy(itr->second->entity);
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

    HierarchyPanel::HierarchyPanel( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* e )
    :   kege::ui::Panel( "Hierarchy", pm, gui, e )
    ,   _creation_count(0)
    {
        text_create = gui->layout()->text("create", 20);
        _text_create_entity = gui->layout()->text("Create", 20);
        _text_create_entity.align = ui::AlignText::Center;
        _text_create_entity.color = 0xFFFFFF6F;

        _text_create_folder = gui->layout()->text("+ Folder", 20);

        _text_delete = gui->layout()->text("Delete", 20);
        _text_delete.align = ui::AlignText::Center;
        _text_delete.color = 0xFFFFFF6F;

        _text_duplicate = gui->layout()->text("Duplicate", 20);
        _text_duplicate.align = ui::AlignText::Center;
        _text_duplicate.color = 0xFFFFFF6F;

        _text_expand_all = gui->layout()->text("Expand All", 20);
        _text_collapse_all = gui->layout()->text("Collapse All", 20);
        _text_focus = gui->layout()->text("Focus", 20);
    }

}
