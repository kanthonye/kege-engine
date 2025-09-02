//
//  capsule-vs-cylinder.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "capsule-vs-cylinder.hpp"

namespace kege::algo{

    bool capsuleCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const Capsule* capsule = a->collider->getCapsule();
        const Cylinder* cylinder = b->collider->getCylinder();

        Line capsule_line;
        vec3 half_height_vec = capsule->axes[0] * (capsule->height * 0.5f);
        capsule_line.start = capsule->center + half_height_vec;
        capsule_line.end = capsule->center - half_height_vec;

        // Calculate the endpoints of the cylinder's axis
        Line cyline = { cylinder->center, cylinder->center + cylinder->axes[0] * cylinder->height };

        // Find the closest point on the cylinder's axis to the capsule's line segment
        float t;
        vec3 closest_point_cylinder = closestPointSegment( capsule_line.start, cyline, t );

        // Find the closest point on the capsule's line segment to the closest point on the cylinder's axis
        vec3 closest_point_capsule = closestPointSegment( closest_point_cylinder, capsule_line, t );

        // Calculate the vector from the closest point on the cylinder to the closest point on the capsule
        vec3 closest_to_capsule = closest_point_capsule - closest_point_cylinder;

        // Calculate the distance
        float distance = magnSq( closest_to_capsule );

        // Check if the capsule and cylinder are colliding
        if (distance <= capsule->radius + cylinder->radius)
        {
            // Calculate the normal of the collision
            vec3 normal = closest_to_capsule / distance;

            // Calculate the penetration depth
            float penetrationDepth = (capsule->radius + cylinder->radius) - distance;

            CollisionManifold* collision = collisions.generate();
            // Populate the collision struct
            collision->objects[0] = a;
            collision->objects[1] = b;
            collision->normal = normal;

            // Calculate the penetration point
            vec3 penetrationPoint = closest_point_cylinder + (cylinder->radius * normal);

            // Populate the penetration array
            collision->contacts[0].point = penetrationPoint;
            collision->contacts[0].depth = penetrationDepth;
            collision->contact_count = 1;
            return true;
        }
        return false;
    }

    bool cylinderCapsuleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        uint32_t index = collisions.count();
        if( capsuleCylinderCollision( b, a, collisions ) )
        {
            collisions[index]->objects[0] = b;
            collisions[index]->objects[1] = a;
            return true;
        }
        return false;
    }

}
