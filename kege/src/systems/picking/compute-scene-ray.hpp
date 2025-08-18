//
//  compute-scene-ray.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/30/24.
//

#ifndef compute_camera_ray_hpp
#define compute_camera_ray_hpp

#include "../-/system-dependencies.hpp"

namespace kege{

    class ComputeCameraRaySystem : public kege::EntitySystem
    {
    public:

        void operator()( const MappedInputs& inputs );
        ComputeCameraRaySystem( Engine* engine );
        bool initialize();
        void shutdown();
        vec2 pointer;
    };

}
#endif /* compute_camera_ray_hpp */
