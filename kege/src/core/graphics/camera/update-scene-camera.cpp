//
//  camera-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/17/25.
//

#include "update-scene-camera.hpp"

namespace kege{

    void UpdateSceneCamera::update( double dms )
    {
        if ( !_entities ) return;
        
        for( Entity entity : *_entities )
        {
            Camera* camera = entity.get< Camera >();
            Transform* transform = entity.get< Transform >();

            if ( camera->modified )
            {
                camera->matrices.projection = camera->projection->get();
                camera->modified = false;
            }
            camera->matrices.position = transform->position;
            camera->matrices.transform = viewMatrix( transform->orientation, transform->position );

            _engine->renderManager()->setSceneCamera( camera->matrices );
        }
    }

    bool UpdateSceneCamera::initialize()
    {
        return EntitySystem::initialize();
    }

    void UpdateSceneCamera::shutdown()
    {
        Communication::remove< const MappedInputs&, UpdateSceneCamera >( this );
        return EntitySystem::shutdown();
    }

    UpdateSceneCamera::UpdateSceneCamera( kege::Engine* engine )
    :   kege::EntitySystem( engine, "update-scene-camera", REQUIRE_UPDATE )
    {
        _signature = createEntitySignature< Camera, kege::Transform >();
    }

    KEGE_REGISTER_ENTITY_SYSTEM( UpdateSceneCamera, "update-scene-camera" );
}
