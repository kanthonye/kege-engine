//
//  capsule-vs-sphere.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef capsule_vs_sphere_hpp
#define capsule_vs_sphere_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool capsuleSphereCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool sphereCapsuleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}

#endif /* capsule_vs_sphere_hpp */
