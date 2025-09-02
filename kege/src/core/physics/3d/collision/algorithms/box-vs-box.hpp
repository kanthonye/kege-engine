//
//  box-vs-box.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef box_vs_box_hpp
#define box_vs_box_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{
    
    bool boxBoxCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
}
#endif /* box_vs_box_hpp */
