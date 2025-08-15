//
//  collisions-3D-shapes.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#include "collisions-3D-shapes.hpp"
namespace kege{

    float length(const Line& line)
    {
       return magn(line.end - line.start);
    }

    float lengthSq(const Line& line)
    {
       return magnSq(line.end - line.start);
    }

    int testAABBAABB( const AABB& a, const AABB& b )
    {
        if (a.max.x < b.min.x || a.min.x > b.max.x) return 0;
        if (a.max.y < b.min.y || a.min.y > b.max.y) return 0;
        if (a.max.z < b.min.z || a.min.z > b.max.z) return 0;
        return 1;
    }

    int testSphereSphere(Sphere a, Sphere b)
    {
        // Calculate squared distance between centers
        float dist2 = magnSq( a.center - b.center );
        // if squared distance is less than squared sum Spheres intersect
        float radius_sum = a.radius + b.radius;
        return dist2 <= radius_sum * radius_sum;
    }


    int testOBBOBB( const OBB& a, const OBB& b )
    {
        float ra, rb;
        mat33 R, AbsR;
        // Compute rotation matrix expressing b in a’s coordinate frame
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                R[i][j] = dot(a.axes[i], b.axes[j]);

        // Compute translation vector t
        vec3 t = b.center - a.center;
        // Bring translation into a’s coordinate frame
        t = vec3(dot(t, a.axes[0]), dot(t, a.axes[1]), dot(t, a.axes[2]));

        // Compute common subexpressions. Add in an epsilon term to
        // counteract arithmetic errors when two edges are parallel and
        // their cross product is (near) null (see text for details)
        for (int i = 0; i < 3; i++)
           for (int j = 0; j < 3; j++)
               AbsR[i][j] = abs( R[i][j] ) + KEGE_EPSILON_F;

        // Test axes L = A0, L = A1, L = A2
        for (int i = 0; i < 3; i++)
        {
            ra = a.extents[i];
            rb = b.extents[0] * AbsR[i][0] + b.extents[1] * AbsR[i][1] + b.extents[2] * AbsR[i][2];
            if ( abs(t[i]) > ra + rb) return 0;
        }

        // Test axes L = B0, L = B1, L = B2
        for (int i = 0; i < 3; i++)
        {
            ra = a.extents[0] * AbsR[0][i] + a.extents[1] * AbsR[1][i] + a.extents[2] * AbsR[2][i];
            rb = b.extents[i];
            if ( abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb ) return 0;
        }

        // Test axis L = A0 x B0
        ra = a.extents[1] * AbsR[2][0] + a.extents[2] * AbsR[1][0];
        rb = b.extents[1] * AbsR[0][2] + b.extents[2] * AbsR[0][1];
        if ( abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb ) return 0;

        // Test axis L = A0 x B1
        ra = a.extents[1] * AbsR[2][1] + a.extents[2] * AbsR[1][1];
        rb = b.extents[0] * AbsR[0][2] + b.extents[2] * AbsR[0][0];
        if ( abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb ) return 0;

        // Test axis L = A0 x B2
        ra = a.extents[1] * AbsR[2][2] + a.extents[2] * AbsR[1][2];
        rb = b.extents[0] * AbsR[0][1] + b.extents[1] * AbsR[0][0];
        if ( abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb ) return 0;

        // Test axis L = A1 x B0
        ra = a.extents[0] * AbsR[2][0] + a.extents[2] * AbsR[0][0];
        rb = b.extents[1] * AbsR[1][2] + b.extents[2] * AbsR[1][1];
        if ( abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb ) return 0;

        // Test axis L = A1 x B1
        ra = a.extents[0] * AbsR[2][1] + a.extents[2] * AbsR[0][1];
        rb = b.extents[0] * AbsR[1][2] + b.extents[2] * AbsR[1][0];
        if ( abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb ) return 0;

        // Test axis L = A1 x B2
        ra = a.extents[0] * AbsR[2][2] + a.extents[2] * AbsR[0][2];
        rb = b.extents[0] * AbsR[1][1] + b.extents[1] * AbsR[1][0];
        if ( abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb ) return 0;

        // Test axis L = A2 x B0
        ra = a.extents[0] * AbsR[1][0] + a.extents[1] * AbsR[0][0];
        rb = b.extents[1] * AbsR[2][2] + b.extents[2] * AbsR[2][1];
        if ( abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb ) return 0;

        // Test axis L = A2 x B1
        ra = a.extents[0] * AbsR[1][1] + a.extents[1] * AbsR[0][1];
        rb = b.extents[0] * AbsR[2][2] + b.extents[2] * AbsR[2][0];
        if ( abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb ) return 0;

        // Test axis L = A2 x B2
        ra = a.extents[0] * AbsR[1][2] + a.extents[1] * AbsR[0][2];
        rb = b.extents[0] * AbsR[2][1] + b.extents[1] * AbsR[2][0];
        if ( abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb ) return 0;

        // The OBBs must be intersecting since no separating axis is found
        return 1;
    }

    bool testPointSphere(const Point& point, const Sphere& sphere, vec3& n, float& dist_sq)
    {
        dist_sq = magnSq((n = point - sphere.center));
        return dist_sq <= sphere.radius * sphere.radius;
    }

    bool testPointSphere(const Point& point, const Sphere& sphere)
    {
        float mag_sq = magnSq(point - sphere.center );
        float rad_sq = sphere.radius * sphere.radius;
        return mag_sq <= rad_sq;
    }

    bool testPointAABB( const Point& point, const AABB& aabb )
    {
        if ( point.x < aabb.min.x || point.y < aabb.min.y || point.z < aabb.min.z ) return false;
        if ( point.x > aabb.max.x || point.y > aabb.max.y || point.z > aabb.max.z ) return false;
        return true;
    }

    bool testPointPlane(const Point& point, const Plane& plane)
    {
        float d = dot( point, plane.normal );
        // To make this more robust, use an epsilon check
        // The CMP macro performs epsilon tests:”
        // CMP(dot - plane.distance, 0.0f)
        return d - plane.distance == 0.0f;
    }

    bool testPointOnLine(const Point& point, const Line& line)
    {
        Point closest = closestPointLine(point, line);
        float distanceSq = magnSq(closest - point);
        return distanceSq == 0.0f;
    }

    bool testPointRay(const Point& point, const Ray& ray)
    {
        if (point == ray.origin) return true;

        vec3 norm = point - ray.origin;
        normalized(norm);

        float diff = dot(norm, ray.direction);

        // If BOTH vectors point in the same direction,
        // their dot product (diff) should be 1
        return diff == 1.0f; // Consider using epsilon!
    }

    bool testPointOBB(const Point& point, const OBB& obb)
    {
        float distance;
        vec3 dir = obb.center - point;

        distance = abs( dot(dir, obb.axes[0]) );
        if (distance - obb.extents[0] > 0.000005 ) return false;

        distance = abs( dot(dir, obb.axes[1]) );
        if (distance - obb.extents[1] > 0.000005 ) return false;

        distance = abs( dot(dir, obb.axes[2]) );
        if (distance - obb.extents[2] > 0.000005  ) return false;

        return true;
    }

    // Determine whether sphere s is fully behind (inside negative halfspace of) plane p
     int testSpherePlane( const Sphere& sphere, const Plane& plane )
     {
         float dist = dot(sphere.center, plane.normal) - plane.distance;
         return dist < -sphere.radius;
     }

