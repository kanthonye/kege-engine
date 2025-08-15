//
//  cubic-emitter.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#ifndef cubic_spawn_shape_hpp
#define cubic_spawn_shape_hpp

#include "emitter.hpp"

namespace kege{

    class EmitterCube : public Emitter
    {
    public:
        EmitterCube
        (
            float emissions_per_second, bool burst,
            float width,
            float height,
            float depth
        );
        kege::point3 emit();
        kege::point3 extents;
    };
}

#endif /* cubic_spawn_shape_hpp */
