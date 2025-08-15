//
//  box-vs-sphere.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "box-vs-sphere.hpp"
namespace kege::algo{

    bool boxSphereCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const OBB* box = a->collider->getBox();
        const Sphere* sphere = b->collider->getSphere();

        Point closest_point = closestPointPointBox( sphere->center, box );
        vec3 n = sphere->center - closest_point;
        float dist_sq = dot( n, n );

        if ( dist_sq > sphere->radius * sphere->radius )
        {
            return false;
        }

        float penetration_depth = sqrt( dist_sq );

        CollisionManifold* collision = collisions.generate();
        collision->objects[0] = a;
        collision->objects[1] = b;
        collision->contact_count = 0;
        if (penetration_depth > 0.0f)
        {
            collision->normal = n / penetration_depth;
            collision->contacts[ collision->contact_count ].point = closest_point - collision->normal * penetration_depth;
            collision->contacts[ collision->contact_count ].depth = sphere->radius - penetration_depth;
            collision->contact_count++;
        }
        else // Sphere is exactly at the center of the OBB, arbitrary normal
        {
            collision->normal = {0.0f, 1.0f, 0.0f};
            collision->contacts[ collision->contact_count ].point = closest_point;
            collision->contacts[ collision->contact_count ].depth = penetration_depth;
            collision->contact_count++;
        }
        return true;
    }

    bool sphereBoxCollision(Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        uint32_t index = collisions.count();
        if( boxSphereCollision( b, a, collisions ) )
        {
            collisions[index]->objects[0] = b;
            collisions[index]->objects[1] = a;
            return true;
        }
        return false;
    }

}
