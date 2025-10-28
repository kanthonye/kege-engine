//
//  control-camera-movement.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#ifndef control_camera_movement_hpp
#define control_camera_movement_hpp

#include "../../ecs/entity-system.hpp"
#include "movement-controls.hpp"

namespace kege{

    class MovementSystem : public kege::EntitySystem
    {
    public:

        MovementSystem( kege::EntitySystemManager* esm );
        void input( double dms );
        bool initialize();
        void shutdown();
    };

}
#endif /* control_camera_movement_hpp */
