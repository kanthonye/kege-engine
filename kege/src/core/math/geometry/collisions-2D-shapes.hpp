//
//  collisions-2D-shapes.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#ifndef collisions_2D_shapes_hpp
#define collisions_2D_shapes_hpp

#include "primitive-2D-shapes.hpp"
namespace kege{
bool CircleCircle(const Circle& c1, const Circle& c2);

bool CircleAABB2D( const Circle& circle, const AABB2D& box );
#define RectangleCircle(box, circle) CircleAABB2D(circle, box)

bool CircleOBB2D(const Circle& circle, const OBB2D& box);
#define OBB2DCircle( box, circle ) CircleOBB2D(circle, box)


#define CLAMP(number, minimum, maximum) number = (number < minimum) ? minimum : ( (number > maximum) ? maximum : number )

typedef struct Interval2D {
   float min;
   float max;
} Interval2D;



bool CircleCircle(const Circle& c1, const Circle& c2);
bool CircleAABB2D(const Circle& circle, const AABB2D& aabb);
bool CircleOrientedRectangle(const Circle& circle, const OBB2D& obb);
bool RectangleRectangle(const AABB2D& aabb1, const AABB2D& aabb2);
Interval2D getInterval(const AABB2D& aabb, const vec2& axis);
bool OverlapOnAxis(const AABB2D& rect1, const AABB2D& rect2, const vec2& axis);
bool AABB2DAABB2DSAT(const AABB2D& rect1, const AABB2D& rect2);

bool AABBOBB(const AABB2D& rect1,const OBB2D& rect2);
#define OBBAABB(oriented, regular) AABBOBB(regular, oriented)

Interval2D getInterval(const OBB2D& obb, const vec2& axis);
bool overlapOnAxis(const OBB2D& rect1, const AABB2D& rect2, const vec2& axis);

bool overlapOnAxis(const AABB2D& rect1, const OBB2D& rect2, const vec2& axis);
bool RectangleOrientedRectangle(const AABB2D& rect1, const OBB2D& rect2);
bool OrientedRectangleOrientedRectangle( const OBB2D& r1, const OBB2D& r2);
float signed2DTriArea( const Point2D& a, const Point2D& b, const Point2D& c);

int test2DSegmentSegment(const Point2D& a, const Point2D& b, const Point2D& c, const Point2D& d, float &t, Point2D &p);
}
#endif /* collisions_2D_shapes_hpp */
