//
//  ecs-layer.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#ifndef kege_ecs_layer_hpp
#define kege_ecs_layer_hpp

#include "app-layer.hpp"
#include "entity-manager.hpp"
#include "render-graph.hpp"
#include "entity-system-manager.hpp"

namespace kege{

    class ECSLayer : public kege::AppLayer
    {
    public:

        bool initialize();
        void shutdown();
        void update();

        ECSLayer( ref::InputContextManager icm, ref::AssetManager am, ref::RenderGraph rg, const ref::ProjectManager& pm );
        ~ECSLayer();

    private:

        ref::InputContextManager _input_context_manager;
        ref::EntitySystemManager _entity_systems;
        ref::EntityManager _entity_manager;
        ref::ProjectManager _project_manager;
        ref::RenderGraph _render_graph;
        ref::AssetManager _asset_manager;
    };

}

#endif /* kege_ecs_layer_hpp */
