//
//  rayhit-capsule.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-capsule.hpp"
#include "rayhit-sphere.hpp"
#include "rayhit-cylinder.hpp"

namespace kege::algo{

    /**
     Notes
     We leverage your existing rayhitSphere() and rayhitCylinder() here.
     The closest hit (if there are multiple) is reported.
     This version doesn’t return the normal, but you can compute it manually when needed (from the capsule surface).
     */
    bool rayhitCapsule(const Ray& ray, const Capsule& capsule, RayHit* out_hit)
    {
        RayHit hit_cylinder;
        bool hit_any = false;
        float min_distance = std::numeric_limits<float>::max();

        vec3 start = capsule.center - capsule.axes[0] * capsule.height * 0.5;
        vec3 end = capsule.center + capsule.axes[0] * capsule.height * 0.5;

        // Treat the cylinder body
        Cylinder body;
        body.center = capsule.center;
        body.axes[0] = capsule.axes[0];
        body.axes[1] = capsule.axes[1];
        body.radius = capsule.radius;
        body.height = capsule.height;

        if (rayhitCylinder(ray, body, &hit_cylinder))
        {
            min_distance = hit_cylinder.distance;
            if (out_hit) *out_hit = hit_cylinder;
            hit_any = true;
        }

        // Check hemisphere at start
        Sphere sphere_a;
        sphere_a.center = start;
        sphere_a.radius = capsule.radius;

        RayHit hit_sphere_a;
        if (rayhitSphere(ray, sphere_a, &hit_sphere_a))
        {
            if (hit_sphere_a.distance < min_distance)
            {
                min_distance = hit_sphere_a.distance;
                if (out_hit) *out_hit = hit_sphere_a;
            }
            hit_any = true;
        }

        // Check hemisphere at end
        Sphere sphere_b;
        sphere_b.center = end;
        sphere_b.radius = capsule.radius;

        RayHit hit_sphere_b;
        if (rayhitSphere(ray, sphere_b, &hit_sphere_b))
        {
            if (hit_sphere_b.distance < min_distance)
            {
                min_distance = hit_sphere_b.distance;
                if (out_hit) *out_hit = hit_sphere_b;
            }
            hit_any = true;
        }

        return hit_any;
    }

}
