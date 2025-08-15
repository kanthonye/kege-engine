//
//  box-vs-cone.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/16/25.
//

#ifndef box_vs_cone_hpp
#define box_vs_cone_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool boxConeCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool coneBoxCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
}

#endif /* box_vs_cone_hpp */
