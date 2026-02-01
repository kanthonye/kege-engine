//
//  scene.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#include "scene.hpp"

namespace kege{

    ecs::Entity Scene::getEntityChild( ecs::Entity entity, const std::string& name )
    {
        for (ecs::Entity child = _ecs.begin( entity ); _ecs.valid(child); child = _ecs.next( entity ) )
        {
            std::string* str = _ecs.get< std::string >( child );
            if ( str )
            {
                if ( *str == name )
                {
                    return child;
                }
            }
        }

        for (ecs::Entity child = _ecs.begin( entity ); _ecs.valid(child); child = _ecs.next( entity ) )
        {
            ecs::Entity e = getEntityChild( child, name );
            if ( _ecs.valid(e) )
            {
                return e;
            }
        }

        return {};
    }

    kege::AssetManager* Scene::getAssetManager()
    {
        return _asset_manager;
    }

    kege::RenderGraph* Scene::getRenderGraph()
    {
        return _render_graph;
    }

    void Scene::setCameraEntity( const ecs::Entity& entity )
    {
        _camera = entity;
    }

    const ecs::Entity& Scene::getCameraEntity()const
    {
        return _camera;
    }

    void Scene::setPlayer( const ecs::Entity& entity )
    {
        _player = entity;
    }

    const ecs::Entity& Scene::getPlayer()const
    {
        return _player;
    }

    ecs::Entity Scene::get( const std::string& name )
    {
        return getEntityChild( _root, name.data() );
    }

    void Scene::remove( const std::string& name )
    {
        ecs::Entity entity = get( name );
        remove( entity );
    }

    void Scene::registerEntities( const ecs::Entity& entity )
    {
        ecs::EntityRegistry::insert( entity );
        for (ecs::Entity e = _ecs.begin(entity); _ecs.valid(e); e = _ecs.next(e))
        {
            registerEntities(e);
        }
    }
    void Scene::insert( ecs::Entity& entity )
    {
        _ecs.attach(_root, entity);
        registerEntities( entity );
    }

    void Scene::remove( ecs::Entity& entity )
    {
        for (ecs::Entity e = _ecs.begin(entity); _ecs.valid(e); e = _ecs.next(e))
        {
            remove(e);
        }
        ecs::EntityRegistry::remove( entity );
        _ecs.detach(entity);
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


//        Entity entity = ecs::Entity::create();
//        entity.add< EntityTag >( "camera" );
//        insert( entity );
//
//        entity = ecs::Entity::create();
//        entity.add< EntityTag >( "circle" );
//        insert( entity );
//
//        Entity square = ecs::Entity::create();
//        square.add< EntityTag >( "square" );
//        {
//            entity = ecs::Entity::create();
//            entity.add< EntityTag >( "triangle1" );
//            square.attach( entity );
//
//            entity = ecs::Entity::create();
//            entity.add< EntityTag >( "triangle2" );
//            square.attach( entity );
//        }
//        insert( square );


        _ready = true;
        return true;
    }

    void Scene::shutdown()
    {
        _ecs.destroy(_root);
//        _ready = false;
//        if( _camera ) _camera.destroy();
//        if( _player ) _player.destroy();
//        if( _root ) _root.destroy();
//        _registry.clear();
//        _camera = 0;
//        _player = 0;
//        _root = 0;
    }

    ecs::Entity Scene::root()
    {
        return _root;
    }
    bool Scene::ready()const
    {
        return _ready;
    }

    const std::string& Scene::name()const
    {
        return _name;
    }

    Scene::~Scene()
    {
        shutdown();
    }

    Scene::Scene( const std::string& name, ecs::EntityManager &ecs, kege::AssetManager* am, kege::RenderGraph* rg )
    :   kege::ecs::EntityRegistry( ecs )
    ,   _asset_manager( am )
    ,   _render_graph( rg )
    ,   _name( name )
    ,   _ready( false )
    {
        _root = _ecs.create();
    }
}
