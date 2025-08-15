//
//  collisions-2D-shapes.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#include "collisions-2D-shapes.hpp"
namespace kege{

    bool CircleCircle(const Circle& c1, const Circle& c2) 
    {
        Line2D line( c1.center, c2.center );
        float radiiSum = c1.radius + c2.radius;
        return lengthSq( line ) <= radiiSum * radiiSum;
    }

    bool CircleAABB2D(const Circle& circle, const AABB2D& aabb)
    {
        Point2D closest_point = circle.center;
        CLAMP(closest_point.x, aabb.min.x, aabb.max.x);
        CLAMP(closest_point.y, aabb.min.y, aabb.max.y);

        Line2D line(circle.center, closest_point);
        return lengthSq(line) <= circle.radius * circle.radius;
    }


    bool CircleOrientedRectangle(const Circle& circle, const OBB2D& obb)
    {
        vec2 r = circle.center - obb.center;
        mat22 rot = rot2D< float >( -DEG2RAD( obb.rotation ) );
        Circle local_circle(r + obb.extents, circle.radius);
        AABB2D local_rect(Point2D(), obb.extents * 2.0f);
        return CircleAABB2D( local_circle, local_rect );
    }


    bool RectangleRectangle(const AABB2D& aabb1, const AABB2D& aabb2)
    {
        bool overX = ((aabb2.min.x<= aabb1.max.x) && (aabb1.min.x<= aabb2.max.x));
        bool overY = ((aabb2.min.y<= aabb1.max.y) && (aabb1.min.y<= aabb2.max.y));
        return overX && overY;
    }


