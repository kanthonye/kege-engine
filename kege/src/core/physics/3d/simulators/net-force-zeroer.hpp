//
//  net-force-zeroer.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#ifndef net_force_zeroer_hpp
#define net_force_zeroer_hpp

#include "simulator.hpp"

namespace kege::physics{

    struct NetForceZeroer : public Simulator
    {
    public:

        void simulate( double time_step );
    };

}

#endif /* net_force_zeroer_hpp */
