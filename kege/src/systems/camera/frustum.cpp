//
//  frustum.cpp
//  kege
//
//  Created by Kenneth Esdaile on 10/1/24.
//

#include "frustum.hpp"
namespace kege{

    Frustum getFrustum( const kege::mat44& projection, const kege::mat44& transform )
    {
        Frustum result;
        kege::mat44 vp = projection * transform;// .getViewProj();
        vec3 col1(vp[0][0], vp[1][0], vp[2][0]);//, vp._41
        vec3 col2(vp[0][1], vp[1][1], vp[2][1]);//, vp._42
        vec3 col3(vp[0][2], vp[1][2], vp[2][2]);//, vp._43
        vec3 col4(vp[0][3], vp[1][3], vp[2][3]);//, vp._44

        result.left.normal     = col4 + col1;
        result.right.normal    = col4 - col1;
        result.bottom.normal   = col4 + col2;
        result.top.normal      = col4 - col2;
        result.near.normal     = col3;
        result.far.normal      = col4 - col3;
        result.left.distance   = vp[3][3] + vp[3][0];
        result.right.distance  = vp[3][3] - vp[3][0];
        result.bottom.distance = vp[3][3] + vp[3][1];
        result.top.distance    = vp[3][3] - vp[3][1];
        result.near.distance   = vp[3][2];
        result.far.distance    = vp[3][3] - vp[3][2];

        for (int i = 0; i < 6; ++i)
        {
            float mag = 1.0f / magn( result.planes[i].normal );
            result.planes[i].normal =
            result.planes[i].normal*mag;
            result.planes[i].distance *= mag;
        }

        return result;
    }

    Point getIntersectionPoint(Plane p1, Plane p2, Plane p3)
    {
        kege::mat33 D
        (
            p1.normal.x, p2.normal.x, p3.normal.x,
            p1.normal.y, p2.normal.y, p3.normal.y,
            p1.normal.z, p2.normal.z, p3.normal.z
        );
        vec3 A(-p1.distance, -p2.distance, -p3.distance);

        kege::mat33 Dx = D;
        kege::mat33 Dy = D;
        kege::mat33 Dz = D;
        Dx[0][0] = A.x; Dx[0][1] = A.y; Dx[0][2] = A.z;
        Dy[1][0] = A.x; Dy[1][1] = A.y; Dy[1][2] = A.z;
        Dz[2][0] = A.x; Dz[2][1] = A.y; Dz[2][2] = A.z;

        float detD = kege::det( D );

        if ( CMP( detD, 0.f ) )
        {
              return Point();
        }
        float det_dx = kege::det(Dx);
        float det_dy = kege::det(Dy);
        float det_dz = kege::det(Dz);
        return Point(det_dx / detD, det_dy / detD, det_dz / detD);
    }

    void GetCorners(const Frustum& f, vec3* outCorners)
    {
       outCorners[0] = getIntersectionPoint(f.near, f.top,    f.left);
       outCorners[1] = getIntersectionPoint(f.near, f.top,    f.right);
       outCorners[2] = getIntersectionPoint(f.near, f.bottom, f.left);
       outCorners[3] = getIntersectionPoint(f.near, f.bottom, f.right);
       outCorners[4] = getIntersectionPoint(f.far,  f.top,    f.left);
       outCorners[5] = getIntersectionPoint(f.far,  f.top,    f.right);
       outCorners[6] = getIntersectionPoint(f.far,  f.bottom, f.left);
       outCorners[7] = getIntersectionPoint(f.far,  f.bottom, f.right);
    }

    float classify( const Plane& plane, const vec3& center, const vec3& extents )
    {
        float r = fabsf( extents.x * plane.normal.x )
        + fabsf( extents.y * plane.normal.y )
        + fabsf( extents.z * plane.normal.z );
        float d = dot(plane.normal, center) + plane.distance;
        if (fabsf(d) < r) {
            return 0.0f;
        }
        else if (d < 0.0f)
        {
            return d + r;
        }
        return d - r;
    }

    float classify( const Plane& plane, const OBB& obb )
    {
        vec3 normal = obb.axes * plane.normal;
        float r = fabsf(obb.extents.x * normal.x)
        + fabsf(obb.extents.y * normal.y)
        + fabsf(obb.extents.z * normal.z);

        // signed distance between box center and plane
        float d = dot(plane.normal, obb.center) + plane.distance;

        // return signed distance
        if (fabsf(d) < r) {
            return 0.0f;
        }
        else if (d < 0.0f) {
            return d + r;
        }
        return d - r;
    }

    bool testFrustumAABB( const Frustum& f, const AABB& aabb )
    {
        vec3 extents = (aabb.max - aabb.min);
        vec3 center = (aabb.max + aabb.min) * 0.5f;
        for (int i = 0; i < 6; ++i)
        {
            if ( classify(f.planes[i], center, extents) < 0.f )
            {
                return false;
            }
        }
        return true;
    }

    bool testFrustumOBB( const Frustum& f, const OBB& obb )
    {
        for (int i = 0; i < 6; ++i)
        {
            if ( classify( f.planes[i], obb ) < 0.f )
            {
                return false;
            }
        }
        return true;
    }

    bool testFrustumPoint( const Frustum& f, const Point& p )
    {
       for (int i = 0; i < 6; ++i)
       {
           vec3 normal = f.planes[i].normal;
           float dist = f.planes[i].distance;
           float side = dot(p, normal) + dist;
           if (side < 0.0f)
           {
               return false;
           }
       }
       return true;
    }

    bool testFrustumSphere( const Frustum& f, const Sphere& s )
    {
       for (int i = 0; i < 6; ++i)
       {
           vec3 normal = f.planes[i].normal;
           float dist = f.planes[i].distance;
           float side = dot( s.center, normal ) + dist;
           if (side < -s.radius) {
               return false;
           }
       }
       return true;
    }
}
