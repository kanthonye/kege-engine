//
//  application.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#ifndef application_hpp
#define application_hpp

#include "ref.hpp"
#include "app-layer-stack.hpp"
#include "app-window.hpp"
#include "project-manager.hpp"
#include "graphics.hpp"
#include "render-layer.hpp"
#include "input-layer.hpp"
#include "ecs-layer.hpp"
#include "virtual-directory.hpp"

namespace kege{

    class Application : public kege::RefCounter
    {
    public:

        virtual ~Application();
        Application();
        void run();

    protected:

        virtual bool initialize();
        virtual void shutdown();

    protected:

        ref::AppLayerStack _app_layer_stack;
        ref::ProjectManager _project_manager;
        ref::RenderGraph _render_graph;
        ref::Graphics _graphics;
        ref::AppWindow _window;
        //ref::RenderLayer _render_layer;
        //ref::InputLayer _input_layer;

        kege::VirtualDirectory _virdir;
        bool _running;
    };

    kege::Ref< Application > createApplication();

}

namespace kege::ref{
    typedef kege::Ref< kege::Application > Application;
}
#endif /* application_hpp */
