//
//  frustum.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/1/24.
//

#ifndef frustum_hpp
#define frustum_hpp

#include "camera.hpp"
#include "collisions-3D-shapes.hpp"

namespace kege{

    struct Frustum
    {
        union
        {
            struct
            {
                Plane top;
                Plane bottom;
                Plane left;
                Plane right;
                Plane near;
                Plane far;
            };
            Plane planes[6];
        };
        inline Frustum(){}
    };



    Frustum getFrustum( const kege::mat44& projection, const kege::mat44& transform );
    void GetCorners(const Frustum& f, vec3* outCorners);

    float classify( const Plane& plane, const vec3& center, const vec3& extents );
    float classify( const Plane& plane, const OBB& obb );
    Point getIntersectionPoint(Plane p1, Plane p2, Plane p3);

    bool testFrustumSphere( const Frustum& f, const Sphere& s );
    bool testFrustumPoint( const Frustum& f, const Point& p );
    bool testFrustumAABB( const Frustum& f, const AABB& aabb );
    bool testFrustumOBB( const Frustum& f, const OBB& obb );
}
#endif /* frustum_hpp */
