//
//  rayhit-rect.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-rect.hpp"
#include "rayhit-plane.hpp"

namespace kege::algo{

    bool rayhitRect(const Ray& ray, const Rect& rect, RayHit* out_hit)
    {
        vec3 normal = normalize(cross(rect.axie[0], rect.axie[1]));

        Plane plane;
        plane.normal = normal;
        plane.point = rect.center;
        plane.one_sided = 0;

        RayHit temp_hit;
        if (!rayhitPlane(ray, plane, &temp_hit))
            return false;

        // Convert hit point into local space of rectangle
        vec3 local = temp_hit.point - rect.center;

        float x = dot(local, rect.axie[0]);
        float y = dot(local, rect.axie[1]);

        if (fabs(x) <= rect.extents.x && fabs(y) <= rect.extents.y)
        {
            if (out_hit)
            {
                *out_hit = temp_hit;
            }
            return true;
        }

        return false;
    }

}
