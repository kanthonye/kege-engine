//
//  scene-module.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/17/25.
//

#include "engine.hpp"
#include "scene-module.hpp"

namespace kege{

    SceneModule::SceneModule( kege::Engine* engine )
    :   kege::Module( engine, "SceneModule" )
    ,   _module( nullptr )
    {}

    const kege::Scene* SceneModule::operator ->()const
    {
        return _module.ref();
    }

    kege::Scene* SceneModule::operator ->()
    {
        return _module.ref();
    }

    SceneModule::operator bool()const
    {
        return _module != nullptr;
    }

    void SceneModule::setScene( kege::Ref< kege::Scene > scene )
    {
        if ( _module )
        {
            _module->shutdown();
        }
        _module = scene;

        // alert systems of the scene change
        if ( _engine->esm() )
        {
            _engine->esm()->onSceneChange();
        }
    }

    void SceneModule::changeScene( uint32_t scene_id )
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
    
    kege::Scene* SceneModule::getScene()
    {
        return _module.ref();
    }

    bool SceneModule::initialize()
    {
        // 10. Load Initial Scene
        // Example: Load the first scene file path if available
        if ( !_scene_files.empty() )
        {
            _module = SceneLoader::load( _scene_files[0] );
            if ( _module )
            {
                kege::Log::error << "( LOADING_FAILED ) -> " << _scene_files[0] << Log::nl;
                KEGE_LOG_ERROR << "Failed to create scene from file " << _scene_files[0] << kege::Log::nl;
                return false;
            }
        }
        else
        {
            _module = kege::Ref< kege::Scene >( new kege::Scene() );
        }

        _module->initialize(); // Initialize the newly created scene
        if ( !_module->ready() )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> Scene" << Log::nl;
            return false;
        }
        return true;
    }

    void SceneModule::shutdown()
    {
        if ( _module )
        {
            _module->shutdown();
            _module.clear();
        }
        _scene_files.clear();
    }

    void SceneModule::add()
    {
        _engine->addModule( this );
        kege::Log::info << getName() << ": added to engine" << Log::nl;
    }

}
