//
//  plane-emitter.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#ifndef plane_spawn_shape_hpp
#define plane_spawn_shape_hpp

#include "emitter.hpp"

namespace kege{

    class EmitterPlane : public Emitter
    {
    public:
        EmitterPlane
        (
            float emissions_per_second, bool burst, 
            const kege::point3& a,
            const kege::point3& b,
            const kege::point3& c,
            const kege::point2& extents
        );
        kege::point3 emit();
        kege::point3 position;
        kege::vec3 axes[ 2 ];
        kege::point2 extents;
    };
}

#endif /* plane_spawn_shape_hpp */
