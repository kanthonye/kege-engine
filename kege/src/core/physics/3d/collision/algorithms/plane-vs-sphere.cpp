//
//  plane-vs-sphere.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "plane-vs-sphere.hpp"
namespace kege::algo{

    bool planeSphereCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const Plane* plane = a->collider->getPlane();
        const Sphere* sphere = b->collider->getSphere();

        // Find the distance from the plane->
        float dist = dot( plane->normal, sphere->center ) - sphere->radius - plane->distance;
        if ( dist >= 0 ) return false;

        CollisionManifold* collision = collisions.generate();
        collision->objects[0] = a;
        collision->objects[1] = b;
        collision->contact_count = 0;
        collision->contacts[ collision->contact_count ].point = sphere->center - plane->normal * (dist + sphere->radius);
        collision->contacts[ collision->contact_count ].depth = -dist;
        collision->normal = plane->normal;
        collision->contact_count++;

        return true;
    }

    bool spherePlaneCollision(Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        uint32_t index = collisions.count();
        if( planeSphereCollision( b, a, collisions ) )
        {
            collisions[index]->objects[0] = b;
            collisions[index]->objects[1] = a;
            return true;
        }
        return false;
    }

}
