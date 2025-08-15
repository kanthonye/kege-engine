//
//  cone-emitter.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#ifndef cone_spawn_shape_hpp
#define cone_spawn_shape_hpp

#include "emitter.hpp"

namespace kege{

    class EmitterCone : public Emitter
    {
    public:

        EmitterCone
        (
            float emissions_per_second, bool burst,
            kege::vec3 axes[2],
            float height, float
            min_radius,
            float max_radius
        );
        
        kege::point3 emit();
        kege::vec3 axes[2];
        float radius[2];
        float height;
    };
}

#endif /* cone_spawn_shape_hpp */
