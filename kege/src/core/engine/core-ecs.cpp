//
//  ecs-system.cpp
//  physics
//
//  Created by Kenneth Esdaile on 4/19/25.
//

#include "core-ecs.hpp"
namespace kege{

    CoreECS::CoreECS( kege::Engine* engine )
    :   kege::CoreSystem< kege::EntityManager >( engine, "ecs" )
    {}

    bool CoreECS::initialize()
    {
        _module = new kege::EntityManager;
        if ( !Entity::initialize( _module.ref() ) )
        {
            return false;
        }
        return true;
    }

    void CoreECS::shutdown()
    {
        if ( _module )
        {
            _module->shutdown();
            _module.clear();
        }
    }

    KEGE_REGISTER_SYSTEM( CoreECS, "ecs" );
}
