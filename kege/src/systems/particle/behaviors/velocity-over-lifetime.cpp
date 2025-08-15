//
//  velocity-over-lifetime.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#include "velocity-over-lifetime.hpp"

namespace kege{

    void VelocityOverLifetime::update( double dms, Particle& particle )
    {
        if ( velocities.size() == 0) return;

        float t = particle.health / particle.max_health;

        if (t <= 0.0f)
        {
            particle.velocity += velocities[0];
            return;
        }

        if (t >= 1.0f)
        {
            particle.velocity += velocities[ velocities.size() - 1 ];
            return;
        }

        int min = int( velocities.size() - 1 ) * t;
        int max = min + 1;

        particle.velocity += lerp( velocities[ min ], velocities[ max ], t );
    }

    VelocityOverLifetime::VelocityOverLifetime( const array< vec3 >& velocities )
    :   velocities( velocities )
    {}

}
