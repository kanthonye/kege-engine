//
//  sphere-vs-cone.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef sphere_vs_cone_hpp
#define sphere_vs_cone_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool sphereConeCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool coneSphereCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
}

#endif /* sphere_vs_cone_hpp */
