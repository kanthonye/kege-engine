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
        _project_manager = nullptr;
        _asset_manager = nullptr;
        _render_graph.clear();
    }

    bool RenderLayer::update()
    {
        if (!_project_manager) return false;

        kege::Graphics* graphics = _render_graph->getGraphics();
        kege::Swapchain* swapchain = graphics->getSwapchain();

        if ( !graphics->beginFrame() )
        {
            KEGE_LOG_ERROR << "Failed to begin Frame\n";
            return false;
        }

        int image_index = swapchain->acquireNextImage();
        if (image_index < 0)
        {
            if ( swapchain->shouldRecreate() )
            {
                if ( !recreate( swapchain ) )
                {
                    kege::Log::error << "Swapchain recreate failed in render()." << Log::nl;
                    return -1;
                }
                
                // Important: do NOT try to acquire again immediately in most cases
                // Just skip this frame — next frame will (hopefully) succeed
                graphics->endFrame();
                return true;   // ← still "success" (we handled resize)
            }
            else
            {
                return true;
            }
        }

        // image_index valid here
        ref::Semaphore image_available = swapchain->getImageAvailableSemaphore(image_index);
        ref::Semaphore render_complete = swapchain->getRenderFinishSemaphore(image_index);
        _render_graph->execute(image_available, render_complete);

        graphics->endFrame();

        // PRESENT
        bool ok = graphics->present(swapchain, swapchain->getRenderFinishSemaphore(image_index));
        // If present returned OUT_OF_DATE → flag for next frame
        if (!ok || swapchain->shouldRecreate())
        {
            // SUBOPTIMAL_KHR is **not** an error — you can still use the swapchain
            // but most people recreate anyway to get crisp scaling
            if(!recreate( swapchain ))
            {
                return false;
            }
        }
        return true;
    }

    bool RenderLayer::recreate( kege::Swapchain* swapchain )
    {
        if (swapchain->recreate() != kege::Result::SUCCESS)
        {
            kege::Log::error << "Swapchain recreate failed in render()." << Log::nl;
            return false;
        }
        _render_graph->setImagePhysicalHandles( "swapchain_color", swapchain->getColorImages() );
        _render_graph->setImagePhysicalHandles( "swapchain_depth", swapchain->getDepthImages() );
        return true;
    }

    RenderLayer::RenderLayer( kege::AssetManager* am, ref::RenderGraph& rg, kege::ProjectManager* pm )
    :   kege::AppLayer( "RenderLayer" )
    ,   _project_manager( pm )
    ,   _render_graph( rg )
    ,   _asset_manager( am )
    {}

    RenderLayer::~RenderLayer()
    {
        shutdown();
    }
    
}
