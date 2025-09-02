//
//  capsule-vs-capsule.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef capsule_vs_capsule_hpp
#define capsule_vs_capsule_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    /**
     * Capsule CollisionManifold Detection:
     * The TestCapsuleCapsuleCollision function calculates the closest points between the two capsule line segments.
     * It then computes the distance between these points and checks if it is less than the sum of the capsule radii.
     * If a collision is detected, it calculates the penetration depth, contact point, and collision normal.
     */
    bool capsuleCapsuleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* capsule_vs_capsule_hpp */
