//
//  ui-transform.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#include "ui-transform.hpp"

namespace kege::ui
{
    bool rotation( ui::Layout& layout, TreeNode& tree, float& x, float& y, float& z, float& w, const char* text )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(5);
            setupDroplist( layout, tree, text );
            tree.elements.push_back({ makeRow( layout ) });
            tree.children.resize(5);
        }

        bool active = false;
        layout.push( tree.elements[0] );
        {
            layout.push( tree.elements[1] );
            {
                layout.put( tree.elements[2] );
            }
            layout.pop();

            if ( droplistOpen( layout, tree ) )
            {
                layout.push( tree.elements[3] );
                {
                    if( numeric( layout, tree.children[1], w, "angle:" ) ) active = true;

                    layout.push( tree.elements[4] );
                    {
                        if( numeric( layout, tree.children[2], x, "x:" ) ) active = true;
                        if( numeric( layout, tree.children[3], y, "y:" ) ) active = true;
                        if( numeric( layout, tree.children[4], z, "z:" ) ) active = true;
                    }
                    layout.pop();
                }
                layout.pop();
            }
        }
        layout.pop();
        return active || tree.state[1];
    }

    bool transform( ui::Layout& layout, ui::Tree& tree, kege::Entity& entity )
    {
        Transform* transform = entity.get< kege::Transform >();
        ui::TreeNode& node = tree[ transform ];

        if ( node.elements.empty() )
        {
            node.elements.reserve(4);
            setupDroplist( layout, node, "Transform" );
            node.children.resize(3);
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
                    numeric3
                    (
                        layout, node.children[0],
                        transform->position.x,
                        transform->position.y,
                        transform->position.z,
                        "Position"
                    );

                    rotation
                    ( 
                        layout, node.children[1],
                        transform->orientation.x,
                        transform->orientation.y,
                        transform->orientation.z,
                        transform->orientation.w,
                        "Orientation"
                    );

                    numeric3
                    ( 
                        layout, node.children[2],
                        transform->scale.x,
                        transform->scale.y,
                        transform->scale.z,
                        "Scale"
                    );
                }
                layout.pop();
            }
        }
        layout.pop();
        return node.state[1];
    }
}
