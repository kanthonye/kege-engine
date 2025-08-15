//
//  air-resistance.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#include "air-resistance.hpp"

namespace kege{

    void AirResistance::update( double dms, Particle& particle )
    {
        if ( particle.invmass <= 0.0f ) return; // Skip immovable particles

        // Calculate drag force magnitude
        float speed = kege::magn( particle.velocity );
        if (speed <= 0.0001f) return; // Avoid division by zero

        kege::vec3 drag_force;
        if ( this->use_quadratic_drag )
        {
            // Quadratic drag (realistic): F = -0.5 * ρ * v² * C_d * A * v̂
            float area = particle.size *particle.size;
            float forceMagnitude = 0.5f * this->fluid_density * speed * speed * this->coefficient * area;
            drag_force = -kege::normalize(particle.velocity) * forceMagnitude;
        }
        else
        {
            // Linear drag (game-friendly): F = -k * v
            drag_force = -particle.velocity * this->coefficient;
        }

        // Apply force to velocity (F = ma → a = F * inverseMass)
        particle.velocity += drag_force * static_cast<float>( dms ) * particle.invmass;
    }

    AirResistance::AirResistance
    (
        float coefficient,
        float fluid_density,
        bool use_quadratic_drag
    )
    :   coefficient( coefficient )
    ,   fluid_density( fluid_density )
    ,   use_quadratic_drag( use_quadratic_drag )
    {}

//    class AirResistance : public ParticleBehavior
//    {
//    public:
//        void update( uint32_t particle_count, std::vector< Particle >& particles );
//        AirResistance(float air_density);
//        float air_density;
//    };

//    void AirResistance::update( uint32_t particle_count, std::vector< Particle >& particles )
//    {
//        for (int i=0; i<particle_count; ++i)
//        {
//            float circle = particles[i].size * particles[i].size;
//            kege::vec3 drag = circle * particles[i].velocity * air_density;
//            particles[i].velocity -= drag * particles[i].invmass;
//        }
//    }
//    AirResistance::AirResistance(float air_density)
//    :   air_density(air_density)
//    {}


}
