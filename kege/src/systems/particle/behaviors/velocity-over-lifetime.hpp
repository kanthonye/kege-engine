//
//  velocity-over-lifetime.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#ifndef velocity_over_lifetime_hpp
#define velocity_over_lifetime_hpp

#include "behavior.hpp"

namespace kege{

    struct VelocityOverLifetime : public Behavior
    {
        void update( double dms, Particle& particle );
        VelocityOverLifetime( const array< vec3 >& velocities );
        array< vec3 > velocities;
    };

}
#endif /* velocity_over_lifetime_hpp */
