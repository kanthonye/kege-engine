//
//  collision-position-resolver.cpp
//  kege
//
//  Created by Kenneth Esdaile on 10/27/24.
//

#include <set>
#include "collider.hpp"
#include "collision-position-solver.hpp"

#include "physics-simulation.hpp"

#include "draw-commands.hpp"
#include "communication.hpp"

namespace kege::physics{

    auto compareCollisions( const CollisionManifold* a, const CollisionManifold* b )
    {
        // Use the minimum Y-position of the two objects in each collision
        float a_min_y = std::min(a->objects[0]->center.y, a->objects[1]->center.y);
        float b_min_y = std::min(b->objects[0]->center.y, b->objects[1]->center.y);
        return a_min_y < b_min_y; // Lower objects first
    }

    void PositionCorrectionSolver::simulate( double dms )
    {
        CollisionRegistry& collision_registry = _simulator->getCollisionRegistry();

        std::vector< CollisionManifold* > sorted_collisions;
        for (int i = 0; i < collision_registry.count(); ++i)
        {
            sorted_collisions.push_back( collision_registry[ i ] );
        }

        // Sort collisions before resolving. Sort collisions from bottom-to-top
        std::sort( sorted_collisions.begin(), sorted_collisions.end(), compareCollisions );

        for (int i=0; i<_iterations; ++i)
        {
            rotationalCorrection( dms, sorted_collisions );
        }
    }


    void PositionCorrectionSolver::rotationalCorrection( double dms, std::vector< CollisionManifold* >& sorted_collisions )
    {
        Rigidbody *a, *b;

        float inv_mass_sum;
        float correction;

        kege::vec3 torque[2];
        kege::vec3 angular[2];
        kege::vec3 relative_position[2];
        kege::quat rotation_correction[2];

        const float bias = 0.0;
        const float correction_scale = 1.0 / float(_iterations);

        for ( CollisionManifold* collision : sorted_collisions )
        {
            if ( collision->contact_count == 0 ) continue;

            a = collision->objects[0];
            b = collision->objects[1];

            inv_mass_sum = 0;
            if ( !a->immovable ) inv_mass_sum += a->linear.invmass;
            if ( !b->immovable ) inv_mass_sum += b->linear.invmass;

            for (int j = 0; j < collision->contact_count; ++j)
            {
                correction  = collision->contacts[j].depth / (inv_mass_sum + bias);
                correction /= float(collision->contact_count);
                correction *= correction_scale;
                //correction  = std::min(correction, max_correction);

                if ( !a->immovable )
                {
                    relative_position[0] = collision->contacts[j].point - a->center;
                    torque[0] = kege::cross(relative_position[0],-collision->normal);
                    angular[0] = a->angular.inertia_inverse * torque[0];
                    rotation_correction[0] = kege::quat(angular[0] * correction);

                    a->center -= collision->normal * correction * a->linear.invmass;
                    a->orientation = kege::normalize( rotation_correction[0] * a->orientation );
                }

                if ( !b->immovable )
                {
                    relative_position[1] = collision->contacts[j].point - b->center;
                    torque[1] = kege::cross(relative_position[1], collision->normal);
                    angular[1] = b->angular.inertia_inverse * torque[1];
                    rotation_correction[1] = kege::quat(angular[1] * correction);

                    b->center += collision->normal * correction * b->linear.invmass;
                    b->orientation = kege::normalize( rotation_correction[1] * b->orientation );
                }
            }
        }
    }

    void PositionCorrectionSolver::linearCorrection( double dms, std::vector< CollisionManifold* >& sorted_collisions )
    {
        Rigidbody *a, *b;
        vec3 correction;
        float inv_mass_sum = 0;
        vec3 relative_position[2];

        const float correction_scale = 1.0 / float(_iterations);

        for ( CollisionManifold* collision : sorted_collisions )
        {
            if ( collision->contact_count == 0 )
            {
                continue;
            }
            
            correction = {0.f, 0.f, 0.f};
            for (int j = 0; j < collision->contact_count; ++j)
            {
                correction += collision->normal * collision->contacts[j].depth;
            }
            correction /= (collision->contact_count != 0) ? float(collision->contact_count): 1.f;
            correction *= correction_scale;

            a = collision->objects[0];
            b = collision->objects[1];

            inv_mass_sum = 0;
            if ( !a->immovable ) inv_mass_sum += a->linear.invmass;
            if ( !b->immovable ) inv_mass_sum += b->linear.invmass;

            if ( inv_mass_sum > 0 )
            {
                if ( !a->immovable )
                {
                    a->center -= correction * (a->linear.invmass / inv_mass_sum);
                    a->collider->integrate( a );
                }
                if ( !b->immovable )
                {
                    b->center += correction * (b->linear.invmass / inv_mass_sum);
                    b->collider->integrate( b );
                }
            }
        }
    }

    PositionCorrectionSolver::PositionCorrectionSolver()
    :   _iterations( 8 )
    {}

}
