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
    :   kege::EditorPanel( pm, l )
    {
        Communication::add< const SetSelectedEntity&, InspectorPanel >( this );
        _main = _layout->make({ .style = _layout->getStyleByName( "Inspector" ), });

        _ui_element_creator[ ComponentCacheT< kege::Camera >::getType() ] = ui::camera;
        _ui_element_creator[ ComponentCacheT< kege::Rigidbody >::getType() ] = ui::rigidbody;
        _ui_element_creator[ ComponentCacheT< kege::Transform >::getType() ] = ui::transform;
    }

    void InspectorPanel::operator()( const SetSelectedEntity& msg )
    {
        _selected_entity = msg.entity;
    }

    void InspectorPanel::update()
    {
        _layout->push( _main );
        if ( _selected_entity )
        {
            const EntityComponentMap& ecm = _selected_entity.getEntityComponents();
            for (EntityComponentMap::const_iterator i = ecm.cbegin(); i != ecm.cend(); i++)
            {
                auto m = _ui_element_creator.find( i->first );
                if ( m == _ui_element_creator.end() )
                {
                    m->second( *_layout, _tree, _selected_entity );
                }
            }
        }
        _layout->pop();
    }

}
