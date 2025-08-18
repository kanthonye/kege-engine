//
//  rigid-shapes.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/23/25.
//

#ifndef rigid_shapes_hpp
#define rigid_shapes_hpp

#include "collider.hpp"
#include "../../../../-/component-dependencies.hpp"

namespace kege{

    struct ColliderBox : public Collider
    {
        void integrate( Rigidbody* body );
        const kege::OBB* getBox()const;
        ColliderBox( const kege::OBB& box );
        ColliderBox();
        kege::OBB solid;
        kege::vec3 offset;
    };

    struct ColliderSphere : public Collider
    {
        void integrate( Rigidbody* body );
        const Sphere* getSphere()const;
        ColliderSphere( const Sphere& sphere );
        ColliderSphere();
        Sphere solid;
        kege::vec3 offset;
    };

    struct ColliderCircle : public Collider
    {
        void integrate( Rigidbody* body );
        const Circle* getCircle()const;

        ColliderCircle( const Circle& shape );
        ColliderCircle();

        Circle shape;
        kege::vec3 offset;
    };

    struct ColliderPlane : public Collider
    {
        void integrate( Rigidbody* body );
        const Plane* getPlane()const;

        ColliderPlane( const Plane& plane );
        ColliderPlane();
        Plane solid;
    };


    struct ColliderCylinder : public Collider
    {
        void integrate( Rigidbody* body );
        const Cylinder* getCylinder()const;

        ColliderCylinder( const Cylinder& shape );
        ColliderCylinder();
        Cylinder solid;
    };
    struct ColliderCapsule : public Collider
    {
        void integrate( Rigidbody* body );
        const Capsule* getCapsule()const;

        ColliderCapsule( const Capsule& shape );
        ColliderCapsule();
        Capsule solid;
    };
    struct ColliderCone : public Collider
    {
        void integrate( Rigidbody* body );
        const Cone* getCone()const;

        ColliderCone( const Cone& shape );
        ColliderCone();
        Cone solid;
    };



    struct ColliderMesh : public Collider
    {
        struct Triangle
        {
            uint32_t a,b,c;
            vec3 normal;
        };

        ColliderMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        ColliderMesh();

        void integrate( Rigidbody* body );

        std::vector< Triangle > triangles;
        std::vector< fvec3 > vertices;
    };


    kege::mat33 computeBoxInverseTensor( const kege::vec3& size, float mass );
    kege::mat33 computeConeInverseTensor( float radius, float height, float mass );
    kege::mat33 computeCylinderInverseTensor( float radius, float height, float mass );
    kege::mat33 computeCapsuleInverseTensor( float radius, float height, float mass );
    kege::mat33 computeSphereInverseTensor( float radius, float mass );
    
}
#endif /* rigid_shapes_hpp */
