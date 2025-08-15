//
//  circle-vs-circle.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/22/25.
//

#include "circle-vs-circle.hpp"

namespace kege::algo{

    bool circleCircleIntersection(const Circle& a, const Circle& b, vec3& normal, vec3& point, float& depth)
    {
        vec3 v = cross(a.normal, b.normal);

        Plane p1 = Plane(a.normal, a.center);
        Plane p2 = Plane(b.normal, b.center);
        Plane p3 = Plane(v, 0.5 * (a.center + b.center));
        /**
         * A point of intersection exists where all three planes meet. This single point
         * lies simultaneously on all three planes.
         */
        intersectThreePlanes(p1, p2, p3, point);

        /**
         * Once we've found the point where all three planes intersect, we need to check if
         * this point lies on the circles. Since both the point and the circles lie on the same
         * planes (the plane containing the circles), this becomes a simple 2D test.
         */
        if ( !isPointInCircle( point, a ) || !isPointInCircle( point, b ) )
        {
            return false;
        }

        Point n[2];
        n[0] = normalize(point - a.center);
        n[1] = normalize(point - b.center);

        Point closest_point[2];
        closest_point[0] = a.center + n[0] * a.radius;
        closest_point[1] = b.center + n[1] * b.radius;

        Point p[2];
        p[0] = point - closest_point[0];
        p[1] = point - closest_point[1];

        float dist[2];
        dist[0] = dot(p[0], b.normal);
        dist[1] = dot(p[1], a.normal);

        float abs_dist[2];
        abs_dist[0] = abs(dist[0]);
        abs_dist[1] = abs(dist[1]);
        if ( abs_dist[0] < abs_dist[1] && abs_dist[0] > 1e-6 )
        {
            if ( dist[0] < 0.0 )
            {
                normal = b.normal;
            }
            else
            {
                normal = -b.normal;
            }
            depth = abs_dist[0];
        }
        else
        {
            if ( dist[1] < 0.0 )
            {
                normal = -a.normal;
            }
            else
            {
                normal = a.normal;
            }
            depth = abs_dist[1];
        }
        return true;
    }


    bool circleCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const Circle* circ[2] = {a->collider->getCircle(), b->collider->getCircle()};

        float depth;
        vec3 normal, intersection;
        if( circleCircleIntersection(*circ[0], *circ[1], normal, intersection, depth) )
        {
            CollisionManifold* manifold = collisions.generate();
            manifold->contact_count = 1;
            manifold->normal = normal;
            manifold->objects[0] = a;
            manifold->objects[1] = b;

            manifold->contacts[0].point = intersection;
            manifold->contacts[0].depth = depth;

            //drawLine({ intersection, intersection + normal * 5 });
            //drawAABB({ intersection - 0.05, intersection + 0.05 });
        }
        return false;
    }


    bool circleBoxCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
    bool circleConeCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
    bool circleCapsuleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
    bool circleCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
//    bool circlePlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
    bool circleSphereCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
    bool circleMeshCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}

    bool boxCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
    bool coneCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
    bool capsuleCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
    bool cylinderCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
//    bool planeCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
    bool sphereCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
    bool meshCircleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions ){return false;}
}
