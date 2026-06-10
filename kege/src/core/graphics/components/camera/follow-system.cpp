//
//  follow-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/20/25.
//

#include "follow-system.hpp"

namespace kege{

    void FollowSystem::update( double dms )
    {
        for(auto [entity, follow, transform] : view<Follow, Transform>())
        {
            if ( follow->target )
            {
                Transform* target = _ecs->get< Transform >( follow->target );
                vec3 direction = getAxesZ( transform->orientation );
                transform->position = target->position + (direction * follow->distance) + follow->offset;
            }
        }
    }

    FollowSystem::FollowSystem( kege::ECS* esm )
    :   kege::ecs::System( esm, "follow-system", REQUIRE_UPDATE  )
    {
    }

    KEGE_REGISTER_ENTITY_SYSTEM( FollowSystem, "follow" );
}
