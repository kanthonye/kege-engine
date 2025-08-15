//
//  particle-emission-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/24/25.
//

#ifndef particle_emission_system_hpp
#define particle_emission_system_hpp

#include "entity-system.hpp"
#include "particle-effect.hpp"

namespace kege{

    class ParticleEmissionSystem : public kege::EntitySystem
    {
    public:

        ParticleEmissionSystem( kege::Engine* engine );
        void update( double dms );
        bool initialize();
        void shutdown();
    };

}
#endif /* particle_emission_system_hpp */
