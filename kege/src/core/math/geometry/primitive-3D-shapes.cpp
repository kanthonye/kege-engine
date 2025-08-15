//
//  primitive-3D-shapes.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//


#include "primitive-3D-shapes.hpp"
namespace kege{

    float Length(const Line& line)
    {
       return magn(line.start - line.end);
    }

    float LengthSq(const Line& line)
    {
       return magnSq(line.start - line.end);
    }

    Ray fromPoints(const Point& from, const Point& to)
    {
       return Ray(from, normalize(to - from));
    }



    // Returns the squared distance between point c and segment ab
    float sqDistPointSegment(Point a, Point b, Point c)
    {
        vec3 ab = b - a, ac = c - a, bc = c - b;
        float e = dot(ac, ab);
        // Handle cases where c projects outside ab
        if (e <= 0.0f) return dot(ac, ac);
        float f = dot(ab, ab);
        if (e >= f) return dot(bc, bc);
        // Handle cases where c projects onto ab
        return dot(ac, ac) - e * e / f;
    }

    float sqDistPointAABB( const Point& point, const AABB& box )
    {
        float sq_dist = 0.0f;
        for (int i = 0; i < 3; i++)
        {
            // For each axis count any excess distance outside box extents
            float v = point[i];
            if (v < box.min[i]) sq_dist += (box.min[i] - v) * (box.min[i] - v);
            if (v > box.max[i]) sq_dist += (v - box.max[i]) * (v - box.max[i]);
        }
        return sq_dist;
    }

    float sqDistPointOBB( const Point& point, const OBB& box)
    {
        vec3 v = point - box.center;
        float sq_dist = 0.0f;
        for (int i = 0; i < 3; i++)
        {
            // Project vector from box center to p on each axis, getting the distance
            // of p along that axis, and count any excess distance outside box extents
            float d = dot(v, box.axes.v[i]), excess = 0.0f;
            if (d < -box.extents[i])
                excess = d + box.extents[i];
            else if (d > box.extents[i])
                excess = d - box.extents[i];
            sq_dist += excess * excess;
        }
       return sq_dist;
    }

    float distPointPlane( const Point& point, const Plane& plane )
    {
         // return dot(q, p.n) - p.d; if plane equation normalized (||p.n||==1)
         return dot(plane.normal, point) - plane.distance;
    }


    Point closestPointSphere(const Sphere& sphere, const Point& point)
    {
        vec3 sphere_to_point = point - sphere.center;
        normalized( sphere_to_point );

        //Resize the normalized vector to the size of the radius:
        sphere_to_point = sphere_to_point * sphere.radius;
        //Return the resized vector offset by the position of the sphere:
        return sphere_to_point + sphere.center;
    }

    Point closestPointPlane( const Point& point, const Plane& plane )
    {
        float t = dot(plane.normal, point) - plane.distance;
        return point - plane.normal * t;
    }

    Point closestPointLine(const Point& point, const Line& line)
    {
        vec3 lVec = line.end - line.start; // Line Vector”
        float t = dot(point - line.start, lVec) / dot(lVec, lVec);
        t = fmaxf(t, 0.0f); // Clamp to 0
        t = fminf(t, 1.0f); // Clamp to 1
        return line.start + lVec * t;
    }

    Point closestPointSegment( const Point& point, const Line& line, float &t )
    {
        vec3 ab = line.end - line.start;
        // Project c onto ab, computing parameterized position d(t) = a + t*(b – a)
        t = dot(point - line.start, ab) / dot(ab, ab);
        // If outside segment, clamp t (and therefore d) to the closest endpoint
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        // Compute projected position from the clamped t
        return line.start + ab * t;
    }

    Point closestPointRay(const Point& point, const Ray& ray)
    {
        float t = dot(point - ray.origin, ray.direction);
        // We assume the direction of the ray is normalized
        // If for some reason the direction is not normalized
        // the below division is needed. So long as the ray
        // direction is normalized, we don't need this divide
        // t /= Dot(ray.direction, ray.direction);”
        t = fmaxf(t, 0.0f);
        return Point(ray.origin + ray.direction * t);
    }

