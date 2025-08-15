//
//  render-graph-system.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/19/25.
//

#ifndef kege_render_graph_system_hpp
#define kege_render_graph_system_hpp

#include "../../core/graphics/graph/render-graph.hpp"
#include "../../core/system/system.hpp"

namespace kege{

    class RenderGraphSystem : public System
    {
    public:

        RenderGraphSystem( kege::Engine* engine );
        bool initialize()override;
        void shutdown()override;

    private:

        kege::Ref< kege::RenderGraph > _graph;

    };

}
#endif /* kege_render_graph_system_hpp */
