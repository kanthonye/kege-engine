//
//  sphere-emitter.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#ifndef sphere_spawn_shape_hpp
#define sphere_spawn_shape_hpp

#include "emitter.hpp"

namespace kege{

    class EmitterSphereArea : public Emitter
    {
    public:
        
        EmitterSphereArea
        (
            float emissions_per_second, bool burst,
            float min_radius,
            float max_radius
        );

        kege::point3 emit();
        float min_radius;
        float max_radius;
    };
}

namespace kege{

    class EmitterSphere : public Emitter
    {
    public:
        EmitterSphere( float emissions_per_second, bool burst, float radius );
        kege::point3 emit();
        float radius;
    };
}

#endif /* sphere_spawn_shape_hpp */
