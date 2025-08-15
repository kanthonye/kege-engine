//
//  sphere-vs-sphere.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "sphere-vs-sphere.hpp"
namespace kege::algo{

    bool sphereSphereCollision(Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const Sphere* sphere1 = a->collider->getSphere();
        const Sphere* sphere2 = b->collider->getSphere();

        float radius_sum = sphere1->radius + sphere2->radius;
        vec3 normal = sphere2->center - sphere1->center;

        float distsq = magnSq( normal );
        if (distsq - radius_sum * radius_sum > 0 || distsq == 0.0f)
        {
            return false;
        }
        float dist = sqrt( distsq );
        normal /= dist;

        float penetration_depth = fabsf( dist - radius_sum );
        float distance_to_intersection = (sphere1->radius - penetration_depth);

        CollisionManifold* collision = collisions.generate();
        collision->objects[0] = a;
        collision->objects[1] = b;
        collision->contact_count = 0;
        collision->contacts[ collision->contact_count ].point = sphere1->center + normal * distance_to_intersection;
        collision->contacts[ collision->contact_count ].depth = penetration_depth;
        collision->normal = normal;
        collision->contact_count++;

        return true;
    }

}
