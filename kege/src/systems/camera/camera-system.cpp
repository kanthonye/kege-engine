//
//  camera-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/17/25.
//

#include "camera-system.hpp"

namespace kege{

    void CameraSystem::update( double dms )
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

            if ( !_camera_buffer_resource )
            {
                _camera_buffer_resource = _engine->renderGraph()->getBufferRgResrc( "camera-buffer" );
                if ( !_camera_buffer_resource ) return;
            }

            kege::BufferHandle camera_buffer = _engine->renderGraph()->getPhysicalBuffer( _camera_buffer_resource );
            if ( !camera_buffer ) return;
            
            _engine->graphics()->updateBuffer( camera_buffer, 0, sizeof( CameraData ), &camera->matrices );
        }
    }

    bool CameraSystem::initialize()
    {
        return EntitySystem::initialize();
    }

    void CameraSystem::shutdown()
    {
        _comm.remove< const MappedInputs&, CameraSystem >( this );
        return EntitySystem::shutdown();
    }

    CameraSystem::CameraSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "camera-system", REQUIRE_UPDATE )
    {
        _signature = createEntitySignature< Camera, kege::Transform >();
    }

    KEGE_REGISTER_SYSTEM( CameraSystem, "camera" );
}
