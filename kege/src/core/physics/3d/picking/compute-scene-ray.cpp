//
//  compute-scene-ray.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/30/24.
//

#include "../../../../core/graphics/camera/camera.hpp"
#include "compute-scene-ray.hpp"

namespace kege{

    void ComputeCameraRaySystem::operator()( const MappedInputs& inputs )
    {
        Entity entity = getScene()->getPlayer();
        if( !entity ) return;

        Transform* transform = entity.get< Transform >();
        Camera* camera = entity.get< Camera >();

        float window_height = getGraphics()->getWindow()->getHeight();
        float window_width = getGraphics()->getWindow()->getWidth();

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

        getScene()->setSceneRay( kege::normalize( ray_world.xyz ) );
    }

    bool ComputeCameraRaySystem::initialize()
    {
        Communication::add< const MappedInputs&, ComputeCameraRaySystem >( this );
        return EntitySystem::initialize();
    }

    void ComputeCameraRaySystem::shutdown()
    {
        Communication::remove< const MappedInputs&, ComputeCameraRaySystem >( this );
        EntitySystem::shutdown();
    }

    ComputeCameraRaySystem::ComputeCameraRaySystem( kege::EntitySystemManager* esm )
    :   kege::EntitySystem( "compute-camera-ray", REQUIRE_INPUT, esm  )
    {}

    KEGE_REGISTER_ENTITY_SYSTEM( ComputeCameraRaySystem, "compute-camera-ray" );
}
