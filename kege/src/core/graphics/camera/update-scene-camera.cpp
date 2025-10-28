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

        Entity camera_entity = getScene()->getCameraEntity();
        for( Entity entity : *_entities )
        {
            Camera* camera = entity.get< Camera >();
            Transform* transform = entity.get< Transform >();


            if( !camera_entity )
            {
                getScene()->setCameraEntity( entity );
                getScene()->setPlayer( entity );
                camera_entity = entity;
            }

            if ( camera->modified )
            {
                camera->matrices.projection = camera->projection->get();
                camera->modified = false;
            }
            camera->matrices.position = transform->position;
            camera->matrices.transform = viewMatrix( transform->orientation, transform->position );

            getRenderExecutor()->setSceneCamera( camera->matrices );
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

    UpdateSceneCamera::UpdateSceneCamera( kege::EntitySystemManager* esm )
    :   kege::EntitySystem( "update-scene-camera", REQUIRE_UPDATE, esm  )
    {
        _signature = createEntitySignature< Camera, kege::Transform >();
    }

    KEGE_REGISTER_ENTITY_SYSTEM( UpdateSceneCamera, "update-scene-camera" );
}
