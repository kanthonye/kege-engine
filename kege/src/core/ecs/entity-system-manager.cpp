//
//  entity-system-manager.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/13/25.
//

#include "entity-system.hpp"
#include "entity-system-manager.hpp"

namespace kege{

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
        kege::Ref< kege::EntitySystem > system = EntitySystemFactory::instance().create( name, this );
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

    bool EntitySystemManager::initialize()
    {
        Log::info << "[ EntitySystemManager ]: initializing..." <<"\n";
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
        Log::info << "[ EntitySystemManager ]: initialization complete."<<Log::nl;
        return true;
    }

    void EntitySystemManager::shutdown()
    {
        Log::info << "[ EntitySystemManager ]: shuting-down" <<"\n";
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
        Log::info << "[ EntitySystemManager ]: shutdown complete."<<Log::nl;
    }

    void EntitySystemManager::operator () ( const kege::Scene::Changed& msg )
    {
        for (kege::Ref< kege::EntitySystem > system : _systems )
        {
            system->onSceneChanged();
        }
    }
    
    kege::RenderExecutor* EntitySystemManager::getRenderExecutor()
    {
        return _render_executor;
    }
    
    kege::AssetManager* EntitySystemManager::getAssetManager()
    {
        return _project_manager->getAssetManager().ref();
    }

    kege::MappedInputs& EntitySystemManager::getMappedInputs()
    {
        return _input_context_manager->getMappedInputs();
    }

    kege::Graphics* EntitySystemManager::getGraphics()
    {
        return _render_executor->getGraphics();
    }
    
    kege::Scene* EntitySystemManager::getScene()
    {
        return _project_manager->getSceneManager()->getScene().ref();
    }

    EntitySystemManager:: ~EntitySystemManager()
    {
        Communication::remove< const kege::Scene::Changed&, EntitySystemManager >( this );
        shutdown();
    }

    EntitySystemManager::EntitySystemManager
    (
        kege::InputContextManager* icm,
        kege::RenderExecutor* rm,
        kege::ProjectManager* pm
    )
    :   _input_context_manager( icm )
    ,   _project_manager( pm )
    ,   _render_executor( rm )
    {
        Communication::add< const kege::Scene::Changed&, EntitySystemManager >( this );
    }

}
