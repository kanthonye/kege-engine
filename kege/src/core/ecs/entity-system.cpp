//
//  entity-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#include "entity-system.hpp"
#include "entity-system-manager.hpp"

namespace kege{

    EntitySystem::EntitySystem( const std::string& n, uint32_t r, kege::EntitySystemManager* m )
    :   _requirements( r )
    ,   _entities( nullptr )
    ,   _manager( m )
    ,   _name( n )
    {}

    bool EntitySystem::checkFlag( StateBitFlag flag )
    {
        return (_requirements & flag) == flag;
    }

    const kege::EntitySignature& EntitySystem::getEntitySignature()const
    {
        return _signature;
    }

    const std::string& EntitySystem::getName() const
    {
        return _name;
    }

    void EntitySystem::onSceneChanged()
    {
        if ( _signature.any() && _manager->getScene() )
        {
            _entities = _manager->getScene()->getEntityRegistry().getEntityView( _signature );
        }
    }

    EntityView* EntitySystem::getEntities()
    {
        return _entities;
    }

    kege::Scene* EntitySystem::getScene()
    {
        return _manager->getScene();
    }

    kege::RenderExecutor* EntitySystem::getRenderExecutor()
    {
        return _manager->getRenderExecutor();
    }
    
    kege::AssetManager* EntitySystem::getAssetManager()
    {
        return _manager->getAssetManager();
    }

    kege::MappedInputs& EntitySystem::getMappedInputs()
    {
        return _manager->getMappedInputs();
    }

    kege::Graphics* EntitySystem::getGraphics()
    {
        return _manager->getGraphics();
    }

    void EntitySystem::input( double dms )
    {
    }

    void EntitySystem::update( double dms )
    {
    }

    void EntitySystem::render( double dms )
    {
    }

    bool EntitySystem::initialize()
    {
        return true;
    }

    void EntitySystem::shutdown()
    {
        _entities = nullptr;
        _manager = nullptr;
    }

    EntitySystem:: ~EntitySystem()
    {}

}




namespace kege{

    void EntitySystemFactory::registerSystem( const std::string& name, EntitySystemFactory::CreateFunc func )
    {
        registry[ name ] = std::move( func );
    }

    kege::Ref< kege::EntitySystem > EntitySystemFactory::create( const std::string& name, kege::EntitySystemManager* m ) const
    {
        auto it = registry.find(name);
        if ( it != registry.end() )
        {
            return it->second( m );
        }
        return nullptr;
    }

    EntitySystemFactory& EntitySystemFactory::instance()
    {
        static EntitySystemFactory factory;
        return factory;
    }

}
