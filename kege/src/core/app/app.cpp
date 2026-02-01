//
//  application.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#include "app.hpp"
#include "glfw-window.hpp"

#include "../resource/image-loader.hpp"
#include "../resource/input-context-loader.hpp"
#include "../resource/shader-pipeline-loader.hpp"

namespace kege{

    void Application::operator()( kege::CallbackRequest< kege::ProjectManager >& request )
    {
        request.callback( _project_manager.ref() );
    }

    void Application::operator()( kege::CallbackRequest< kege::Graphics >& request )
    {
        request.callback( _graphics.ref() );
    }

    void Application::operator()( const kege::Request< kege::ProjectManager >& )
    {
        Communication::broadcast< const kege::Response< kege::ProjectManager* > >({ _project_manager.ref() });
    }

    void Application::operator()( const kege::Request< kege::Graphics >& )
    {
        Communication::broadcast< const kege::Response< kege::Graphics* >& >({ _graphics.ref() });
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
        kege::VirtualDirectory::instance().add( "graphics-shaders", "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/graphics" );
        kege::VirtualDirectory::instance().add( "compute-shaders", "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/compute" );

        //-----------------------------------------------------------------------//
        // Create Asset Manager
        //-----------------------------------------------------------------------//

        _asset_manager = new kege::AssetManager();

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

        _window = new kege::GlfwWindow();
        if ( !_window->create( create_window_info ) )
        {
            kege::Log::error << "Failed to initialize AppWindow."<<Log::nl;
            return false;
        }

        //-----------------------------------------------------------------------//
        // Create Graphics
        //-----------------------------------------------------------------------//

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
        //device_init_info.enable_debug_general = true;
        //device_init_info.enable_debug_performance = true;

        Extent2D window_size = _window->getSize();
        kege::SwapchainDesc swapchain_create_info = {};
        swapchain_create_info.image_count = kege::MAX_FRAMES_IN_FLIGHT + 1;
        swapchain_create_info.name = "swapchain";
        swapchain_create_info.width = window_size.width;
        swapchain_create_info.height = window_size.height;
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
        // Add Asset Loaders
        //-----------------------------------------------------------------------//

        _asset_manager->addLoader< ref::ShaderPipeline, kege::ShaderPipelineLoader >( ".json" );
        _asset_manager->addLoader< ref::InputContext, kege::InputContextLoader >( ".json" );
        _asset_manager->addLoader< ref::Image, kege::ImageLoader >( ".jpg" );
        _asset_manager->addLoader< ref::Image, kege::ImageLoader >( ".png" );

        //-----------------------------------------------------------------------//
        // Create RenderGraph
        //-----------------------------------------------------------------------//

        _render_graph = new kege::RenderGraph( _graphics.ref(), _asset_manager.ref() );
        if( !_render_graph->load( vfs("config/render-graph.json").str() ) ) return false;
        if( !_render_graph->compile() ) return false;

        //-----------------------------------------------------------------------//
        // Create Default Resources add them to AssetManager so they can be accessed
        //-----------------------------------------------------------------------//

        uint32_t color[] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
        ref::Image default_image = _graphics->createImage
        ({
            .debug_name = "default-image",
            .extent  = {2,2,1},
            .array_layers = 1,
            .mip_levels = 1,
            .memory_usage = MemoryUsage::GpuOnly,
            .samples = SampleCount::Count1,
            .format = Format::rgba_u8_norm,
            .data = &color,
            .usage = ImageUsage::Color | ImageUsage::Sampled
        });
        _asset_manager->add< ref::Image >( "default", default_image );

        ref::Sampler default_sampler = _graphics->createSampler
        ({
            .name = "default-sampler",
            .address_mode_u = AddressMode::ClampToEdge,
            .address_mode_v = AddressMode::ClampToEdge,
            .address_mode_w = AddressMode::ClampToEdge,
            .min_filter = Filter::Linear,
            .mag_filter = Filter::Linear,
        });
        _asset_manager->add< ref::Sampler >( "default", default_sampler );

        // fallback shader
        kege::string shader_file = kege::vfs( "graphics-shaders/error/error.json" );
        uint64_t error_shader_handle = _asset_manager->load< ref::ShaderPipeline >( "error-shader", shader_file.c_str() );
        if( error_shader_handle == 0 )
        {
            kege::Log::error << "LOAD_FAILED -> _asset_manager->load< ref::ShaderPipeline >("<< shader_file.c_str() <<")" << Log::nl;
            return false;
        }

        // load shader library
        _asset_manager->setLibrary< ref::ShaderPipeline >( new kege::ShaderLibrary( _graphics.ref(), error_shader_handle ) );
        if( !_asset_manager->loadLibrary<ref::ShaderPipeline>( kege::vfs( "graphics-shaders/shader-library.json" ).c_str() ) )
        {
            kege::Log::error << "LOAD_FAILED -> _asset_manager->loadLibrary< ref::ShaderPipeline >(...)" << Log::nl;
            return false;
        }

        //-----------------------------------------------------------------------//
        // Create Project Manager
        //-----------------------------------------------------------------------//

        _ecs = new kege::ECS;
        
        /**
         * Next step, create our project manager
         */
        _project_manager = new ProjectManager( _graphics, _input_manager.ref(), _ecs, _render_graph );

        //-----------------------------------------------------------------------//
        // AppLayerStack
        //-----------------------------------------------------------------------//

        _app_layer_stack = new kege::AppLayerStack();

        //-----------------------------------------------------------------------//
        // Create and Add Application Layers
        //-----------------------------------------------------------------------//

        success = _app_layer_stack->push( new kege::RenderLayer( _asset_manager.ref(), _render_graph, _project_manager.ref() ) );
        if( !success ) return false;

        success = _app_layer_stack->push( new kege::ECSLayer( _ecs, _asset_manager, _render_graph, _project_manager ) );
        if( !success ) return false;

        _running = success;
        return _running;
    }

    void Application::shutdown()
    {
        _app_layer_stack.clear();
        _render_graph.clear();
        _project_manager.clear();
        _asset_manager.clear();
        _input_manager.clear();
        _graphics.clear();
        _window.clear();
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
            _input_manager->endInput();

            //_app_layer_stack->render()
        }
        shutdown();
    }

    Application::Application()
    :   _running( false )
    {
        Communication::add< kege::Request< kege::ProjectManager >&, kege::Application >( this );
        Communication::add< kege::Request< kege::Graphics >&, kege::Application >( this );

        Communication::add< CallbackRequest< kege::ProjectManager >& >( this );
        Communication::add< CallbackRequest< kege::Graphics >& >( this );
    }

    Application:: ~Application()
    {
        shutdown();
    }
}
