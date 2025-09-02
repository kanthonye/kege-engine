//
//  rayhit-cylinder.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-cylinder.hpp"

namespace kege::algo{

    /**
     Explanation
     We project the ray onto a space orthogonal to the cylinder axis to reduce the problem to 2D.
     We check if the intersection point lies within the finite bounds of the cylinder’s axis (between pa and pb).
     We do not check for intersection with the cylinder caps in this version (we can add that too if needed).
     */
    bool rayhitCylinder(const Ray& ray, const Cylinder& cylinder, RayHit* out_hit)
    {
        const vec3& p = cylinder.center;
        const vec3& d = cylinder.axes[0]; // cylinder's up direction
        const float h = cylinder.height;
        const float r = cylinder.radius;

        vec3 co = ray.origin - p;
        vec3 rd = ray.direction;

        // Project onto plane perpendicular to axis to simplify
        vec3 d_norm = normalize(d);
        vec3 co_perp = co - dot(co, d_norm) * d_norm;
        vec3 rd_perp = rd - dot(rd, d_norm) * d_norm;

        float a = dot(rd_perp, rd_perp);
        float b = 2.0f * dot(rd_perp, co_perp);
        float c = dot(co_perp, co_perp) - r * r;

        //float t_side = -1.0f;
        bool hit = false;
        RayHit best_hit;
        best_hit.distance = FLT_MAX;

        // Solve side intersection (quadratic)
        float disc = b * b - 4 * a * c;
        if (disc >= 0.0f)
        {
            float sqrt_disc = sqrt(disc);
            float inv_denom = 1.0f / (2 * a);
            float t0 = (-b - sqrt_disc) * inv_denom;
            float t1 = (-b + sqrt_disc) * inv_denom;

            for (float t : { t0, t1 })
            {
                if (t > 0.0f)
                {
                    vec3 hit_point = ray.origin + ray.direction * t;
                    float height_proj = dot(hit_point - p, d_norm);

                    if (fabs(height_proj) <= h * 0.5f) // within cylinder height
                    {
                        hit = true;
                        if (t < best_hit.distance)
                        {
                            best_hit.distance = t;
                            best_hit.point = hit_point;
                        }
                    }
                }
            }
        }

        // Cap intersections
        for (int i = -1; i <= 1; i += 2)
        {
            vec3 cap_center = p + d_norm * (0.5f * h * i);
            float denom = dot(ray.direction, d_norm);
            if (fabs(denom) > 1e-6f)
            {
                float t = dot(cap_center - ray.origin, d_norm) / denom;
                if (t > 0.0f)
                {
                    vec3 hit_point = ray.origin + ray.direction * t;
                    vec3 to_center = hit_point - cap_center;
                    if (dot(to_center, to_center) <= r * r)
                    {
                        hit = true;
                        if (t < best_hit.distance)
                        {
                            best_hit.distance = t;
                            best_hit.point = hit_point;
                        }
                    }
                }
            }
        }

        if (hit && out_hit)
        {
            *out_hit = best_hit;
            return true;
        }

        return false;
    }

}