    // Determine whether sphere s intersects negative halfspace of plane p
    int testSphereHalfspace( const Sphere& sphere, const Plane& plane )
    {
        float dist = dot(sphere.center, plane.normal) - plane.distance;
        return dist <= sphere.radius;
    }

    // Returns true if sphere s intersects AABB b, false otherwise
     int testSphereAABB( const Sphere& sphere, const AABB& box )
     {
        // Compute squared distance between sphere center and AABB
        float sq_dist = sqDistPointAABB( sphere.center, box );
        // Sphere and AABB intersect if the (squared) distance
        // between them is less than the (squared) sphere radius
        return sq_dist <= sphere.radius * sphere.radius;
     }

    // Returns true if sphere s intersects AABB b, false otherwise.
    // The point p on the AABB closest to the sphere center is also returned
    int testSphereAABB( const Sphere& sphere, const AABB& box, Point &p )
    {
        //I Find point p on AABB closest to sphere center
        closestPointAABB( sphere.center, box, p );
        // Sphere and AABB intersect if the (squared) distance from sphere
        //l center to point p is less than the (squared) sphere radius
        vec3 v = p - sphere.center;
        return dot(v, v) < sphere.radius * sphere.radius;
    }

//    int testSphereCapsule( const Sphere& sphere, const Capsule& capsule)
//    {
//        // Compute (squared) distance between sphere center and capsule line segment
//        float dist2 = sqDistPointSegment(capsule.line.start, capsule.line.end, sphere.center);
//        // If (squared) distance smaller than (squared) sum of radii, they collide
//        float radius = sphere.radius + capsule.radius;
//        return dist2 <= radius * radius;
//    }

    // Returns true if sphere s intersects OBB b, false otherwise.
    // The point p on the OBB closest to the sphere center is also returned
    int testSphereOBB( const Sphere& sphere, const OBB& box, Point &p )
    {
        // Find point p on OBB closest to sphere center
        p = closestPointOBB( sphere.center, box );
        // Sphere and OBB intersect if the (squared) distance from sphere
        // center to point p is less than the (squared) sphere radius
        vec3 v = p - sphere.center;
        return dot(v, v) <= sphere.radius * sphere.radius;
    }

    // Returns true if sphere s intersects triangle ABC, false otherwise.
    // The point p on abc closest to the sphere center is also returned
    int testSphereTriangle( const Sphere& sphere, const Triangle& triangle, Point &p )
    {
        // Find point P on triangle ABC closest to sphere center
        p = closestPointTriangle( sphere.center, triangle );
        // Sphere and triangle intersect if the (squared) distance from sphere
        // center to point p is less than the (squared) sphere radius
        vec3 v = p - sphere.center;
        return dot(v, v) <= sphere.radius * sphere.radius;
    }


    // Test if OBB b intersects plane p
    int testPlaneOBB(const Plane& plane, const OBB& box)
    {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        float r = box.extents[0] * abs(dot(plane.normal, box.axes[0])) +
        box.extents[1] * abs( dot(plane.normal, box.axes[1]) ) +
        box.extents[2] * abs( dot(plane.normal, box.axes[2]) );

        // Compute distance of box center from plane
        float s = dot( plane.normal, box.center ) - plane.distance;
        // Intersection occurs when distance s falls within [-r,+r] interval
        return abs(s) <= r;
    }

    // Test if AABB b intersects plane p
    int testPlaneAABB(const Plane& plane, const AABB& box)
    {
        // These two lines not necessary with a (center, extents) AABB representation 
        Point c = (box.max + box.min) * 0.5f; // Compute AABB center
        Point e = box.max - c; // Compute positive extents

        // Compute the projection interval radius of b onto L(t) = box.center + t * plane.normal
        float r = e[0] * abs(plane.normal[0]) + e[1] * abs(plane.normal[1]) + e[2] * abs(plane.normal[2]);

        // Compute distance of box center from plane
        float s = dot(plane.normal, c) - plane.distance;

        // Intersection occurs when distance s falls within [-r,+r] interval
        return abs(s) <= r;
    }





    // Test if ray r = p + td intersects sphere s
    int testRaySphere( const Ray& ray, Sphere sphere )
    {
        vec3 m = ray.origin - sphere.center;
        float c = dot(m, m) - sphere.radius * sphere.radius;

        // If there is definitely at least one real root, there must be an intersection
        if (c <= 0.0f) return 1;
        float b = dot(m, ray.direction);

        // Early exit if ray origin outside sphere and ray pointing away from sphere
        if (b > 0.0f) return 0;
        float disc = b*b - c;

        // A negative discriminant corresponds to ray missing sphere
        if (disc < 0.0f) return 0;

        // Now ray must hit sphere
        return 1;
    }

    int testTriangleAABB( Point& v0, Point& v1, Point& v2, const AABB& box)
    {
        float p0, p1, p2, r;
        // Compute box center and extents (if not already given in that format)
        vec3 c = (box.min + box.max) * 0.5f;
        float e0 = (box.max.x - box.min.x) * 0.5f;
        float e1 = (box.max.y - box.min.y) * 0.5f;
        float e2 = (box.max.z - box.min.z) * 0.5f;

        // Translate triangle as conceptually moving AABB to origin
        v0 = v0 - c;
        v1 = v1 - c;
        v2 = v2 - c;

        // Compute edge vectors for triangle
        vec3 f0=v1 - v0, f1 = v2 - v1, f2 = v0 - v2;
        // Test axes a00..a22 (category 3)
        // Test axis a00
        p0 = v0.z*v1.y - v0.y*v1.z;
        p2 = v2.z*(v1.y - v0.y) - v2.z*(v1.z - v0.z); r = e1 * abs(f0.z) + e2 * abs(f0.y);
        if ( max( -max(p0, p2), min(p0, p2) ) > r) return 0; // Axis is a separating axis

        // Repeat similar tests for remaining axes a01..a22...

        // Test the three axes corresponding to the face normals of AABB b (category 1). 
        // Exit if...
        // ... [-e0, e0] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
        if (max(v0.x, v1.x, v2.x) < -e0 || min(v0.x, v1.x, v2.x) > e0) return 0;
        // ... [-e1, e1] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
        if (max(v0.y, v1.y, v2.y) < -e1 || min(v0.y, v1.y, v2.y) > e1) return 0;
        // ... [-e2, e2] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
        if (max(v0.z, v1.z, v2.z) < -e2 || min(v0.z, v1.z, v2.z) > e2) return 0;

        // Test separating axis corresponding to triangle face normal (category 2)
        Plane plane;
        plane.normal = cross(f0, f1);
        plane.distance = dot( plane.normal, v0 );
        return testPlaneAABB( plane, box );
    }



    int intersectPlaneLine(Plane plane, const Line& line, float &t, Point &q)
    {
        // Compute the t value for the directed line ab intersecting the plane
        vec3 ab = line.end - line.start;
        t = (plane.distance - dot(plane.normal, line.start)) / dot(plane.normal, ab);

        // If t in [0..1] compute and return intersection point
        if (t >= 0.0f && t <= 1.0f)
        {
            q = line.start + t * ab;
            return 1;
        }
        // Else no intersection
        return 0;
    }

    // Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
    // returns t value of intersection and intersection point q
    int intersectRaySphere( const Ray& ray, const Sphere& sphere, float &t, Point &q)
    {
        vec3 m = ray.origin - sphere.center;
        float b = dot(m, ray.direction);
        float c = dot(m, m) - sphere.radius * sphere.radius;

        // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
        if (c > 0.0f && b > 0.0f) return 0;
        float discr = b*b - c;

        // A negative discriminant corresponds to ray missing sphere
        if (discr < 0.0f) return 0;

        // Ray now found to intersect sphere, compute smallest t value of intersection t = -b - Sqrt(discr);
        // If t is negative, ray started inside sphere so clamp t to zero
        if (t < 0.0f) t = 0.0f;
        q = ray.origin + t * ray.direction;
        return 1;
    }


    // Intersect ray R(t) = p + t*d against AABB a. When intersecting,
    // return intersection distance tmin and point q of intersection
    int intersectRayAABB(const Point& origin, const vec3& direction, const AABB& a, float &tmin, Point &q)
    {
        tmin = 0.0f; // set to -FLT_MAX to get first hit on line
        float tmax = FLT_MAX; // set to max distance ray can travel (for segment)
                              // For all three slabs
        for (int i = 0; i < 3; i++) 
        {
            if ( abs( direction[i] ) < KEGE_EPSILON_F )
            {
                // Ray is parallel to slab. No hit if origin not within slab
                if ( origin[i] < a.min[i] || origin[i] > a.max[i] ) return 0;
            }
            else
            {
                // Compute intersection t value of ray with near and far plane of slab
                float ood = 1.0f / direction[i];
                float t1 = (a.min[i] - origin[i]) * ood;
                float t2 = (a.max[i] - origin[i]) * ood;

                // Make t1 be intersection with near plane, t2 with far plane
                if (t1 > t2) std::swap(t1, t2);

                // Compute the intersection of slab intersection intervals
                if (t1 > tmin) tmin = t1;
                if (t2 > tmax) tmax = t2;

                // Exit with no collision as soon as slab intersection becomes empty
                if (tmin > tmax) return 0;
            }
        }
        // Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
        q = origin + direction * tmin;
        return 1;
    }

    // Test if segment specified by points p0 and p1 intersects AABB b
    int testLineAABB(const Line& line, const AABB& b)
    {
        Point c = (b.min + b.max) * 0.5f;
        vec3 e = b.max - c;
        Point m = (line.start + line.end) * 0.5f;
        vec3 d = line.end - m;
        m = m - c;

        float adx = abs(d.x);
        if (abs(m.x) > e.x + adx) return 0;

        float ady = abs(d.y);
        if (abs(m.y) > e.y + ady) return 0;

        float adz = abs(d.z);
        if (abs(m.z) > e.z + adz) return 0;
        // Add in an epsilon term to counteract arithmetic errors when segment is
        // (near) parallel to a coordinate axis (see text for detail)
        adx += KEGE_EPSILON_F; ady += KEGE_EPSILON_F; adz += KEGE_EPSILON_F;
        // Try cross products of segment direction vector with coordinate axes
        if (abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) return 0;
        if (abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) return 0;
        if (abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) return 0;

        // No separating axis found; segment must be overlapping AABB
        return 1;
    }

    // Given segment pq and triangle abc, returns whether segment intersects
    // triangle and if so, also returns the barycentric coordinates (u,v,w)
    // of the intersection point
    int intersectLineTriangle( const Line& line, const Triangle& triangle, float &u, float &v, float &w, float &t)
    {
        vec3 ab = triangle.b - triangle.a;
        vec3 ac = triangle.c - triangle.a;
        vec3 qp = line.start - line.end;
        // Compute triangle normal. Can be precalculated or cached if
        // intersecting multiple segments against the same triangle
        vec3 n = cross(ab, ac);
        // Compute denominator d. If d <= 0, segment is parallel to or points
        // away from triangle, so exit early
        float d = dot(qp, n);
        if (d <= 0.0f) return 0;

        // Compute intersection t value of pq with plane of triangle. A ray
        // intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
        // dividing by d until intersection has been found to pierce triangle
        vec3 ap = line.start - triangle.a;
        t = dot(ap, n);
        if (t < 0.0f) return 0;
        if (t > d) return 0; // For segment; exclude this code line for a ray test
                            
        // Compute barycentric coordinate components and test if within bounds
        vec3 e = cross(qp, ap);
        v = dot(ac, e);
        if (v < 0.0f || v > d) return 0;

        w = -dot(ab, e);
        if (w < 0.0f || v + w > d) return 0;

        // Segment/ray intersects triangle. Perform delayed division and
        // compute the last barycentric coordinate component
        float ood = 1.0f / d;
        t *= ood;
        v *= ood;
        w *= ood;
        u = 1.0f - v - w;
        return 1;
    }


    // Intersect segment S(t)=sa+t(sb-sa), 0<=t<=1 against cylinder specified by p, q and r
    int intersectLineCylinder( const Line& line, const Cylinder& cylinder, float& t )
    {
        vec3 m = line.start - cylinder.center;
        vec3 n = line.end - line.start;

        float md = dot(m, cylinder.axes[0]);
        float nd = dot(n, cylinder.axes[0]);
        float dd = dot(cylinder.axes[0], cylinder.axes[0]);

        // Test if segment fully outside either endcap of cylinder
        if (md < 0.0f && md + nd < 0.0f) return 0;

        // Segment outside ’p’ side of cylinder
        if (md > dd && md + nd > dd) return 0;

        // Segment outside ’q’ side of cylinder 
        float nn = dot(n, n);
        float mn = dot(m, n);
        float a = dd * nn - nd * nd;
        float k = dot(m, m) - cylinder.radius * cylinder.radius;
        float c = dd * k - md * md;
        if ( abs(a) < KEGE_EPSILON_F )
        {
            // Segment runs parallel to cylinder axis
            if (c > 0.0f) return 0; // ’a’ and thus the segment lie outside cylinder
                            
            // Now known that segment intersects cylinder; figure out how it intersects
            if (md < 0.0f) t = -mn / nn; // Intersect segment against ’p’ endcap
            else if (md > dd) t = (nd - mn) / nn; // Intersect segment against ’q’ endcap 
            else t = 0.0f;

            // ’a’ lies inside cylinder
            return 1;
        }

        float b = dd * mn - nd * md;
        float discr = b * b - a * c;
        if (discr < 0.0f) return 0;

        t = (-b - sqrt(discr)) / a;
        if (t < 0.0f || t > 1.0f) return 0; // Intersection lies outside segment 

        if (md + t * nd < 0.0f)
        {
            // Intersection outside cylinder on ’p’ side
            if (nd <= 0.0f) return 0; // Segment pointing away from endcap 
            t = -md / nd;
            // Keep intersection if Dot(S(t) - p, S(t) - p) <= r∧2
            return k + 2 * t * (mn + t * nn) <= 0.0f;
        }
        else if (md + t * nd > dd)
        {
            // Intersection outside cylinder on ’q’ side
            if (nd >= 0.0f) return 0; // Segment pointing away from endcap t = (dd – md) / nd;
            // Keep intersection if Dot(S(t) - q, S(t) - q) <= r∧2
            return k + dd - 2 * md + t * (2 * (mn - nd) + t * nn) <= 0.0f;
        }
             
        // Segment intersects cylinder between the endcaps; t is correct
        return 1;
    }

