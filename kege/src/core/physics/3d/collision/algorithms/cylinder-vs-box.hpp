//
//  cylinder-vs-box.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef cylinder_vs_box_hpp
#define cylinder_vs_box_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool cylinderBoxCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

    bool boxCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* cylinder_vs_box_hpp */
