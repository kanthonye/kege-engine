//
//  plane-vs-plane.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef plane_vs_plane_hpp
#define plane_vs_plane_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool planePlaneCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* plane_vs_plane_hpp */
