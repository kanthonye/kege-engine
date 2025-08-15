//
//  camera-controller.hpp
//  physics
//
//  Created by Kenneth Esdaile on 7/3/25.
//

#ifndef kege_camera_controller_hpp
#define kege_camera_controller_hpp

#include "transform.hpp"
#include "../../core/system/entity-system.hpp"
#include "../../core/input/input-commands.hpp"
#include "../../core/graphics/core/graphics.hpp"

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
