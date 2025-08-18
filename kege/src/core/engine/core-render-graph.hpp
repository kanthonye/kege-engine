//
//  render-graph-system.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/19/25.
//

#ifndef kege_render_graph_system_hpp
#define kege_render_graph_system_hpp

#include "../../core/graphics/graph/render-graph.hpp"
#include "../graphics/loaders/pipeline-loader.hpp"
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

        kege::Ref< kege::RenderGraph > operator ->() { return _module; }
        const kege::Ref< kege::RenderGraph > operator ->() const { return _module; }    

    private:

        kege::Ref< kege::RenderGraph > _module;
    };

}
#endif /* kege_render_graph_system_hpp */
