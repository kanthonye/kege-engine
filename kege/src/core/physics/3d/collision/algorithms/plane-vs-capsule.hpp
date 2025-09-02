//
//  plane-vs-capsule.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef plane_vs_capsule_hpp
#define plane_vs_capsule_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool planeCapsuleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool capsulePlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* plane_vs_capsule_hpp */
