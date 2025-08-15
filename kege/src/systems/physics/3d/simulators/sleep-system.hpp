//
//  sleep-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/20/25.
//

#ifndef sleep_system_hpp
#define sleep_system_hpp

#include "entity-system.hpp"
#include "rigidbody.hpp"

namespace kege{

    struct WakeSleepableObjects{};

    class SleepSystem : public kege::EntitySystem
    {
    public:

        void operator()( const WakeSleepableObjects& );
        void update( double dms );
        bool initialize();
        void shutdown();

        SleepSystem( kege::Engine* engine );

        float _sleep_threshold;     // Velocity threshold to enter sleep
        float _sleep_motion_bias;   // Damping factor for the body calculation
        bool _wake_sleepable_objects;
    };

}
#endif /* sleep_system_hpp */
