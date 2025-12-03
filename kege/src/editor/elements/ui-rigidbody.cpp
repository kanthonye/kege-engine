//
//  ui-rigidbody.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#include "ui-rigidbody.hpp"

namespace kege::ui
{
    bool rigidbody( kege::ECS* ecs, ui::Layout& layout, ui::Tree& tree, ecs::Entity& entity )
    {
        kege::Rigidbody* body = ecs->get< kege::Rigidbody >( entity );
        ui::TreeNode& node = tree[ body ];

        if ( node.elements.empty() )
        {
            node.elements.reserve(4);
            setupDroplist( layout, node, "Rigidbody" );
            node.children.resize(8);
        }

        layout.push( node.elements[0] );
        {
            layout.push( node.elements[1] );
            {
                layout.put( node.elements[2] );
            }
            layout.pop();

            if ( droplistOpen( layout, node ) )
            {
                layout.push( node.elements[3] );
                {
                    float mass = 1.0 / body->linear.invmass;
                    numeric( layout, node.children[0], mass, "Mass" );
                    body->linear.invmass = 1.0 / mass;

                    numeric( layout, node.children[1], body->friction, "Friction" );
                    numeric( layout, node.children[2], body->cor, "Restitution" );
                    numeric( layout, node.children[3], body->linear.damping, "Linear Damping" );
                    numeric( layout, node.children[4], body->angular.damping, "Angular Damping" );
                    numeric3
                    (
                        layout,
                        node.children[5],
                        body->linear.velocity.x,
                        body->linear.velocity.y,
                        body->linear.velocity.z,
                        "Velocity"
                    );
                    //numeric3( layout, node.children[6], body->center.x, body->center.y, body->center.z, "Position" );
                    //rotation( layout, node.children[7], body->orientation.x, body->orientation.y, body->orientation.z, body->orientation.w, "Orientation" );
                }
                layout.pop();
            }
        }
        layout.pop();
        return node.state[1];
    }
}
