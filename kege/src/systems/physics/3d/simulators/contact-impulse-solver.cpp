//
//  contact-impulse-solver.cpp
//  kege
//
//  Created by Kenneth Esdaile on 10/27/24.
//

#include "collider.hpp"
#include "draw-commands.hpp"
#include "contact-impulse-solver.hpp"

#include "physics-simulation.hpp"

namespace kege::physics{

    void ContactImpulseSolver::resolveImpulse( double time_step, CollisionRegistry& collision_manager )
    {
        CollisionState state;
        CollisionManifold* collision;
        Rigidbody* a;
        Rigidbody* b;

        vec3 impulse;

        for (int k = 0; k < collision_manager.count(); k++)
        {
            collision = collision_manager[ k ];
            a = collision_manager[ k ]->objects[0];
            b = collision_manager[ k ]->objects[1];
            setCollisionState( collision, &state );

            if ( state.inv_mass_sum > 0.f )
            {
                for (int i = 0; i < collision->contact_count; ++i)
                {
                    computeRelativeVelocity( collision->contacts[i], &state );
                    if ( state.v_dot_n < 0 )
                    {
                        impulse = getNormalImpulse( a, b, &state );
                        impulse /= float( collision->contact_count );

                        if ( !a->immovable )
                        {
                            applyLinearImpulse( a, -impulse );
                            applyAngularImpulse( a, state.relative_position[0], -impulse );
                        }
                        if ( !b->immovable )
                        {
                            applyLinearImpulse( b, impulse );
                            applyAngularImpulse( b, state.relative_position[1], impulse );
                        }

                        impulse = getFrictionImpulse( a, b, &state );
                        impulse /= float( collision->contact_count );

                        if ( !a->immovable )
                        {
                            applyLinearImpulse( a, -impulse );
                            applyAngularImpulse( a, state.relative_position[0], -impulse );
                        }
                        if ( !b->immovable )
                        {
                            applyLinearImpulse( b, impulse );
                            applyAngularImpulse( b, state.relative_position[1], impulse );
                        }
                    }
                }
            }
        }
    }

    void ContactImpulseSolver::setCollisionState( CollisionManifold* collision, CollisionState* state )
    {
        state->contact_count = collision->contact_count;
        state->normal = collision->normal;
        state->immovable[0] = collision->objects[0]->immovable;
        state->immovable[1] = collision->objects[1]->immovable;
        state->shape[0] = collision->objects[0]->collider->shape_type;
        state->shape[1] = collision->objects[1]->collider->shape_type;
        /**
         * To maintain stability in the system, it's crucial to store the initial values
         * of the objects' states (e.g., position, velocity, orientation) before resolving
         * each contact point. This ensures that all contact points are resolved using the
         * initial state data, rather than the updated state data that may have changed
         * after resolving previous contact points.
         *
         * If the updated state data is used, it can lead to instabilities in the system,
         * such as:
         *
         *  • Drifting: objects slowly moving away from their intended positions
         *  • Vibration: objects oscillating or wobbling uncontrollably
         *  • Gittery motion: objects exhibiting jerky or unpredictable motion
         *
         * By using the initial state data, you can ensure a more consistent and stable
         * resolution of contact points, which is essential for maintaining a realistic
         * and predictable simulation.
         */
        state->angular_velocity[0] = collision->objects[0]->angular.velocity;
        state->linear_velocity[0] = collision->objects[0]->linear.velocity;
        state->center[0] = collision->objects[0]->center;

        state->angular_velocity[1] = collision->objects[1]->angular.velocity;
        state->linear_velocity[1] = collision->objects[1]->linear.velocity;
        state->center[1] = collision->objects[1]->center;

        // compute friction between the two objects
        state->friction = sqrtf( collision->objects[0]->friction * collision->objects[1]->friction);
        state->static_friction = 0.75 * state->friction;

        // compute coefficient of restitution between the two objects
        state->restitution = fminf( collision->objects[0]->cor, collision->objects[1]->cor);

        // compute the inverse mass sum. immovable object should not contribute
        state->inv_mass_sum = 0;
        if ( !collision->objects[0]->immovable ) state->inv_mass_sum += collision->objects[0]->linear.invmass;
        if ( !collision->objects[1]->immovable ) state->inv_mass_sum += collision->objects[1]->linear.invmass;
        //if ( !collision->objects[0]->immovable ) setAwake( collision->objects[0], true );
        //if ( !collision->objects[1]->immovable ) setAwake( collision->objects[1], true );
    }

