//
//  grounded-detector.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#include "grounded-detector.hpp"
#include "../simulation/physics-simulation.hpp"

namespace kege::physics{

    void GroundedDetector::simulate( double time_step )
    {
        CollisionRegistry& collision_registry = _simulator->getCollisionRegistry();
        CollisionManifold* collision;
        Rigidbody *a, *b;

        for (int i = 0; i < collision_registry.count(); ++i)
        {
            collision = collision_registry[i];
            a = collision->objects[0];
            b = collision->objects[1];

            for (int j = 0; j < collision->contact_count; ++j)
            {
                if ( !a->immovable ) testGrounded( a, b, collision->contacts[j].point );
                if ( !b->immovable ) testGrounded( b, a, collision->contacts[j].point );
            }
        }
    }

    void GroundedDetector::testGrounded( Rigidbody* body, Rigidbody* ground, const kege::vec3& contact_point )
    {
        /**
         * Calculate the dot product between the relative position (contact point minus object center)
         * and the object's up vector. This result indicates whether the collision occurred at the
         * object's foot (negative value) or head (positive value).
         */
        float offset = dot( body->up, contact_point - body->center );

        /**
         * We're looking for the object positioned above. If the offset is positive,
         * then the object is below us, so we can exit the loop.
         */
        if ( offset < 0 )
        {
            Ray ray;
            ray.direction = -body->up;
            ray.origin = body->center;

            algo::RayHit hit;
            algo::rayhit(ray, ground->collider.ref(), &hit);

            offset = abs( abs( hit.distance ) - abs( offset ) );
            if ( -_grounded_threshold < offset && offset < _grounded_threshold  )
            {
                //body->linear.forces -= (body->up * dot( body->up, body->linear.velocity )) / body->linear.invmass;
                body->grounded = true;
            }
        }
    }

}
