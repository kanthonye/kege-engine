//
//  lookat-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/20/25.
//

#include "lookat-system.hpp"

namespace kege{

    void LookAtSystem::update( double dms )
    {
        for( auto [entity, lookat, transform] : view< LookAt, Transform >() )
        {
            if ( lookat->target )
            {
                Transform* target = _ecs->get< Transform >( lookat->target );
                vec3 direction = target->position - transform->position;
                transform->orientation = quatLookAt( direction, lookat->up );
            }
        }
    }

    LookAtSystem::LookAtSystem( kege::ECS* ecs )
    :   kege::ecs::System( ecs, "lookat-system", REQUIRE_UPDATE  )
    {
    }

    KEGE_REGISTER_ENTITY_SYSTEM( LookAtSystem, "lookat" );
}
