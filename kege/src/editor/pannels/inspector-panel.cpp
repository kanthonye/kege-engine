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

    InspectorPanel::InspectorPanel( kege::ProjectManager* pm, ui::Layout* l )
    :   kege::ui::Panel( "Inspector", pm, l )
    ,   _show_component_selections( false )
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

        _ui_element_creator[ ComponentCacheT< kege::Camera >::getType()    ] = ui::camera;
        _ui_element_creator[ ComponentCacheT< kege::Rigidbody >::getType() ] = ui::rigidbody;
        _ui_element_creator[ ComponentCacheT< kege::Transform >::getType() ] = ui::transform;

        add("Transform", ComponentCacheT< kege::Transform >::getType(), [](Entity& e){ e.add< kege::Transform >(); });
        add("Rigidbody", ComponentCacheT< kege::Rigidbody >::getType(), [](Entity& e){ e.add< kege::Rigidbody >(); });
        add("Camera", ComponentCacheT< kege::Camera >::getType(), [](Entity& e){ e.add< kege::Camera >({new Perspective(1.0, 45.0, 0.1, 1000.0)}); });
        add("Mesh", ComponentCacheT< kege::ref::Mesh >::getType(), [](Entity& e){ e.add< kege::ref::Mesh >(); });
    }

    void InspectorPanel::add(const std::string& stype, int component_type, void(*funct)( Entity& ))
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
                        _entity_component_factory[ i->second ]( _selected_entity );
                        _show_component_selections = false;
                    }
                }
            }

            const EntityComponentMap& ecm = _selected_entity.getEntityComponents();
            for (EntityComponentMap::const_iterator i = ecm.cbegin(); i != ecm.cend(); i++)
            {
                auto m = _ui_element_creator.find( i->first );
                if ( m != _ui_element_creator.end() )
                {
                    m->second( *_layout, _tree, _selected_entity );
                }
            }
        }
        _layout->pop();
    }

}
