//
//  particle-effect-update-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/24/25.
//

#ifndef particle_effect_update_system_hpp
#define particle_effect_update_system_hpp

#include "particle-effect.hpp"
#include "../../../../scene/ecs.hpp"

namespace kege{

    class ParticleEffectSystem : public kege::ecs::System
    {
    public:

        ParticleEffectSystem( kege::ECS* ecs );
        void update( double dms );
        bool initialize();
        void shutdown();
    };

}
#endif /* particle_effect_update_system_hpp */
