//
//  primitive-2D-shapes.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#include "primitive-2D-shapes.hpp"
namespace kege{

float length(const Line2D& line)
{
   return magn(line.end - line.start);
}

float lengthSq(const Line2D& line)
{
   return magnSq(line.end - line.start);
}


Rect2D FromMinMax(const vec2& min, const vec2& max) {
   return Rect2D(min, max - min);
}


bool pointOnLine(const Point2D& p, const Line2D& line) {
   // Find the slope
   float dy = (line.end.y - line.start.y);
   float dx = (line.end.x - line.start.x);
   float M = dy / dx;
   // Find the Y-Intercept
   float B = line.start.y - M * line.start.x;
   // Check line equation
   return CMP(p.y, M * p.x + B);
}

bool pointInCircle(const Point2D& point, const Circle& c) {
   Line2D line(point, c.center);
   if (lengthSq(line) < c.radius * c.radius) {
      return true;
   }
   return false;
}

bool pointInAABB(const Point2D& point,const AABB2D& aabb)
{
   return  aabb.min.x<= point.x && aabb.min.y<= point.y && point.x<= aabb.max.x && point.y<= aabb.max.y;
}

bool pointInOBB(const Point2D& point,const OBB2D& box )
{
    vec2 vec = point - box.center;
    mat22 rot = rot2D( -DEG2RAD( box.rotation ) );

    vec = vec * rot;

    AABB2D local_rectangle( Point2D(), box.extents * 2.0f );
    vec2 local_point = vec + box.extents;
    return pointInAABB( local_point, local_rectangle );
}


bool LineCircle( const Line2D& l, const Circle& c )
{
   vec2 ab = l.end - l.start;
   float t = dot(c.center - l.start, ab) / dot(ab, ab);
   if (t < 0.0f || t > 1.0f)
   {
      return false;
   }
   Point2D closestPoint = l.start + ab * t;

   Line2D circleToClosest(c.center, closestPoint);
   return lengthSq(circleToClosest) < c.radius * c.radius;
}

bool LineAABB(const Line2D& l, const AABB2D& aabb)
{
   if ( pointInAABB(l.start, aabb) || pointInAABB(l.end, aabb) ) {
      return true;
   }

   vec2 norm = normalize(l.end - l.start);
   norm.x = (norm.x != 0) ? 1.0f / norm.x : 0;
   norm.y = (norm.y != 0) ? 1.0f / norm.y : 0;
   vec2 min = (aabb.min - l.start) * norm;
   vec2 max = (aabb.max - l.start) * norm;

   float tmin = fmaxf(fminf(min.x, max.x),fminf(min.y, max.y));
   float tmax = fminf(fmaxf(min.x, max.x),fmaxf(min.y, max.y));
   if (tmax< 0 || tmin>tmax) {
      return false;
   }
   float t = (tmin< 0.0f) ? tmax : tmin;
   return t > 0.0f && t * t <lengthSq(l);
}

bool LineOBB(const Line2D& line, const OBB2D& box)
{
    float theta = -DEG2RAD( box.rotation );

    vec2 v;
    Line2D local_line;
    mat22 m = rot2D( theta );

    v = line.start - box.center;
    v = v * m;
    local_line.start = v + box.extents;

    v = line.end - box.center;
    v = v * m;
    local_line.end = v + box.extents;

    AABB2D local_box( Point2D(), box.extents * 2.0f );
    return LineAABB( local_line, local_box );
}



// Test if 2D point P lies inside the counterclockwise 2D triangle ABC
int PointInTriangle(Point2D p, Point2D a, Point2D b, Point2D c)
{
    // If P to the right of AB then outside triangle
    if ( cross(p - a, b - a) < 0.0f) return 0;

    // If P to the right of BC then outside triangle
    if ( cross(p - b, c - b) < 0.0f) return 0;

    // If P to the right of CA then outside triangle
    if ( cross(p - c, a - c) < 0.0f) return 0;
    
    // Otherwise P must be in (or on) the triangle
    return 1;
}
}
