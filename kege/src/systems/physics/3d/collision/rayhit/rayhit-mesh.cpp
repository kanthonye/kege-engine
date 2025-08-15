//
//  rayhit-mesh.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-mesh.hpp"
#include "rayhit-plane.hpp"

namespace kege::algo{

    bool rayhitPolygon(const Ray& ray, const Polygon& polygon, RayHit* out_hit)
    {
        if (polygon.points.size() < 3)
            return false;

        // Build plane from first 3 points
        vec3 a = polygon.points[0];
        vec3 b = polygon.points[1];
        vec3 c = polygon.points[2];

        vec3 normal = normalize(cross(b - a, c - a));

        Plane plane;
        plane.normal = normal;
        plane.point = a;
        plane.one_sided = 0;

        RayHit temp_hit;
        if (!rayhitPlane(ray, plane, &temp_hit))
            return false;

        // Project hit point into 2D for polygon test
        vec3 u = normalize(b - a);
        vec3 v = normalize(cross(normal, u)); // orthogonal to u and normal

        // Convert point to local 2D coordinates in polygon space
        vec2 hit_2d = { dot(temp_hit.point - a, u), dot(temp_hit.point - a, v) };

        // Convert polygon points into local 2D space
        std::vector<vec2> poly_2d;
        for (const vec3& pt : polygon.points)
        {
            vec2 p = { dot(pt - a, u), dot(pt - a, v) };
            poly_2d.push_back(p);
        }

        // Point-in-polygon using edge-crossing
        bool inside = true;
        for (size_t i = 0, j = poly_2d.size() - 1; i < poly_2d.size(); j = i++)
        {
            const vec2& pi = poly_2d[i];
            const vec2& pj = poly_2d[j];

            vec2 edge = pi - pj;
            vec2 to_point = hit_2d - pj;
            float cross_z = edge.x * to_point.y - edge.y * to_point.x;

            if (cross_z < 0.0f)
            {
                inside = false;
                break;
            }
        }

        if (inside)
        {
            if (out_hit)
                *out_hit = temp_hit;
            return true;
        }

        return false;
    }

}
