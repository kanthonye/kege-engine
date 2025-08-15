//
//  collisions-3D-shapes.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#ifndef collisions_3D_shapes_hpp
#define collisions_3D_shapes_hpp

#include "primitive-3D-shapes.hpp"
namespace kege{

    bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const vec3& axis);
    Interval getInterval(const AABB& rect, const vec3& axis);
    Interval getInterval(const OBB& rect, const vec3& axis);
    bool AABBOBB(const AABB& aabb, const OBB& obb);
    #define OBBAABB(obb, aabb) AABBOBB(aabb, obb)


    Interval getInterval(const Triangle& triangle, const vec3& axis);
    Interval getInterval(const AABB& aabb, const vec3& axis);
    Interval getInterval(const OBB& obb, const vec3& axis);

    bool OverlapOnAxis(const AABB& aabb, const Triangle& triangle, const vec3& axis);
    Interval getInterval(const Triangle& triangle, vec3& axis);
    bool testTriangleAABB(const Triangle& t, const AABB& a);
    #define testAABBTriangle(aabb, triangle) testTriangleAABB(triangle, aabb)

    bool OverlapOnAxis(const OBB& obb, const Triangle& triangle, const vec3& axis);
    bool testTriangleOBB(const Triangle& t, const OBB& o);
    #define testOBBTriangle(obb, triangle) testTriangleOBB(triangle, obb)

    bool testTrianglePlane(const Triangle& t, const Plane& p);
    #define testPlaneTriangle(plane, triangle) testTrianglePlane(triangle, plane)



    int testAABBAABB( const AABB& a, const AABB& b );

    int testSphereSphere(Sphere a, Sphere b);

    int testOBBOBB( const OBB& a, const OBB& b );


    bool testPointSphere(const Point& point, const Sphere& sphere, vec3& n, float& dist_sq);
    bool testPointSphere(const Point& point, const Sphere& sphere);

    bool testPointAABB( const Point& point, const AABB& aabb );

    bool testPointPlane(const Point& point, const Plane& plane);

    bool testPointOnLine(const Point& point, const Line& line);

    bool PointOnRay(const Point& point, const Ray& ray);

    bool testPointOBB(const Point& point, const OBB& obb);

    // Test if point P lies inside the counterclockwise triangle ABC
    bool testPointTriangle( const Point& p, const Triangle& t );

    // Determine whether sphere s is fully behind (inside negative halfspace of) plane p
     int testSpherePlane( const Sphere& sphere, const Plane& plane );

    // Determine whether sphere s intersects negative halfspace of plane p
    int testSphereHalfspace( const Sphere& sphere, const Plane& plane );

    // Returns true if sphere s intersects AABB b, false otherwise
     int testSphereAABB( const Sphere& sphere, const AABB& box );

    // Returns true if sphere s intersects AABB b, false otherwise.
    // The point p on the AABB closest to the sphere center is also returned
    int testSphereAABB( const Sphere& sphere, const AABB& box, Point &p );

    //int testSphereCapsule( const Sphere& sphere, const Capsule& capsule);

    // Returns true if sphere s intersects OBB b, false otherwise.
    // The point p on the OBB closest to the sphere center is also returned
    int testSphereOBB( const Sphere& sphere, const OBB& box, Point &p );

    // Returns true if sphere s intersects triangle ABC, false otherwise.
    // The point p on abc closest to the sphere center is also returned
    int testSphereTriangle( const Sphere& sphere, const Point& a, const Point& b, const Point& c, Point &p );

    // Test if OBB b intersects plane p
    int testPlaneOBB(const Plane& plane, const OBB& box);

    // Test if AABB b intersects plane p
    int testPlaneAABB(const Plane& plane, const AABB& box);

    int testTriangleAABB( Point& v0, Point& v1, Point& v2, const AABB& box);

    int intersectPlaneLine(Plane plane, const Line& line, float &t, Point &q);

    // Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
    // returns t value of intersection and intersection point q
    int intersectRaySphere( const Ray& ray, const Sphere& sphere, float &t, Point &q);

    // Intersect ray R(t) = p + t*d against AABB a. When intersecting,
    // return intersection distance tmin and point q of intersection
    int intersectRayAABB(const Point& origin, const vec3& direction, const AABB& a, float &tmin, Point &q);

    // Test if segment specified by points p0 and p1 intersects AABB b
    int testLineAABB(const Line& line, const AABB& b);

    // Given segment pq and triangle abc, returns whether segment intersects
    // triangle and if so, also returns the barycentric coordinates (u,v,w)
    // of the intersection point
    int intersectLineTriangle( const Line& line, const Triangle& triangle, float &u, float &v, float &w, float &t);

    // Intersect segment S(t)=sa+t(sb-sa), 0<=t<=1 against cylinder specified by p, q and r
    int intersectLineCylinder( const Line& line, const Cylinder& cylinder, float& t );

    // Intersect segment S(t)=A+t(B-A), 0<=t<=1 against convex polyhedron specified
    // by the n halfspaces defined by the planes p[]. On exit tfirst and tlast
    // define the intersection, if any
    int intersectLinePolyhedron(Point a, Point b, Plane p[], int n, float &tfirst, float &tlast);

    // Given planes p1 and p2, compute line L = p+t*d of their intersection.
    // Return 0 if no such line exists
    int intersectPlanes(Plane p1, Plane p2, Point &p, vec3 &d);

    // Compute the point p at which the three planes p1, p2 and p3 intersect (if at all)
    int intersectPlanes(Plane p1, Plane p2, Plane p3, Point &p);

    // Intersect sphere s with movement vector v with plane p. If intersecting
    // return time t of collision and point q at which sphere hits plane
    int intersectMovingSpherePlane( const Sphere& s, const vec3& v, const Plane& p, float &t, Point &q);

    // Test if sphere with radius r moving from a to b intersects with plane p
    int testMovingSpherePlane(Point a, Point b, float r, Plane p);

    int testMovingSphereSphere(Sphere s0, Sphere s1, vec3 v0, vec3 v1, float &t);

    // Intersect AABBs ‘a’ and ‘b’ moving with constant velocities va and vb.
    // On intersection, return time of first and last contact in tfirst and tlast
    int intersectMovingAABBAABB(AABB a, AABB b, vec3 va, vec3 vb, float &tfirst, float &tlast);



    float raycast(const Ray& ray, const Sphere& sphere);
    float raycast(const Ray& ray, const AABB& aabb);

    bool RayIntersectsOBB(const Ray& ray, const OBB& obb);
    float raycast(const Ray& ray, const OBB& obb);
    float raycast(const Ray& ray, const Plane& plane);
    bool linetest( const Line& line, const Sphere& sphere);
    bool linetest(const Line& line, const AABB& aabb);
    bool linetest(const OBB& obb, const Line& line);
    bool linetest(const Line& line, const Plane& plane);
}
#endif /* collisions_3D_shapes_hpp */


