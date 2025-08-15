//
//  control-camera-movement.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#ifndef control_camera_movement_hpp
#define control_camera_movement_hpp

#include "rigidbody.hpp"
#include "input-system.hpp"
#include "entity-system.hpp"

namespace kege{

    class MovementSystem : public kege::EntitySystem
    {
    public:

        void operator()( const MappedInputs& input_commands );
        MovementSystem( kege::Engine* engine );
        bool initialize()override;
        void shutdown()override;
    };

    struct MovementControl
    {
        kege::vec3 speed =  {0.1, 0.1, 0.1};
    };
}
#endif /* control_camera_movement_hpp */
