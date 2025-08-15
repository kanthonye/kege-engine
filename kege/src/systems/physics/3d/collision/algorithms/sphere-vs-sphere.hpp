//
//  sphere-vs-sphere.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef sphere_vs_sphere_hpp
#define sphere_vs_sphere_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool sphereSphereCollision(Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* sphere_vs_sphere_hpp */
