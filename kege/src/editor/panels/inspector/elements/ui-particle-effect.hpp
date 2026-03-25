//
//  ui-particle-effect.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/6/26.
//

#ifndef ui_particle_effect_hpp
#define ui_particle_effect_hpp

#include "../../../common/dependencies.hpp"

namespace kege::ui{

    bool particleEffect( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
}
#endif /* ui_particle_effect_hpp */
