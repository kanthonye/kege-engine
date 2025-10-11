//
//  entity-system-manager.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/13/25.
//

#include "entity-system.hpp"
#include "entity-system-manager.hpp"

namespace kege{

    EntitySystemManager::EntitySystemManager( kege::Engine* engine )
    :   kege::System( engine, "entity-system-manager" )
    ,   _engine( engine )
    {
    }

    void EntitySystemManager::addSystem( kege::Ref< kege::EntitySystem > system )
    {
        system->_engine = _engine;
        _systems.push_back( system );

        if ( system->checkFlag( kege::EntitySystem::REQUIRE_UPDATE ) )
        {
            _system_updates.push_back( system.ref() );
        }

        if ( system->checkFlag( kege::EntitySystem::REQUIRE_RENDER ) )
        {
            _system_renders.push_back( system.ref() );
        }

        if ( system->checkFlag( kege::EntitySystem::REQUIRE_INPUT ) )
        {
            _system_inputs.push_back( system.ref() );
        }
    }

    void EntitySystemManager::addSystem( const std::string& name )
    {
        kege::Ref< kege::EntitySystem > system = EntitySystemFactory::instance().create( name, _engine );
        if ( system )
        {
            addSystem( system );
        }
        else
        {
            Log::error << "system create function for -> " << name <<" dont exist." <<Log::nl;
        }
    }
    
    void EntitySystemManager::update( double dms )
    {
        for ( kege::EntitySystem* system : _system_updates )
        {
            system->update( dms );
        }
    }

    void EntitySystemManager::render( double dms )
    {
        for ( kege::EntitySystem* system : _system_renders )
        {
            system->render( dms );
        }
    }
 
    void EntitySystemManager::input( double dms )
    {
        for ( kege::EntitySystem* system : _system_inputs )
        {
            system->input( dms );
        }
    }

    void EntitySystemManager::onSceneChange()
    {
        for (kege::Ref< kege::EntitySystem > system : _systems )
        {
            system->onSceneChange();
        }
    }

    bool EntitySystemManager::initialize()
    {
        Log::info << "initializing -> " << getName()  <<"\n";
        for (kege::Ref< kege::EntitySystem >& system : _systems )
        {
            Log::info << "- initializing -> " << system->getName() <<"... ";
            if ( !system->initialize() )
            {
                Log::info << "FAILED... ";
                return false;
            }
            Log::info <<"complete" <<"\n";
        }
        Log::info << getName() << ", initialization complete."<<Log::nl;
        return true;
    }

    void EntitySystemManager::shutdown()
    {
        Log::info << "shuting-down -> " << getName()  <<"\n";
        std::vector< kege::Ref< kege::EntitySystem > >::reverse_iterator syst;
        for ( syst = _systems.rbegin(); syst != _systems.rend(); syst++ )
        {
            Log::info << "- shuting-down -> " << (*syst)->getName() <<".\n";
            (*syst)->shutdown();
        }
        _systems.clear();
        _system_updates.clear();
        _system_renders.clear();
        _system_inputs.clear();
        Log::info << getName() << ", shutdown complete."<<Log::nl;
    }

    EntitySystemManager:: ~EntitySystemManager()
    {
        shutdown();
    }


}
