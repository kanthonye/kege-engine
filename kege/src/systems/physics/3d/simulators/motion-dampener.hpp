//
//  motion-dampener.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#ifndef motion_dampener_hpp
#define motion_dampener_hpp

#include "simulator.hpp"

namespace kege::physics{

    struct MotionDampener : public Simulator
    {
    public:

        void simulate( double time_step );
    };

}
#endif /* motion_dampener_hpp */
