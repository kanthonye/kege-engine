//
//  ecs-layer.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#ifndef kege_ecs_layer_hpp
#define kege_ecs_layer_hpp

#include "../app/app-layer.hpp"
#include "../app/project-manager.hpp"
#include "../scene/ecs.hpp"
#include "../graphics/render/graph/render-graph.hpp"

namespace kege{

    class ECSLayer : public kege::AppLayer
    {
    public:

        bool initialize();
        void shutdown();
        bool update();
        bool render();

        ECSLayer( ref::ECS ecs, ref::AssetManager am, const ref::ProjectManager& pm );
        ~ECSLayer();

    private:

        //ref::InputContextManager _input_context_manager;
        //ref::EntityManager _entity_manager;
        ref::ProjectManager _project_manager;
        ref::AssetManager _asset_manager;
        ref::ECS _ecs;
    };

}

#endif /* kege_ecs_layer_hpp */
