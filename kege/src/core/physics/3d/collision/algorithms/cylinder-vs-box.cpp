//
//  cylinder-vs-box.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "cylinder-vs-box.hpp"

namespace kege::algo{

    bool cylinderBoxCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        const Cylinder* cylinder = a->collider->getCylinder();
        const OBB* obb = b->collider->getBox();

        // Define the axes to test
        vec3 axesToTest[7] =
        {
            obb->axes[0],
            obb->axes[1],
            obb->axes[2], // OBB axes
            cylinder->axes[0],                         // Cylinder axis
            cross(obb->axes[0], cylinder->axes[0]),      // Cross products
            cross(obb->axes[1], cylinder->axes[0]),
            cross(obb->axes[2], cylinder->axes[0])
        };

        float minPenetrationDepth = std::numeric_limits<float>::max();
        vec3 collisionNormal;

        // Check for overlap on each axis
        for (const vec3& axis : axesToTest)
        {
            if ( magn( axis ) < 1e-6 ) continue; // Skip near-zero length axes

            Interval interval[2];

            // Project OBB onto the axis
            projectOBB( obb, axis, interval[0] );

            // Project Cylinder onto the axis
            projectCylinder(cylinder, axis, interval[1]);

            // Check for overlap
            if (interval[0].max < interval[1].min || interval[1].max < interval[0].min)
            {
                return false; // No collision
            }

            // Calculate penetration depth
            float penetrationDepth = std::min(interval[0].max, interval[1].max) - std::max(interval[0].min, interval[1].min);
            if (penetrationDepth < minPenetrationDepth) {
                minPenetrationDepth = penetrationDepth;
                collisionNormal = axis;
            }
        }

        // If all axes overlap, there is a collision
        CollisionManifold* collision = collisions.generate();
        collision->normal = normalize( collisionNormal );
        collision->contacts[0].depth = minPenetrationDepth;
        collision->contact_count = 1;
        collision->objects[0] = a;
        collision->objects[1] = b;
        // Additional collision response data can be calculated here
        return true;
    }

    bool boxCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions )
    {
        uint32_t index = collisions.count();
        if( cylinderBoxCollision( b, a, collisions ) )
        {
            collisions[index]->objects[0] = b;
            collisions[index]->objects[1] = a;
            return true;
        }
        return false;
    }

}
