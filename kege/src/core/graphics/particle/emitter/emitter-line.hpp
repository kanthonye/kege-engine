//
//  line-emitter.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#ifndef line_spawn_shape_hpp
#define line_spawn_shape_hpp

#include "emitter.hpp"

namespace kege{

    class EmitterLine : public Emitter
    {
    public:

        EmitterLine
        (
            float emissions_per_second, bool burst,
            const kege::point3& start,
            const kege::point3& end
        );
        
        kege::point3 emit();
        kege::vec3 direction;
        kege::vec3 origin;
        float length;
    };
}

#endif /* line_spawn_shape_hpp */
