//
//  render-layer.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#include "render-layer.hpp"

namespace kege{

    bool RenderLayer::initialize()
    {
        return true;
    }

    void RenderLayer::shutdown()
    {
        _render_graph.clear();
        _project_manager.clear();
    }

    void RenderLayer::update()
    {
        if ( !_project_manager ) return;

        kege::Graphics* graphics = _render_graph->getGraphics();
        bool success = graphics->beginFrame();
        if ( !success )
        {
            kege::Log::error << "Failed to begin Frame" <<Log::nl;
            return;
        }

        kege::Swapchain* swapchain = graphics->getSwapchain();

        int image_index = swapchain->acquireNextImage();
        ref::Semaphore image_available = swapchain->getImageAvailableSemaphore( image_index );
        ref::Semaphore render_complete = swapchain->getRenderFinishSemaphore( image_index );
        _render_graph->execute( image_available, render_complete );
        graphics->endFrame();

        graphics->present( swapchain, render_complete );
    }
    
    RenderLayer::RenderLayer( ref::RenderGraph& rg, const ref::ProjectManager& pm )
    :   kege::AppLayer( "RenderLayer" )
    ,   _project_manager( pm )
    ,   _render_graph( rg )
    {}

    RenderLayer::~RenderLayer()
    {
        _project_manager.clear();
    }
    
}
