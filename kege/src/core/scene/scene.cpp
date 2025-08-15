//
//  scene.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#include "scene.hpp"
#include "entity-tag.hpp"

namespace kege{

    kege::AssetSystem& Scene::getAssetSystem()
    {
        return _asset_system;
    }

    EntityRegistry& Scene::getEntityRegistry()
    {
        return _registry;
    }

    void Scene::setCameraEntity( const Entity& entity )
    {
        _camera = entity;
    }

    const Entity& Scene::getCameraEntity()const
    {
        return _camera;
    }

    void Scene::setPlayer( const Entity& entity )
    {
        _player = entity;
    }

    const Entity& Scene::getPlayer()const
    {
        return _player;
    }

    Entity Scene::get( const std::string& name )
    {
        return getEntityChild( _root, name.data() );
    }

    bool Scene::remove( const std::string& name )
    {
        Entity entity = get( name );
        return remove( entity );
    }

    void Scene::insert( Entity entity )
    {
        _root.attach( entity );
        _registry.insert( entity );
    }

    bool Scene::remove( Entity& entity )
    {
        if ( _root != entity )
        {
            _registry.remove( entity );
            entity.destroy();
            return true;
        }
        return false;
    }

    void Scene::setSceneRay( const kege::vec3& ray )
    {
        _ray = ray;
    }

    const kege::vec3& Scene::getSceneRay()const
    {
        return _ray;
    }

    bool Scene::initialize()
    {
        _root = kege::Entity::create();
        _ready = true;
        return true;
    }

    void Scene::shutdown()
    {
        _ready = false;
        if( _camera ) _camera.destroy();
        if( _player ) _player.destroy();
        if( _root ) _root.destroy();
        _registry.clear();
        _root.destroy();
        _root = 0;
        
        _asset_system.shutdown();
    }

    bool Scene::ready()const
    {
        return _ready;
    }

    Scene::~Scene()
    {
        shutdown();
    }

    Scene::Scene()
    :   _ready( false )
    {
    }
}
