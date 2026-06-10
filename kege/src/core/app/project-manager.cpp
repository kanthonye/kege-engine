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
        _current_project = new kege::Project( filepath, _ecs );
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

    kege::Project* ProjectManager::getProject()
    {
        return _current_project.ref();
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

    ProjectManager::ProjectManager( ref::ECS& ecs )
    :   _ecs( ecs )
    {
        createProject( "new-project" );
    }

}
