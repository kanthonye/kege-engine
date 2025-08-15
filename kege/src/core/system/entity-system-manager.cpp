//
//  entity-system-manager.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/13/25.
//

#include "entity-system-manager.hpp"
#include "../../core/system/entity-system.hpp"

namespace kege{

    EntitySystemManager::EntitySystemManager( kege::Engine* engine )
    :   kege::System( engine, "entity-system-manager" )
    {
    }

    void EntitySystemManager::addSystem( kege::Ref< kege::EntitySystem > system )
    {
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
            Log::error << "system create function for -> "<< name <<" dont exist." <<Log::nl;
        }
    }
    
    void EntitySystemManager::update( double dms )
    {
        for (kege::Ref< kege::EntitySystem > system : _systems )
        {
            system->update( dms );
        }
    }

    void EntitySystemManager::render( double dms )
    {
        for (kege::Ref< kege::EntitySystem > system : _systems )
        {
            system->render( dms );
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
        Log::info << "initializing -> " << getName()  <<Log::nl;
        for (kege::Ref< kege::EntitySystem >& system : _systems )
        {
            Log::info << "initializing -> " << system->getName() <<"\n";
            if ( !system->initialize() )
            {
                Log::info << system->getName() << " initialization failed." <<"\n";
                return false;
            }
            Log::info <<system->getName() << ": initialization complete." <<"\n";
        }
        Log::info << getName() << ", initialization complete."<<Log::nl;
        return true;
    }

    void EntitySystemManager::shutdown()
    {
        Log::info << "shuting-down -> " << getName()  <<Log::nl;
        std::vector< kege::Ref< kege::EntitySystem > >::reverse_iterator syst;
        for ( syst = _systems.rbegin(); syst != _systems.rend(); syst++ )
        {
            Log::info << "shuting-down -> " << (*syst)->getName() <<"\n";
            (*syst)->shutdown();
            Log::info  << "shutdown complete -> " << (*syst)->getName() <<"\n";
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
