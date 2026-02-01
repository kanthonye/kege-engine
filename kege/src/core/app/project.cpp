//
//  project.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#include "project.hpp"
namespace kege{

    Project::Project( const std::string& name, ref::ECS& ecs, ref::RenderGraph rg )
    :   file( nullptr )
    ,   name( name )
    {
        asset_manager = new AssetManager;
        scene_manager = new SceneManager( rg, ecs, asset_manager.ref() );
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
