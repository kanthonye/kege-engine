//
//  collision-detector.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#ifndef collision_detector_hpp
#define collision_detector_hpp

#include "contact.hpp"
#include "simulator.hpp"

namespace kege::physics{

    typedef bool (*CollisionDetectorFunction)( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

    struct CollisionDetector : public Simulator
    {
    public:

        void simulate( double time_step );
        CollisionDetector();

        CollisionDetectorFunction _collision_function_table[ RIGID_SHAPE_MAX_COUNT ][ RIGID_SHAPE_MAX_COUNT ];
    };

}
#endif /* collision_detector_hpp */
