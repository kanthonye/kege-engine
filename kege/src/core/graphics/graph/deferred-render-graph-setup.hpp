//
//  deferred-render-graph-setup.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/26/25.
//

#ifndef deferred_render_graph_setup_hpp
#define deferred_render_graph_setup_hpp

#include "../../graphics/graph/render-graph.hpp"

kege::RenderPassDefn copyRenderPass( uint32_t width, uint32_t height, uint32_t frames_in_flight, kege::RenderGraph* graph );
kege::RenderPassDefn displayTriangleRenderPass( kege::Graphics* graphics, kege::RenderGraph* graph );

#endif /* deferred_render_graph_setup_hpp */
