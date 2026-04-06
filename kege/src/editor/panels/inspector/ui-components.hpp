//
//  ui-components.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/29/26.
//

#ifndef ui_components_hpp
#define ui_components_hpp

#include "../../common/dependencies.hpp"

namespace kege::ui{
        
    bool transform( const ui::UID& uid, int16_t layer, kege::AssetManager* am, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
    bool perspectiveCamera( const ui::UID& uid, int16_t layer, kege::AssetManager* am, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
    bool orthographicCamera( const ui::UID& uid, int16_t layer, kege::AssetManager* am, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
    bool rigidbody( const ui::UID& uid, int16_t layer, kege::AssetManager* am, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
    bool directionalLight( const ui::UID& uid, int16_t layer, kege::AssetManager* am, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
    bool pointLight( const ui::UID& uid, int16_t layer, kege::AssetManager* am, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
    bool spotLight( const ui::UID& uid, int16_t layer, kege::AssetManager* am, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
    bool renderable( const ui::UID& uid, int16_t layer, kege::AssetManager* am, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
}
#endif /* ui_components_hpp */