    // Intersect segment S(t)=A+t(B-A), 0<=t<=1 against convex polyhedron specified 
    // by the n halfspaces defined by the planes p[]. On exit tfirst and tlast
    // define the intersection, if any
    int intersectLinePolyhedron(Point a, Point b, Plane p[], int n, float &tfirst, float &tlast)
    {
        // Compute direction vector for the segment
        vec3 d = b - a;
        // Set initial interval to being the whole segment. For a ray, tlast should be
        // set to +FLT_MAX. For a line, additionally tfirst should be set to –FLT_MAX tfirst = 0.0f;
        tlast = 1.0f;
        // Intersect segment against each plane
        for (int i = 0; i < n; i++) 
        {
            float denom = dot(p[i].normal, d);
            float dist = p[i].distance - dot(p[i].normal, a);
            // Test if segment runs parallel to the plane
            if (denom == 0.0f) 
            {
                // If so, return “no intersection” if segment lies outside plane
                if (dist > 0.0f) return 0;
            } else 
            {
                // Compute parameterized t value for intersection with current plane
                float t = dist / denom;
                if (denom < 0.0f) 
                {
                    // When entering halfspace, update tfirst if t is larger
                    if (t > tfirst) tfirst = t;
                } 
                else
                {
                    // When exiting halfspace, update tlast if t is smaller
                    if (t < tlast) tlast = t;
                }
                // Exit with “no intersection” if intersection becomes empty
                if (tfirst > tlast) return 0;
            }
        }
        // A nonzero logical intersection, so the segment intersects the polyhedron
        return 1;
    }

    // Given planes p1 and p2, compute line L = p+t*d of their intersection. 
    // Return 0 if no such line exists
    int intersectPlanes(Plane p1, Plane p2, Point &p, vec3 &d)
    {
        // Compute direction of intersection line
        d = cross( p1.normal, p2.normal );
        // If d is (near) zero, the planes are parallel (and separated)
        // or coincident, so they’re not considered intersecting
        float denom = dot(d, d);
        if (denom < KEGE_EPSILON_F) return 0;
        // Compute point on intersection line
        p = cross(p1.distance * p2.normal - p2.distance * p1.normal, d) / denom;
        return 1;
    }

    // Compute the point p at which the three planes p1, p2 and p3 intersect (if at all)
    int intersectPlanes(Plane p1, Plane p2, Plane p3, Point &p)
    {
        vec3 u = cross(p2.normal, p3.normal);
        float denom = dot(p1.normal, u);
        if ( abs(denom) < KEGE_EPSILON_F ) return 0; // Planes do not intersect in a point
        p = (p1.distance * u + cross(p1.normal, p3.distance * p2.normal - p2.distance * p3.normal)) / denom;
        return 1;
    }



    // Intersect sphere s with movement vector v with plane p. If intersecting
    // return time t of collision and point q at which sphere hits plane
    int intersectMovingSpherePlane( const Sphere& s, const vec3& v, const Plane& p, float &t, Point &q)
    {
        // Compute distance of sphere center to plane
        float dist = dot(p.normal, s.center) - p.distance;
        if ( abs(dist) <= s.radius )
        {
            // The sphere is already overlapping the plane. Set time of
            // intersection to zero and q to sphere center
            t = 0.0f;
            q = s.center;
            return 1;
        } 
        else
        {
            float denom = dot(p.normal, v);
            if (denom * dist >= 0.0f)
            {
                // No intersection as sphere moving parallel to or away from plane
                return 0;
            } 
            else
            {
                // Sphere is moving towards the plane
                // Use +r in computations if sphere in front of plane, else -r
                float r = dist > 0.0f ? s.radius : -s.radius;
                t = (r - dist) / denom;
                q = s.center + t * v - r * p.normal;
                return 1;
            }
        }
    }
    // Test if sphere with radius r moving from a to b intersects with plane p
    int testMovingSpherePlane(Point a, Point b, float r, Plane p)
    {
        // Get the distance for both a and b from plane p
        float adist = dot(a, p.normal) - p.distance;
        float bdist = dot(b, p.normal) - p.distance;

        // Intersects if on different sides of plane (distances have different signs)
        if (adist * bdist < 0.0f) return 1;

        // Intersects if start or end position within radius from plane
        if ( abs(adist) <= r || abs(bdist) <= r ) return 1;

        // No intersection
        return 0;
    }

    int testMovingSphereSphere(Sphere s0, Sphere s1, vec3 v0, vec3 v1, float &t)
    {
        // Subtract movement of s1 from both s0 and s1, making s1 stationary
        vec3 v = v0 - v1;

        // Can now test directed segment s = s0.c + tv, v = (v0-v1)/||v0-v1|| against
        // the expanded sphere for intersection
        float vlen = magn(v);

        // Expand sphere s1 by the radius of s0
        Sphere s = {s1.center, s1.radius + s0.radius};
        Ray ray = {s0.center, v / vlen};
        Point p;

        if ( intersectRaySphere( ray, s, t, p ) )
        {
            return t <= vlen;
        }
        return 0;
    }


    // Intersect AABBs ‘a’ and ‘b’ moving with constant velocities va and vb.
    // On intersection, return time of first and last contact in tfirst and tlast
    int intersectMovingAABBAABB(AABB a, AABB b, vec3 va, vec3 vb, float &tfirst, float &tlast)
    {
        if ( testAABBAABB(a, b) )
        {
            tfirst = tlast = 0.0f;
           return 1;
        }

        // Use relative velocity; effectively treating ’a’ as stationary
        vec3 v = vb - va;
        // Initialize times of first and last contact
        tfirst = 0.0f;
        tlast = 1.0f;
        // For each axis, determine times of first and last contact, if any
        for (int i = 0; i < 3; i++)
        {
            if (v[i] < 0.0f)
            {
                if (b.max[i] < a.min[i]) return 0; // Nonintersecting and moving apart
                if (a.max[i] < b.min[i]) tfirst = max((a.max[i] - b.min[i]) / v[i], tfirst);
                if (b.max[i] > a.min[i]) tlast = min((a.min[i] - b.max[i]) / v[i], tlast);
            }
            if (v[i] > 0.0f)
            {
                if (b.min[i] > a.max[i]) return 0; // Nonintersecting and moving apart
                if (b.max[i] < a.min[i]) tfirst = max((a.min[i] - b.max[i]) / v[i], tfirst);
                if (a.max[i] > b.min[i]) tlast = min((a.max[i] - b.min[i]) / v[i], tlast);
            }
            // No overlap possible if time of first contact occurs after time of last contact
           if (tfirst > tlast) return 0;
        }
        return 1;
    }

    // Support function that returns the AABB vertex with index n
     Point Corner(AABB b, int n)
     {
         Point p;
         p.x = ((n & 1) ? b.max.x : b.min.x);
         p.y = ((n & 1) ? b.max.y : b.min.y);
         p.z = ((n & 1) ? b.max.z : b.min.z);
         return p;
    }




