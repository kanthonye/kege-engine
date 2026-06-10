//
//  render-particle-system.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/29/24.
//

#ifndef render_particle_system_hpp
#define render_particle_system_hpp

#include "particle-effect.hpp"
#include "../../../../scene/ecs.hpp"

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
        ref::Image texture;
    };

    class BillboardParticleRenderer : public kege::ecs::System
    {
    public:

        //void operator()( kege::RenderPassContext* context );
        BillboardParticleRenderer( kege::ECS* ecs );
        bool initialize();
        void shutdown();


        //kege::PipelineHandle _pipeline;
        ref::Buffer _storage_buffer;
    };

}
#endif /* render_particle_system_hpp */
