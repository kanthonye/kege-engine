//
//  scene-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#include "scene-manager.hpp"

namespace kege{

    ref::Scene SceneManager::createScene( const std::string& name )
    {
        _scene_fast_index_lookup[ name ] = _scenes.size();
        _scenes.push_back( new kege::Scene( name ) );
        return _scenes[ _scenes.size() - 1 ];
    }

    ref::Scene SceneManager::findScene( const std::string& name )
    {
        auto i = _scene_fast_index_lookup.find( name );
        if( i == _scene_fast_index_lookup.end() ) return {};
        return _scenes[ i->second ];
    }

    ref::Scene SceneManager::getScene()
    {
        return _curr_scene;
    }

    bool SceneManager::loadScenes( const std::string& name )
    {
        return true;
    }

    bool SceneManager::saveScenes( const std::string& name )
    {
        return true;
    }

    void SceneManager::setScene( ref::Scene scene )
    {
        if ( !findScene( scene->name() ) )
        {
            _scene_fast_index_lookup[ scene->name() ] = _scenes.size();
            _scenes.push_back( scene );
        }

        if ( _curr_scene )
        {
            _curr_scene->shutdown();
        }
        _curr_scene = scene;

        // alert systems of the scene change
        kege::Scene::Changed event = { _curr_scene };
        _communication.broadcast< kege::Scene::Changed >( event );
        //_entity_systems->onSceneChange();
    }

    bool SceneManager::initialize()
    {
        return true;
    }

}
