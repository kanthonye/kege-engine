//
//  force-integrator.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#ifndef force_integrator_hpp
#define force_integrator_hpp

#include "simulator.hpp"

namespace kege::physics{

    struct ForceIntegrator : public Simulator
    {
    public:

        void simulate( double time_step );
    };

}
#endif /* force_integrator_hpp */
