//
//  cylinder-vs-sphere.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef cylinder_vs_sphere_hpp
#define cylinder_vs_sphere_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool cylinderSphereCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool sphereCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* cylinder_vs_sphere_hpp */
