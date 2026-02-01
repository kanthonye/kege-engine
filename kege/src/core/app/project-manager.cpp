//
//  project-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#include "project-manager.hpp"

namespace kege{

    bool ProjectManager::createProject( const std::string& filepath )
    {
        _current_project = new kege::Project( filepath, _ecs, _render_graph );
        return true;
    }
    
    bool ProjectManager::openProject( const std::string& filepath )
    {
        return true;
    }

    bool ProjectManager::saveProject()
    {
        return true;
    }

    ref::AssetManager ProjectManager::getAssetManager()
    {
        return _current_project->asset_manager;
    }

    ref::SceneManager ProjectManager::getSceneManager()
    {
        return _current_project->scene_manager;
    }

    ref::RenderGraph ProjectManager::getRenderGraph()
    {
        return _render_graph;
    }

    ref::Graphics ProjectManager::getGraphics()
    {
        return _graphics;
    }

    bool ProjectManager::empty()const
    {
        return _current_project.ref() == nullptr;
    }

    ProjectManager::~ProjectManager()
    {
        saveProject();
        _current_project.clear();
    }

    ProjectManager::ProjectManager( ref::Graphics graphics, kege::InputManager* im, ref::ECS& ecs, ref::RenderGraph rg )
    :   _graphics( graphics )
    ,   _render_graph( rg )
    ,   _ecs( ecs )
    {}

}
