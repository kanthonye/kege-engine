//
//  camera-controller.hpp
//  physics
//
//  Created by Kenneth Esdaile on 7/3/25.
//

#ifndef kege_camera_controller_hpp
#define kege_camera_controller_hpp

#include "../-/system-dependencies.hpp"

namespace kege{

    class CameraControlSystem : public kege::EntitySystem
    {
    public:

        CameraControlSystem( kege::Engine* engine );

        void operator()( const MappedInputs& inputs );
        void update( double dms )override;
        bool initialize()override;
        void shutdown()override;
    };

}

#endif /* kege_camera_controller_hpp */
