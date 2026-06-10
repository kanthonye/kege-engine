//
//  project.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#include "project.hpp"
namespace kege{

    Project::Project( const std::string& name, ref::ECS& ecs )
    :   file( nullptr )
    ,   name( name )
    {
        asset_manager = new AssetManager;
        scene_manager = new SceneManager( ecs, asset_manager.ref() );
    }

    kege::SceneManager* Project::getSceneManager()
    {
        return scene_manager.ref();
    }

    Project::~Project()
    {
        asset_manager->shutdown();
        scene_manager->shutdown();
        asset_manager.clear();
        scene_manager.clear();
        
        if ( file != nullptr )
        {
            fclose( file );
            file = nullptr;
        }
    }
}
