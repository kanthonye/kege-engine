//
//  rayhit-aabb.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-aabb.hpp"
namespace kege::algo{

    bool rayhitAABB(const Ray& ray, const AABB& box, RayHit* result)
    {
        /**
         * The slab method treats each axis-aligned pair of box faces as a slab.
         * We check where the ray enters and exits each slab.
         * If at any point the slabs’ ranges don’t overlap (tMin > tMax), there’s no intersection.
         * If the intersection is valid, we populate the RayHit with distance and hit point.
         */
        float tMin = 0.0f;
        float tMax = std::numeric_limits<float>::max();

        for (int i = 0; i < 3; ++i)
        {
            float origin = ray.origin[i];
            float direction = ray.direction[i];
            float min = box.min[i];
            float max = box.max[i];

            if (std::abs(direction) < KEGE_EPSILON_F)
            {
                // Ray is parallel to slab. No hit if origin not within slab
                if (origin < min || origin > max)
                    return false;
            }
            else
            {
                float ood = 1.0f / direction;
                float t1 = (min - origin) * ood;
                float t2 = (max - origin) * ood;

                if (t1 > t2) std::swap(t1, t2);

                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);

                if (tMin > tMax)
                    return false;
            }
        }

        // If we got here, ray hits box between tMin and tMax
        if (result)
        {
            result->distance = tMin;
            result->point = ray.origin + ray.direction * tMin;
        }

        return true;
    }

}
