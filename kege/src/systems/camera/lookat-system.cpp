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
        vec3 direction;
        LookAt* look;
        Transform* target;
        Transform* transform;

        for( Entity entity : *_entities )
        {
            transform = entity.get< Transform >();
            look = entity.get< LookAt >();

            if ( look->target )
            {
                target = look->target.get< Transform >();
                direction = target->position - transform->position;
                transform->orientation = quatLookAt( direction, look->up );
            }
        }
    }

    bool LookAtSystem::initialize()
    {
        return EntitySystem::initialize();
    }

    void LookAtSystem::shutdown()
    {
        return EntitySystem::shutdown();
    }

    LookAtSystem::LookAtSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "lookat-system", REQUIRE_UPDATE )
    {
        _signature = createEntitySignature< LookAt, Transform >();
    }

    KEGE_REGISTER_SYSTEM( LookAtSystem, "lookat" );
}
