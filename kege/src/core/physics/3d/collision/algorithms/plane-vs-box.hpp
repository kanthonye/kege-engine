//
//  plane-vs-box.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef plane_vs_box_hpp
#define plane_vs_box_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool planeBoxCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool boxPlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* plane_vs_box_hpp */
