//
//  rayhit.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/11/25.
//

#ifndef rayhit_hpp
#define rayhit_hpp

#include "../../collision/collider/collider.hpp"

#include "../../collision/rayhit/rayhit-obb.hpp"
#include "../../collision/rayhit/rayhit-aabb.hpp"
#include "../../collision/rayhit/rayhit-cone.hpp"
#include "../../collision/rayhit/rayhit-mesh.hpp"
#include "../../collision/rayhit/rayhit-rect.hpp"
#include "../../collision/rayhit/rayhit-plane.hpp"
#include "../../collision/rayhit/rayhit-sphere.hpp"
#include "../../collision/rayhit/rayhit-pyramid.hpp"
#include "../../collision/rayhit/rayhit-capsule.hpp"
#include "../../collision/rayhit/rayhit-cylinder.hpp"
#include "../../collision/rayhit/rayhit-triangle.hpp"

namespace kege::algo{

    bool rayVsBox( const Ray& ray, const Collider* collider, RayHit* result );

    bool rayVsSphere( const Ray& ray, const Collider* collider, RayHit* result );

    bool rayVsPlane( const Ray& ray, const Collider* collider, RayHit* result );

    bool rayVsCone( const Ray& ray, const Collider* collider, RayHit* hit );

    bool rayVsCapsule( const Ray& ray, const Collider* collider, RayHit* hit );

    bool rayVsCylinder( const Ray& ray, const Collider* collider, RayHit* hit );

    bool rayVsMesh( const Ray& ray, const Collider* collider, RayHit* hit );

    bool rayVsPolygon( const Ray& ray, const Collider* collider, RayHit* hit );

    bool rayhit( const Ray& ray, const Collider* collider, RayHit* result );

    void initializeRayHitFunctionTable();
}
#endif /* rayhit_hpp */
