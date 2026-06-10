//
//  control-camera-movement.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#ifndef control_camera_movement_hpp
#define control_camera_movement_hpp

#include "../../../input/input-context-manager.hpp"
#include "../../../scene/ecs.hpp"
#include "movement-controls.hpp"

namespace kege{

    class MovementSystem : public kege::ecs::System
    {
    public:

        //void input( const MappedInputs& inputs, double dms );
        MovementSystem( kege::ECS* ecs );
        void input( double dms );
    };

}
#endif /* control_camera_movement_hpp */
