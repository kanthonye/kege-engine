//
//  capsule-vs-capsule.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "capsule-vs-capsule.hpp"

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool capsuleCapsuleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        vec3 closest_points[2];
        float s,t;

        Line line[2];
        vec3 half_height_vec;

        const Capsule* capsule1 = a->collider->getCapsule();
        half_height_vec = capsule1->axes[0] * (capsule1->height * 0.5f);
        line[0].start = capsule1->center + half_height_vec;
        line[0].end = capsule1->center - half_height_vec;

        const Capsule* capsule2 = b->collider->getCapsule();
        half_height_vec = capsule2->axes[0] * (capsule2->height * 0.5f);
        line[1].start = capsule2->center + half_height_vec;
        line[1].end = capsule2->center - half_height_vec;

        closestPointLineLine( line[0], line[1], s, t, closest_points );

        vec3 delta = closest_points[1] - closest_points[0];
        float distanceSquared = kege::magnSq( delta );
        float radiusSum = capsule1->radius + capsule1->radius;

        if (distanceSquared > radiusSum * radiusSum)
        {
            return false;
        }

        // CollisionManifold detected
        CollisionManifold* collision = collisions.generate();
        collision->objects[0] = a;
        collision->objects[1] = b;

        float distance = std::sqrt(distanceSquared);
        vec3 normal = delta / distance;
        float penetrationDepth = radiusSum - distance;

        // Store collision data
        collision->normal = normal;
        collision->contacts[0].point = closest_points[0] + normal * capsule1->radius;
        collision->contacts[0].depth = penetrationDepth;
        collision->contact_count = 1;

        return true;
    }

}