    //// Test collision between objects a and b moving over the time interval
    //// [startTime, endTime]. When colliding, time of collision is returned in hitTime
    //int IntervalCollision(Object a, Object b, float start_time, float end_time, float &hit_time)
    //{
    //    // Compute the maximum distance objects a and b move over the time interval
    //    float maxMoveA = MaximumObjectMovementOverTime(a, start_time, end_time);
    //    float maxMoveB = MaximumObjectMovementOverTime(b, start_time, end_time);
    //    float maxMoveDistSum = maxMoveA + maxMoveB;
    //    // Exit if distance between a and b at start larger than sum of max movements float minDistStart = MinimumObjectDistanceAtTime(a, b, startTime);
    //    if (minDistStart > maxMoveDistSum) return 0;
    //    // Exit if distance between a and b at end larger than sum of max movements
    //    float minDistEnd = MinimumObjectDistanceAtTime(a, b, end_time);
    //    if (minDistEnd > maxMoveDistSum) return 0;
    //    // Cannot rule collision out: recurse for more accurate testing. To terminate the // recursion, collision is assumed when time interval becomes sufficiently small if (endTime – startTime < INTERVAL_EPSILON) {
    //    hit_time = start_time;
    //    return 1; }
    //    // Recursively test first half of interval; return collision if detected
    //    float midTime = (startTime + endTime) * 0.5f;
    //    if (IntervalCollision(a, b, startTime, midTime, hitTime)) return 1; // Recursively test second half of interval
    //    return IntervalCollision(a, b, midTime, endTime, hitTime);
    //}


    //// Test whether sphere s intersects polygon p
    // int testSpherePolygon( const Sphere& sphere, const Polygon& polygon)
    //{
    //     // Compute normal for the plane of the polygon
    //     vec3 n = normalize( cross( polygon.points[1] - polygon.points[0], polygon.points[2] - polygon.points[0] ) );
    //
    //     // Compute the plane equation for p
    //     Plane plane;
    //     plane.normal = n;
    //     plane.distance = -dot(n, polygon.points[0]);
    //
    //     // No intersection if sphere not intersecting plane of polygon
    //     if ( !testSpherePlane( sphere, plane ) ) return 0;
    //
    //     // Test to see if any one of the polygon edges pierces the sphere
    //     for (size_t k = polygon.points.size(), i = 0, j = k - 1; i < k; j = i, i++)
    //     {
    //         float t;
    //         Point q;
    //        // Test if edge (p.v[j], p.v[i]) intersects s
    //        if ( intersectRaySphere( polygon.points[j], polygon.points[i] - polygon.points[j], sphere, t, q ) && t <= 1.0f )
    //          return 1;
    //     }
    //    // Test if the orthogonal projection q of the sphere center onto m is inside p
    //    Point q = closestPointPlane( sphere.center, plane );
    //    return PointInPolygon(q, p);
    //}

    ////Given line pq and ccw triangle abc, return whether line pierces triangle. If
    ////so, also return the barycentric coordinates (u,v,w) of the intersection point
    //int IntersectLineTriangle(Point p, Point q, Point a, Point b, Point c, float &u, float &v, float &w)
    //{
    //    vec3 pq = q - p;
    //    vec3 pa = a - p;
    //    vec3 pb = b - p;
    //    vec3 pc = c - p;
    //    // Test if pq is inside the edges bc, ca and ab. Done by testing
    //    // that the signed tetrahedral volumes, computed using scalar triple
    //    // products, are all positive
    //    u = scalarTriple(pq, pc, pb);
    //    if (u < 0.0f) return 0;
    //    v = scalarTriple(pq, pa, pc);
    //    if (v < 0.0f) return 0;
    //    w = scalarTriple(pq, pb, pa);
    //    if (w < 0.0f) return 0;
    //    // Compute the barycentric coordinates (u, v, w) determining the
    //    // intersection point r, r = u*a + v*b + w*c
    //    float denom = 1.0f / (u + v + w);
    //    u *= denom;
    //    v *= denom;
    //    w *= denom; // w = 1.0f - u - v; return 1;
    //}


    //// Given line pq and ccw quadrilateral abcd, return whether the line
    //// pierces the triangle. If so, also return the point r of intersection
    //int IntersectLineQuad(Point p, Point q, Point a, Point b, Point c, Point d, Point &r)
    //{
    //    vec3 pq = q - p;
    //    vec3 pa = a - p;
    //    vec3 pb = b - p;
    //    vec3 pc = c - p;
    //// Determine which triangle to test against by testing against diagonal first Vector m = Cross(pc, pq);
    //float v = Dot(pa, m); // ScalarTriple(pq, pa, pc); if (v >= 0.0f) {
    //// Test intersection against triangle abc
    //float u = -Dot(pb, m); // ScalarTriple(pq, pc, pb);
    //if (u < 0.0f) return 0;
    //float w = ScalarTriple(pq, pb, pa);
    //if (w < 0.0f) return 0;
    //// Compute r, r = u*a + v*b + w*c, from barycentric coordinates (u, v, w) float denom = 1.0f / (u + v + w);
    //u *= denom;
    //v *= denom;
    //w *= denom; // w = 1.0f - u - v;
    //r = u*a + v*b + w*c;
    //} else {
    //        // Test intersection against triangle dac
    //Vector pd = d - p;
    //float u = Dot(pd, m); // ScalarTriple(pq, pd, pc);
    //if (u < 0.0f) return 0;
    //float w = ScalarTriple(pq, pa, pd);
    //if (w < 0.0f) return 0;
    //v = -v;
    //// Compute r, r = u*a + v*d + w*c, from barycentric coordinates (u, v, w) float denom = 1.0f / (u + v + w);
    //u *= denom;
    //v *= denom;
    //w *= denom; // w = 1.0f - u - v;
    //r = u*a + v*d + w*c;
    //}
    //return 1;
    //}


    int testPointPolyhedron(Point p, Plane *h, int n)
    {
        for (int i = 0; i < n; i++)
        {
            if ( distPointPlane(p, h[i]) > 0.0f) return 0;
        }
        return 1;
    }



    Interval getInterval(const AABB& aabb, const vec3& axis)
    {
        vec3 vertex[8] = 
        {
            vec3(aabb.min.x, aabb.max.y, aabb.max.z),
            vec3(aabb.min.x, aabb.max.y, aabb.min.z),
            vec3(aabb.min.x, aabb.min.y, aabb.max.z),
            vec3(aabb.min.x, aabb.min.y, aabb.min.z),
            vec3(aabb.max.x, aabb.max.y, aabb.max.z),
            vec3(aabb.max.x, aabb.max.y, aabb.min.z),
            vec3(aabb.max.x, aabb.min.y, aabb.max.z),
            vec3(aabb.max.x, aabb.min.y, aabb.min.z)
        };

        Interval result;
        result.min = result.max = dot(axis, vertex[0]);

        for (int i = 1; i < 8; ++i) 
        {
            float projection = dot(axis, vertex[i]);
            result.min = (projection < result.min) ? projection : result.min;
            result.max = (projection > result.max) ? projection : result.max;
        }

        return result;
    }

