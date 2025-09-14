//
//  inspector-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "inspector-panel.hpp"
namespace kege{


    void InspectorPanel::put( ui::Layout& layout )
    {
        layout.push( _main );
        if ( _selected_entity )
        {
            Camera* camera = _selected_entity.get< Camera >();
            if ( camera )
            {
                ui::camera( layout, _tree[ camera ], camera );
            }

            Rigidbody* rigidbody = _selected_entity.get< Rigidbody >();
            if ( rigidbody )
            {
                ui::rigidbody( layout, _tree[ rigidbody ], rigidbody );
            }

            Transform* transform = _selected_entity.get< Transform >();
            if ( transform )
            {
                ui::transform( layout, _tree[ transform ], transform );
            }
        }
        layout.pop();
    }

    InspectorPanel& InspectorPanel::init( Engine* engine, ui::Layout& layout )
    {
        _engine = engine;
        _main = layout.make
        ({
            .style = layout.getStyleByName( "inspector" ),
        });
        return *this;
    }

    void InspectorPanel::operator()( const SetSelectedEntity& msg )
    {
        _selected_entity = msg.entity;
    }
    InspectorPanel::InspectorPanel()
    :   _engine( nullptr )
    {
        Communication::add< const SetSelectedEntity&, InspectorPanel >( this );
    }

}
