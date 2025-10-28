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
        void update();

        RenderLayer( ref::RenderGraph& rg, const ref::ProjectManager& pm );
        ~RenderLayer();

    private:

        ref::ProjectManager _project_manager;
        ref::RenderGraph _render_graph;
    };

}
namespace kege::ref{
    typedef kege::Ref< kege::RenderLayer > RenderLayer;
}
#endif /* render_layer_hpp */
