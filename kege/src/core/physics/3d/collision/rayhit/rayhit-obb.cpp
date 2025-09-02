//
//  rayhit-obb.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-obb.hpp"

namespace kege::algo{

    bool rayhitOBB( const Ray& ray, const OBB& obb, RayHit* result )
    {
        // Transform ray into OBB's local space
        Vec3 direction = ray.origin - obb.center;

        // Project ray origin and direction onto OBB's local axes
        float tMin = -1e10f; // Negative infinity (or a large negative value)
        float tMax = 1e10f;  // Positive infinity (or a large positive value)

        // Test intersection along each OBB axis
        for (int i = 0; i < 3; i++)
        {
            float originProj = dot(direction, obb.axes[i]);
            float dir_proj = dot(ray.direction, obb.axes[i]);

            // Avoid division by zero
            if (abs(dir_proj) < 0.0001f)
            {
                // Ray is parallel to this slab
                if (originProj < -obb.extents[i] || originProj > obb.extents[i])
                {
                    return false; // No intersection, outside slab
                }
            }
            else
            {
                // Compute intersection distances with the two planes (slabs) of this axis
                float t1 = (-obb.extents[i] - originProj) / dir_proj;
                float t2 = (obb.extents[i] - originProj) / dir_proj;

                // Ensure t1 is the near intersection, t2 is the far
                if (t1 > t2) {
                    float temp = t1;
                    t1 = t2;
                    t2 = temp;
                }

                // Update the overall tMin and tMax
                tMin = max(tMin, t1);
                tMax = min(tMax, t2);

                // Early exit if no valid intersection range remains
                if (tMin > tMax)
                {
                    return false; // No intersection
                }
            }
        }

        if ( result )
        {
            result->distance = tMin; // Ray origin is inside
            result->point = ray.origin + abs(result->distance) * ray.direction;
        }

        // If tMin is positive, there’s an intersection ahead of the ray origin
        if (tMin >= 0)
        {
            return true;
        }
        // If tMax is negative, intersection is behind the ray origin (ignore)
        else if (tMax < 0)
        {
            return false;
        }
        // If tMin is negative and tMax is positive, ray starts inside the OBB
        else
        {
            return true;
        }

        return false;
    }

}
