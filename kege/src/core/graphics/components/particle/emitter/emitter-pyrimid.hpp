//
//  pyrimid-emitter.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#ifndef pyrimid_spawn_shape_hpp
#define pyrimid_spawn_shape_hpp

#include "emitter.hpp"

namespace kege{

    class EmitterPyrimid : public Emitter
    {
    public:
        
        EmitterPyrimid
        (
            float emissions_per_second, bool burst,
            float width,
            float height,
            float depth
        );

        kege::point3 emit();

        float width, height, depth;
    };
}

#endif /* pyrimid_spawn_shape_hpp */

