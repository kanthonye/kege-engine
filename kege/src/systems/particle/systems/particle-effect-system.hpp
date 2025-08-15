//
//  particle-effect-update-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/24/25.
//

#ifndef particle_effect_update_system_hpp
#define particle_effect_update_system_hpp

#include "entity-system.hpp"
#include "particle-effect.hpp"

namespace kege{

    class ParticleEffectSystem : public kege::EntitySystem
    {
    public:

        ParticleEffectSystem( kege::Engine* engine );
        void update( double dms );
        bool initialize();
        void shutdown();
    };

}
#endif /* particle_effect_update_system_hpp */
