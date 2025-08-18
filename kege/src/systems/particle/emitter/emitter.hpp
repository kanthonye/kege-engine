//
//  emitter.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#ifndef spawn_shape_hpp
#define spawn_shape_hpp

#include "../../../core/memory/ref.hpp"
#include "../../../core/math/algebra/vmath.hpp"

namespace kege{

    /**
     * @brief Base class for particle emitters.
     *
     * An emitter is responsible for producing particles based on time. This is
     * an abstract base class intended to be subclassed with concrete emit logic.
     * Inherits from `RefCounter` to support reference counting.
     */
    struct Emitter : public RefCounter
    {
    public:

        /**
         * @brief Emits a new particle position based on the given transform.
         *
         * This is a pure virtual method that must be implemented by concrete emitter types.
         *
         * @return A position vector where the particle should be emitted in world space.
         */
        virtual vec3 emit() = 0;

        /**
         * @brief Updates the internal emission timer and computes how many particles to emit.
         *
         * Call this function every simulation step. It uses a time accumulator to determine
         * how many particles should be emitted based on `_emission_count`.
         *
         * @param dms The delta time in milliseconds since the last update.
         * @return The number of particles that should be emitted this frame.
         */
        uint32_t update(double dms);

        /**
         * @brief Constructs an Emitter instance.
         *
         * @param emissions_per_second The number of particles emitted per second.
         * @param burst Whether the emitter operates in burst mode (all at once).
         */
        Emitter(float emissions_per_second, bool burst);

        /**
         * @brief Virtual destructor.
         */
        virtual ~Emitter() {}

    protected:

        /**
         * @brief Number of particles to emit per second.
         *
         * Used to compute how many particles to emit each frame.
         */
        float _emission_count;

        /**
         * @brief Accumulates elapsed time between updates.
         *
         * Tracks how much time has passed and determines when emissions should occur.
         */
        float _time_accumulator;

        /**
         * @brief Whether this emitter emits in burst mode.
         *
         * If true, all particles are emitted in one large burst (e.g., explosion).
         * If false, particles are emitted gradually over time.
         */
        bool _burst;
    };

}
#endif /* spawn_shape_hpp */
