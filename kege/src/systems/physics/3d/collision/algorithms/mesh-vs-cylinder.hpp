//
//  mesh-vs-cylinder.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef mesh_vs_cylinder_hpp
#define mesh_vs_cylinder_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    bool meshCylinderCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );
    bool cylinderMeshCollision( Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions );

}
#endif /* mesh_vs_cylinder_hpp */