    void ContactImpulseSolver::computeRelativeVelocity( const Contact& contact, CollisionState* state )
    {
        state->relative_position[0] = vec3{0,0,0};
        state->relative_position[1] = vec3{0,0,0};
        vec3 v[2], p[2];
        v[0] = vec3{0,0,0};
        v[1] = vec3{0,0,0};

        if ( !state->immovable[0] )
        {
            v[0] = state->linear_velocity[0];

            if ( state->shape[0] != RIGID_SHAPE_PLANE )
                state->relative_position[0] = contact.point - state->center[0];
        }

        if ( !state->immovable[1] )
        {
            v[1] = state->linear_velocity[1];

            if ( state->shape[1] != RIGID_SHAPE_PLANE )
                state->relative_position[1] = contact.point - state->center[1];
        }

        p[0] = v[0] + cross( state->angular_velocity[0], state->relative_position[0] );
        p[1] = v[1] + cross( state->angular_velocity[1], state->relative_position[1] );
        state->relative_velocity = p[1] - p[0];

        state->v_dot_n = dot(state->relative_velocity, state->normal);
    }

    vec3 ContactImpulseSolver::getNormalImpulse( Rigidbody* a, Rigidbody* b, CollisionState* state )
    {
        float numerator = -(1.0f + state->restitution) * state->v_dot_n;
        vec3 p0 = cross( a->angular.inertia_inverse * cross( state->relative_position[0], state->normal), state->relative_position[0] );
        vec3 p1 = cross( b->angular.inertia_inverse * cross( state->relative_position[1], state->normal), state->relative_position[1] );
        float denominator = state->inv_mass_sum + dot( state->normal, p0 + p1);
        state->j = (CMP(denominator, 0.0f)) ? 0.0f : (numerator / denominator);
        return state->j * state->normal;
    }

    vec3 ContactImpulseSolver::getFrictionImpulse( Rigidbody* a, Rigidbody* b, CollisionState* state )
    {
        kege::vec3 tangent = state->relative_velocity - (state->normal * state->v_dot_n);
        float tangent_speed = magn( tangent );
        if (tangent_speed < 1e-4) return vec3{0,0,0};

        float static_threshold = state->j * state->static_friction;

        // Check for static vs. kinetic friction
        if (tangent_speed < static_threshold)
        {
            // Apply full static friction to zero out tangent speed
            return -tangent;
        }
        else // kinetic friction
        {
            tangent = tangent / tangent_speed; // normalize tangent
            float numerator = -dot( state->relative_velocity, tangent);
            vec3 p0 = cross( a->angular.inertia_inverse * cross( state->relative_position[0], tangent), state->relative_position[0] );
            vec3 p1 = cross( a->angular.inertia_inverse * cross( state->relative_position[1], tangent), state->relative_position[1] );
            float denominator = state->inv_mass_sum + dot(tangent, p0 + p1);
            float jt = (CMP(denominator, 0.0f)) ? 0.0f : numerator / denominator;
            
            jt = clamp(jt, -state->j * state->friction, state->j * state->friction);

            // the friction impulse
            return tangent * jt;
        }
    }

    void ContactImpulseSolver::simulate( double dms )
    {
        for (int i = 0; i < _update_iterations; i++)
        {
            resolveImpulse( dms, _simulator->getCollisionRegistry() );
        }
    }

    ContactImpulseSolver::ContactImpulseSolver()
    :   _update_iterations( 8 )
    {}
}
