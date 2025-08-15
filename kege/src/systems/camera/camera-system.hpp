//
//  camera-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/17/25.
//

#ifndef camera_system_hpp
#define camera_system_hpp

#include "transform.hpp"
#include "../../core/system/entity-system.hpp"
#include "../../core/input/input-commands.hpp"
#include "../../core/graphics/core/graphics.hpp"

namespace kege{

    class CameraSystem : public kege::EntitySystem
    {
    public:

        CameraSystem( kege::Engine* engine );

        void update( double dms )override;
        bool initialize()override;
        void shutdown()override;


        RgResrcHandle _camera_buffer_resource;
    };

}

#endif /* camera_system_hpp */
