//
//  plane-vs-cylinder.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef plane_vs_cylinder_hpp
#define plane_vs_cylinder_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool planeCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool cylinderPlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* plane_vs_cylinder_hpp */
