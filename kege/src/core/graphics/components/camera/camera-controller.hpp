//
//  camera-controller.hpp
//  physics
//
//  Created by Kenneth Esdaile on 7/3/25.
//

#ifndef kege_camera_controller_hpp
#define kege_camera_controller_hpp

#include "camera.hpp"
#include "../../../scene/ecs-system-factory.hpp"

namespace kege{

    class CameraControlSystem : public kege::ecs::System
    {
    public:

        CameraControlSystem( kege::ECS* ecs );
        void update( double dms )override;
        void input( double dms )override;
    };

}

#endif /* kege_camera_controller_hpp */
