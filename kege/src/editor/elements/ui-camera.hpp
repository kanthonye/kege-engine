//
//  ui-camera.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef ui_camera_hpp
#define ui_camera_hpp

#include "ui-tree-node.hpp"
#include "../../core/graphics/camera/camera.hpp"

namespace kege::ui
{
    bool camera( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
}
#endif /* ui_camera_hpp */
