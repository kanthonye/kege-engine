//
//  plane-vs-sphere.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef plane_vs_sphere_hpp
#define plane_vs_sphere_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool planeSphereCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool spherePlaneCollision(Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* plane_vs_sphere_hpp */
