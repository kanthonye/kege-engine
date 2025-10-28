//
//  project.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#ifndef kege_app_project_hpp
#define kege_app_project_hpp

#include <filesystem>
#include "../utils/asset-manager.hpp"
#include "scene-manager.hpp"

namespace kege{

    struct Project : public kege::RefCounter
    {
        Project( const std::string& name );
        ~Project();

        ref::AssetManager asset_manager;
        ref::SceneManager scene_manager;

        std::filesystem::path path;
        std::string name;
        FILE* file;
    };

}
namespace kege::ref{
    typedef kege::Ref< Project > Project;
}
#endif /* kege_app_project_hpp */
