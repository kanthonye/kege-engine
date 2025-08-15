//
//  ecs-system.cpp
//  physics
//
//  Created by Kenneth Esdaile on 4/19/25.
//

#include "ecs-system.hpp"
namespace kege{

    bool ECSystem::initialize()
    {
        if ( !Entity::initialize() )
        {
            return false;
        }
        return System::initialize();
    }

    void ECSystem::shutdown()
    {
        Entity::shutdown();
        System::shutdown();
    }

    ECSystem::ECSystem( kege::Engine* engine )
    :   kege::System( engine, "ecs", 0 )
    {}

    KEGE_REGISTER_SYSTEM( ECSystem, "ecs" );
}
