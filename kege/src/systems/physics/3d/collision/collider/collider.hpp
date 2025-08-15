//
//  collider.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/20/25.
//

#ifndef kege_collider_hpp
#define kege_collider_hpp

#include "ref.hpp"
#include "primitive-3D-shapes.hpp"
#include "collisions-3D-shapes.hpp"

namespace kege{

    enum RigidShape
    {
        RIGID_SHAPE_SPHERE,
        RIGID_SHAPE_PLANE,
        RIGID_SHAPE_OBB,
        RIGID_SHAPE_AABB,
        RIGID_SHAPE_CONE,
        RIGID_SHAPE_CAPSULE,
        RIGID_SHAPE_CYLINDER,
        RIGID_SHAPE_CIRCLE,
        RIGID_SHAPE_MESH,
        RIGID_SHAPE_MAX_COUNT
    };

    struct Rigidbody;

    struct Collider : public RefCounter
    {
        virtual const OBB* getBox()const{ return nullptr; }
        virtual const AABB* getAABB()const{ return nullptr; }
        virtual const Sphere* getSphere()const{ return nullptr; }
        virtual const Plane* getPlane()const{ return nullptr; }
        virtual const Cylinder* getCylinder()const{ return nullptr; }
        virtual const Capsule* getCapsule()const{ return nullptr; }
        virtual const Polygon* getPolygons()const{ return nullptr; }
        virtual const Cone* getCone()const{ return nullptr; }
        virtual const Circle* getCircle()const{ return nullptr; }
        virtual void integrate( Rigidbody* body ){}
        RigidShape getShapeType()const{ return shape_type; }
        Collider( RigidShape shape ): shape_type( shape ) {}

        RigidShape shape_type;
        float friction;
        float cor;
        bool is_trigger;
    };

}

#endif /* kege_collider_hpp */
