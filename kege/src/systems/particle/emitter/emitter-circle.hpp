//
//  circle-emitter.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#ifndef kege_emitter_circle_hpp
#define kege_emitter_circle_hpp

#include "emitter.hpp"

namespace kege{

    class EmitterCircleLine : public Emitter
    {
    public:

        EmitterCircleLine( float emissions_per_second, bool burst, kege::vec3 axes[ 2 ], float radius );
        kege::point3 emit();

        kege::vec3 axes[ 2 ];
        float radius;
    };
}

namespace kege{

    class EmitterCircleArea : public Emitter
    {
    public:
        EmitterCircleArea( float emissions_per_second, bool burst, kege::vec3 axes[ 2 ], float min_radius, float max_radius );
        kege::point3 emit();

        kege::vec3 axes[ 2 ];
        float radius[ 2 ];
    };
}

#endif /* kege_emitter_circle_hpp */
