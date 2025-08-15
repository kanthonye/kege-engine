//
//  collision-manager.cpp
//  kege
//
//  Created by Kenneth Esdaile on 10/27/24.
//

#include "contact.hpp"

//#include "../../collision/algorithms/box-vs-box.hpp"
//#include "../../collision/algorithms/box-vs-cone.hpp"
//#include "../../collision/algorithms/box-vs-sphere.hpp"
//
//#include "../../collision/algorithms/plane-vs-box.hpp"
//#include "../../collision/algorithms/plane-vs-cone.hpp"
//#include "../../collision/algorithms/plane-vs-sphere.hpp"
//#include "../../collision/algorithms/plane-vs-cone.hpp"
//#include "../../collision/algorithms/plane-vs-capsule.hpp"
//#include "../../collision/algorithms/plane-vs-capsule.hpp"
//#include "../../collision/algorithms/plane-vs-cylinder.hpp"
//#include "../../collision/algorithms/plane-vs-mesh.hpp"
//#include "../../collision/algorithms/plane-vs-plane.hpp"
//
//#include "../../collision/algorithms/capsule-vs-cone.hpp"
//#include "../../collision/algorithms/capsule-vs-box.hpp"
//#include "../../collision/algorithms/capsule-vs-sphere.hpp"
//#include "../../collision/algorithms/capsule-vs-capsule.hpp"
//#include "../../collision/algorithms/capsule-vs-cylinder.hpp"
//
//#include "../../collision/algorithms/cylinder-vs-box.hpp"
//#include "../../collision/algorithms/cylinder-vs-cone.hpp"
//#include "../../collision/algorithms/cylinder-vs-sphere.hpp"
//#include "../../collision/algorithms/cylinder-vs-cylinder.hpp"
//
//#include "../../collision/algorithms/mesh-vs-box.hpp"
//#include "../../collision/algorithms/mesh-vs-cone.hpp"
//#include "../../collision/algorithms/mesh-vs-sphere.hpp"
//#include "../../collision/algorithms/mesh-vs-cylinder.hpp"
//#include "../../collision/algorithms/mesh-vs-capsule.hpp"
//#include "../../collision/algorithms/mesh-vs-mesh.hpp"
//
//#include "../../collision/algorithms/sphere-vs-cone.hpp"
//#include "../../collision/algorithms/sphere-vs-sphere.hpp"
//
//#include "../../collision/algorithms/cone-vs-cone.hpp"
//#include "../../collision/algorithms/circle-vs-circle.hpp"
//#include "../../collision/algorithms/plane-vs-circle.hpp"

namespace kege{

    CollisionManifold* CollisionRegistry::operator[](uint32_t i)
    {
        return &_collisions[i];
    }

    void CollisionRegistry::resize(uint32_t size)
    {
        _collisions.resize( size );
        _count = min( _count, size );
    }

    void CollisionRegistry::reset()
    {
        _count = 0;
    }

    CollisionManifold* CollisionRegistry::generate()
    {
        return &_collisions[ _count++ ];
    }

    uint32_t CollisionRegistry::count()const
    {
        return _count;
    }

    CollisionRegistry::CollisionRegistry()
    :   _count( 0 )
    {
    }

}
