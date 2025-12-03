//
//  ui-camera.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef ui_camera_hpp
#define ui_camera_hpp

#include "ui-numeric.hpp"
#include "../../core/graphics/camera/camera.hpp"

namespace kege::ui
{
    bool orthographic( ui::Layout& layout, TreeNode& tree, Orthographic* orthographic );
    bool perspective( ui::Layout& layout, TreeNode& tree, Perspective* perspective );
    bool camera( kege::ECS* ecs, ui::Layout& layout, ui::Tree& tree, ecs::Entity& entity );
}
#endif /* ui_camera_hpp */
