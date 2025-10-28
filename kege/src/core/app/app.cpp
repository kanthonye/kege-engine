//
//  application.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#include "app.hpp"
#include "glfw-window.hpp"

namespace kege{

    bool Application::initialize()
    {
        /**
         * Here i am setting up the virtual directorys aliases and path.
         */
        kege::VirtualDirectory::instance().add( "root", "/Users/kae/Developer/vscode/kege-engine/kege" );
        kege::VirtualDirectory::instance().add( "assets", "/Users/kae/Developer/vscode/kege-engine/kege/assets" );
        kege::VirtualDirectory::instance().add( "config", "/Users/kae/Developer/vscode/kege-engine/kege/assets/config" );
        kege::VirtualDirectory::instance().add( "graphics-shaders", "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/graphics" );
        kege::VirtualDirectory::instance().add( "compute-shaders", "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/compute" );


        /**
         * Next, create the application window.
         */
        kege::WindowCreateInfo create_window_info = {};
        create_window_info.title = "KEGE";
        create_window_info.width = 1536;
        create_window_info.height = 896;
        create_window_info.visible = true;
        create_window_info.resizable = true;
        create_window_info.fullscreen = false;
        create_window_info.maximized = false;
        create_window_info.decorated = true;
        create_window_info.vsync = false;

        _window = new kege::GlfwWindow();
        if ( !_window->create( create_window_info ) )
        {
            kege::Log::error << "Failed to initialize AppWindow."<<Log::nl;
            return false;
        }

        /**
         * Then, create and initialize the graphics renderer.
         */
        kege::DeviceInitializationInfo device_init_info = {};
        device_init_info.window = _window.ref();
        device_init_info.preferred_API = kege::GraphicsAPI::Vulkan;
        device_init_info.enable_raytracing = false;
        device_init_info.prefer_discrete_gpu = true;
        device_init_info.prefer_higher_api_version = true;
        device_init_info.require_shader_float64 = false;
        device_init_info.engine = "KEGE";
        device_init_info.name = "dev";
        device_init_info.enable_debug_validation = true;

        kege::SwapchainDesc swapchain_create_info = {};
        swapchain_create_info.image_count = kege::MAX_FRAMES_IN_FLIGHT + 1;
        swapchain_create_info.name = "swapchain";
        swapchain_create_info.width = _window->getWidth();
        swapchain_create_info.height = _window->getHeight();
        swapchain_create_info.color_format = kege::Format::bgra_u8_norm;
        swapchain_create_info.depth_format = kege::Format::depth_32;
        swapchain_create_info.present_mode = kege::PresentMode::Fifo;
        swapchain_create_info.present_queue_type = kege::QueueType::Graphics;
        swapchain_create_info.image_usage = kege::ImageUsage::Color | kege::ImageUsage::TransferDst;

        _graphics = new kege::Graphics();
        if( !_graphics->initalize( device_init_info, swapchain_create_info ) )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> Graphics" << Log::nl;
            return false;
        }

        /**
         * With that completed, create the render manager
         */
        _render_graph = new kege::RenderGraph( _graphics.ref() );
        bool success = RenderGraphLoader::load( *_render_graph, vfs("config/render-graph.json").str() );
        if( !success ) return false;
        if( !_render_graph->compile() ) return false;

        /**
         * After that, create and initialize our input handler.
         */
        ref::InputContextManager icm = new kege::InputContextManager;
        if( !icm->initialize( _window.ref() ) )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> InputContextManager" << Log::nl;
            return false;
        }

        /**
         * Next step, create our project manager
         */
        _project_manager = new ProjectManager( _graphics );

        /**
         * Finally, create the app stack and add it layers, then initalize then
         */
        _app_layer_stack = new kege::AppLayerStack();

        success = _app_layer_stack->push( new kege::InputLayer( icm ) );
        if( !success ) return false;

        success = _app_layer_stack->push( new kege::RenderLayer( _render_graph, _project_manager ) );
        if( !success ) return false;

        success = _app_layer_stack->push( new kege::ECSLayer( icm, _render_graph, _project_manager ) );
        if( !success ) return false;

        _running = success;
        return _running;
    }

    void Application::shutdown()
    {
        _app_layer_stack.clear();
        _project_manager.clear();
        _render_graph.clear();
        _graphics.clear();
        _window.clear();
    }

    void Application::run()
    {
        initialize();
        while ( _running && !_window->shouldClose() )
        {
            _window->pollEvents();
            _app_layer_stack->update();
        }
        shutdown();
    }

    Application::Application()
    {}

    Application:: ~Application()
    {
        shutdown();
    }
}
