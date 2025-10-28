//
//  rigidbody-to-transform.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 7/4/25.
//

#include "rigidbody-to-transform.hpp"

namespace kege{

    RigidbodyToTransform::RigidbodyToTransform( kege::EntitySystemManager* esm )
    :   kege::EntitySystem( "rigidbody-to-transform", REQUIRE_UPDATE, esm  )
    {
        _signature = createEntitySignature< kege::Rigidbody, kege::Transform >();
    }

    void RigidbodyToTransform::update( double dms )
    {
        for (kege::Entity entity : *_entities )
        {
            kege::Rigidbody* rigidbody = entity.get< kege::Rigidbody >();
            kege::Transform* transform = entity.get< kege::Transform >();

            transform->position = rigidbody->center;
            transform->orientation = rigidbody->orientation;
        }
    }

    bool RigidbodyToTransform::initialize()
    {
        _signature = kege::createEntitySignature< kege::Rigidbody, kege::Transform >();
        return EntitySystem::initialize();
    }

    void RigidbodyToTransform::shutdown()
    {
        return EntitySystem::shutdown();
    }

    KEGE_REGISTER_ENTITY_SYSTEM( RigidbodyToTransform, "rigidbody-to-transform" );

}