    Interval getInterval(const OBB& obb, const vec3& axis) 
    {
        vec3 vertex[8];
        vec3 C = obb.center;    // OBB Center
        vec3 E = obb.extents;    // OBB Extents

        vec3 A[] =
        {
            obb.axes[ 0 ],
            obb.axes[ 1 ],
            obb.axes[ 2 ]
        };

        vertex[0] = C + A[0]*E[0] + A[1]*E[1] + A[2]*E[2];
        vertex[1] = C - A[0]*E[0] + A[1]*E[1] + A[2]*E[2];
        vertex[2] = C + A[0]*E[0] - A[1]*E[1] + A[2]*E[2];
        vertex[3] = C + A[0]*E[0] + A[1]*E[1] - A[2]*E[2];
        vertex[4] = C - A[0]*E[0] - A[1]*E[1] - A[2]*E[2];
        vertex[5] = C + A[0]*E[0] - A[1]*E[1] - A[2]*E[2];
        vertex[6] = C - A[0]*E[0] + A[1]*E[1] - A[2]*E[2];
        vertex[7] = C - A[0]*E[0] - A[1]*E[1] + A[2]*E[2];

        Interval result;
        result.min = result.max = dot(axis, vertex[0]);

        for (int i = 1; i < 8; ++i) 
        {
            float projection = dot(axis, vertex[i]);
            result.min = (projection < result.min) ? projection : result.min;
            result.max = (projection > result.max) ? projection : result.max;
        }

        return result;
    }

    bool OverlapOnAxis(const AABB& aabb, const OBB& obb,
        const vec3& axis) {
        Interval a = getInterval(aabb, axis);
        Interval b = getInterval(obb, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }

    bool testAABBOBB(const AABB& aabb, const OBB& obb)
    {

        vec3 test[15] =
        {
            vec3(1, 0, 0),          // AABB axis 1
            vec3(0, 1, 0),          // AABB axis 2
            vec3(0, 0, 1),          // AABB axis 3
            obb.axes[ 0 ], // OBB axis 1
            obb.axes[ 1 ], // OBB axis 2
            obb.axes[ 2 ]  // OBB axis 3
           // We will fill out the remaining axis in the next step
        };

        for (int i = 0; i < 3; ++i) 
        {
            // Fill out rest of axis
            test[6 + i * 3 + 0] = cross(test[i], test[0]);
            test[6 + i * 3 + 1] = cross(test[i], test[1]);
            test[6 + i * 3 + 2] = cross(test[i], test[2]);
        }

        for (int i = 0; i < 15; ++i)
        {
            if (!OverlapOnAxis(aabb, obb, test[i])) 
            {
                return false; // Seperating axis found
            }
        }

        return true; // Seperating axis not found
    }


    float raycast(const Ray& ray, const Sphere& sphere) 
    {
        vec3 oc = ray.origin - sphere.center;

        // Coefficients of the quadratic equation
        float a = dot(ray.direction, ray.direction);
        float b = 2.0f * dot(oc, ray.direction);
        float c = dot(oc, oc) - sphere.radius * sphere.radius;

        // Calculate discriminant
        float discriminant = b * b - 4 * a * c;

        // No real roots, so no intersection
        if (discriminant < 0) {
            return 0.0f;
        }

        // Find the nearest root that lies in front of the ray
        float sqrtDiscriminant = std::sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

        // Return the smallest positive t
        if (t1 >= 0.0f) {
            return t1;
        } else if (t2 >= 0.0f) {
            return t2;
        }

        // No valid intersection
        return 0.0f;
    }

    float raycast(const Ray& ray, const AABB& aabb)
    {
        // NOTE: Any component of direction could be 0!
        // to avoid a division by 0, you need to add
        // additional safety checks.
        float t1 = (aabb.min.x - ray.origin.x) / ray.direction.x;
        float t2 = (aabb.max.x - ray.origin.x) / ray.direction.x;
        float t3 = (aabb.min.y - ray.origin.y) / ray.direction.y;
        float t4 = (aabb.max.y - ray.origin.y) / ray.direction.y;
        float t5 = (aabb.min.z - ray.origin.z) / ray.direction.z;
        float t6 = (aabb.max.z - ray.origin.z) / ray.direction.z;

        float tmin = fmaxf( fmaxf( fminf(t1, t2), fminf(t3, t4) ), fminf(t5, t6) );
        float tmax = fminf( fminf( fmaxf(t1, t2), fmaxf(t3, t4) ), fmaxf(t5, t6) );
        if (tmax< 0) {
            return -1;
        }
        if (tmin>tmax) {
            return -1;
        }
        if (tmin< 0.0f) {
            return tmax;
        }
        return tmin;
    }


    bool RayIntersectsOBB(const Ray& ray, const OBB& obb) 
    {
        // Transform ray into the local space of the OBB
        vec3 p = ray.origin - obb.center;

        // Apply inverse rotation to transform ray into OBB's local space
        vec3 localRayOrigin = transpose(obb.axes) * p;
        vec3 localRayDir = transpose(obb.axes) * ray.direction;

        // Initialize variables for ray-box intersection
        vec3 min = -obb.extents;
        vec3 max = obb.extents;

        float tmin = 0.0f;
        float tmax = FLT_MAX;

        // Check for intersection in each axis
        for (int i = 0; i < 3; ++i) 
        {
            if (fabs(localRayDir[i]) < 1e-8) 
            {
                // Ray is parallel to slab, check if the origin is within the slab
                if (localRayOrigin[i] < min[i] || localRayOrigin[i] > max[i]) 
                {
                    return false; // No intersection
                }
            } 
            else
            {
                // Compute intersection with the slabs
                float invDir = 1.0f / localRayDir[i];
                float t1 = (min[i] - localRayOrigin[i]) * invDir;
                float t2 = (max[i] - localRayOrigin[i]) * invDir;

                // Ensure t1 is the near intersection and t2 is the far intersection
                if (t1 > t2) std::swap(t1, t2);

                // Update tmin and tmax for the intersection interval
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);

                // If tmin exceeds tmax, no intersection
                if (tmin > tmax) 
                    return false;
            }
        }

        return true; // Intersection detected
    }

