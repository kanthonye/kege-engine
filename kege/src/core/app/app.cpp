//
//  application.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/22/25.
//


#include "../resource/image-loader.hpp"
#include "../resource/input-context-loader.hpp"
#include "../graphics/render/renderer.hpp"
#include "../graphics/core/pipeline/shader-system/json-shader-loader.hpp"
//#include "../graphics/shader-system/shader-pipeline-loader.hpp"

#include "app.hpp"
#include "render-layer.hpp"
#include "ecs-layer.hpp"

namespace kege{

    void Application::operator()( kege::CallbackRequest< kege::ProjectManager >& request )
    {
        request.callback( _project_manager.ref() );
    }

    void Application::operator()( kege::CallbackRequest< kege::GraphicsDevice >& request )
    {
        request.callback( _renderer->getDevice() );
    }

    void Application::operator()( const kege::Request< kege::ProjectManager >& )
    {
        Communication::broadcast< const kege::Response< kege::ProjectManager > >({ _project_manager.ref() });
    }

    void Application::operator()( const kege::Request< kege::GraphicsDevice >& )
    {
        Communication::broadcast< const kege::Response< kege::GraphicsDevice >& >({ _renderer->getDevice() });
    }

    bool Application::initialize()
    {
        bool success;

        /**
         * Here i am setting up the virtual directorys aliases and path.
         */
        kege::VirtualDirectory::instance().add( "root", "/Users/kae/Developer/vscode/kege-engine/kege" );
        kege::VirtualDirectory::instance().add( "assets", "/Users/kae/Developer/vscode/kege-engine/kege/assets" );
        kege::VirtualDirectory::instance().add( "config", "/Users/kae/Developer/vscode/kege-engine/kege/assets/config" );
        kege::VirtualDirectory::instance().add( "fonts", "/Users/kae/Developer/vscode/kege-engine/kege/assets/fonts" );
        kege::VirtualDirectory::instance().add( "graphics-shaders", "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/graphics" );
        kege::VirtualDirectory::instance().add( "compute-shaders", "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/compute" );
        kege::VirtualDirectory::instance().add( "snippets", "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/snippets" );

        //-----------------------------------------------------------------------//
        // Create Asset Manager
        //-----------------------------------------------------------------------//
        
        _asset_manager = new kege::AssetManager();
        _asset_manager->addLoader< ref::InputContext, kege::InputContextLoader >( ".json" );
        _asset_manager->addLoader< ref::ShaderPipeline, kege::JsonShaderLoader >( ".json" );

        //-----------------------------------------------------------------------//
        // Create Application Window
        //-----------------------------------------------------------------------//

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

        if ( !kege::GlfwWindow::init() )
        {
            kege::Log::error << "Failed to initialize GlfwWindow."<<Log::nl;
            return false;
        }
        
        _window = new kege::GlfwWindow();
        if ( !_window->create( create_window_info ) )
        {
            kege::Log::error << "Failed to initialize AppWindow."<<Log::nl;
            return false;
        }

        //-----------------------------------------------------------------------//
        // Create Renderer
        //-----------------------------------------------------------------------//

        _renderer = new kege::Renderer();
        if( !_renderer->initialize( _window.ref(), _asset_manager.ref() ) )
        {
            kege::Log::error << "Failed to initialize Renderer."<<Log::nl;
            return false;
        }

        ref::GraphicsDevice graphics = _renderer->getDevice();

        //-----------------------------------------------------------------------//
        // Create Input Context Manager
        //-----------------------------------------------------------------------//

        _input_manager = new kege::InputManager( _window.ref() );
        if( !_input_manager->initialize() )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> InputLayer" << Log::nl;
            return false;
        }

        //-----------------------------------------------------------------------//
        // Create Project Manager
        //-----------------------------------------------------------------------//

        _ecs = new kege::ECS;

        //-----------------------------------------------------------------------//
        // GUI: creation & Initialization
        //-----------------------------------------------------------------------//

        _gui = new kege::GUI( graphics.ref(), _window.ref(), _input_manager.ref(), _asset_manager.ref() );
        kege::Extent2D extent = _renderer->getWindow()->getSize();

        GuiConfig config;
        config.layout_rects[ 0 ] = kege::ui::Rect{0.f, 0.f, float(extent.width), float(extent.height)};
        config.layout_transforms[ 0 ] = kege::mat44(1.0);
        // Capacity --- --- --- --- --- --- --- ---
        config.max_layers_per_layout = 8;
        config.max_widgets = 10000;
        config.max_layouts = 1;
        config.target_extent = extent;
        config.font_size = 16;
        config.icon_theme_path = "";
        config.font_path = "fonts/monaco.tga";
        config.pipeline_filename = "graphics-shaders/gui/gui-rounded-corner-sdf-text.json";
        // Performance --- --- --- --- --- --- --- ---
        config.enable_batching = true;
        config.vertex_buffer_size_mb = 4;
        // Editor specific (ignored by Runtime)
        config.enable_debug_overlays = false;
        config.enable_imgui_style_panels = false;

        if( !_gui->initialize(config) )
        {
            return false;
        }

        //-----------------------------------------------------------------------//
        // ProjectManager
        //-----------------------------------------------------------------------//

        /**
         * Next step, create our project manager
         */
        _project_manager = new ProjectManager( _ecs );

        //-----------------------------------------------------------------------//
        // AppLayerStack
        //-----------------------------------------------------------------------//

        _app_layer_stack = new kege::AppLayerStack();

        //-----------------------------------------------------------------------//
        // Create and Add Application Layers
        //-----------------------------------------------------------------------//

        success = _app_layer_stack->push( new kege::RenderLayer( _renderer.ref(), _project_manager.ref(), _gui.ref() ) );
        if( !success ) return false;

        success = _app_layer_stack->push( new kege::ECSLayer( _ecs, _asset_manager, _project_manager ) );
        if( !success ) return false;

        _running = success;
        return _running;
    }

    void Application::shutdown()
    {
        _app_layer_stack.clear();
        _gui.clear();
        _project_manager.clear();
        _input_manager.clear();
        _asset_manager.clear();
        _renderer.clear();
        _window.clear();
        kege::GlfwWindow::terminate();
    }

    void Application::run()
    {
        initialize();
        while ( _running && !_window->shouldClose() )
        {
            _input_manager->beginInput();
            if( !_app_layer_stack->update() )
            {
                _running = false;
            }
            if( !_app_layer_stack->render() )
            {
                _running = false;
            }
            _input_manager->endInput();
        }
        shutdown();
    }

    Application::Application()
    :   _running( false )
    {
        Communication::add< kege::Request< kege::ProjectManager >&, kege::Application >( this );
        Communication::add< kege::Request< kege::GraphicsDevice >&, kege::Application >( this );

        Communication::add< kege::CallbackRequest< kege::ProjectManager >& >( this );
        Communication::add< kege::CallbackRequest< kege::GraphicsDevice >& >( this );
    }

    Application:: ~Application()
    {
        shutdown();
    }
}
