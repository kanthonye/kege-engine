//
//  cylinder-emitter.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#ifndef cylinder_spawn_shape_hpp
#define cylinder_spawn_shape_hpp

#include "emitter.hpp"

namespace kege{

    class EmitterCylinderSurface : public Emitter
    {
    public:

        EmitterCylinderSurface
        (
            float emissions_per_second, bool burst,
            const kege::vec3 axes[2],
            float height,
            float radius
        );

        kege::point3 emit();
        kege::vec3 axes[2];
        float height;
        float radius;
    };
}

namespace kege{

    class EmitterCylinder : public Emitter
    {
    public:
        
        EmitterCylinder
        (
            float emissions_per_second, bool burst,
            const kege::vec3 axes[2],
            float height,
            float min_radius,
            float max_radius
        );

        kege::point3 emit();
        kege::vec3 axes[2];
        float radius[2];
        float height;
    };
}

#endif /* cylinder_spawn_shape_hpp */
