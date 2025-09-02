//
//  capsule-vs-box.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef capsule_vs_box_hpp
#define capsule_vs_box_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool capsuleBoxCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

    bool boxCapsuleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* capsule_vs_box_hpp */
