//
//  particle-force-aggregator.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/29/24.
//

#ifndef particle_force_aggregator_hpp
#define particle_force_aggregator_hpp

#include "behavior.hpp"

namespace kege{

    class WindBehavior : public Behavior
    {
    public:

        void update( double dms, Particle& particle );
        WindBehavior
        (
            kege::vec3 direction = kege::vec3(1.0f, 0.0f, 0.0f), // Normalized wind direction
            float strength = 2.0f,                             // Base wind force magnitude
            float turbulence = 0.3f,                           // Noise-based randomness (0 = none)
            float falloffRadius = 10.0f,                       // Wind influence radius (0 = infinite)
            // Optional: Frequency for animated wind/turbulence
            float frequency = 0.5f,
            float time = 0.0f
        );

        kege::vec3 direction ; // Normalized wind direction
        float strength;                             // Base wind force magnitude
        float turbulence;                           // Noise-based randomness (0 = none)
        float falloffRadius;                       // Wind influence radius (0 = infinite)
        float frequency;
        float time;
    };

}
#endif /* particle_force_aggregator_hpp */
