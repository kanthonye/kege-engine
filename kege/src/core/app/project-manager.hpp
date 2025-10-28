//
//  project-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#ifndef kege_project_manager_hpp
#define kege_project_manager_hpp

#include "project.hpp"
#include "../graphics/render/core/graphics.hpp"

namespace kege{

    class ProjectManager : public kege::RefCounter
    {
    public:

        bool createProject( const std::string& filepath );
        bool openProject( const std::string& filepath );
        bool saveProject();

        ref::AssetManager getAssetManager();
        ref::SceneManager getSceneManager();
        ref::Graphics getGraphics();

        bool empty()const;

        ProjectManager( ref::Graphics graphics );
        ~ProjectManager();

    private:
        
        ref::Project _current_project;
        ref::Graphics _graphics;
    };

}

namespace kege::ref{
    typedef kege::Ref< ProjectManager > ProjectManager;
}
#endif /* kege_project_manager_hpp */
