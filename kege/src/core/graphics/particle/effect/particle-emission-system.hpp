//
//  particle-emission-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/24/25.
//

#ifndef particle_emission_system_hpp
#define particle_emission_system_hpp

#include "particle-effect.hpp"
#include "../../../scene/ecs.hpp"

namespace kege{

    class ParticleEmissionSystem : public kege::ecs::System
    {
    public:

        ParticleEmissionSystem( kege::ECS* ecs );
        void update( double dms );
        bool initialize();
        void shutdown();
    };

}
#endif /* particle_emission_system_hpp */
