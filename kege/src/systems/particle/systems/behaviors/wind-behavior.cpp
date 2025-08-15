//
//  particle-force-aggregator.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/29/24.
//

#include "wind-behavior.hpp"

namespace kege{

    void WindBehavior::update( double dms, Particle& particle )
    {
        if (particle.invmass <= 0.0f) return; // Skip immovable particles

        // Calculate wind origin (use windZone.transform if provided, else assume global wind)
        kege::vec3 windOrigin( 0.0f );

        // 1. Apply distance falloff (if falloffRadius > 0)
        float distance = kege::magn(particle.position - windOrigin);
        float falloff = 1.0f;
        if (falloffRadius > 0.0f)
        {
            falloff = 1.0f - kege::clamp(distance / falloffRadius, 0.0f, 1.0f);
            if (falloff <= 0.0f) return; // Outside influence zone
        }

        // 2. Calculate base wind force
        kege::vec3 windForce = kege::normalize(direction) * strength;

        // 3. Add turbulence (Perlin noise for smooth randomness)
        if (turbulence > 0.0f)
        {
            time += static_cast<float>( dms ) * frequency;
            ///kege::vec3 noiseInput = particle.position * 0.1f + kege::vec3(time);
            float noise = 1.0f;//PerlinNoise3D(noiseInput.x, noiseInput.y, noiseInput.z);
            windForce += turbulence * noise * kege::vec3(1.0f, 0.5f, 1.0f); // Anisotropic
        }

        // 4. Apply falloff and inverse mass
        windForce *= falloff * particle.invmass;

        // 5. Update particle velocity
        particle.velocity += windForce * static_cast<float>( dms );
    }

    WindBehavior::WindBehavior
    (
        kege::vec3 direction,
        float strength,
        float turbulence,
        float falloffRadius,
        float frequency,
        float time
    )
    :   direction( direction )
    ,   strength( strength )
    ,   turbulence( turbulence )
    ,   falloffRadius( falloffRadius )
    ,   frequency( frequency )
    ,   time( time )
    {}

}
