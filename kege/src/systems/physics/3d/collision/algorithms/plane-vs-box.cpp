//
//  plane-vs-box.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "plane-vs-box.hpp"
namespace kege::algo{

    bool planeBoxCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const Plane* plane = a->collider->getPlane();
        const OBB* box = b->collider->getBox();

        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        float r = box->extents[0] * abs(dot(plane->normal, box->axes[0])) +
        box->extents[1] * abs( dot(plane->normal, box->axes[1]) ) +
        box->extents[2] * abs( dot(plane->normal, box->axes[2]) );

        // Compute distance of box center from plane
        float s = dot( plane->normal, box->center ) - plane->distance;
        if( s <= r )
        {
            vec3 points[8];
            getBoxCorners( box, points );

            float dist;
            CollisionManifold* collision = collisions.generate();
            collision->objects[0] = a;
            collision->objects[1] = b;
            collision->contact_count = 0;
            for ( int i=0; i<8 && collision->contact_count < 4; ++i)
            {
                dist = dot( points[ i ] - box->center, plane->normal );
                if ( dist  <= 0 )
                {
                    dist = dot( points[ i ], plane->normal ) - plane->distance;
                    if ( dist <= 0.0 )
                    {
                        collision->contacts[ collision->contact_count ].point = points[ i ];
                        collision->contacts[ collision->contact_count ].depth = abs(dist);
                        collision->contact_count++;
                    }
                }
            }
            collision->normal = plane->normal;
            return true;
        }
        return false;
    }

    bool boxPlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        uint32_t index = collisions.count();
        if( planeBoxCollision( b, a, collisions ) )
        {
            collisions[index]->objects[0] = b;
            collisions[index]->objects[1] = a;
            return true;
        }
        return false;
    }

}
