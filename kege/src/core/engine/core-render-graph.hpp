//
//  render-graph-system.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/19/25.
//

#ifndef kege_render_graph_system_hpp
#define kege_render_graph_system_hpp

#include "../graphics/render/graph/render-graph.hpp"
#include "../graphics/render/pipeline/pipeline-loader.hpp"
#include "module.hpp"

namespace kege{

    class CoreRenderGraph : public Module
    {
    public:

        CoreRenderGraph( kege::Engine* engine );

        operator bool()const override{ return _module != nullptr; }
        bool initialize()override;
        void shutdown()override;
        void add()override;

        const kege::Ref< kege::RenderGraph > operator ->() const { return _module; }
        kege::Ref< kege::RenderGraph > operator ->() { return _module; }
        kege::RenderGraph* get() { return _module.ref(); }

    private:

        kege::Ref< kege::RenderGraph > _module;
    };

}

#endif /* kege_render_graph_system_hpp */
