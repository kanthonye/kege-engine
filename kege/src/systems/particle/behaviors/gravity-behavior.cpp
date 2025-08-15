//
//  gravity-behavior.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#include "gravity-behavior.hpp"

namespace kege{

    void DirectionalGravity::update( double dms, Particle& particle )
    {
        particle.velocity -= force * particle.invmass;;
    }
    DirectionalGravity::DirectionalGravity(const kege::vec3& force)
    :   force( force )
    {
    }



    void CenterOfMassGravity::update( double dms, Particle& particle )
    {
        // Direction from particle to center of mass
        kege::vec3 dir = -particle.position;
        float distance = kege::magn(dir);

        // Normalize direction (skip if distance is zero)
        if (distance > 0.0f) dir /= distance;

        // Calculate force magnitude
        float force_magnitude = strength;

        if (falloff)
        {
            // Inverse-square law: F ~ 1/r² (realistic gravity)
            force_magnitude /= (distance * distance + 0.001f); // +epsilon to avoid div/0
        }

        // Adjust for particle mass (F = ma, but particle stores 1/mass for stability)
        if (particle.invmass > 0.0f)
        {
            force_magnitude /= particle.invmass; // Equivalent to F *= m
        }

        // Apply force to particle velocity (Euler integration)
        particle.velocity += dir * force_magnitude * static_cast<float>(dms);
    }

    CenterOfMassGravity::CenterOfMassGravity
    (
        float strength,
        bool  falloff
    )
    :   strength( strength )
    ,   falloff( falloff )
    {}

}
