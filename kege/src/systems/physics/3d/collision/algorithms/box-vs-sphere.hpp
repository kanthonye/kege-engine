//
//  box-vs-sphere.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef box_vs_sphere_hpp
#define box_vs_sphere_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool boxSphereCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

    bool sphereBoxCollision(Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* box_vs_sphere_hpp */
