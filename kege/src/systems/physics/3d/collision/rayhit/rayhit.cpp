//
//  rayhit.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/11/25.
//

#include "rayhit.hpp"

namespace kege::algo{

    typedef bool (*RayHitFunction)( const Ray& ray, const Collider* collider, RayHit* hit );
    static RayHitFunction rayhit_function_table[ RIGID_SHAPE_MAX_COUNT ];

    bool rayVsBox( const Ray& ray, const Collider* collider, RayHit* result )
    {
        return rayhitOBB( ray, *collider->getBox(), result );
    }

    bool rayVsSphere( const Ray& ray, const Collider* collider, RayHit* result )
    {
        return rayhitSphere( ray, *collider->getSphere(), result );
    }

    bool rayVsPlane( const Ray& ray, const Collider* collider, RayHit* result )
    {
        return rayhitPlane( ray, *collider->getPlane(), result );
    }

    bool rayVsCone( const Ray& ray, const Collider* collider, RayHit* hit )
    {
        return rayhitCone( ray, *collider->getCone(), hit );
    }

    bool rayVsCapsule( const Ray& ray, const Collider* collider, RayHit* hit )
    {
        return rayhitCapsule( ray, *collider->getCapsule(), hit );
    }

    bool rayVsCylinder( const Ray& ray, const Collider* collider, RayHit* hit )
    {
        return rayhitCylinder( ray, *collider->getCylinder(), hit );
    }

    bool rayVsPolygon( const Ray& ray, const Collider* collider, RayHit* hit )
    {
        return rayhitPolygon( ray, *collider->getPolygons(), hit );
    }

    bool rayVsAABB( const Ray& ray, const Collider* collider, RayHit* hit )
    {
        return rayhitAABB( ray, *collider->getAABB(), hit );
    }

    bool rayVsMesh( const Ray& ray, const Collider* collider, RayHit* hit )
    {
        return {};
    }

    bool rayVsCircle( const Ray& ray, const Collider* collider, RayHit* hit )
    {
        return rayhitCircle( ray, *collider->getCircle(), hit );
    }

    bool rayhit( const Ray& ray, const Collider* collider, RayHit* result )
    {
        return rayhit_function_table[ collider->shape_type ]( ray, collider, result );
    }

    void initializeRayHitFunctionTable()
    {
        rayhit_function_table[ RIGID_SHAPE_AABB           ] = rayVsAABB;
        rayhit_function_table[ RIGID_SHAPE_OBB            ] = rayVsBox;
        rayhit_function_table[ RIGID_SHAPE_PLANE          ] = rayVsPlane;
        rayhit_function_table[ RIGID_SHAPE_SPHERE         ] = rayVsSphere;
        rayhit_function_table[ RIGID_SHAPE_CAPSULE        ] = rayVsCapsule;
        rayhit_function_table[ RIGID_SHAPE_CYLINDER       ] = rayVsCylinder;
        rayhit_function_table[ RIGID_SHAPE_MESH           ] = rayVsMesh;
        rayhit_function_table[ RIGID_SHAPE_CONE           ] = rayVsCone;
        rayhit_function_table[ RIGID_SHAPE_CIRCLE         ] = rayVsCircle;
    }
}
