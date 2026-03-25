//
//  ui-light.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/6/26.
//

#ifndef ui_light_hpp
#define ui_light_hpp

#include "../../../common/dependencies.hpp"

namespace kege::ui{

    bool light( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
}
#endif /* ui_light_hpp */
