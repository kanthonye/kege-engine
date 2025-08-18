//
//  particle-effect.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/29/24.
//

#ifndef particle_effect_hpp
#define particle_effect_hpp

#include "../emitter/emitter.hpp"
#include "../behaviors/behavior.hpp"

namespace kege{

    /**
     * @brief Represents a particle emitter component.
     *
     * The ParticleEmitter structure holds a reference to an emitter object
     * which is responsible for spawning particles into a particle system.
     */
    struct ParticleEmitter
    {
        Ref<Emitter> emitter; ///< Reference to the emitter implementation that handles particle spawning
    };

    /**
     * @brief Defines the behavior and appearance of particles.
     *
     * The ParticleEffect structure contains all components that determine:
     * - How particles behave over time (through behaviors)
     * - Initial particle properties (through initails)
     * - The overall effect lifetime/duration
     */
    struct ParticleEffect
    {
        array<Ref<Behavior>> behaviors; ///< List of behaviors that modify particles over time
        Ref<ParticleInitails> initails; ///< Initial properties for spawned particles
        float rate_of_deterioration = 1; ///< Controls how quickly the effect decays (1.0 = normal rate)
    };

    /**
     * @brief Storage buffer for particle data.
     *
     * Contains the actual particle data and maintains the current count
     * of active particles in the system.
     */
    struct ParticleBuffer
    {
        array<kege::Particle> particles; ///< Contiguous storage for particle data
        uint32_t particle_count = 0; ///< Number of currently active particles (<= particles.size())
    };
}
#endif /* particle_effect_hpp */