    Interval2D getInterval(const AABB2D& aabb, const vec2& axis)
    {
        Interval2D result;

        vec2 verts[] =
        {
            vec2(aabb.min.x, aabb.min.y), vec2(aabb.min.x, aabb.max.y),
            vec2(aabb.max.x, aabb.max.y), vec2(aabb.max.x, aabb.min.y)
        };

        result.min = result.max = dot(axis, verts[0]);
        for (int i = 1; i < 4; ++i) 
        {
            float projection = dot(axis, verts[i]);
            if (projection < result.min)
            {
                result.min = projection;
            }
            if (projection > result.max)
            {
                result.max = projection;
            }
        }
        return result;
    }
    bool OverlapOnAxis(const AABB2D& rect1, const AABB2D& rect2, const vec2& axis)
    {
        Interval2D a = getInterval(rect1, axis);
        Interval2D b = getInterval(rect2, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }


    bool AABB2DAABB2DSAT(const AABB2D& rect1, const AABB2D& rect2)
    {
        vec2 axisToTest[] = { vec2(1, 0), vec2(0, 1) };
        for (int i = 0; i < 2; ++i) 
        {
          // Intervals don't overlap,seperating axis found
          if (!OverlapOnAxis(rect1, rect2, axisToTest[i])) 
          {
             return false; // No collision has taken place
          }
       }
       // All intervals overlapped, seperating axis not found
       return true; // We have a collision
    }

    bool AABBOBB(const AABB2D& rect1,const OBB2D& rect2);
    #define OBBAABB(oriented, regular) AABBOBB(regular, oriented)

    Interval2D getInterval(const OBB2D& obb, const vec2& axis)
    {
        AABB2D aabb = AABB2D
        (
            Point2D(obb.center - obb.extents),
         obb.extents * 2.0f
        );

        vec2 verts[] =
        {
            aabb.min, aabb.max,
            vec2(aabb.min.x, aabb.max.y),
            vec2(aabb.max.x, aabb.min.y)
        };

        float t = DEG2RAD(obb.rotation);
        mat22 z_rot = rot2D< float >( t );

        for (int i = 0; i < 4; ++i)
        {
            vec2 r = verts[i] - obb.center;

            r = r * z_rot;

            verts[i] = r + obb.center;
        }

        Interval2D res;
        res.min = res.max = dot(axis, verts[0]);
        for (int i = 1; i < 4; ++i) 
        {
            float proj = dot(axis, verts[i]);
            res.min = (proj<res.min)?proj :res.min;
            res.max = (proj>res.max)?proj :res.max;
        }
       return res;
    }

    bool overlapOnAxis(const OBB2D& rect1, const AABB2D& rect2, const vec2& axis);

    bool overlapOnAxis(const AABB2D& rect1, const OBB2D& rect2, const vec2& axis)
    {
        Interval2D a = getInterval(rect1, axis);
        Interval2D b = getInterval(rect2, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }

    bool RectangleOrientedRectangle(const AABB2D& rect1, const OBB2D& rect2)
    {
        //know the first two axes to test are going to be the X and Y axis. Fill in dummy da for the other two axes for now.

        vec2 axisToTest[] =
        {
            vec2(1, 0),vec2(0, 1), vec2(), vec2()
        };

        float t = DEG2RAD(rect2.rotation);
        mat22 z_rot = rot2D< float >( t );

        vec2 axis = normalize(vec2(rect2.extents.x, 0));

        axisToTest[2] = axis *  z_rot;
        axis = normalize(vec2(0, rect2.extents.y));

        axisToTest[3] = axis *  z_rot;

        for (int i = 0; i < 4; ++i)
        {
          if (!overlapOnAxis(rect1, rect2, axisToTest[i]))
          {
             return false; // No collision has taken place
          }
       }
       return true; // We have a collision”
    }


    bool OrientedRectangleOrientedRectangle( const OBB2D& r1, const OBB2D& r2)
    {
        //Transform r1 ito the local space of r1 (itsself):
        AABB2D local1(Point2D(), r1.extents * 2.0f);
        //Make a copy of r2 which will later be translated into the local space of r1:
        vec2 r = r2.center - r1.center;

        OBB2D local2( r2.center, r2.extents, r2.rotation );
        local2.rotation = r2.rotation - r1.rotation;
        //Construct a rotation matrix which represents a rotation in the opposite direction of r1:

        float t = -DEG2RAD(r1.rotation);
        mat22 z_rot = rot2D( t );
        r = r * z_rot;

        local2.center = r + r1.extents;
        //Now that both rectangles are in the local space of r1, we can perform a RectangleOrientedRectangle intersection test:
        return RectangleOrientedRectangle(local1, local2);
    }



    float signed2DTriArea( const Point2D& a, const Point2D& b, const Point2D& c)
    {
         return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
    }

    int test2DSegmentSegment(const Point2D& a, const Point2D& b, const Point2D& c, const Point2D& d, float &t, Point2D &p)
    {
        // Sign of areas correspond to which side of ab points c and d are
        float a1 = signed2DTriArea(a, b, d); // Compute winding of abd (+ or -)
        float a2 = signed2DTriArea(a, b, c); // To intersect, must have sign opposite of a1
        if (a1 * a2 < 0.0f)
        {
            // Compute signs for a and b with respect to segment cd
            float a3 = signed2DTriArea(c, d, a); // Compute winding of cda (+ or -)

            // Since area is constant a1 - a2 = a3 - a4, or a4 = a3 + a2 - a1
            //float a4 = Signed2DTriArea(c, d, b); // Must have opposite sign of a3
            float a4 = a3 + a2 - a1;

            // Points a and b on different sides of cd if areas have different signs
            if (a3 * a4 < 0.0f) 
            {
                // Segments intersect. Find intersection point along L(t) = a + t * (b - a).
                // Given height h1 of an over cd and height h2 of b over cd,
                // t = h1 / (h1 - h2) = (b*h1/2) / (b*h1/2 - b*h2/2) = a3 / (a3 - a4),
                // where b (the base of the triangles cda and cdb, i.e., the length
                // of cd) cancels out.
                t = a3 / (a3 - a4);
                p = a + t * (b - a);
                return 1;
            }
        }
        return 0;
    }
}
