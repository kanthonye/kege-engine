//
//  grounded-detector.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#ifndef grounded_detector_hpp
#define grounded_detector_hpp

#include "simulator.hpp"

namespace kege::physics{

//    struct CollisionsResolver : public Simulator
//    {
//    public:
//        void simulate( double time_step );
//    };

    struct GroundedDetector : public Simulator
    {
    public:

        void testGrounded( Rigidbody* body, Rigidbody* ground, const kege::vec3& contact_point );
        void simulate( double time_step );

        float _grounded_threshold;
    };

}
#endif /* grounded_detector_hpp */
