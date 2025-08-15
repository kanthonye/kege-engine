//
//  triangle-emitter.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#ifndef triangle_spawn_shape_hpp
#define triangle_spawn_shape_hpp

#include "emitter.hpp"

namespace kege{

    class EmitterTriangle : public Emitter
    {
    public:
        
        EmitterTriangle
        (
            float emissions_per_second, bool burst, 
            const kege::point3& a,
            const kege::point3& b,
            const kege::point3& c,
            float height,
            float base
        );
        kege::point3 emit();

        kege::point3 position;
        kege::vec3 axes[ 2 ];
        kege::point2 extents;
        float height;
        float base;
    };
}

#endif /* triangle_spawn_shape_hpp */
