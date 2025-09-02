//
//  plane-vs-cylinder.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "plane-vs-circle.hpp"
#include "plane-vs-cylinder.hpp"

namespace kege::algo{

    bool planeCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const Cylinder* cylinder = b->collider->getCylinder();
        const Plane* plane = a->collider->getPlane();

        float center_dist = dot( plane->normal, cylinder->center ) - plane->distance;

        Circle circle[2];
        circle[0].center = cylinder->center + cylinder->axes[0] * cylinder->height * 0.5;
        circle[0].normal = cylinder->axes[0];
        circle[0].right  = cylinder->axes[1];
        circle[0].radius = cylinder->radius;

        circle[1].center = cylinder->center - cylinder->axes[0] * cylinder->height * 0.5;
        circle[1].normal = -cylinder->axes[0];
        circle[1].right  = cylinder->axes[1];
        circle[1].radius = cylinder->radius;

//        kege::vec3 point[2];
//        float depth[2];
        float dist[2];
//        int count = 0;

        std::vector< Intersection > intersections;
        dist[0] = dot( plane->normal, circle[0].center ) - circle[0].radius - plane->distance;
        if ( dist[0] < 0 )
        {
            if( !planeCircleIntersection( *plane, circle[0], intersections ) )
            {
                if ( center_dist < -circle[0].radius )
                {
                    dist[0] = abs( dot( plane->normal, circle[0].center ) - plane->distance );
                    intersections.push_back({ circle[0].center, dist[0] });
                }
            }
        }

        dist[1] = dot( plane->normal, circle[1].center ) - circle[1].radius - plane->distance;
        if ( dist[1] < 0 )
        {
            if( !planeCircleIntersection( *plane, circle[1], intersections ) )
            {
                if ( center_dist < -circle[1].radius )
                {
                    dist[0] = abs( dot( plane->normal, circle[1].center ) - plane->distance );
                    intersections.push_back({ circle[1].center, dist[0] });
                    //point[count] = circle[1].center;
                    //depth[count] = abs( dot( plane->normal, circle[1].center ) - plane->distance );
                    //count++;
                }
            }
        }

        if ( !intersections.empty() )
        {
            CollisionManifold* manifold = collisions.generate();
            if ( manifold )
            {
                manifold->contact_count = (int)intersections.size();
                manifold->normal = plane->normal;
                manifold->objects[0] = a;
                manifold->objects[1] = b;

                for (int i=0; i<intersections.size(); ++i)
                {
                    manifold->contacts[i].point = intersections[i].point;
                    manifold->contacts[i].depth = intersections[i].penetration;

                    drawLine({ intersections[i].point, intersections[i].point + plane->normal });
                    drawAABB({ intersections[i].point - 0.05, intersections[i].point + 0.05 });
                }
//                for (int i=0; i<count && i < MAX_CONTACTS; ++i)
//                {
//                    collision->contacts[i].point = point[i];
//                    collision->contacts[i].depth = depth[i];
//                    collision->contacts[i].normal = plane->normal;
//                }
            }
            return true;
        }
        return false;
    }

    bool cylinderPlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        uint32_t index = collisions.count();
        if( planeCylinderCollision( b, a, collisions ) )
        {
            collisions[index]->objects[0] = b;
            collisions[index]->objects[1] = a;
            return true;
        }
        return false;
    }

}
