//
//  particle-effect-update-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/24/25.
//

#include "particle-effect-system.hpp"

namespace kege{

    void ParticleEffectSystem::update( double dms )
    {
        for ( kege::Entity entity : *_entities )
        {
            ParticleEffect* effect = entity.get< ParticleEffect >();
            ParticleBuffer* buffer = entity.get< ParticleBuffer >();

            for (int i=0; i < buffer->particle_count; ++i)
            {
                Particle& particle = buffer->particles[ i ];

                if( particle.max_health == 0.0f )
                {
                    if ( effect->initails )
                    {
                        particle.color      = effect->initails->color.gen() * effect->initails->saturation.gen();
                        particle.velocity   = effect->initails->velocity.gen() * effect->initails->speed.gen();
                        particle.rotation   = effect->initails->mass.gen();
                        particle.size       = effect->initails->size.gen();
                        particle.invmass    = 1.0 / effect->initails->mass.gen();
                        particle.max_health = effect->initails->lifetime.gen();
                        particle.health     = particle.max_health;
                    }
                    else
                    {
                        particle.velocity   = {0.f,0.f,0.f};
                        particle.max_health     = 1.f;
                        particle.health     = 1.f;
                        particle.invmass    = 1.f;
                        particle.rotation   = 0.f;
                        particle.size       = 1.f;
                    }
                }

                if ( particle.health <= 0 )
                {
                    // delete expired particles
                    Particle p = buffer->particles[i];
                    buffer->particles[i] = buffer->particles[ buffer->particle_count - 1];
                    buffer->particles[ buffer->particle_count - 1] = p;
                    buffer->particle_count--;
                    i -= 1;
                    continue;
                }

                for (int j=0; j < effect->behaviors.size(); ++j)
                {
                    effect->behaviors[i]->update( dms, particle );
                }

                particle.health -= dms * effect->rate_of_deterioration;
                particle.health = kege::max( 0.f, particle.health );
                particle.position += particle.velocity * dms;
            }
        }
    }

    bool ParticleEffectSystem::initialize()
    {
        _signature = createEntitySignature< ParticleEffect, Transform >();
        return EntitySystem::initialize();
    }
    
    void ParticleEffectSystem::shutdown()
    {
        return EntitySystem::shutdown();
    }

    ParticleEffectSystem::ParticleEffectSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "particle-effect-updater", REQUIRE_UPDATE )
    {}

    KEGE_REGISTER_SYSTEM( ParticleEffectSystem, "particle-effect-updater" );
}


