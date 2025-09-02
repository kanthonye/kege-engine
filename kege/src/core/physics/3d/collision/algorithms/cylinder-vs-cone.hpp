//
//  cylinder-vs-cone.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef cylinder_vs_cone_hpp
#define cylinder_vs_cone_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool cylinderConeCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool coneCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
}

#endif /* cylinder_vs_cone_hpp */
