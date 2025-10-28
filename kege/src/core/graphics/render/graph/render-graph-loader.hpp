//
//  render-graph-loader.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/25/25.
//

#ifndef render_graph_loader_hpp
#define render_graph_loader_hpp

#include <map>
#include <string>
#include <vector>
#include <functional>
#include "json-parser.hpp"
#include "string-to-enum-types.hpp"

namespace kege{

    class RenderGraph;
    class RenderGraphLoader
    {
        public: static bool load( kege::RenderGraph& graph, const std::string& filename );
    };
}

#endif /* render_graph_loader_hpp */
