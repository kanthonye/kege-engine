//
//  control-camera-movement.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#ifndef control_camera_movement_hpp
#define control_camera_movement_hpp

#include "../-/system-dependencies.hpp"

namespace kege{

    class MovementSystem : public kege::EntitySystem
    {
    public:

        void operator()( const MappedInputs& input_commands );
        MovementSystem( kege::Engine* engine );
        bool initialize();
        void shutdown();
    };

}
#endif /* control_camera_movement_hpp */
