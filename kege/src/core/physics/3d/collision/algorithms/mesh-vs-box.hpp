//
//  mesh-vs-box.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef mesh_vs_box_hpp
#define mesh_vs_box_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool meshBoxCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool boxMeshCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* mesh_vs_box_hpp */
