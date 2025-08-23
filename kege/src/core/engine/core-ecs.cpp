//
//  ecs-system.cpp
//  physics
//
//  Created by Kenneth Esdaile on 4/19/25.
//

#include "engine.hpp"
#include "core-ecs.hpp"

namespace kege{

    ECSModule::ECSModule( kege::Engine* engine )
    :   kege::Module( engine, "ECSModule" )
    {}

    bool ECSModule::initialize()
    {
        if ( _module != nullptr )
        {
            kege::Log::error << "ECS-Module already initialized!" << Log::nl;
            return false;
        }

        _module = new kege::EntityManager;
        if ( !_module->initialize() )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> EntityManager" << Log::nl;
            return false;
        }
        Entity::setManager( _module.ref() );
        return true;
    }

    void ECSModule::shutdown()
    {
        if ( _module )
        {
            _module->shutdown();
            _module.clear();
        }
    }

    void ECSModule::add()
    {
        _engine->addModule( this );
        kege::Log::info << "ECS-Module module added to engine" << Log::nl;
    }

}
