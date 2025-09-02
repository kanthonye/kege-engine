//
//  rayhit-sphere.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "rayhit-sphere.hpp"

namespace kege::algo{

    bool rayhitSphere( const Ray& ray, const Sphere& sphere, RayHit* result )
    {
        // Calculate the vector from the ray origin to the sphere center
        kege::vec3 L = kege::vec3(sphere.center.x, sphere.center.y, sphere.center.z) - ray.origin;

        // Calculate the dot product of the ray direction and the vector L
        float tca = kege::dot(L, ray.direction);

        // If tca is less than 0, the sphere is behind the ray origin
        if (tca < 0) return false;

        // Calculate the squared distance from the ray origin to the sphere center
        float d2 = dot(L, L) - tca * tca;

        // If d2 is greater than the squared radius, the ray misses the sphere
        if (d2 > sphere.radius * sphere.radius) return false;

        // Calculate the distance from the ray origin to the hit point
        float thc = std::sqrt(sphere.radius * sphere.radius - d2);

        // Calculate the distance to the hit point
        float distance = tca - thc;

        // If distance is less than 0, the hit point is behind the ray origin
        if (distance < 0) distance = tca + thc;

        // Calculate the hit point
        kege::vec3 point = ray.origin + ray.direction * distance;

        // Store the hit data
        if ( result )
        {
            result->distance = distance;
            result->point = point;
        }
        return true;
    }


//    bool rayhitCircle( const Ray& ray, const Circle& circle, RayHit* result )
//    {
//        // Vector from the ray origin to the circle center
//        vec3 originToCenter = circle.center - ray.origin;
//
//        // Project the vector from the origin to the center onto the ray direction
//        float projectionLength = dot(originToCenter, ray.direction);
//
//        // Calculate the closest point on the ray to the circle center
//        vec3 closestPoint = ray.origin + ray.direction * projectionLength;
//
//        // Calculate the distance from the closest point to the circle center
//        float distanceToClosest = magn(closestPoint - ray.origin);
//
//        // If the distance is greater than the radius, the ray misses the circle
//        if (distanceToClosest > circle.radius) {
//            return false;
//        }
//
//        // Calculate the distance from the closest point to the intersection point
//        float distanceToIntersection = sqrtf(circle.radius * circle.radius - distanceToClosest * distanceToClosest);
//
//        // Calculate the intersection point
//        if ( result )
//        {
//            result->distance = distanceToIntersection;
//            result->point = closestPoint - ray.direction * distanceToIntersection; // First intersection
//        }
//        return true;
//    }
    bool rayhitCircle( const Ray& ray, const Circle& circle, RayHit* result )
    {
        float denom = kege::dot(ray.direction, circle.normal);
        if (std::abs(denom) < 1e-6f) {
            // Ray is parallel to circle's plane
            return false;
        }

        float t = kege::dot(circle.center - ray.origin, circle.normal) / denom;
        if (t < 0.0f) {
            // Intersection is behind ray origin
            return false;
        }

        kege::vec3 hitPoint = ray.origin + t * ray.direction;

        // Check if the point lies within the circle
        float dist_sq = kege::magn(hitPoint - circle.center);
        if (dist_sq <= circle.radius * circle.radius)
        {
            if ( result )
            {
                result->distance = t;
                result->point = ray.origin + t * ray.direction; // First intersection
            }
            return true;
        }

        return false;
    }

}
