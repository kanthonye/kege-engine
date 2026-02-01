//
//  project.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#ifndef kege_app_project_hpp
#define kege_app_project_hpp

#include <filesystem>
#include "../resource/asset-manager.hpp"
#include "../scene/scene-manager.hpp"
#include "../graphics/render/graph/render-graph.hpp"

namespace kege{

    struct Project : public kege::RefCounter
    {
        Project( const std::string& name, ref::ECS& ecs, ref::RenderGraph rg );
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
