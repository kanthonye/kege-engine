//
//  mesh-vs-cone.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef mesh_vs_cone_hpp
#define mesh_vs_cone_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool meshConeCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool coneMeshCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* mesh_vs_cone_hpp */
