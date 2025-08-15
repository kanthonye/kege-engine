//
//  primitive-2D-shapes.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#ifndef primitive_2D_shapes_hpp
#define primitive_2D_shapes_hpp

#include "../../math/algebra/vmath.hpp"
namespace kege{
typedef vec2 Point2D;

typedef struct Line2D
{
    inline Line2D(const Point2D& s, const Point2D& e)
    :   start(s), end(e)
    {}

    inline Line2D()
    {}

    Point2D start;
    Point2D end;
}
Line2D;


typedef struct Circle
{
   Point2D center;
   float radius;

    inline Circle(const Point2D& p, float r)
    :   center(p)
    ,   radius(r)
    {}

    inline Circle()
    :   radius(1.0f)
    {}
}
Circle;


typedef struct AABB2D
{
    inline AABB2D(const Point2D& m, const Point2D& n)
    :   min(m)
    ,   max(n)
    {}

    inline AABB2D()
    {}

    Point2D min;
    Point2D max;
}
Rect2D;


typedef struct OBB2D
{
    Point2D center;
    Point2D extents;
    float rotation;


    inline OBB2D(const Point2D& pos, const Point2D& ext, float rot)
    :   center(pos), extents(ext), rotation(rot)
    {}

    inline OBB2D(const Point2D& p, const vec2& e)
    :   center(p), extents(e), rotation(0.0f)
    {}

    inline OBB2D()
    :   extents(1.0f, 1.0f)
    ,   rotation(0.0f)
    {}
}
OBB2D;




float length(const Line2D& line);
float lengthSq(const Line2D& line);

Rect2D fromMinMax(const vec2& min, const vec2& max);

bool pointOnLine(const Point2D& point, const Line2D& line);
bool pointInCircle(const Point2D& point, const Circle& c);
bool pointInAABB(const Point2D& point, const Rect2D& rectangle);
bool pointInOBB(const Point2D& point, const OBB2D& obb);

bool lineCircle(const Line2D& line, const Circle& circle);
bool lineAABB(const Line2D& l, const AABB2D& box );
bool lineOBB(const Line2D& line, const OBB2D& box);

#define pointLine(point, line)  pointOnLine(point, line)
#define linePoint(line, point)  pointOnLine(point, line)
#define circleLine(circle, line) lineCircle(line, circle)
#define AABBLine(box, line) lineRectangle(line, box);
#define OBBLine(box, line) lineOrientedRectangle(line, box);

}
#endif /* primitive_2D_shapes_hpp */
