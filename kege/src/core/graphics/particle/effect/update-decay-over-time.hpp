//
//  update-decay-over-time.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/30/24.
//

#ifndef update_decay_over_time_hpp
#define update_decay_over_time_hpp

#include "ecs.hpp"

namespace kege{

    struct DecayOverTime
    {
        double lifespand = 0.0;
    };

    class UpdateDecayOverTime : public kege::ecs::System
    {
    public:

        UpdateDecayOverTime( kege::ECS* ecs );
        void update( double dms );
        //bool initialize();
    };
}
#endif /* update_decay_over_time_hpp */