    void closestPointAABB( const Point& point, const AABB& aabb, Point& result )
    {
        result = point;
        result.x = (result.x < aabb.min.x) ? aabb.min.x : result.x;
        result.y = (result.y < aabb.min.x) ? aabb.min.y : result.y;
        result.z = (result.z < aabb.min.x) ? aabb.min.z : result.z;
        result.x = (result.x > aabb.max.x) ? aabb.max.x : result.x;
        result.y = (result.y > aabb.max.x) ? aabb.max.y : result.y;
        result.z = (result.z > aabb.max.x) ? aabb.max.z : result.z;
    }

    Point closestPointOBB( const Point& point, const OBB& box )
    {
        float distance;
        Point result = box.center;
        vec3 d = point - box.center;

        distance = dot(d, box.axes[0]);
        if (distance >  box.extents[0]) distance =  box.extents[0];
        if (distance < -box.extents[0]) distance = -box.extents[0];
        result += distance * box.axes[0];

        distance = dot(d, box.axes[1]);
        if (distance >  box.extents[1]) distance =  box.extents[1];
        if (distance < -box.extents[1]) distance = -box.extents[1];
        result += distance * box.axes[1];

        distance = dot(d, box.axes[2]);
        if (distance >  box.extents[2]) distance =  box.extents[2];
        if (distance < -box.extents[2]) distance = -box.extents[2];
        result += distance * box.axes[2];
        return result;
    }

    void closestPointRect( const Point& point, const Rect& rect, Point &result)
    {
        vec3 d = point - rect.center;
        // Start result at center of rect; make steps from there
        result = rect.center;
        // For each rect axis...
        for (int i = 0; i < 2; i++)
        {
            // ...project d onto that axis to get the distance
            // along the axis of d from the rect center
            float dist = dot(d, rect.axie[i]);
            // If distance farther than the rect extents, clamp to the rect
            if (dist > rect.extents[i]) dist = rect.extents[i];
            if (dist < -rect.extents[i]) dist = -rect.extents[i];
            // Step that distance along the axis to get world coordinate
            result += dist * rect.axie[i];
        }
    }

    void closestPointRect(const Point& p, const Triangle& triangle, Point &result)
    {
        vec3 ab = triangle.b - triangle.a; // vector across rect
        vec3 ac = triangle.c - triangle.a; // vector down rect
        vec3 d = p - triangle.a;

        // Start result at top-left corner of rect; make steps from there q = a;
        // clamp p’ (projection of p to plane of r) to rectangle in the across direction
        float dist = dot(d, ab);
        float maxdist = dot(ab, ab);
        if (dist >= maxdist)
            result += ab;
        else if (dist > 0.0f)
            result += (dist / maxdist) * ab;

        // clamp p’ (projection of p to plane of r) to rectangle in the down direction
        dist = dot(d, ac);
        maxdist = dot(ac, ac);
        if (dist >= maxdist)
            result += ac;
        else if (dist > 0.0f)
            result += (dist / maxdist) * ac;
    }

