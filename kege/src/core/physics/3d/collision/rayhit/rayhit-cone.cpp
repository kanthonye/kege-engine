//
//  rayhit-cone.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-cone.hpp"
#include "rayhit-plane.hpp"

namespace kege::algo{

    bool rayhitCone(const Ray& ray, const Cone& cone, RayHit* out_hit)
    {
        vec3 apex_to_origin = ray.origin - cone.apex;
        float cos_theta = cone.radius / sqrt(cone.radius * cone.radius + cone.height * cone.height);
        float cos2 = cos_theta * cos_theta;

        vec3 d = ray.direction;
        vec3 v = cone.direction;

        float dv = dot(d, v);
        float ao_v = dot(apex_to_origin, v);

        vec3 d_proj = d - dv * v;
        vec3 ao_proj = apex_to_origin - ao_v * v;

        float a = dot(d_proj, d_proj) - cos2 * dv * dv;
        float b = 2.0f * (dot(d_proj, ao_proj) - cos2 * dv * ao_v);
        float c = dot(ao_proj, ao_proj) - cos2 * ao_v * ao_v;

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f)
            return false;

        float sqrt_disc = sqrt(discriminant);
        float t0 = (-b - sqrt_disc) / (2.0f * a);
        float t1 = (-b + sqrt_disc) / (2.0f * a);

        float t = std::numeric_limits<float>::max();
        bool hit = false;

        // Check intersection within valid cone height
        for (float ti : {t0, t1})
        {
            if (ti <= 0.0f)
                continue;

            vec3 p = ray.origin + d * ti;
            float height_along_cone = dot(p - cone.apex, v);
            if (height_along_cone >= 0.0f && height_along_cone <= cone.height)
            {
                if (ti < t)
                {
                    t = ti;
                    if (out_hit)
                    {
                        out_hit->distance = t;
                        out_hit->point = ray.origin + ray.direction * t;
                    }
                    hit = true;
                }
            }
        }

        // Optionally check hit on base disc
        vec3 base_center = cone.apex + cone.direction * cone.height;
        Plane base_plane;
        base_plane.normal = cone.direction;
        base_plane.point = base_center;

        RayHit base_hit;
        if (rayhitPlane(ray, base_plane, &base_hit))
        {
            vec3 to_hit = base_hit.point - base_center;
            if (dot(to_hit, to_hit) <= cone.radius * cone.radius)
            {
                if (base_hit.distance < t && base_hit.distance > 0.0f)
                {
                    if (out_hit)
                        *out_hit = base_hit;
                    hit = true;
                }
            }
        }

        return hit;
    }

}
