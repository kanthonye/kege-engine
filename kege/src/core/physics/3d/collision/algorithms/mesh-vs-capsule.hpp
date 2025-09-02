//
//  mesh-vs-capsule.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef mesh_vs_capsule_hpp
#define mesh_vs_capsule_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool meshCapsuleCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool capsuleMeshCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* mesh_vs_capsule_hpp */
