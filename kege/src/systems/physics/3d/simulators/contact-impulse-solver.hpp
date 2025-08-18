//
//  contact-impulse-solver.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/27/24.
//

#ifndef kege_contact_impulse_solver_hpp
#define kege_contact_impulse_solver_hpp

#include "simulator.hpp"

namespace kege::physics{

    struct CollisionState
    {
        vec3 relative_velocity;
        vec3 normal;
        vec3 angular_velocity[2];
        vec3 linear_velocity[2];
        vec3 center[2];
        vec3 relative_position[2];

        RigidShape shape[2];
        bool immovable[2];

        float static_friction;
        float friction;
        float inv_mass_sum;
        float restitution;
        float v_dot_n;
        float j;

        uint32_t contact_count;
    };

    class ContactImpulseSolver : public Simulator
    {
    public:

        void resolveImpulse( double time_step, CollisionRegistry& collision_manager );

        void setCollisionState( CollisionManifold* collision, CollisionState* state );

        void computeRelativeVelocity( const Contact& contact, CollisionState* state );
        vec3 getFrictionImpulse( Rigidbody* a, Rigidbody* b, CollisionState* state );
        vec3 getNormalImpulse( Rigidbody* a, Rigidbody* b, CollisionState* state );

        void simulate( double dms )override;

        ContactImpulseSolver();

    public:

        uint32_t _update_iterations;
    };
}
#endif /* kege_contact_impulse_solver_hpp */
