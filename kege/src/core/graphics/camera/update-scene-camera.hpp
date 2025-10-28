//
//  camera-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/17/25.
//

#ifndef update_scene_camera_hpp
#define update_scene_camera_hpp

#include "../../ecs/entity-system.hpp"
#include "../render/graph/render-graph.hpp"
#include "camera.hpp"

namespace kege{

    class UpdateSceneCamera : public kege::EntitySystem
    {
    public:

        UpdateSceneCamera( kege::EntitySystemManager* esm );

        void update( double dms )override;
        bool initialize()override;
        void shutdown()override;
    };

}

#endif /* camera_system_hpp */
