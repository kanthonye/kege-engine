//
//  rigidbody.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/20/25.
//

#ifndef kege_physics_3d_motion_hpp
#define kege_physics_3d_motion_hpp

#include "ref.hpp"
#include "vectors.hpp"
#include "matrices.hpp"
#include "quaternion.hpp"
#include "collider.hpp"

namespace kege{

    struct Collider;

    struct LinearMotion
    {
        kege::fvec3 acceleration;
        kege::fvec3 velocity;
        kege::fvec3 forces;
        double invmass = 0.06125;
        float damping = 0.85f;
    };

    struct AngularMotion
    {
        kege::mat33 inertia_inverse;
        kege::fvec3 velocity;
        kege::fvec3 rotation;
        kege::fvec3 torques;
        float damping = 0.85f;
    };

    struct Rigidbody
    {
        Ref< Collider > collider;
        uint32_t        collider_id;

        AngularMotion   angular;    // angular body characteristics
        LinearMotion    linear;     // linear body characteristics

        kege::quat      orientation; // objects orientation
        kege::vec3      center;      // center of mass
        kege::vec3      prev;        // previous center of mass

        vec3            up;         // a uint vector, the up direction of the object

        float           friction;   // the surface friction of the object
        float           cor;        // coefficient of restitution

        bool            immovable;  // indicate whether the object is immovable

        // members for sleep system

        float           frames_since_move;
        float           body;     // Tracks current body (linear and angular combined)
        bool            grounded;
        bool            is_awake;   // Indicates if the body is awake
        bool            sleepable;  // If the body is allowed to sleep
        bool            anti_gravity; // If the body is not affected by gravity
    };



//    struct Movement
//    {
//        vec3 speed;
//    };

    void applyAngularImpulse( Rigidbody* body, const vec3& relative_position, const vec3& impulse );
    void applyLinearImpulse( Rigidbody* body, const vec3& impulse );
    void setAwake( Rigidbody* body, bool awake );



    std::ostream& operator<<(std::ostream& out, kege::LinearMotion& body );
    std::ostream& operator<<(std::ostream& out, kege::AngularMotion& body );
    std::ostream& operator<<(std::ostream& out, kege::Rigidbody& body );
}
#endif /* kege_physics_3d_motion_hpp */
