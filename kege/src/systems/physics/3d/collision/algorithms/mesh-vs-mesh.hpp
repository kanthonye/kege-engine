//
//  mesh-vs-mesh.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef mesh_vs_mesh_hpp
#define mesh_vs_mesh_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool meshMeshCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* mesh_vs_mesh_hpp */
