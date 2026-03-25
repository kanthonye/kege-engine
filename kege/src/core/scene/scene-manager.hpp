//
//  scene-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#ifndef kege_scene_manager_hpp
#define kege_scene_manager_hpp

#include "ecs.hpp"
#include "scene.hpp"
#include "../utils/communication.hpp"
#include "../graphics/render/graph/render-graph.hpp"

namespace kege{

    class SceneManager : public kege::RefCounter
    {
    public:

        ref::Scene createScene( const std::string& name );
        ref::Scene changeScene( const std::string& name );
        ref::Scene findScene( const std::string& name );
        ref::Scene getScene( uint32_t index );
        ref::Scene getScene();

        bool loadScenes( const std::string& name );
        bool saveScenes( const std::string& name );

        void setScene( ref::Scene scene );

        bool initialize();
        void shutdown();

        SceneManager(ref::RenderGraph rg, ref::ECS& ecs, ref::AssetManager asset_manager );

        ~SceneManager();
        
    private:

        std::unordered_map< std::string, size_t > _scene_fast_index_lookup;
        std::vector< ref::Scene > _scenes;

        ref::AssetManager _asset_manager;
        ref::RenderGraph _render_graph;
        ref::Scene _curr_scene;
        ref::ECS _ecs;
        kege::Communication _communication;
    };

}
namespace kege::ref{
    typedef kege::Ref< kege::SceneManager > SceneManager;
}
#endif /* kege_scene_manager_hpp */
