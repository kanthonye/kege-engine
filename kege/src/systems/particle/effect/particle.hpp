//
//  particle.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/29/24.
//

#ifndef particle_hpp
#define particle_hpp

#include "../../../core/container/array.hpp"
#include "../../../core/graphics/core/graphics.hpp"
#include "../../../core/math/algebra/vmath.hpp"

namespace kege{

    /**
     * @brief Represents a particle in a particle system.
     *
     * This struct defines the properties of a particle, including its position,
     * velocity, color, size, rotation, and health. It is used in particle systems
     * to manage individual particles.
     */
    struct Particle
    {
        vec3  position;
        vec3  velocity;
        vec4  color;
        vec4  sprite;
        float size;
        float rotation;
        float invmass;
        float max_health;
        float health;
    };

    /**
     * @brief Represents initial properties for a particle.
     *
     * This struct defines the initial random properties of a particle, such as
     * saturation, velocity, color, rotation, speed, mass, size, and lifetime.
     * It is used to initialize particles in a particle system.
     */
    struct ParticleInitails : public RefCounter
    {
        struct Data
        {
            rand1f saturation;
            rand3f velocity;
            rand4f color;
            rand1f rotation;
            rand1f speed;
            rand1f mass;
            rand1f size;
            rand1f lifetime;
        };

        ParticleInitails( const ParticleInitails::Data& data = {} )
        :   saturation( data.saturation )
        ,   velocity( data.velocity )
        ,   color( data.color )
        ,   rotation( data.rotation )
        ,   speed( data.speed )
        ,   mass( data.mass )
        ,   size( data.size )
        ,   lifetime( data.lifetime )
        {}

        ParticleInitails(){}

        rand1f saturation;
        rand3f velocity;
        rand4f color;
        rand1f rotation;
        rand1f speed;
        rand1f mass;
        rand1f size;
        rand1f lifetime;
    };

}
#endif /* particle_hpp */
