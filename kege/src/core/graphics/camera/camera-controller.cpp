//
//  camera-controller.cpp
//  physics
//
//  Created by Kenneth Esdaile on 7/3/25.
//

#include "camera-controller.hpp"

namespace kege{

    void CameraControlSystem::input( double dms )
    {
        Entity entity = _engine->scene().getScene()->getCameraEntity();
        if( !entity ) return;


        CameraControls* controls = entity.get< CameraControls >();
        if( !controls ) return;

        const MappedInputs& inputs = _engine->input()->getMappedInputs();
        if ( inputs[ kege::ACTION_CONTROL_CAMERA ] )
        {
            if ( inputs[ kege::ACTION_LOOK_LEFT ] )
            {
                controls->angles.y += controls->sensitivity * inputs[ kege::ACTION_LOOK_LEFT ];
            }
            else if ( inputs[ kege::ACTION_LOOK_RIGHT ] )
            {
                controls->angles.y += controls->sensitivity * inputs[ kege::ACTION_LOOK_RIGHT ];
            }

            if ( inputs[ kege::ACTION_LOOK_UP ] )
            {
                controls->angles.x += controls->sensitivity * inputs[ kege::ACTION_LOOK_UP ];
            }
            else if ( inputs[ kege::ACTION_LOOK_DOWN ] )
            {
                controls->angles.x += controls->sensitivity * inputs[ kege::ACTION_LOOK_DOWN ];
            }
        }
    }

    void CameraControlSystem::update( double dms )
    {
        if ( !_entities ) return;

        for( Entity entity : *_entities )
        {
            CameraControls* controls = entity.get< CameraControls >();

            vec3 up;
            quat* orientation;

            if ( entity.has< Rigidbody >() )
            {
                orientation = &entity.get< Rigidbody >()->orientation;
                up = entity.get< Rigidbody >()->up;
            }
            else
            {
                orientation = &entity.get< Transform >()->orientation;
                up = vec3( 0.f ,1.f , 0.f );
            }

            controls->euler.x = lerp( controls->euler.x, controls->angles.x, dms * controls->smoothness );
            controls->euler.y = lerp( controls->euler.y, controls->angles.y, dms * controls->smoothness );

            quat rotation = quat( controls->euler.y, up );
            *orientation = quat( controls->euler.x, getAxesX( rotation ) ) * rotation;
        }
    }

    bool CameraControlSystem::initialize()
    {
        return EntitySystem::initialize();
    }

    void CameraControlSystem::shutdown()
    {
        return EntitySystem::shutdown();
    }

    CameraControlSystem::CameraControlSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "camera-controller", REQUIRE_UPDATE | REQUIRE_INPUT )
    {
        _signature = createEntitySignature< kege::Camera, kege::CameraControls >();
    }

    KEGE_REGISTER_ENTITY_SYSTEM( CameraControlSystem, "camera-controller" );
}
