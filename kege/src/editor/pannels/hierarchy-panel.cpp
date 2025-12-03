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
        _layout->push( _panel );
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

                _layout->put( _create_entity );

                const ecs::Entity& root = scene->root();
                for (ecs::Entity e = _ecs->begin(root); e != 0 ; e = _ecs->next(e) )
                {
                    buildHierarchy( e, 0 );
                }
            }
        }
        _layout->pop();

        if ( _scene && _layout->click( _create_entity ) )
        {
            ecs::Entity entity = _ecs->createWith<Tag>();
            Tag* tag = _ecs->get< Tag >( entity );
            *tag = "entity";
            _scene->insert( entity );
            _butn_down = true;
        }
    }

    ui::HierarchyDroplist* HierarchyPanel::makeEntityUI( ecs::Entity& entity, int space )
    {
        const bool entity_has_children = _ecs->isParent(entity);

        uint64_t key = ecs::to_uint64(entity);
        ui::HierarchyDroplist* list = 0;
        auto i = _hierarchy.find( key );
        if ( i == _hierarchy.end() )
        {
            const kege::Tag* tag = _ecs->get< kege::Tag >(entity);
            const char* entity_name = ( tag ) ? tag->c_str() : "un-named";

            list = &_hierarchy[ key ];

            list->text_field.init( _layout, entity_name );

            list->spacer_style = ui::Style
            {
                .width = ui::fixed( space ),
                .height = ui::extend(),
            };

            list->spacer = _layout->make
            ({
                .visible = false,
                .style = &list->spacer_style,
            });

            list->container = _layout->make
            ({
                .style = _layout->getStyleByName( "droplist" ),
            });

            list->field = _layout->make
            ({
                .style = _layout->getStyleByName( "droplist-field" ),
                .single_click = ui::ClickTrigger::Immediate,
            });

            list->expand_toggle = _layout->make
            ({
                .style = _layout->getStyleByName( "droplist-icon" ),
                .single_click = ui::ClickTrigger::OnRelease,
                .text = {"-",0,0,0,0},
            });

            list->delete_button = _layout->make
            ({
                .style = _layout->getStyleByName( "droplist-label" ),
                .single_click = ui::ClickTrigger::OnRelease,
                .text = {"x", 0, 0, 0, 0},
            });

            list->content = _layout->make
            ({
                .style = _layout->getStyleByName( "droplist-content" ),
                .mouseover = false,
            });
        }
        else
        {
            list = &i->second;
            //list->label->text = {entity_name, 0, 0, 0, 0};
        }

        if(_scene && _layout->click( list->delete_button ))
        {
            _hierarchy.erase( _hierarchy.find( key ) );
            _scene->remove( entity );
            return nullptr;
        }

        list->expand_toggle->text.text = ( entity_has_children )
        ? (( list->open[1] ) ? "-" : "+") : "-";

        // droplist hierarchy
        _layout->push( list->container );
        {
            _layout->put( list->spacer );
            _layout->push( list->field );
            {
                _layout->put( list->expand_toggle );
                if ( list->text_field.update( _layout ) )
                {
                    Communication::broadcast< const SetSelectedEntity& >({ entity });
                }
                _layout->put( list->delete_button );
            }
            _layout->pop();
        }
        _layout->pop();

        return list;
    }

    void HierarchyPanel::buildHierarchy( ecs::Entity& entity, int spacer )
    {
        ui::HierarchyDroplist* list = makeEntityUI( entity, spacer );
        if ( list == nullptr ) return;
        
        if ( clicked( list ) )
        {
            _layout->push( list->content );
            for (ecs::Entity e = _ecs->begin(entity); e != 0 ; e = _ecs->next(e) )
            {
                buildHierarchy( e, spacer + 15 );
            }
            _layout->end();
        }

        if ( list->text_field.modified )
        {
            list->text_field.modified = false;
            Tag* tag = _ecs->get< Tag >(entity);
            *tag = list->text_field.text->text.text.c_str();
        }
    }

    bool HierarchyPanel::clicked( ui::HierarchyDroplist* list )
    {
        if ( _layout->click( list->expand_toggle ) )
        {
            if ( !list->open[0] )
            {
                list->open[0] = true;
                list->open[1] = !list->open[1];
            }
        }
        else if( list->open[0] && _layout->input()->buttonDown() )
        {
            list->open[0] = false;
        }
        return list->open[1];
    }

    ecs::Entity HierarchyPanel::getSelectedEntity()
    {
        return _selected_entity;
    }

    HierarchyPanel::HierarchyPanel( kege::ProjectManager* pm, ui::Layout* l, kege::ECS* e )
    :   kege::ui::Panel( "Hierarchy", pm, l, e )
    {
        _panel = _layout->make
        ({
            .style = _layout->getStyleByName( "panel" )
        });
        _create_entity = _layout->make
        ({
            .single_click = ui::ClickTrigger::OnRelease,
            .style = _layout->getStyleByName( "button" ),
            .text = {"Create Entity", 0, 0, 0, 0},
        });
    }

}
