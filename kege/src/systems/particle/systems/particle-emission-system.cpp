//
//  particle-emission-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/24/25.
//

#include "particle-emission-system.hpp"
namespace kege{

    void ParticleEmissionSystem::update( double dms )
    {
        for ( kege::Entity entity : *_entities )
        {
            ParticleBuffer* buffer = entity.get< ParticleBuffer >();
            ParticleEmitter* emitter = entity.get< ParticleEmitter >();

            uint32_t count = emitter->emitter->update( dms );

            for (int i=0; i < count && buffer->particles.size() <= buffer->particle_count; ++i)
            {
                Particle& particle = buffer->particles[ buffer->particle_count++ ];
                particle.position = ( emitter->emitter ) ? emitter->emitter->emit() : vec3(0.0);
                particle.velocity   = {0.f, 0.f, 0.f};
                particle.max_health = 0.f;
                particle.health     = 0.f;
                particle.invmass    = 1.f;
                particle.rotation   = 0.f;
                particle.size       = 1.f;
            }
        }
    }

    bool ParticleEmissionSystem::initialize()
    {
        return EntitySystem::initialize();
    }
    
    void ParticleEmissionSystem::shutdown()
    {
        return EntitySystem::shutdown();
    }

    ParticleEmissionSystem::ParticleEmissionSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "particle-emitter-updater", REQUIRE_UPDATE )
    {
        _signature = createEntitySignature< ParticleEffect, ParticleBuffer, Transform >();
    }

    KEGE_REGISTER_SYSTEM( ParticleEmissionSystem, "particle-emitter-updater" );
}

