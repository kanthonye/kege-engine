//
//  render-particle-system.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/29/24.
//

#ifndef render_particle_system_hpp
#define render_particle_system_hpp

#include "../effect/particle-effect.hpp"
#include "../../-/system-dependencies.hpp"

namespace kege{

    struct BillboardParticleData
    {
        vec3  position;
        vec4  color;
        vec4  sprite;
        float size;
        float rotation;
    };

    enum class BillboardType
    {
        Spherical,
        Cylindrical
    };

    class BillboardSprite
    {
        kege::ImageHandle texture;
    };

    class BillboardParticleRenderer : public kege::EntitySystem
    {
    public:

        void operator()( kege::RenderPassContext* context );
        BillboardParticleRenderer( kege::Engine* engine );
        bool initialize();
        void shutdown();


        kege::PipelineHandle _pipeline;
        kege::BufferHandle _storage_buffer;
    };

}
#endif /* render_particle_system_hpp */
