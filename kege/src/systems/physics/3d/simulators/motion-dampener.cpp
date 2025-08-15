//
//  motion-dampener.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#include "physics-simulation.hpp"
#include "motion-dampener.hpp"

namespace kege::physics{

    void MotionDampener::simulate( double dms )
    {
        for (ComponentCacheT< Rigidbody >::Iterator body = _simulator->rigidbodies().begin(); body != _simulator->rigidbodies().end(); body++ )
        {
            body->angular.velocity *= pow( body->angular.damping, dms );
            body->linear.velocity *= pow( body->linear.damping, dms );
        }
    }

}
