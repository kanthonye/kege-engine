//
//  core-scene-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#include "engine.hpp"
#include "core-scene-manager.hpp"

namespace kege{

    void SceneManager::setScene( kege::Ref< kege::Scene > scene )
    {
        if ( _scene )
        {
            _scene->shutdown();
        }
        _scene = scene;

        // alert systems of the scene change
        _entity_systems->onSceneChange();
    }

    void SceneManager::changeScene( uint32_t scene_id )
    {
        if ( _scene_files.size() <= scene_id )
        {
            KEGE_LOG_ERROR << "INVALID_SCENE_INDEX : scene index out of bound." <<Log::nl;
            return;
        }

        kege::Ref< kege::Scene > scene = SceneLoader::load( _scene_files[ scene_id ] );
        if ( scene == nullptr )
        {
            KEGE_LOG_ERROR << "LOADING_FAILED : loadScene -> " << _scene_files[ scene_id ] <<Log::nl;
            return;
        }

        setScene( scene );
    }

    kege::Scene* SceneManager::getScene()
    {
        return _scene.ref();
    }

    const kege::EntitySystemManager* SceneManager::operator ->()const
    {
        return _entity_systems.ref();
    }

    kege::EntitySystemManager* SceneManager::operator ->()
    {
        return _entity_systems.ref();
    }

    SceneManager::operator bool()const
    {
        return _scene != nullptr;
    }

    void SceneManager::update( double dms )
    {
        _entity_systems->update( dms );
    }

    void SceneManager::render( double dms )
    {
        _entity_systems->render( dms );
    }

    void SceneManager::input( double dms )
    {
        _entity_systems->input( dms );
    }

    bool SceneManager::initialize()
    {
        _entity_manager = new kege::EntityManager;
        if ( !_entity_manager->initialize() )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> EntityManager" << Log::nl;
            return false;
        }
        Entity::setManager( _entity_manager.ref() );

        _entity_systems = new kege::EntitySystemManager( _engine );

        _entity_systems->addSystem( "camera-controller" );
        _entity_systems->addSystem( "lookat" );
        _entity_systems->addSystem( "follow" );
        _entity_systems->addSystem( "movement-controller" );

        _entity_systems->addSystem( "compute-camera-ray" );
        _entity_systems->addSystem( "entity-selecter" );
        _entity_systems->addSystem( "entity-dragging" );

        _entity_systems->addSystem( "particle-emitter-updater" );
        _entity_systems->addSystem( "particle-effect-updater" );

        _entity_systems->addSystem( "physics" );
        _entity_systems->addSystem( "rigidbody-to-transform" );

        _entity_systems->addSystem( "terrain" );
        _entity_systems->addSystem( "update-scene-camera" );
        _entity_systems->addSystem( "mesh-rendering" );

        //_module->addSystem( "entity-factory" );

        if ( !_entity_systems->initialize() )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> EntitySystemManager" << Log::nl;
            return false;
        }

        SceneLoader loader;
        // 10. Load Initial Scene
        // Example: Load the first scene file path if available
        if ( !_scene_files.empty() )
        {
            _scene = loader.load( vfs( _scene_files[0].c_str() ).c_str() );
            if ( !_scene )
            {
                kege::Log::error << "( LOADING_FAILED ) -> " << _scene_files[0] << Log::nl;
                kege::Log::error << "Failed to create scene from file " << _scene_files[0] << kege::Log::nl;
                return false;
            }
        }
        else
        {
            _scene = kege::Ref< kege::Scene >( new kege::Scene() );
        }

        _scene->initialize(); // Initialize the newly created scene
        if ( !_scene->ready() )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> Scene" << Log::nl;
            return false;
        }

        _entity_systems->onSceneChange();
        return true;
    }

    void SceneManager::shutdown()
    {
        if ( _scene )
        {
            _scene->shutdown();
            _scene.clear();
        }
        if ( _entity_manager )
        {
            _entity_manager->shutdown();
            _entity_manager.clear();
        }
        if ( _entity_systems )
        {
            _entity_systems->shutdown();
            _entity_systems.clear();
        }
    }

    void SceneManager::add()
    {
        _engine->addModule( this );
        kege::Log::info << "SceneManager module added to engine" << Log::nl;
    }

    SceneManager::SceneManager( kege::Engine* engine )
    :   Module( engine, "SceneManagerModule" )
    ,   _scene_files({ "assets/scene/scene.json" })
    {}

}
