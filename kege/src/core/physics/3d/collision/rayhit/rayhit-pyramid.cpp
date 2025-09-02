//
//  rayhit-pyramid.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-pyramid.hpp"
#include "rayhit-triangle.hpp"

namespace kege::algo{

    bool rayhitPyramid(const Ray& ray, const Pyramid& pyramid, RayHit* out_hit)
    {
        bool hit = false;
        float closest_t = std::numeric_limits<float>::max();
        RayHit temp_hit;

        // Check the 4 side faces (triangles)
        for (int i = 0; i < 4; ++i)
        {
            int next = (i + 1) % 4;
            Triangle tri = { pyramid.apex, pyramid.base[i], pyramid.base[next] };

            if (rayhitTriangle(ray, tri, &temp_hit))
            {
                if (temp_hit.distance < closest_t)
                {
                    closest_t = temp_hit.distance;
                    if (out_hit)
                        *out_hit = temp_hit;
                    hit = true;
                }
            }
        }

        // Check the base (as 2 triangles)
        Triangle base_tri_1 = { pyramid.base[0], pyramid.base[1], pyramid.base[2] };
        Triangle base_tri_2 = { pyramid.base[0], pyramid.base[2], pyramid.base[3] };

        if (rayhitTriangle(ray, base_tri_1, &temp_hit))
        {
            if (temp_hit.distance < closest_t)
            {
                closest_t = temp_hit.distance;
                if (out_hit)
                    *out_hit = temp_hit;
                hit = true;
            }
        }

        if (rayhitTriangle(ray, base_tri_2, &temp_hit))
        {
            if (temp_hit.distance < closest_t)
            {
                closest_t = temp_hit.distance;
                if (out_hit)
                    *out_hit = temp_hit;
                hit = true;
            }
        }

        return hit;
    }

}
