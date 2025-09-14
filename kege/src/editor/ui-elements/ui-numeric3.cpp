//
//  ui-numeric3.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#include "ui-numeric3.hpp"

namespace kege::ui{

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



    bool transform( ui::Layout& layout, TreeNode& tree, Transform* transform )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(4);
            setupDroplist( layout, tree, "Transform" );
            tree.children.resize(3);
        }

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
                    numeric3( layout, tree.children[0], transform->position.x, transform->position.y, transform->position.z, "Position" );
                    rotation( layout, tree.children[1], transform->orientation.x, transform->orientation.y, transform->orientation.z, transform->orientation.w, "Orientation" );
                    numeric3( layout, tree.children[2], transform->scale.x, transform->scale.y, transform->scale.z, "Scale" );
                }
                layout.pop();
            }
        }
        layout.pop();
        return tree.state[1];
    }

    bool rigidbody( ui::Layout& layout, TreeNode& tree, kege::Rigidbody* body )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(4);
            setupDroplist( layout, tree, "Rigidbody" );
            tree.children.resize(8);
        }

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
                    float mass = 1.0 / body->linear.invmass;
                    numeric( layout, tree.children[0], mass, "Mass" );
                    body->linear.invmass = 1.0 / mass;

                    numeric( layout, tree.children[1], body->friction, "Friction" );
                    numeric( layout, tree.children[2], body->cor, "Restitution" );
                    numeric( layout, tree.children[3], body->linear.damping, "Linear Damping" );
                    numeric( layout, tree.children[4], body->angular.damping, "Angular Damping" );
                    numeric3
                    (
                        layout,
                        tree.children[5],
                        body->linear.velocity.x,
                        body->linear.velocity.y,
                        body->linear.velocity.z,
                        "Velocity"
                    );
                    numeric3( layout, tree.children[6], body->center.x, body->center.y, body->center.z, "Position" );
                    rotation( layout, tree.children[7], body->orientation.x, body->orientation.y, body->orientation.z, body->orientation.w, "Orientation" );
                }
                layout.pop();
            }
        }
        layout.pop();
        return tree.state[1];
    }



    bool perspective( ui::Layout& layout, TreeNode& tree, Perspective* perspective )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(4);
            setupDroplist( layout, tree, "Camera" );
            tree.children.resize(4);
        }
        bool active = false;
        if( numericD( layout, tree.children[0], perspective->aspect_ratio, "Aspect Ratio" ) ) active = true;
        if( numericD( layout, tree.children[1], perspective->fov, "Field Of View" ) ) active = true;
        if( numericD( layout, tree.children[2], perspective->znear, "Z Near" ) ) active = true;
        if( numericD( layout, tree.children[3], perspective->zfar, "Z Far" ) ) active = true;
        return active;
    }
    
    bool orthographic( ui::Layout& layout, TreeNode& tree, Orthographic* orthographic )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(4);
            setupDroplist( layout, tree, "Camera" );
            tree.children.resize(4);
        }
        bool active = false;
        if( numericD( layout, tree.children[0], orthographic->left, "Left" ) ) active = true;
        if( numericD( layout, tree.children[1], orthographic->right, "Right" ) ) active = true;
        if( numericD( layout, tree.children[2], orthographic->above, "Above" ) ) active = true;
        if( numericD( layout, tree.children[3], orthographic->below, "Below" ) ) active = true;
        if( numericD( layout, tree.children[4], orthographic->znear, "Z Near" ) ) active = true;
        if( numericD( layout, tree.children[5], orthographic->zfar, "Z Far" ) ) active = true;
        return active;
    }

    bool camera( ui::Layout& layout, TreeNode& tree, Camera* camera )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(4);
            setupDroplist( layout, tree, "Camera" );
            tree.children.resize(3);
        }

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
                if ( camera->projection->type == Projection::PERSPECTIVE )
                {
                    Perspective* projection = (Perspective*) camera->projection.ref();
                    if( perspective( layout, tree.children[0], projection ) )
                    {
                        camera->modified = true;
                    }
                }
                else if ( camera->projection->type == Projection::ORTHOGRAPHIC )
                {
                    Orthographic* projection = (Orthographic*) camera->projection.ref();
                    if( orthographic( layout, tree.children[0], projection ) )
                    {
                        camera->modified = true;
                    }
                }
                layout.pop();
            }
        }
        layout.pop();
        return tree.state[1];
    }


}
