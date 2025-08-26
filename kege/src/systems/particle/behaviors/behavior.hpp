//
//  behavior.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#ifndef behavior_hpp
#define behavior_hpp

#include <vector>
#include "../../../core/utils/array.hpp"
#include "../effect/particle.hpp"

namespace kege{

    class Behavior : public RefCounter
    {
    public:

        virtual void update( double dms, Particle& particle ) = 0;
        virtual ~Behavior(){}
    };

}
#endif /* behavior_hpp */
