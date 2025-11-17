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

    int RenderLayer::render( kege::Swapchain* swapchain )
    {
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

                // Try acquire again once
                image_index = swapchain->acquireNextImage();
                if (image_index < 0)
                {
                    // Give up this frame
                    return -1;
                }
            }
            else
            {
                return -1;
            }
        }

        // image_index valid here
        ref::Semaphore image_available = swapchain->getImageAvailableSemaphore(image_index);
        ref::Semaphore render_complete = swapchain->getRenderFinishSemaphore(image_index);
        _render_graph->execute(image_available, render_complete);
        return image_index;
    }


    void RenderLayer::update()
    {
        if (!_project_manager) return;

        kege::Graphics* graphics = _render_graph->getGraphics();
        if ( !graphics->beginFrame() )
        {
            KEGE_LOG_ERROR << "Failed to begin Frame\n";
            return;
        }

        kege::Swapchain* swapchain = graphics->getSwapchain();
        int image_index = render( swapchain );
        if (image_index < 0)
        {
            // acquireNextImage failed after attempting recreate → skip frame
            graphics->endFrame();
            return;
        }

        graphics->endFrame();

        // PRESENT
        bool ok = graphics->present(swapchain, swapchain->getRenderFinishSemaphore(image_index));
        // If present returned OUT_OF_DATE → flag for next frame
        if (!ok || swapchain->shouldRecreate())
        {
            recreate( swapchain );
        }
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
