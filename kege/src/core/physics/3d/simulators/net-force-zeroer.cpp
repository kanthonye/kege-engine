//
//  net-force-zeroer.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#include "net-force-zeroer.hpp"
#include "../simulation/physics-simulation.hpp"

namespace kege::physics{

    void NetForceZeroer::simulate( double time_step )
    {
        for (auto [entity, body] : _simulator->rigidbodies() )
        {
            body->angular.torques = {0.f, 0.f, 0.f};
            body->linear.forces = {0.f, 0.f, 0.f};
        }
    }

}
