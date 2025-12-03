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

    InspectorPanel::InspectorPanel( kege::ProjectManager* pm, ui::Layout* l, kege::ECS* e )
    :   kege::ui::Panel( "Inspector", pm, l, e )
    ,   _show_component_selections( false )
    ,   _selected_entity{}
    {
        Communication::add< const SetSelectedEntity&, InspectorPanel >( this );
        _main = _layout->make({ .style = _layout->getStyleByName( "panel" ), .mouseover = false });

        _add_component = _layout->make
        ({
            .mouseover = true,
            .single_click = ui::ClickTrigger::OnRelease,
            .style = _layout->getStyleByName( "button" ),
            .text = {"Add Component", 0, 0, 0, 0},
        });

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
        _entity_component_factory[ component_type ] = funct;
        _string_to_component_type[ stype ] = component_type;
        _component_string_types.push_back( stype );
    }

    void InspectorPanel::operator()( const SetSelectedEntity& msg )
    {
        _selected_entity = msg.entity;
    }


    int InspectorPanel::select( ui::Layout* layout, const std::vector< std::string >& options )
    {
        if ( _selection_elements.size() != options.size() )
        {
            _selection_elements.resize( options.size() );
            for (int i=0; i < _selection_elements.size(); ++i)
            {
                _selection_elements[i] = layout->make
                ({
                    .mouseover = true,
                    .single_click = ui::ClickTrigger::Immediate,
                    .style = layout->getStyleByName( "button" ),
                    .text = {options[i].c_str(), 0, 0, 0, 0},
                });
            }
            _selection_container = layout->make
            ({
                .mouseover = false,
                .style = layout->getStyleByName( "v-container" )
            });
        }

        layout->push( _selection_container );
        for (int i=0; i < _selection_elements.size(); ++i)
        {
            layout->put( _selection_elements[i] );
        }
        layout->pop();

        // select the clicked tab element
        for (int i=0; i < _selection_elements.size(); ++i)
        {
            if( layout->click( _selection_elements[i] ) )
            {
                return i;
            }
        }
        return -1;
    }

    void InspectorPanel::update()
    {
        _layout->push( _main );
        if ( _selected_entity )
        {
            _layout->put( _add_component );
            if( _layout->click( _add_component ) )
            {
                _show_component_selections = true;
            }

            if ( _show_component_selections && _selected_entity )
            {
                int selection;
                if (( selection = select( _layout, _component_string_types ) ) >= 0)
                {
                    auto i = _string_to_component_type.find( _component_string_types[ selection ] );
                    if (i != _string_to_component_type.end())
                    {
                        _entity_component_factory[ i->second ]( _ecs, _selected_entity );
                        _show_component_selections = false;
                    }
                }
            }

            const ecs::Component::Layout& layout = _ecs->getLayout( _selected_entity );
            for (int i = 0; i < layout.attributes.size(); ++i)
            {
                auto m = _ui_element_creator.find( layout.attributes[i].info->type );
                if ( m != _ui_element_creator.end() )
                {
                    m->second( _ecs, *_layout, _tree, _selected_entity );
                }
            }
        }
        _layout->pop();
    }

}
