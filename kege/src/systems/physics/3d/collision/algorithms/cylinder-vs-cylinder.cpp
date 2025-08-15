//
//  cylinder-vs-cylinder.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "cylinder-vs-cylinder.hpp"

namespace kege::algo{

    float projectPlane(const vec3& center, const vec3& normal, const vec3& point)
    {
        return dot(point - center, normal);
    }

    bool cylinderPointIntersection( const Cylinder& cylinder, const vec3& point, vec3* normal, float* penetration )
    {
        vec3 n[3] = {cylinder.axes[0], -cylinder.axes[0]};

        Line line;
        line.start = cylinder.center + n[0] * cylinder.height * 0.5;
        line.end   = cylinder.center + n[1] * cylinder.height * 0.5;

        float dist[3];
        dist[0] = projectPlane( line.start, n[0], point );
        dist[1] = projectPlane( line.end  , n[1], point );

        if( !( dist[0] < 0.0 && dist[1] < 0.0 ) )
        {
            return false;
        }

        vec3 closest_point = closestPointLine(point, line);
        Sphere sphere = {closest_point, cylinder.radius};
        if( !testPointSphere( point, sphere, n[2], dist[2]) )
        {
            return false;
        }
        dist[2] = sqrt( dist[2] );

        vec3* pp,*pn;
        if (dist[0] > dist[1])
        {
            pn = &n[0];
            pp = &line.start;
        }
        else
        {
            pn = &n[1];
            pp = &line.end;
        }

        n[1] = (dist[0] > dist[1])? cylinder.axes[0] : -cylinder.axes[0];
        dist[0] = -dot(point - *pp, *pn);
        dist[1] = cylinder.radius - sqrt( dist[2] );

        if (dist[0] <= dist[1] && !(dist[0] < 1e-6))
        {
            drawAABB({ point[1] - 0.1, point[1] + 0.1 });
            drawLine({ point, point + *pn });
            if(penetration) *penetration = dist[0];
            if(normal) *normal = *pn;
        }
        else if (dist[0] > dist[1])
        {
            n[2] = n[2] / dist[2];
            drawAABB({ point[1] - 0.1, point[1] + 0.1 });
            drawLine({ point, point + n[2] });
            if(penetration) *penetration = dist[1];
            if(normal) *normal = n[2];
        }

        return true;
    }

    bool cylinderCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const Cylinder* c[2] = { a->collider->getCylinder(), b->collider->getCylinder() };
        /**
         * create the line segment that passes through the center of each cylinder
         */
        Line line[2];
        line[0].start = c[0]->center + c[0]->axes[0] * c[0]->height * 0.5;
        line[0].end   = c[0]->center - c[0]->axes[0] * c[0]->height * 0.5;
        line[1].start = c[1]->center + c[1]->axes[0] * c[1]->height * 0.5;
        line[1].end   = c[1]->center - c[1]->axes[0] * c[1]->height * 0.5;
        vec3 closest_points[2];

        float t[2];
        closestPointLineLine( line[0], line[1], t[0], t[1], closest_points );

        vec3 n[3];
        /**
         * make a vector from cylinder 1 center to line segment b end points this will be
         * use to compute the collision normal vector
         */
        n[0] = closest_points[1] - closest_points[0];
        /**
         * compute the collision normal vector from cylinder a to cylinder b
         */
        n[1] = cross(c[0]->axes[0], n[0]);
        n[1] = normalize( cross(n[1], c[0]->axes[0]) );
        /**
         * compute the collision normal vector from cylinder b to cylinder a
         */
        n[2] = cross(c[1]->axes[0], -n[0]);
        n[2] = normalize( cross(n[2], c[1]->axes[0]) );

        vec3 closest_surface_points[2];
        closest_surface_points[0] = closest_points[0] + n[1] * c[0]->radius;
        closest_surface_points[1] = closest_points[1] + n[2] * c[1]->radius;

        vec3 intersections[2];
        float penetration[2];
        int count=0;

        if ( cylinderPointIntersection( *c[0], closest_surface_points[1], &n[0], &t[0] ) )
        {
            intersections[count] = closest_surface_points[1];
            penetration[count] = t[0];
            //n[0] = n[];
            count++;
        }
        else if ( cylinderPointIntersection( *c[1], closest_surface_points[0], &n[0], &t[0] ) )
        {
            intersections[count] = closest_surface_points[1];
            penetration[count] = t[0];
            n[0] = -n[0];
            count++;
        }

#ifdef KEGE_DEBUG_CONTACT_GENERATION
//        drawAABB({ closest_points[0] - 0.05, closest_points[0] + 0.0 });
//        drawAABB({ closest_points[1] - 0.05, closest_points[1] + 0.0 });
//        drawAABB({ closest_surface_points[0] - 0.05, closest_surface_points[0] + 0.05 });
//        drawAABB({ closest_surface_points[1] - 0.05, closest_surface_points[1] + 0.05 });
#endif

        if( count )
        {
            CollisionManifold* collision = collisions.generate();
            collision->contact_count = count;
            collision->objects[0] = a;
            collision->objects[1] = b;
            collision->normal = n[0];
            for (int i=0; i<count; ++i)
            {
                collision->contacts[i].point = intersections[ i ];
                collision->contacts[i].depth = penetration[ i ];
            }
        }

        return false;
    }

}
