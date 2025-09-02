//
//  body.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/20/25.
//

#include "rigidbody.hpp"

namespace kege{


    void applyAngularImpulse( Rigidbody* body, const vec3& relative_position, const vec3& impulse )
    {
        body->angular.velocity += body->angular.inertia_inverse * cross( relative_position, impulse );
    }

    void applyLinearImpulse( Rigidbody* body, const vec3& impulse )
    {
        body->linear.velocity += body->linear.invmass * impulse;
    }
    
    void setAwake( Rigidbody* body, bool awake )
    {
        if (awake)
        {
            body->is_awake = true;
            // Reset the body when waking up
            body->body = 2.0f;// * SleepMotionThreshold;
        }
        else
        {
            body->is_awake = false;
            body->linear.velocity = {0.f,0.f,0.f};
            body->angular.velocity = {0.f,0.f,0.f};
        }
    }
    
    std::ostream& operator<<(std::ostream& out, kege::LinearMotion& body )
    {
        out <<"{\n";
        out <<"    velocity: " << body.velocity <<"\n";
        out <<"    forces:   " << body.forces <<"\n";
        out <<"    invmass:  " << body.invmass <<"\n";
        out <<"    damping:  " << body.damping <<"\n";
        out <<"}\n";
        return out;
    }
    std::ostream& operator<<(std::ostream& out, kege::AngularMotion& body )
    {
        out <<"{\n";
        out <<"    velocity: " << body.velocity <<"\n";
        out <<"    torques:  " << body.torques <<"\n";
        out <<"    inertia:  " << body.inertia_inverse <<"\n";
        out <<"    damping:  " << body.damping <<"\n";
        out <<"}\n";
        return out;
    }
    std::ostream& operator<<(std::ostream& out, kege::Rigidbody& body )
    {
        out <<"{\n";
        out <<"    linear:      " << body.linear <<"\n";
        out <<"    angular:     " << body.angular <<"\n";
        out <<"    center:      " << body.center <<"\n";
        out <<"    orientation: " << body.orientation <<"\n";
        out <<"}\n";
        return out;
    }
}
