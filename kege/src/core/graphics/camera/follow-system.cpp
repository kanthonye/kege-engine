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
        if ( !_entities ) return;
        
        for( Entity entity : *_entities )
        {
            Transform* transform = entity.get< Transform >();
            Follow* follow = entity.get<Follow>();

            if ( follow->target )
            {
                Transform* target = follow->target.get< Transform >();
                vec3 direction = getAxesZ( transform->orientation );
                transform->position = target->position + (direction * follow->distance) + follow->offset;
            }
        }
    }

    bool FollowSystem::initialize()
    {
        return EntitySystem::initialize();
    }

    void FollowSystem::shutdown()
    {
        return EntitySystem::shutdown();
    }

    FollowSystem::FollowSystem( kege::EntitySystemManager* esm )
    :   kege::EntitySystem( "follow-system", REQUIRE_UPDATE, esm  )
    {
        _signature = createEntitySignature< Follow, Transform >();
    }

    KEGE_REGISTER_ENTITY_SYSTEM( FollowSystem, "follow" );
}
