//
//  mesh-vs-sphere.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef mesh_vs_sphere_hpp
#define mesh_vs_sphere_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool meshSphereCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool sphereMeshCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* mesh_vs_sphere_hpp */
