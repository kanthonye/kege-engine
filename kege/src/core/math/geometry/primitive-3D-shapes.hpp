//
//  primitive-3D-shapes.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#ifndef primitive_3D_shapes_hpp
#define primitive_3D_shapes_hpp

#include <vector>
#include "../../math/algebra/vmath.hpp"
namespace kege{

    typedef vec3 Point;

    typedef struct Line
    {
        inline Line(const Point& s, const Point& e)
        : start(s), end(e)
        {}

        inline Line()
        {}

        Point start;
        Point end;
    }
    Line;

    typedef struct Ray
    {
        inline Ray(const Point& p, const vec3& d)
        : origin(p), direction(d)
        {}

        inline Ray()
        {}

        Point origin;
        vec3 direction;
    }
    Ray;

    struct Plane
    {
        inline Plane(const vec3& n, const vec3& p)
        :   normal(n), distance(dot(p, n)), point(p)
        {}

        inline Plane(const vec3& n, float d)
        :   normal(n), point(n*d), distance(d)
        {}

        inline Plane()
        {}

        vec3 normal;
        vec3 point;
        float distance;

        int one_sided = 0;
        // -1 = test only negative half-space
        //  0 = test both sides (default plane behavior)
        // +1 = test only positive half-space
    };

    // Structure to represent a circle in 3D
    struct Circle
    {
        vec3 center;
        vec3 normal; // Unit normal vector of the circle's plane
        vec3 right;
        float radius;
    };



    struct AABB
    {
        inline AABB(const Point& m, const Point& n)
        : min(m)
        , max(n)
        {}

        inline AABB()
        {}

       Point min;
       Point max;
    };

    struct Sphere
    {
        inline Sphere(const Point& c, float r)
        : center(c), radius(r)
        {}

        inline Sphere()
        : radius(1.0f)
        {}

        Point center;
        float radius;
    };

    struct OBB
    {
        inline OBB(const Point& p, const vec3& s, const mat33& o)
        : center(p)
        , extents(s)
        , axes(o)
        {}

        inline OBB(const Point& p, const vec3& s)
        : center(p)
        , extents(s)
        {}

        inline OBB()
        : extents(1, 1, 1)
        {}

        mat33 axes;
        Point center;
        vec3 extents;
    };

    typedef struct Triangle
    {
        inline Triangle(const Point& p1, const Point& p2, const Point& p3)
        : a(p1), b(p2), c(p3)
        {}

        inline Triangle()
        {}

        union
        {
            struct
            {
                Point a;
                Point b;
                Point c;
            };
            Point points[3];
            float values[9];
        };
    }
    Triangle;


    struct Cone
    {
        Point apex;          // Tip of the cone
        vec3 direction;      // Direction pointing toward base (unit vector)
        float height;        // Height from apex to base
        float radius;        // Radius of the base
    };


    struct Cylinder
    {
        vec3 center;
        vec3 axes[2];
        float height;
        float radius;
    };

    struct Capsule
    {
        vec3 center;
        vec3 axes[2];
        float height;
        float radius;
    };

    struct Pyramid
    {
        Point apex;
        Point base[4]; // ordered either clockwise or counter-clockwise
    };

    struct Lozenge {
        Point a;
        vec3 axes[2];
        float radius;
    };

    struct Rect
    {
        Point center; // center point of rectangle
        vec3 axie[2];// unit vectors determining local x and y axes for the rectangle
        vec2 extents; // the halfwidth extents of the rectangle along the axes
    };

    struct Polygon
    {
        std::vector< vec3 > points;
    };


    typedef struct Interval
    {
       float min;
       float max;
    }
    Interval;





    float Length(const Line& line);

    float LengthSq(const Line& line);

    Ray fromPoints(const Point& from, const Point& to);



    // Returns the squared distance between point c and segment ab
    float sqDistPointSegment(Point a, Point b, Point c);

    float sqDistPointAABB( const Point& point, const AABB& box );

    float sqDistPointOBB( const Point& point, const OBB& box);

    float distPointPlane( const Point& point, const Plane& plane );


    Point closestPointSphere(const Sphere& sphere, const Point& point);

    Point closestPointPlane( const Point& point, const Plane& plane );

    Point closestPointLine(const Point& point, const Line& line);

    Point closestPointRay(const Point& point, const Ray& ray);

    Point closestPointSegment( const Point& point, const Line& line, float &t );

    void closestPointAABB( const Point& point, const AABB& aabb, Point& result );

    Point closestPointOBB( const Point& point, const OBB& box );

    void closestPointRect( const Point& point, const Rect& rect, Point &result);

    void closestPointRect(const Point& p, const Triangle& triangle, Point &result);

    Point closestPointTriangle(const Point& p, const Triangle& triangle);


// Computes closest points C1 and C2 of S1(s)=P1+s*(Q1-P1) and
// S2(t)=P2+t*(Q2-P2), returning s and t. Function result is squared // distance between between S1(s) and S2(t)
    void closestPointLineLine( const Line& l1, const Line& l2, float &s, float &t, Point (&closest_points)[2] );


    inline float distance(const Plane& plane, const vec3& point)
    {
        return dot( plane.normal, point ) + plane.distance;
    }



    float distancePointLine( const vec3& point, const Line& line );
    float distance( const Line& line1, const Line& line2 );
}
#endif /* primitive_3D_shapes_hpp */
