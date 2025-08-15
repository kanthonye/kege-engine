//
//  plane-vs-circle.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/22/25.
//

#ifndef plane_vs_circle_hpp
#define plane_vs_circle_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool planeCircleIntersection(const Plane& plane, const Circle& circle, std::vector< Intersection >& intersections );
    bool planeCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool circlePlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* plane_vs_circle_hpp */
