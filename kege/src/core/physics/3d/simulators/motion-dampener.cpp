//
//  motion-dampener.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#include "motion-dampener.hpp"

namespace kege::physics{

    void MotionDampener::simulate( double dms )
    {
        for (auto [entity, body] : _simulator->rigidbodies() )
        {
            body->angular.velocity *= pow( body->angular.damping, dms );
            body->linear.velocity *= pow( body->linear.damping, dms );
        }
    }

}