    float raycast(const Ray& ray, const OBB& obb)
    {
        vec3 p = obb.center - ray.origin;

        vec3 f
        (
            dot(obb.axes[0], ray.direction),
            dot(obb.axes[1], ray.direction),
            dot(obb.axes[2], ray.direction)
        );

        vec3 e
        (
            dot(obb.axes[0], p),
            dot(obb.axes[1], p),
            dot(obb.axes[1], p)
        );

        float t[6] = { 0, 0, 0, 0, 0, 0 };
        for (int i = 0; i < 3; ++i)
        {
            if ( CMP(f[i], 0.f) )
            {
                if (-e[i] - obb.extents[i]>0 || -e[i] + obb.extents[i]<0)
                {
                    return -1;
                }
                f[i] = 0.00001f; // Avoid div by 0!
            }
            t[i * 2 + 0] = (e[i] + obb.extents[i]) / f[i]; // min
            t[i * 2 + 1] = (e[i] - obb.extents[i]) / f[i]; // max
        }

        float tmin = fmaxf( fmaxf( fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]) );
        float tmax = fminf( fminf( fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]) );

        if (tmax< 0)
        {
            return -1.0f;
        }
        if (tmin>tmax)
        {
            return -1.0f;
        }
        if (tmin< 0.0f)
        {
            return tmax;
        }

        return tmin;
    }

    float raycast(const Ray& ray, const Plane& plane)
    {
        float nd = dot(ray.direction, plane.normal);
        float pn = dot(ray.origin, plane.normal);

        if (nd>= 0.0f)
        {
            return -1;
        }

        float t = (plane.distance - pn) / nd;
        if (t >= 0.0f) 
        {
            return t;
        }

        return -1;
    }




    bool linetest( const Line& line, const Sphere& sphere)
    {
       Point closest = closestPointLine(sphere.center, line);
       float distSq = magnSq(sphere.center - closest);
       return distSq<= (sphere.radius * sphere.radius);
    }

    bool linetest(const Line& line, const AABB& aabb)
    {
       Ray ray;
       ray.origin = line.start;
       ray.direction = normalize(line.end - line.start);
       float t = raycast( ray, aabb );

       return t >= 0 && t * t <= LengthSq(line);
    }

    bool linetest(const OBB& obb, const Line& line)
    {
       Ray ray;
       ray.origin = line.start;
       ray.direction = normalize(line.end - line.start);
       float t = raycast(ray, obb);
       return t >= 0 && t * t <= LengthSq(line);
    }

    bool linetest(const Line& line, const Plane& plane)
    {
        vec3 ab = line.end - line.start;

        float nA = dot(plane.normal, line.start);
        float nAB = dot(plane.normal, ab);

        // If the line and plane are parallel, nAB will be 0
        // This will cause a divide by 0 exception below
        // If you plan on testing parallel lines and planes
        // it is sage to early out when nAB is 0.

        float t = (plane.distance - nA) / nAB;
        return t >= 0.0f && t <= 1.0f;
    }



    // Test if point P lies inside the counterclockwise triangle ABC
    bool testPointTriangle( const Point& p, const Triangle& t )
    {
        // Translate point and triangle so that point lies at origin
        vec3 a = t.a - p;
        vec3 b = t.b - p;
        vec3 c = t.c - p;

        // Compute normal vectors for triangles pab and pbc
        vec3 u = cross(b, c);
        vec3 v = cross(c, a);

        // Make sure they are both pointing in the same direction
        if (dot(u, v) < 0.0f) return 0;

        // Compute normal vector for triangle pca
        vec3 w = cross(a, b);

        // Make sure it points in the same direction as the first two
        if ( dot(u, w) < 0.0f) return 0;

        // Otherwise P must be in (or on) the triangle
        return 1;
    }

    Plane fromTriangle(const Triangle& t)
    {
        Plane result;
        result.normal = normalize( cross(t.b - t.a, t.c - t.a) );
        result.distance = dot(result.normal, t.a);
        return result;
    }

    bool TriangleSphere(const Triangle& triangle, const Sphere& sphere)
    {
        Point closest = closestPointTriangle(sphere.center, triangle);
        float magsq = magnSq(closest - sphere.center);
        return magsq <= sphere.radius * sphere.radius;
    }

    Point closestPointTriangle(const Point& p, const Triangle& t)
    {
        Plane plane = fromTriangle(t);
        Point closest = closestPointPlane( p,plane );
        if ( testPointTriangle( closest, t ) )
        {
            return closest;
        }
        Point c1 = closestPointLine(p, Line(t.a, t.b)); // Line AB
        Point c2 = closestPointLine(p, Line(t.b, t.c)); // Line BC
        Point c3 = closestPointLine(p, Line(t.c, t.a)); // Line CA”
        float magSq1 = magnSq(p - c1);
        float magSq2 = magnSq(p - c2);
        float magSq3 = magnSq(p - c3);

        if (magSq1 < magSq2 && magSq1 < magSq3)
        {
            return c1;
        }
        else if (magSq2 < magSq1 && magSq2 < magSq3)
        {
            return c2;
        }

        return c3;
    }



    Interval getInterval(const Triangle& triangle, const vec3& axis)
    {
        Interval result;
        result.min = dot(axis, triangle.points[0]);
        result.max = result.min;
        for (int i = 1; i < 3; ++i)
        {
            float value = dot(axis, triangle.points[i]);
            result.min = fminf(result.min, value);
            result.max = fmaxf(result.max, value);
        }

        return result;
    }

    bool OverlapOnAxis(const AABB& aabb, const Triangle& triangle, const vec3& axis)
    {
        Interval a = getInterval(aabb, axis);
        Interval b = getInterval(triangle, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }

    bool TriangleAABB(const Triangle& t, const AABB& a) 
    {
        vec3 f0 = t.b - t.a;
        vec3 f1 = t.c - t.b;
        vec3 f2 = t.a - t.c;

        vec3 u0(1.0f, 0.0f, 0.0f);
        vec3 u1(0.0f, 1.0f, 0.0f);
        vec3 u2(0.0f, 0.0f, 1.0f);

        vec3 test[13] =
        {
            u0, // AABB Axis 1
            u1, // AABB Axis 2
            u2, // AABB Axis 3
            cross(f0, f1),
            cross(u0, f0), 
            cross(u0, f1),
            cross(u0, f2),
            cross(u1, f0), 
            cross(u1, f1),
            cross(u1, f2),
            cross(u2, f0), 
            cross(u2, f1),
            cross(u2, f2)
        };

        for (int i = 0; i < 13; ++i) 
        {
            if (!OverlapOnAxis(a, t, test[i])) 
            {
                return false; // Separating axis found
            }
        }
        return true; // Separating axis not found
    }



    bool OverlapOnAxis(const OBB& obb, const Triangle& triangle, const vec3& axis)
    {
        Interval a = getInterval(obb, axis);
        Interval b = getInterval(triangle, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }

    bool testTriangleOBB(const Triangle& t, const OBB& o) 
    {
        // Compute the edge vectors of the triangle  (ABC)
        vec3 f0 = t.b - t.a;
        vec3 f1 = t.c - t.b;
        vec3 f2 = t.a - t.c;
        vec3 test[13] = 
        {
            o.axes[0], // OBB Axis 1
            o.axes[1], // OBB Axis 2
            o.axes[2], // OBB Axis 3”
            cross(f0, f1), // Normal of the Triangle
            cross(o.axes[0], f0),
            cross(o.axes[0], f1),
            cross(o.axes[0], f2),
            cross(o.axes[1], f0),
            cross(o.axes[1], f1),
            cross(o.axes[1], f2),
            cross(o.axes[2], f0),
            cross(o.axes[2], f1),
            cross(o.axes[2], f2)
        };
        for (int i = 0; i < 13; ++i) 
        {
            if (!OverlapOnAxis(o, t, test[i]))
            {
                return false; // Separating axis found
            }
        }
        return true; // Separating axis not found
    }


    float PlaneEquation(const Point& point, const Plane& plane)
    {
       return dot(point, plane.normal) - plane.distance;
    }

    bool testTrianglePlane(const Triangle& t, const Plane& p)
    {
        float side1 = PlaneEquation(t.a, p);
        float side2 = PlaneEquation(t.b, p);
        float side3 = PlaneEquation(t.c, p);
        if (CMP(side1, 0.f) && CMP(side2, 0.f) && CMP(side3, 0.f))
        {
                return true;
        }
        if (side1 > 0 && side2 > 0 && side3 > 0) 
        {
                return false;
        }
        if (side1 < 0 && side2 < 0 && side3 < 0) 
        {
                return false;
        }
        return true; // Intersection
    }


    bool OverlapOnAxis(const Triangle& t1, const Triangle& t2, const vec3& axis);
    bool testTriangleTriangle(const Triangle& t1, const Triangle& t2);

    bool OverlapOnAxis(const Triangle& t1, const Triangle& t2, const vec3& axis)
    {
        Interval a = getInterval(t1, axis);
        Interval b = getInterval(t2, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }
    bool testTriangleTriangle(const Triangle& t1, const Triangle& t2)
    {
        vec3 t1_f0 = t1.b - t1.a; // Triangle 1, Edge 0
        vec3 t1_f1 = t1.c - t1.b; // Triangle 1, Edge 1
        vec3 t1_f2 = t1.a - t1.c; // Triangle 1, Edge 2
        vec3 t2_f0 = t2.b - t2.a; // Triangle 2, Edge 0
        vec3 t2_f1 = t2.c - t2.b; // Triangle 2, Edge 1
        vec3 t2_f2 = t2.a - t2.c; // Triangle 2, Edge 2
        vec3 axisToTest[] = 
        {
            cross(t1_f0, t1_f1),
            cross(t2_f0, t2_f1),
            cross(t2_f0, t1_f0),
            cross(t2_f0, t1_f1),
            cross(t2_f0, t1_f2),
            cross(t2_f1, t1_f0),
            cross(t2_f1, t1_f1),
            cross(t2_f1, t1_f2),
            cross(t2_f2, t1_f0),
            cross(t2_f2, t1_f1),
            cross(t2_f2, t1_f2)
        };
        for (int i = 0; i < 11; ++i) 
        {
            if (!OverlapOnAxis(t1, t2, axisToTest[i])) 
            {
                return false; // Seperating axis found
            }
        }
        return true;
    }


    vec3 Barycentric(const Point& p, const Triangle& t)
    {
        vec3 ap = p - t.a;
        vec3 bp = p - t.b;
        vec3 cp = p - t.c;
        vec3 ab = t.b - t.a;
        vec3 ac = t.c - t.a;
        vec3 bc = t.c - t.b;
        vec3 cb = t.b - t.c;
        vec3 ca = t.a - t.c;
        vec3 v = ab - project(ab, cb);
        float a = 1.0f - (dot(v, ap) / dot(v, ab));

        v = bc - project(bc, ac);
        float b = 1.0f - (dot(v, bp) / dot(v, bc));

        v = ca - project(ca, ab);
        float c = 1.0f - (dot(v, cp) / dot(v, ca));

        return vec3(a, b, c);
    }

    float raycast(const Ray& ray, const Triangle& triangle)
    {
        Plane plane = fromTriangle(triangle);
        float t = raycast(ray, plane);
        if (t < 0.0f) {
            return t;
        }
        Point result = ray.origin + ray.direction * t;
        vec3 barycentric = Barycentric(result, triangle);
        if
        (
            barycentric.x >= 0.0f && barycentric.x <= 1.0f &&
            barycentric.y >= 0.0f && barycentric.y <= 1.0f &&
            barycentric.z >= 0.0f && barycentric.z <= 1.0f
        )
        {
            return t;
        }

        return -1;
    }

    bool Linetest(const Triangle& triangle, const Line& line) {
        Ray ray;
        ray.origin = line.start;
        ray.direction = normalize(line.end - line.start);
        float t = raycast(ray, triangle);
        return t >= 0 && t * t <= lengthSq(line);
    }





    typedef struct BVHNode
    {
        AABB bounds;
        BVHNode* children;
        int numTriangles;
        int* triangles;
        BVHNode() : children(0), numTriangles(0), triangles(0) {}
    } BVHNode;

    typedef struct Mesh {
        int numTriangles;
        union {
            Triangle* triangles;
            Point* vertices;
            float* values;
        };
        BVHNode* accelerator; // THIS IS NEW!
        // The constructor is also new
        Mesh() : numTriangles(0), values(0), accelerator(0) {}
    } Mesh;

    void SplitBVHNode(BVHNode* node, const Mesh& model, int depth)
    {
        if (depth-- == 0)
        {
            return; // Decrements depth
        }
        if (node->children == 0)
        {
            // Only split if it's a leaf
            // Only split if this node contains triangles
            if (node->numTriangles > 0)
            {
                node->children = new BVHNode[8];
                vec3 c = (node->bounds.max + node->bounds.min) * 0.5f;
                vec3 e = (node->bounds.max + node->bounds.min) * 0.5f;

                node->children[0].bounds = AABB(c + vec3(-e.x, +e.y, -e.z), e);
                node->children[1].bounds = AABB(c + vec3(+e.x, +e.y, -e.z), e);
                node->children[2].bounds = AABB(c + vec3(-e.x, +e.y, +e.z), e);
                node->children[3].bounds = AABB(c + vec3(+e.x, +e.y, +e.z), e);
                node->children[4].bounds = AABB(c + vec3(-e.x, -e.y, -e.z), e);
                node->children[5].bounds = AABB(c + vec3(+e.x, -e.y, -e.z), e);
                node->children[6].bounds = AABB(c + vec3(-e.x, -e.y, +e.z), e);
                node->children[7].bounds = AABB(c + vec3(+e.x, -e.y, +e.z), e);
            }
        }
        // If this node was just split
        if (node->children != 0 && node->numTriangles > 0) 
        {
            for (int i = 0; i < 8; ++i)
            {
                // For each child”
                node->children[i].numTriangles = 0;
                for (int j = 0; j < node->numTriangles; ++j)
                {
                    Triangle t = model.triangles[node->triangles[j]];
                    if (TriangleAABB(t, node->children[i].bounds))
                    {
                        node->children[i].numTriangles += 1;
                    }
                }
                if (node->children[i].numTriangles == 0) 
                {
                    continue;
                }
                node->children[i].triangles = new int[node->children[i].numTriangles];
                int index = 0;
                for (int j = 0; j < node->numTriangles; ++j) 
                {
                    Triangle t = model.triangles[node->triangles[j]];
                    if (TriangleAABB(t, node->children[i].bounds))
                    {
                        node->children[i].triangles[index++] = node->triangles[j];
                    }
                }
            }
            node->numTriangles = 0;
            delete[] node->triangles;
            node->triangles = 0;
            for (int i = 0; i < 8; ++i)
            {
                SplitBVHNode(&node->children[i], model, depth);
            }
        }
    }

    void AccelerateMesh(Mesh& mesh)
    {
        if (mesh.accelerator != 0) 
        {
            return;
        }

        vec3 min = mesh.vertices[0];
        vec3 max = mesh.vertices[0];

        for (int i = 1; i < mesh.numTriangles * 3; ++i)
        {
            min.x = fminf(mesh.vertices[i].x, min.x);
            min.y = fminf(mesh.vertices[i].y, min.y);
            min.z = fminf(mesh.vertices[i].z, min.z);
            max.x = fmaxf(mesh.vertices[i].x, max.x);
            max.y = fmaxf(mesh.vertices[i].y, max.y);
            max.z = fmaxf(mesh.vertices[i].z, max.z);
        }
        mesh.accelerator = new BVHNode();
        mesh.accelerator->bounds = {min, max};
        mesh.accelerator->numTriangles = mesh.numTriangles;
        mesh.accelerator->triangles = new int[mesh.numTriangles];
        for (int i = 0; i < mesh.numTriangles; ++i) 
        {
            mesh.accelerator->triangles[i] = i;
        }
        SplitBVHNode(mesh.accelerator, mesh, 3);
    }

}
