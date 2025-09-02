//
//  plane-vs-capsule.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "plane-vs-capsule.hpp"
namespace kege::algo{

    bool planeCapsuleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const Plane* plane = a->collider->getPlane();
        const Capsule* capsule = b->collider->getCapsule();

        vec3 half_height_vec = capsule->axes[0] * (capsule->height * 0.5f);
        vec3 start = capsule->center + half_height_vec;
        vec3 end = capsule->center - half_height_vec;

        // Calculate the distance from the capsule's start to the plane
        float startDistance = dot(plane->normal, start) - plane->distance;

        // Calculate the distance from the capsule's end to the plane
        float endDistance = dot(plane->normal, end) - plane->distance;

        // Check if the capsule intersects the plane
        if
        (
            (startDistance <= capsule->radius && startDistance >= -capsule->radius) ||
            (endDistance <= capsule->radius && endDistance >= -capsule->radius) ||
            (startDistance * endDistance < 0)
        )
        {
            CollisionManifold* collision = collisions.generate();
            collision->objects[0] = a;
            collision->objects[1] = b;
            collision->normal = plane->normal;
            collision->contacts[0].depth = std::min(std::abs(startDistance), std::abs(endDistance));
            collision->contacts[0].point = (startDistance < endDistance) ? start : end;
            collision->contact_count = 1;
            return collision;
        }

        return false;
    }

    bool capsulePlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        uint32_t index = collisions.count();
        if( planeCapsuleCollision( b, a, collisions ) )
        {
            collisions[index]->objects[0] = b;
            collisions[index]->objects[1] = a;
            return true;
        }
        return false;
    }

}
