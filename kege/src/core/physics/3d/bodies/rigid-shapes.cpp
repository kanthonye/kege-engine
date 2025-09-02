//
//  rigid-shapes.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/23/25.
//

#include "rigidbody.hpp"
#include "rigid-shapes.hpp"
namespace kege{

    void ColliderBox::integrate( Rigidbody* body )
    {
        solid.center = body->center + offset;
        solid.axes = kege::quatToM33( body->orientation );
    }
    const OBB* ColliderBox::getBox()const{
        return &solid;
    }
    ColliderBox::ColliderBox( const OBB& box )
    :   Collider( RIGID_SHAPE_OBB )
    ,   solid( box )
    {}
    ColliderBox::ColliderBox()
    :   Collider( RIGID_SHAPE_OBB )
    ,   solid({})
    ,   offset(0, 0, 0)
    {}


    void ColliderCircle::integrate( Rigidbody* body )
    {
        shape.center = body->center + offset;

        mat33 axes = kege::quatToM33( body->orientation );
        shape.normal = axes[2];
        shape.right = axes[0];
    }
    const Circle* ColliderCircle::getCircle()const
    {
        return &shape;
    }

    ColliderCircle::ColliderCircle( const Circle& shape )
    :   Collider( RIGID_SHAPE_CIRCLE )
    ,   offset({0,0,0})
    ,   shape(shape)
    {}
    ColliderCircle::ColliderCircle()
    :   Collider( RIGID_SHAPE_CIRCLE )
    ,   offset({0,0,0})
    ,   shape({})
    {}

    void ColliderSphere::integrate( Rigidbody* body )
    {
        solid.center = body->center + offset;
    }
    const Sphere* ColliderSphere::getSphere()const
    {
        return &solid;
    }
    ColliderSphere::ColliderSphere( const Sphere& sphere )
    :   Collider( RIGID_SHAPE_SPHERE )
    ,   solid( sphere )
    {}
    ColliderSphere::ColliderSphere()
    :   Collider( RIGID_SHAPE_SPHERE )
    ,   offset({})
    ,   solid({})
    {}


    void ColliderPlane::integrate( Rigidbody* body )
    {
    }
    const Plane* ColliderPlane::getPlane()const
    {
        return &solid;
    }
    ColliderPlane::ColliderPlane( const Plane& plane )
    :   Collider( RIGID_SHAPE_PLANE )
    ,   solid( plane )
    {}
    ColliderPlane::ColliderPlane()
    :   Collider( RIGID_SHAPE_PLANE )
    ,   solid({})
    {}



    void ColliderCylinder::integrate( Rigidbody* body )
    {
        solid.center = body->center;

        mat33 axes = kege::quatToM33( body->orientation );
        solid.axes[0] = axes[2];
        solid.axes[1] = axes[1];
    }
    const Cylinder* ColliderCylinder::getCylinder()const
    {
        return &solid;
    }
    ColliderCylinder::ColliderCylinder( const Cylinder& shape )
    :   Collider( RIGID_SHAPE_CYLINDER )
    ,   solid(shape)
    {}
    ColliderCylinder::ColliderCylinder()
    :   Collider( RIGID_SHAPE_CYLINDER )
    ,   solid({})
    {}


    void ColliderCapsule::integrate( Rigidbody* body )
    {
        solid.center = body->center;

        mat33 axes = kege::quatToM33( body->orientation );
        solid.axes[0] = axes[0];
        solid.axes[1] = axes[1];
    }
    const Capsule* ColliderCapsule::getCapsule()const
    {
        return &solid;
    }
    ColliderCapsule::ColliderCapsule( const Capsule& shape )
    :   Collider( RIGID_SHAPE_CAPSULE )
    ,   solid(shape)
    {}
    ColliderCapsule::ColliderCapsule()
    :   Collider( RIGID_SHAPE_CAPSULE )
    ,   solid({})
    {}


    void ColliderCone::integrate( Rigidbody* body )
    {
        solid.apex = body->center;

        mat33 axes = kege::quatToM33( body->orientation );
        solid.direction = axes[1];
    }
    const Cone* ColliderCone::getCone()const
    {
        return &solid;
    }
    ColliderCone::ColliderCone( const Cone& shape )
    :   Collider( RIGID_SHAPE_CONE )
    ,   solid(shape)
    {}
    ColliderCone::ColliderCone()
    :   Collider( RIGID_SHAPE_CONE )
    ,   solid({})
    {}


    kege::mat33 computeBoxInverseTensor( const kege::vec3& size, float mass )
    {
        float m = mass / 12.0;
        kege::mat33 inertia(1.0);
        inertia[0][0] = m * (kege::sq(size.y) + kege::sq(size.z));
        inertia[1][1] = m * (kege::sq(size.x) + kege::sq(size.z));
        inertia[2][2] = m * (kege::sq(size.x) + kege::sq(size.y));
        return kege::inverse( inertia );
    }

    kege::mat33 computeConeInverseTensor( float radius, float height, float mass )
    {
        kege::mat33 inertia(1.0);

        // Calculate moments of inertia
        float Iy = (3.0f/10.0f) * mass * radius * radius;
        float Ix_Iz = (3.0f/20.0f) * mass * (radius * radius + (4.0f/3.0f) * height * height);

        // Calculate inverse moments of inertia
        float invIy = 1.0f / Iy;
        float invIx_Iz = 1.0f / Ix_Iz;

        // Create inverse inertia tensor in principal axis frame
        return kege::mat33(kege::vec3{ invIx_Iz, invIy, invIx_Iz });
    }

    kege::mat33 computeCylinderInverseTensor( float radius, float height, float mass )
    {
        // Calculate moments of inertia
        float Iy = (3.0f/10.0f) * mass * radius * radius;
        float Ix_Iz = (3.0f/20.0f) * mass * (radius * radius + (4.0f/3.0f) * height * height);

        // Calculate inverse moments of inertia
        float invIy = 1.0f / Iy;
        float invIx_Iz = 1.0f / Ix_Iz;

        // Create inverse inertia tensor in principal axis frame
        return kege::mat33(kege::vec3{ invIx_Iz, invIy, invIx_Iz });
    }

    kege::mat33 computeCapsuleInverseTensor( float radius, float height, float mass )
    {
        // Calculate mass distribution
        float totalHeight = height + 2.0f * radius; // Total height including hemispheres
        float cylinderMass = mass * height / totalHeight;
        float sphereMass = mass * (2.0f * radius) / totalHeight;

        // Calculate moments of inertia
        float Iy = (0.5f * cylinderMass * radius * radius) +
                   (0.4f * sphereMass * radius * radius);

        float Ix_Iz = (1.0f/12.0f) * cylinderMass * (3.0f * radius * radius + height * height) +
                      sphereMass * ((2.0f/5.0f) * radius * radius + (height/2.0f) * (height/2.0f));

        // Calculate inverse moments of inertia
        float invIy = 1.0f / Iy;
        float invIx_Iz = 1.0f / Ix_Iz;

        // Create inverse inertia tensor
        return kege::mat33(kege::vec3{ invIx_Iz, invIy, invIx_Iz });
    }

    kege::mat33 computeSphereInverseTensor( float radius, float mass )
    {
        float inverse_tensor_value = 5.0f / (2.0f * mass * radius * radius);
        return kege::mat33( inverse_tensor_value );
    }


}
