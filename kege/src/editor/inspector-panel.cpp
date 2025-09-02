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
            Rigidbody* rigidbody = _selected_entity.get< Rigidbody >();
            if ( rigidbody )
            {
                Droplist& droplist = _droplists[ rigidbody ];
                if ( droplist.begin( layout, true, 10, "Rigidbody" ) )
                {
                    ui::Numeric3 center = {};
                    ui::Numeric4 orientation = {};

                    droplist.beginContent( layout );
                    center.update
                    (
                        layout,
                        rigidbody->center.x,
                        rigidbody->center.y,
                        rigidbody->center.z,
                        "Center:"
                    );
                    orientation.update
                    (
                        layout,
                        rigidbody->orientation.x,
                        rigidbody->orientation.y,
                        rigidbody->orientation.z,
                        rigidbody->orientation.w,
                        "Orientation:"
                    );

                    Droplist* list = &_droplists[ &rigidbody->linear ];
                    if ( list->begin( layout, true, 10, "Linear Motion" ) )
                    {
                        list->beginContent( layout );
                        ui::Numeric3 velocity = {};
                        ui::Numeric dampling = {};
                        ui::Numeric invmass = {};
                        velocity.update
                        (
                            layout,
                            rigidbody->linear.velocity.x,
                            rigidbody->linear.velocity.y,
                            rigidbody->linear.velocity.z,
                            "Velocity:"
                        );
                        dampling.update( layout, rigidbody->linear.damping, "Damping" );
                        if ( rigidbody->linear.damping < 0) rigidbody->linear.damping = 0.f;

                        float imass = rigidbody->linear.invmass;
                        invmass.update( layout, imass, "InvMass" );
                        if ( imass < 0) imass = 0.f;
                        rigidbody->linear.invmass = imass;
                    }
                    list->endContent( layout );

                    droplist.endContent( layout );
                }
                droplist.end( layout );
            }
            Transform* transform = _selected_entity.get< Transform >();
            if ( transform )
            {
                Droplist& droplist = _droplists[ transform ];
                if ( droplist.begin( layout, true, 10, "Transform" ) )
                {
                    ui::Numeric3 scale = {};
                    ui::Numeric3 position = {};
                    ui::Numeric4 orientation = {};

                    droplist.beginContent( layout );
                    position.update
                    (
                        layout,
                        transform->position.x,
                        transform->position.y,
                        transform->position.z,
                        "Position:"
                    );
                    orientation.update
                    (
                        layout,
                        transform->orientation.x,
                        transform->orientation.y,
                        transform->orientation.z,
                        transform->orientation.x,
                        "Rotation:"
                    );
                    scale.update
                    (
                        layout,
                        transform->scale.x,
                        transform->scale.y,
                        transform->scale.z,
                        "Scale:"
                    );
                    droplist.endContent( layout );
                }
                droplist.end( layout );
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
