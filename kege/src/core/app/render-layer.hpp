//
//  render-layer.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/23/25.
//

#ifndef render_layer_hpp
#define render_layer_hpp

#include "../app/app-layer.hpp"
#include "../app/project-manager.hpp"
#include "../graphics/render/renderer.hpp"

namespace kege{

    class RenderLayer : public kege::AppLayer
    {
    public:

        bool initialize();
        void shutdown();
        bool render();

        RenderLayer( kege::Renderer* r, kege::ProjectManager* pm, kege::GUI* ui );
        ~RenderLayer();

    private:

        kege::ProjectManager* _project_manager;
        kege::Renderer* _renderer;
        kege::GUI* _gui;
    };

}
namespace kege::ref{
    typedef kege::Ref< kege::RenderLayer > RenderLayer;
}
#endif /* render_layer_hpp */
