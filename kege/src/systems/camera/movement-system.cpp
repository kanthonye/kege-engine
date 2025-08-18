//
//  control-camera-movement.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#include "movement-system.hpp"

namespace kege{

    MovementSystem::MovementSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "player-movement-system", 0 )
    {}

    bool MovementSystem::initialize()
    {
        _comm.add< const MappedInputs&, MovementSystem >( this );
        return EntitySystem::initialize();
    }

    void MovementSystem::shutdown()
    {
        _comm.remove< const MappedInputs&, MovementSystem >( this );
        EntitySystem::shutdown();
    }

    void MovementSystem::operator()( const MappedInputs& input_commands )
    {
        Entity entity = _engine->scene()->getPlayer();
        if( !entity ) return;

        kege::Rigidbody* body = entity.get< Rigidbody >();
        if ( !body ) return;
        
        kege::MovementControl* control = entity.get< MovementControl >();
        if ( !control ) return;

        kege::vec3 speed = {0.f, 0.f, 0.f};

        if ( input_commands[ kege::ACTION_MOVE_FORWARD ] )
        {
            speed.z -= control->speed.z;
        }
        if ( input_commands[ kege::ACTION_MOVE_BACKWARD ] )
        {
            speed.z += control->speed.z;
        }

        if ( input_commands[ kege::ACTION_MOVE_RIGHTWARD ] )
        {
            speed.x += control->speed.x;
        }
        if ( input_commands[ kege::ACTION_MOVE_LEFTWARD ] )
        {
            speed.x -= control->speed.x;
        }

        if ( input_commands[ kege::ACTION_MOVE_DOWNWARD ] )
        {
            speed.y -= control->speed.y;
        }
        if ( input_commands[ kege::ACTION_MOVE_UPWARD ] )
        {
            speed.y += control->speed.y;
        }

        mat33 axies = quatToM33( body->orientation );
        body->linear.velocity += (speed.x * axies[0] + speed.y * axies[1] + speed.z * axies[2]);
    }

    KEGE_REGISTER_SYSTEM( MovementSystem, "movement-controller" );
}
