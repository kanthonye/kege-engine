//
//  render-layer.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#ifndef render_layer_hpp
#define render_layer_hpp

#include "app-layer.hpp"
#include "graphics.hpp"
#include "project-manager.hpp"
#include "render-graph.hpp"

namespace kege{

    class RenderLayer : public kege::AppLayer
    {
    public:

        bool initialize();
        void shutdown();
        bool update();

        RenderLayer( kege::AssetManager* am, ref::RenderGraph& rg, kege::ProjectManager* pm );
        ~RenderLayer();

    private:

        bool recreate( kege::Swapchain* swapchain );

        kege::ProjectManager* _project_manager;
        kege::AssetManager* _asset_manager;
        ref::RenderGraph _render_graph;
    };

}
namespace kege::ref{
    typedef kege::Ref< kege::RenderLayer > RenderLayer;
}
#endif /* render_layer_hpp */
