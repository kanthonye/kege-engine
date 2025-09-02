//
//  cylinder-vs-cylinder.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef cylinder_vs_cylinder_hpp
#define cylinder_vs_cylinder_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool cylinderPointIntersection( const Cylinder& cylinder, const vec3& point, vec3* normal = nullptr, float* penetration = nullptr );
    bool cylinderCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* cylinder_vs_cylinder_hpp */
