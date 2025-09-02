//
//  circle-vs-circle.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/22/25.
//

#ifndef circle_vs_circle_hpp
#define circle_vs_circle_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool circleCircleIntersection(const Circle& a, const Circle& b, vec3& normal, vec3& point, float& depth);
    bool circleCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

    bool circleBoxCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool circleConeCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool circleCapsuleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool circleCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool circleSphereCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool circleMeshCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

    bool boxCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool coneCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool capsuleCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool cylinderCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool sphereCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool meshCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
}

#endif /* circle_vs_circle_hpp */
