//
//  plane-vs-cone.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef plane_vs_cone_hpp
#define plane_vs_cone_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool planeConeCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool conePlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
}
#endif /* plane_vs_cone_hpp */
