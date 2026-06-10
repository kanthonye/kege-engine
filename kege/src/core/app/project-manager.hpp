//
//  project-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#ifndef kege_project_manager_hpp
#define kege_project_manager_hpp

#include "project.hpp"
#include "../input/input-manager.hpp"
#include "../input/input-context-manager.hpp"
//#include "../graphics/core/core.hpp"

namespace kege{

    class ProjectManager : public kege::RefCounter
    {
    public:

        bool createProject( const std::string& filepath );
        bool openProject( const std::string& filepath );
        bool saveProject();

        kege::Project* getProject();

        bool empty()const;

        ProjectManager( ref::ECS& ecs );
        ~ProjectManager();

    private:

        ref::Project _current_project;
        ref::ECS _ecs;
    };

}

namespace kege::ref{
    typedef kege::Ref< ProjectManager > ProjectManager;
}
#endif /* kege_project_manager_hpp */
