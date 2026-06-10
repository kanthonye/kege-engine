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
        _renderer = nullptr;
    }

    bool RenderLayer::render()
    {
        kege::Project* project = _project_manager->getProject();
        if (project != nullptr)
        {
            kege::ref::Scene scene = project->getSceneManager()->getScene();
            if ( scene )
            {
                _renderer->render( scene.ref(), _gui );
            }
        }
        return true;
    }

    RenderLayer::RenderLayer( kege::Renderer* rr, kege::ProjectManager* pm, kege::GUI* ui )
    :   kege::AppLayer( "RenderLayer" )
    ,   _project_manager( pm )
    ,   _renderer( rr )
    ,   _gui( ui )
    {}

    RenderLayer::~RenderLayer()
    {
        shutdown();
    }
    
}
