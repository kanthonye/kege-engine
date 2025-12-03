//
//  rigidbody-to-transform.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 7/4/25.
//

#include "rigidbody-to-transform.hpp"

namespace kege{

    RigidbodyToTransform::RigidbodyToTransform( kege::ECS* ecs )
    :   kege::ecs::System( ecs, "RigidbodyToTransform", REQUIRE_UPDATE  )
    {}
    void RigidbodyToTransform::update( double dms )
    {
        for (auto [entity, rigidbody, transform] : view< kege::Rigidbody, kege::Transform >() )
        {
            transform->position = rigidbody->center;
            transform->orientation = rigidbody->orientation;
        }
    }
    KEGE_REGISTER_ENTITY_SYSTEM( RigidbodyToTransform, "RigidbodyToTransform" );

}

namespace kege{
    
    TransformToRigidbody::TransformToRigidbody( kege::ECS* ecs )
    :   kege::ecs::System( ecs, "TransformToRigidbody", REQUIRE_UPDATE  )
    {}
    void TransformToRigidbody::update( double dms )
    {
        for (auto [entity, rigidbody, transform] : view< kege::Rigidbody, kege::Transform >() )
        {
            rigidbody->center = transform->position;
            rigidbody->orientation = transform->orientation;
        }
    }
    KEGE_REGISTER_ENTITY_SYSTEM( TransformToRigidbody, "TransformToRigidbody" );

}
