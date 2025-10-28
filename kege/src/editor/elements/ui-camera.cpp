//
//  ui-camera.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#include "ui-camera.hpp"

namespace kege::ui
{
    bool orthographic( ui::Layout& layout, TreeNode& tree, Orthographic* orthographic )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(4);
            setupDroplist( layout, tree, "Orthographic" );
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

    bool perspective( ui::Layout& layout, TreeNode& tree, Perspective* perspective )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(4);
            setupDroplist( layout, tree, "Perspective" );
            tree.children.resize(4);
        }
        bool active = false;
        if( numericD( layout, tree.children[0], perspective->aspect_ratio, "Aspect Ratio" ) ) active = true;
        if( numericD( layout, tree.children[1], perspective->fov, "Field Of View" ) ) active = true;
        if( numericD( layout, tree.children[2], perspective->znear, "Z Near" ) ) active = true;
        if( numericD( layout, tree.children[3], perspective->zfar, "Z Far" ) ) active = true;
        return active;
    }

    bool camera( ui::Layout& layout, ui::Tree& tree, kege::Entity& entity )
    {
        kege::Camera* camera = entity.get< kege::Camera >();
        ui::TreeNode& node = tree[ camera ];

        if ( node.elements.empty() )
        {
            node.elements.reserve(4);
            ui::setupDroplist( layout, node, "Camera" );
            node.children.resize(3);
        }

        layout.push( node.elements[0] );
        {
            layout.push( node.elements[1] );
            {
                layout.put( node.elements[2] );
            }
            layout.pop();

            if ( ui::droplistOpen( layout, node ) )
            {
                layout.push( node.elements[3] );
                if ( camera->projection->type == Projection::PERSPECTIVE )
                {
                    Perspective* projection = (Perspective*) camera->projection.ref();
                    if( perspective( layout, node.children[0], projection ) )
                    {
                        camera->modified = true;
                    }
                }
                else if ( camera->projection->type == Projection::ORTHOGRAPHIC )
                {
                    Orthographic* projection = (Orthographic*) camera->projection.ref();
                    if( orthographic( layout, node.children[0], projection ) )
                    {
                        camera->modified = true;
                    }
                }
                layout.pop();
            }
        }
        layout.pop();
        return node.state[1];
    }
}
