//
//  compute-scene-ray.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/30/24.
//

#include "camera.hpp"
#include "transform.hpp"
#include "compute-scene-ray.hpp"
#include "graphics.hpp"

namespace kege{

    void ComputeCameraRaySystem::operator()( const MappedInputs& inputs )
    {
        Entity entity = _engine->scene()->getPlayer();
        if( !entity ) return;

        Transform* transform = entity.get< Transform >();
        Camera* camera = entity.get< Camera >();

        float window_height = _engine->graphics()->windowHeight();
        float window_width = _engine->graphics()->windowWidth();

        pointer.x = inputs.coordX();
        pointer.y = inputs.coordY();

        vec4 ray_eye, normalised_device_coord;
        normalised_device_coord.x = (2.0f * pointer.x) / window_width - 1.0f;
        normalised_device_coord.y = 1.0f - (2.0f * pointer.y) / window_height;
        normalised_device_coord.z = -1.0f;
        normalised_device_coord.z = 0.f;

        ray_eye = kege::inverse( camera->matrices.projection ) * normalised_device_coord;
        ray_eye.z = -1.f;
        ray_eye.w = 0.f;

        mat44 viewmatrix = viewMatrix( transform->orientation, transform->position );
        vec4 ray_world = kege::inverse( viewmatrix ) * ray_eye;

        _engine->scene()->setSceneRay( kege::normalize( ray_world.xyz ) );
    }

    bool ComputeCameraRaySystem::initialize()
    {
        _comm.add< const MappedInputs&, ComputeCameraRaySystem >( this );
        return EntitySystem::initialize();
    }

    void ComputeCameraRaySystem::shutdown()
    {
        _comm.remove< const MappedInputs&, ComputeCameraRaySystem >( this );
        EntitySystem::shutdown();
    }

    ComputeCameraRaySystem::ComputeCameraRaySystem( Engine* engine )
    :   kege::EntitySystem( engine, "compute-camera-ray", REQUIRE_INPUT )
    {}

    KEGE_REGISTER_SYSTEM( ComputeCameraRaySystem, "compute-camera-ray" );
}
