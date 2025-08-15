//
//  force-integrator.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#include "physics-simulation.hpp"
#include "force-integrator.hpp"

namespace kege::physics{

    void ForceIntegrator::simulate( double time_step )
    {
        for (ComponentCacheT< Rigidbody >::Iterator body = _simulator->rigidbodies().begin(); body != _simulator->rigidbodies().end(); body++ )
        {
            if ( body->sleepable )
            {
                if ( !body->is_awake )
                {
                    return;
                };
            }

            if ( !body->immovable )
            {
                /**
                 * compute the Linear Acceleration by integrating the forces acting on the object
                 */
                body->linear.acceleration = body->linear.forces * body->linear.invmass;

                /**
                 * update the linear velocity by acceleration
                 */
                body->linear.velocity += body->linear.acceleration * time_step;
                zeroSmallComponents( body->linear.velocity, 1e-4f );

                /**
                 * update the objects position
                 */
                body->prev = body->center;
                body->center += body->linear.velocity * time_step;

                /**
                 * Angular acceleration is not directly the amount of rotation, it represents the
                 * rate of change of angular velocity. Angular Acceleration tells you how much the
                 * angular velocity changes.
                 */
                kege::vec3 acceleration = body->angular.torques * body->angular.inertia_inverse;

                /**
                 * Angular velocity is then used to determine how much the object rotates during
                 * the current time step. This is what is used to update the orientation. Here,
                 * the (angular_velocity_step) is the amount of rotation that happens during this
                 * small time interval.
                 */
                body->angular.velocity += time_step * acceleration;
                zeroSmallComponents( body->angular.velocity, 1e-4f );

                /**
                 * The angle of rotation is the magnitude of the angular velocity vector multiplied
                 * by the time step. Compute the magnitude (the amount of rotation for this step)
                 */
                float angle = magnSq( body->angular.velocity );

                if (angle > 1e-4f)
                {
                    angle = time_step * sqrt( angle );
                    kege::quat angular_change = kege::quat( angle, body->angular.velocity / angle );

                    /**
                     * The orientation (quaternion) is updated based on how much the object rotates
                     * in the current time step:
                     */
                    body->orientation += angular_change * body->orientation * 0.5f;
                    body->orientation = kege::normalize( body->orientation );
                }
            }

            if ( body->collider )
            {
                body->collider->integrate( *body );
            }
        }
    }

}
