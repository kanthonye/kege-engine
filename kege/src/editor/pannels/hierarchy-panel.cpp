//
//  hierarchy-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "hierarchy-panel.hpp"
namespace kege{

    void HierarchyPanel::update()
    {
        _gui->push({ .layer = 0, .style = &_styles[PANEL]});
        if ( !_project_manager->empty() )
        {
            ref::Scene scene = _project_manager->getSceneManager()->getScene();
            if( scene != nullptr )
            {
                if( _scene != scene )
                {
                    _scene = scene;
                    _hierarchy.clear();
                }

                if ( _gui->button(0, _create_entity, text_create) )
                {
                    ecs::Entity entity = _ecs->createWith<Tag>();
                    {
                        Tag* tag = _ecs->get< Tag >( entity );
                        *tag = "entity";
                    }
                    _scene->insert( entity );
                }

                const ecs::Entity& root = scene->root();
                _gui->push({.layer = 0, .style = &_styles[CONTINER]});
                for (ecs::Entity e = _ecs->begin(root); e != 0 ; e = _ecs->next(e) )
                {
                    makeEntityUI( e, 0 );
                }
                _gui->pop(0);
            }
        }
        _gui->pop(0);
    }

    HierarchyPanel::EntityUI* HierarchyPanel::makeEntityUI( ecs::Entity& entity, int space )
    {
        const bool entity_has_children = _ecs->isParent(entity);
        uint64_t key = ecs::to_uint64(entity);

        HierarchyPanel::EntityUI* list = 0;
        auto m = _hierarchy.find( key );
        if ( m == _hierarchy.end() )
        {
            list = &_hierarchy[ key ];
            list->text_delete = _gui->layout()->text("x", 20);
            list->text_expand = _gui->layout()->text(">", 20);
        }
        else list = &m->second;
        //list->text_expand.ptr = ( entity_has_children ) ? (( list->open[1] ) ? "-_+" : "+") : "-";
        //HierarchyPanel::EntityUI* list = &_hierarchy[ key ];
        list->spacer_style.width.size = space;

        if(_scene && _gui->click( list->delete_button ))
        {
            _hierarchy.erase( _hierarchy.find( key ) );
            _scene->remove( entity );
            return nullptr;
        }

        Tag* entt_tag = _ecs->get< Tag >(entity);
        _gui->push({.layer = 0, .uid = &list->container, .style = &_styles[ENTITY], .single_click = ui::ClickTrigger::OnRelease });
        {
            _gui->button( 0, list->expand_toggle, &_styles[ENTITY_BUTON], list->text_expand );
            _gui->label( 0, (entt_tag)? entt_tag->c_str(): "unnamed" );
            _gui->button( 0, list->delete_button, &_styles[ENTITY_BUTON], list->text_delete );
        }
        _gui->pop(0);

        if ( _gui->click(list->container) )
        {
            Communication::broadcast< const SetSelectedEntity& >({ entity });
        }

        if ( expand( list ) && entity_has_children )
        {
            _gui->push({.layer = 0, .style = &_styles[ENTITY_CONTENT]});
            for (ecs::Entity e = _ecs->begin(entity); e != 0 ; e = _ecs->next(e) )
            {
                makeEntityUI( e, space + 15 );
            }
            _gui->pop(0);
        }
        return list;
    }

    bool HierarchyPanel::expand( HierarchyPanel::EntityUI* list )
    {
        if ( _gui->click( list->expand_toggle ) )
        {
            if ( !list->open[0] )
            {
                list->open[0] = true;
                list->open[1] = !list->open[1];
            }
        }
        else if( list->open[0] && _gui->buttonDown() )
        {
            list->open[0] = false;
        }
        return list->open[1];
    }

    ecs::Entity HierarchyPanel::getSelectedEntity()
    {
        return _selected_entity;
    }

    HierarchyPanel::HierarchyPanel( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* e )
    :   kege::ui::Panel( "Hierarchy", pm, gui, e )
    {
        text_create = gui->layout()->text("create", 20);

        _styles[PANEL] = kege::ui::Style
        {
            .background = ui::Background(0x171420FF),
            .height = ui::extend(),
            .width = ui::extend(),
            .padding = {10,10,10,10},
            .gap = {2,2},
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };

        _styles[CONTINER] = kege::ui::Style
        {
            .background = ui::Background(0x17142000),
            .height = ui::flexible(),
            .width = ui::extend(),
            .padding = {4,4,4,4},
            .gap = {2,2},
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };

        _styles[ENTITY] = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF10),
            .height = ui::flexible(),
            .width = ui::extend(),
            .border.corner_curves = {4,4,4,4},
            .padding = {4,4,4,4},
            .gap = {2,2},
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        _styles[ENTITY_BUTON] = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF00),
            .height = ui::fixed(18),
            .width = ui::fixed(18),
            .align_text = ui::AlignText::Center,
            .border.corner_curves = {2,2,2,2},
        };

        _styles[ENTITY_CONTENT] = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF0F),
            .height = ui::flexible(),
            .width = ui::extend(),
            //.padding = {10,10,10,10},
            .gap = {2,2},
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };
    }

}
