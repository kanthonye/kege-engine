//
//  plane-vs-circle.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/22/25.
//

#include "plane-vs-circle.hpp"

namespace kege::algo{

    bool planeCircleIntersection(const Plane& plane, const Circle& circle, std::vector< Intersection >& intersections )
    {
        float dst = dot(circle.center, plane.normal) - plane.distance;
        if ( dst > circle.radius )
        {
            return false;
        }


        /**
         * When we have a 3D circle and a plane, we can create a system of three planes
         * to find their intersection points:
         *
         * - First plane: The original plane given in the problem.
         * - Second plane: The plane containing the circle (which the circle lies on).
         * - Third plane: A new plane created using the circle's center and the cross
         *   product of the two existing planes' normal vectors.
         *
         * With these three planes established, we can solve for their common point of
         * intersection, which will give us the points where the 3D circle and the original
         * plane meet.
         */
        vec3 normal = cross(circle.normal, plane.normal);
        Plane p1 = plane;
        Plane p2 = Plane(circle.normal, circle.center);
        Plane p3 = Plane(normal, circle.center);
        /**
         * A point of intersection exists where all three planes meet. This single point
         * lies simultaneously on all three planes.
         */
        Point point;
        intersectThreePlanes(p1, p2, p3, point);
        /**
         * Once we've found the point where all three planes intersect, we need to check if
         * this point lies on the circle. Since both the point and the circle lie on the same
         * plane (the plane containing the circle), this becomes a simple 2D test.
         */
        if ( !isPointInCircle( point, circle ) )
        {
            if ( dst > 0.0 ) // to privent the flat circlar plane from falling through
            {
                return false;
            }
        }

        float cn_dot_pn = abs( dot(circle.normal, plane.normal) );
        if ( cn_dot_pn >= 0.999976 )
        {
            /**
             * when the circle and plane are parallel, generate multiple points to stablize
             * the circle as ti lies flat on the plane. With out multiple point the next best
             * point would be the circle center, but there was still a little wobbling. with
             * 4 points the circle lies flat anf stable.
             */
            intersections.push_back({circle.center - circle.right * circle.radius, -dst});
            intersections.push_back({circle.center + circle.right * circle.radius, -dst});
            vec3 n = cross(circle.normal, circle.right);
            intersections.push_back({circle.center - n * circle.radius, -dst});
            intersections.push_back({circle.center + n * circle.radius, -dst});
        }
        else
        {
            vec3 n = normalize( point - circle.center );
            vec3 p = circle.center + n * circle.radius;
            float d = dot(p, plane.normal);

            intersections.push_back({point, abs(d)});
        }

        return true;
    }


    bool planeCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const Plane* plane = a->collider->getPlane();
        const Circle* circle = b->collider->getCircle();

        std::vector< Intersection > intersections;
        if( planeCircleIntersection( *plane, *circle, intersections ) )
        {
            CollisionManifold* manifold = collisions.generate();
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
            return true;
        }
        return false;
    }
    
    bool circlePlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        uint32_t index = collisions.count();
        if ( planeCircleCollision( b, a, collisions ) )
        {
            collisions[index]->objects[0] = b;
            collisions[index]->objects[1] = a;
            return true;
        }
        return false;
    }

}
