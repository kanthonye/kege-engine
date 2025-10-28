//
//  control-camera-movement.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#include "movement-system.hpp"

namespace kege{

    MovementSystem::MovementSystem( kege::EntitySystemManager* esm )
    :   kege::EntitySystem( "player-movement-system", REQUIRE_INPUT, esm  )
    {}

    bool MovementSystem::initialize()
    {
        return EntitySystem::initialize();
    }

    void MovementSystem::shutdown()
    {
        EntitySystem::shutdown();
    }

    void MovementSystem::input( double dms )
    {
        Entity entity = getScene()->getPlayer();
        if( !entity ) return;

        kege::Rigidbody* body = entity.get< Rigidbody >();
        if ( !body ) return;
        
        kege::MovementControl* control = entity.get< MovementControl >();
        if ( !control ) return;

        kege::vec3 speed = {0.f, 0.f, 0.f};

        const MappedInputs& inputs = getMappedInputs();
        if ( inputs[ kege::ACTION_MOVE_FORWARD ] )
        {
            speed.z -= control->speed.z;
        }
        if ( inputs[ kege::ACTION_MOVE_BACKWARD ] )
        {
            speed.z += control->speed.z;
        }

        if ( inputs[ kege::ACTION_MOVE_RIGHTWARD ] )
        {
            speed.x += control->speed.x;
        }
        if ( inputs[ kege::ACTION_MOVE_LEFTWARD ] )
        {
            speed.x -= control->speed.x;
        }

        if ( inputs[ kege::ACTION_MOVE_DOWNWARD ] )
        {
            speed.y -= control->speed.y;
        }
        if ( inputs[ kege::ACTION_MOVE_UPWARD ] )
        {
            speed.y += control->speed.y;
        }

        mat33 axies = quatToM33( body->orientation );
        body->linear.velocity += (speed.x * axies[0] + speed.y * axies[1] + speed.z * axies[2]);
    }

    KEGE_REGISTER_ENTITY_SYSTEM( MovementSystem, "movement-controller" );
}
