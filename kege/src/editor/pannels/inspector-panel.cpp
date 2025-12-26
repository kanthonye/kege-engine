//
//  inspector-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "ui-camera.hpp"
#include "ui-rigidbody.hpp"
#include "ui-transform.hpp"
#include "inspector-panel.hpp"

namespace kege{

    InspectorPanel::InspectorPanel( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* e )
    :   kege::ui::Panel( "Inspector", pm, gui, e )
    ,   _show_component_selections( false )
    ,   _selected_entity{}
    {
        _styles[0] = kege::ui::Style
        {
            .background = ui::Background(0x171420FF),
            .height = ui::extend(),
            .width = ui::extend(),
            .padding = {10,10,10,10},
            .gap = {2,2},
            .align =
            {
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };
        _styles[1] = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF10),
            .height = ui::flexible(),
            .width = ui::extend(),
            .padding = {10,10,10,10},
            .gap = {2,2},
            .align =
            {
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };

        Communication::add< const SetSelectedEntity&, InspectorPanel >( this );

        _ui_element_creator[ ecs::Component::type< kege::Camera >()    ] = ui::camera;
        _ui_element_creator[ ecs::Component::type< kege::Rigidbody >() ] = ui::rigidbody;
        _ui_element_creator[ ecs::Component::type< kege::Transform >() ] = ui::transform;

        add("Transform", ecs::Component::type< kege::Transform >(), [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Transform >(e); });
        add("Rigidbody", ecs::Component::type< kege::Rigidbody >(), [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Rigidbody >(e); });
        add("Camera", ecs::Component::type< kege::Camera >(), [](kege::ECS* ecs, ecs::Entity& e){
            ecs->add< kege::Camera >(e)->projection = new Perspective(1.0, 45.0, 0.1, 1000.0);
        });
        add("Mesh", ecs::Component::type< kege::ref::Mesh >(), [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::ref::Mesh >(e); });
    }

    void InspectorPanel::add(const std::string& stype, int component_type, void(*funct)( kege::ECS*, ecs::Entity& ))
    {
        _component_factory[ component_type ] = funct;
        _string_to_component_type[ stype ] = component_type;
        std::pair<kege::UID, std::string> pair;
        pair.second = stype;
        _component_uis.push_back(pair);
    }

    void InspectorPanel::operator()( const SetSelectedEntity& msg )
    {
        _selected_entity = msg.entity;
    }

    void InspectorPanel::update()
    {
        _gui->push({ .style = &_styles[0] });
        if ( _selected_entity )
        {
            if( _gui->button(_add_component, "add component") )
            {
                _show_component_selections = !_show_component_selections;
            }

            if ( _show_component_selections )
            {
                int selection;
                if ( _gui->select(&_styles[1], _component_uis, selection) )
                {
                    auto i = _string_to_component_type.find( _component_uis[ selection ].second );
                    if (i != _string_to_component_type.end())
                    {
                        _component_factory[ i->second ]( _ecs, _selected_entity );
                        _show_component_selections = false;
                    }
                }
            }

//            const ecs::Component::Layout& layout = _ecs->getLayout( _selected_entity );
//            for (int i = 0; i < layout.attributes.size(); ++i)
//            {
//                auto m = _ui_element_creator.find( layout.attributes[i].info->type );
//                if ( m != _ui_element_creator.end() )
//                {
//                    //TODO: m->second( _ecs, *_gui, _tree, _selected_entity );
//                }
//            }
        }
        _gui->pop();
    }

}
