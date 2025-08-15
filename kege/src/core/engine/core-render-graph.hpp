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

    class CoreRenderGraph : public CoreSystem< kege::RenderGraph >
    {
    public:

        CoreRenderGraph( kege::Engine* engine );
        bool initialize();
        void shutdown();
    };

}
#endif /* kege_render_graph_system_hpp */
