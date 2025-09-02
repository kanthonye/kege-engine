//
//  rayhit-triangle.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-triangle.hpp"

namespace kege::algo{

    bool rayhitTriangle(const Ray& ray, const Triangle& triangle, RayHit* out_hit)
    {
        /**
         How It Works
         This algorithm computes barycentric coordinates (u, v) to check if the ray intersects the triangle.
         It avoids explicitly computing the triangle plane and normal.
         Efficient and precise for both front- and back-facing triangles.
         */
        const vec3& a = triangle.a;
        const vec3& b = triangle.b;
        const vec3& c = triangle.c;

        vec3 edge1 = b - a;
        vec3 edge2 = c - a;

        vec3 h = cross(ray.direction, edge2);
        float det = dot(edge1, h);

        if (std::abs(det) < KEGE_EPSILON_F)
            return false; // Ray is parallel to triangle

        float inv_det = 1.0f / det;
        vec3 s = ray.origin - a;
        float u = dot(s, h) * inv_det;

        if (u < 0.0f || u > 1.0f)
            return false;

        vec3 q = cross(s, edge1);
        float v = dot(ray.direction, q) * inv_det;

        if (v < 0.0f || u + v > 1.0f)
            return false;

        float t = dot(edge2, q) * inv_det;

        if (t < KEGE_EPSILON_F)
            return false; // Triangle is behind the ray

        if (out_hit)
        {
            out_hit->distance = t;
            out_hit->point = ray.origin + ray.direction * t;
        }

        return true;
    }

}
