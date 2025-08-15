//
//  cone-vs-cone.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef cone_vs_cone_hpp
#define cone_vs_cone_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool coneConeCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    
}

#endif /* cone_vs_cone_hpp */