    // Computes closest points C1 and C2 of S1(s)=P1+s*(Q1-P1) and
    // S2(t)=P2+t*(Q2-P2), returning s and t. Function result is squared // distance between between S1(s) and S2(t)
    void closestPointLineLine(const Line& l1, const Line& l2, float &s, float &t, Point (&closest_points)[2] )
    {
        vec3 d1 = l1.end - l1.start; // Direction vector of segment S1
        vec3 d2 = l2.end - l2.start; // Direction vector of segment S2
        vec3 r = l1.start - l2.start;
        float a = dot(d1, d1); // Squared length of segment S1, always nonnegative
        float e = dot(d2, d2); // Squared length of segment S2, always nonnegative
        float f = dot(d2, r);
        // Check if either or both segments degenerate into points
        if (a <= KEGE_EPSILON_F && e <= KEGE_EPSILON_F)
        {
            // Both segments degenerate into points
            s = t = 0.0f;
            closest_points[0] = l1.start;
            closest_points[1] = l2.start;
            //return magnSq(closest_points[0] - closest_points[1]);
        }
        if (a <= KEGE_EPSILON_F)
        {
            // First segment degenerates into a point
            s = 0.0f;
            t=f/e; //s=0=>t=(b*s+f)/e=f/e
            t = clamp(t, 0.0f, 1.0f);
        }
        else
        {
            float c = dot(d1, r);
            if (e <= KEGE_EPSILON_F)
            {
                // Second segment degenerates into a point
                t = 0.0f;
                s=clamp(-c/a,0.0f,1.0f); //t=0=>s=(b*t-c)/a=-c/a
            }
            else
            {
                // The general nondegenerate case starts here
                float b = dot(d1, d2);
                float denom = a*e-b*b; // Always nonnegative

               // If segments not parallel, compute closest point on L1 to L2 and
               // clamp to segment S1. Else pick arbitrary s (here 0)
                if (denom != 0.0f)
                {
                    s = clamp((b*f - c*e) / denom, 0.0f, 1.0f);
                }
                else s = 0.0f;
                // Compute point on L2 closest to S1(s) using
                // t = dot((P1 + D1*s) - P2,D2) / dot(D2,D2) = (b*s + f) / e
                t = (b*s + f) / e;

                // If t in [0,1] done. Else clamp t, recompute s for the new value
                // of t using s = dot((P2 + D2*t) - P1,D1) / dot(D1,D1)= (t*b - c) / a // and clamp s to [0, 1]
                if (t < 0.0f)
                {
                    t = 0.0f;
                    s = clamp(-c / a, 0.0f, 1.0f);
                }
                else if (t > 1.0f)
                {
                    t = 1.0f;
                    s = clamp((b - c) / a, 0.0f, 1.0f);
                }
            }
        }
        closest_points[0] = l1.start + d1 * s;
        closest_points[1] = l2.start + d2 * t;
        //return magnSq(closest_points[0] - closest_points[1]);
    }


   
//    float distance( const Line& line1, const Line& line2 )
//    {
//        vec3 AB = line1.end - line1.start;
//        vec3 CD = line2.end - line2.start;
//        vec3 AC = line2.start - line1.start;
//
//        float AB_dot_AB = dot(AB, AB);
//        float CD_dot_CD = dot(CD, CD);
//        float AB_dot_CD = dot(AB, CD);
//        float AB_dot_AC = dot(AB, AC);
//        float CD_dot_AC = dot(CD, AC);
//
//        // Coefficients for the system of equations
//        float denominator = AB_dot_AB * CD_dot_CD - AB_dot_CD * AB_dot_CD;
//
//        // Check if the lines are parallel
//        if (std::fabs(denominator) < 1e-6f)
//        {
//            // Lines are parallel, calculate distance from one segment endpoint to the other segment
//            vec3 closest_point = line2.start + CD * clamp(CD_dot_AC / CD_dot_CD, 0.f, 1.f);
//            return magn(line1.start - closest_point);
//        }
//
//        // Calculate the parameters that minimize the distance
//        float t = (AB_dot_CD * CD_dot_AC - CD_dot_CD * AB_dot_AC) / denominator;
//        float s = (AB_dot_AB * CD_dot_AC - AB_dot_CD * AB_dot_AC) / denominator;
//
//        // Clamp t and s to stay within the segments
//        t = clamp(t, 0.f, 1.f);
//        s = clamp(s, 0.f, 1.f);
//
//        // Closest points on the line segments
//        vec3 closestPointOnAB = line1.start + AB * t;
//        vec3 closestPointOnCD = line2.start + CD * s;
//
//        // Return the distance between the closest points
//        return magn( closestPointOnAB - closestPointOnCD );
//    }

    float distancePointLine( const vec3& point, const Line& line )
    {
        vec3 v[3];
        v[0] = line.end - line.start;   // Vector v0
        v[1] = point - line.start;      // Vector v1
        v[2] = cross( v[1], v[0] );     // Cross product of v1 and v0
        // Distance is the magnitude of the cross product divided by the magnitude of v0
        return magn( v[2] ) / magn( v[0] );
    }
}
