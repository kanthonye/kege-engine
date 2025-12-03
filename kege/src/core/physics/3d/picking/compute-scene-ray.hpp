//
//  compute-scene-ray.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/30/24.
//

#ifndef compute_camera_ray_hpp
#define compute_camera_ray_hpp

#include "../../../math/algebra/vmath.hpp"
#include "../../../scene/ecs.hpp"
#include "../../../input/input-context-manager.hpp"

namespace kege{

    class ComputeCameraRaySystem : public kege::ecs::System
    {
    public:

        void operator()( const MappedInputs& inputs );
        ComputeCameraRaySystem( kege::ECS* ecs );
        bool initialize();
        void shutdown();
        vec2 pointer;
    };

}
#endif /* compute_camera_ray_hpp */
