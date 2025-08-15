//
//  rayhit-plane.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-plane.hpp"

namespace kege::algo{

    bool rayhitPlane( const Ray& ray, const Plane& plane, RayHit* result )
    {
        // Calculate the denominator of the intersection formula
        float denominator = kege::dot(ray.direction, plane.normal);

        // Check if the ray is parallel to the plane
        if (std::abs(denominator) < 0.00001f)
        {
            return false;
        }

        // Calculate the distance along the ray to the intersection point
        result->distance = kege::dot(plane.point - ray.origin, plane.normal) / denominator;
        result->point = ray.origin + ray.direction * result->distance;

        // Check if the intersection point is behind the ray's origin
        if (result->distance < 0)
        {
            return false;
        }
        return true;
    }

}
