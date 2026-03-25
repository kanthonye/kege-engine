//
//  ui-rigidbody.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef ui_rigidbody_hpp
#define ui_rigidbody_hpp

#include "../../../common/dependencies.hpp"

namespace kege::ui
{
    bool rigidbody( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
}
#endif /* ui_rigidbody_hpp */
