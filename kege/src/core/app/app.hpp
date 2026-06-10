//
//  application.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#ifndef application_hpp
#define application_hpp

#include "app-layer-stack.hpp"
#include "project-manager.hpp"
#include "../graphics/render/renderer.hpp"
#include "../input/input-manager.hpp"
#include "../io/virtual-directory.hpp"
#include "../utils/communication.hpp"

namespace kege{

    class Application : public kege::RefCounter
    {
    public:

        void operator()( kege::CallbackRequest< kege::ProjectManager >& request );
        void operator()( kege::CallbackRequest< kege::GraphicsDevice >& request );

        void operator()( const kege::Request< kege::ProjectManager >& );
        void operator()( const kege::Request< kege::GraphicsDevice >& );

        virtual ~Application();
        Application();
        
        void run();

    protected:

        virtual bool initialize();
        virtual void shutdown();

    protected:

        kege::ref::ProjectManager _project_manager;
        kege::ref::AppLayerStack _app_layer_stack;
        kege::ref::AssetManager _asset_manager;
        kege::ref::InputManager _input_manager;
        kege::ref::Renderer _renderer;
        kege::ref::AppWindow _window;
        kege::ref::ECS _ecs;
        kege::ref::GUI _gui;

        kege::VirtualDirectory _virdir;
        bool _running;
    };

    kege::Ref< Application > createApplication();

}

namespace kege::ref{
    typedef kege::Ref< kege::Application > Application;
}
#endif /* application_hpp */
