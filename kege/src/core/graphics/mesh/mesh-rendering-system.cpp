//
//  mesh-rendering-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/18/25.
//

#include "mesh-rendering-system.hpp"

namespace kege{

    void MeshRenderingSystem::render( double dms )
    {
        for (kege::Entity entity : *_entities )
        {
            kege::Geometry* geometry = entity.get< kege::Geometry >();
            kege::Transform* transform = entity.get< kege::Transform >();

            for ( const Ref< MeshSource >& source : geometry->mesh->sources )
            {
                if ( 0 <= source->material_index )
                {
                    _engine->renderManager()->submit(RenderObject{
                        .material = geometry->material->sources[ source->material_index ],
                        .mesh = source,
                        .transform = *transform
                    });
                }
            }
        }
    }

    bool MeshRenderingSystem::initialize()
    {
        return EntitySystem::initialize();
    }

    void MeshRenderingSystem::shutdown()
    {
        EntitySystem::shutdown();
    }

    MeshRenderingSystem::MeshRenderingSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "mesh-rendering-system", 0 )
    {
        _signature = createEntitySignature< kege::Geometry, kege::Transform >();
    }


    KEGE_REGISTER_ENTITY_SYSTEM( MeshRenderingSystem, "mesh-rendering" );
}
