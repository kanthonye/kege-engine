//
//  entity-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#include "../scene/scene.hpp"
#include "entity-system.hpp"

namespace kege{

    EntitySystem::EntitySystem( kege::Engine* engine, const std::string& name, uint32_t requirements )
    :   kege::System( engine, name )
    ,   _requirements( requirements )
    {}

    bool EntitySystem::checkFlag( StateBitFlag flag )
    {
        return (_requirements & flag) == flag;
    }

    void EntitySystem::input( const std::vector< Input >& inputs )
    {
    }

    void EntitySystem::update( double dms )
    {
    }

    void EntitySystem::render( double dms )
    {
    }

    void EntitySystem::onSceneChange()
    {
        if ( _signature.any() && _engine->scene() )
        {
            _entities = _engine->scene()->getEntityRegistry().getEntityView( _signature );
        }
    }
    
    bool EntitySystem::initialize()
    {
        onSceneChange();
        return System::initialize();
    }

    void EntitySystem::shutdown()
    {
        System::shutdown();
    }

    EntitySystem:: ~EntitySystem()
    {}

}




namespace kege{

    void EntitySystemFactory::registerSystem( const std::string& name, EntitySystemFactory::CreateFunc func )
    {
        registry[ name ] = std::move( func );
    }

    kege::Ref< kege::EntitySystem > EntitySystemFactory::create( const std::string& name, kege::Engine* engine ) const
    {
        auto it = registry.find(name);
        if ( it != registry.end() )
        {
            return it->second( engine );
        }
        return nullptr;
    }

    EntitySystemFactory& EntitySystemFactory::instance()
    {
        static EntitySystemFactory factory;
        return factory;
    }

}
