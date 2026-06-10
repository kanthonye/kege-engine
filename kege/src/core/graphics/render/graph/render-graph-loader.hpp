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
#include "../../../utils/json-parser.hpp"
#include "../../../graphics/core/device/core/device.hpp"
#include "render-pass-desc.hpp"

namespace kege{

    struct RenderGraphLoader
    {
        typedef std::map< std::string, uint32_t > StringToIntMap;
        typedef std::function< void( const StringToIntMap&, kege::RenderGraphDescriptor&, Json ) > Function;
        typedef std::map< std::string, Function > FunctionMap;
        
        public: static bool load
        (
            kege::RenderGraphDescriptor& graph,
            const kege::Swapchain* swapchain,
            const std::string& filename
        );
    };
}

#endif /* render_graph_loader_hpp */
