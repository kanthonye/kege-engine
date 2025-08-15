//
//  graphics-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#include "../../core/engine/engine.hpp"
//#include "shader-pipeline-library.hpp"
#include "graphics-system.hpp"

namespace kege{

    GraphicsSystem::GraphicsSystem( kege::Engine* engine )
    :   System( engine, "graphics-system", 0 )
    {}

    bool GraphicsSystem::initialize()
    {
        kege::WindowCreateInfo create_window_info = {};
        create_window_info.title = "KEGE";
        create_window_info.width = 1024;
        create_window_info.height = 512;
        create_window_info.visible = true;
        create_window_info.resizable = true;
        create_window_info.fullscreen = false;
        create_window_info.maximized = false;
        create_window_info.decorated = true;
        create_window_info.vsync = false;
        
        kege::Ref< kege::GraphicsWindow > window = new kege::GlfwWindow();
        if ( !window->create( create_window_info ) )
        {
            KEGE_LOG_ERROR << "Failed to initialize GraphicsWindow."<<Log::nl;
            return false;
        }
        System::setWindow( window );

        kege::DeviceInitializationInfo device_init_info = {};
        device_init_info.window = window.ref();
        device_init_info.preferred_API = kege::GraphicsAPI::Vulkan;
        device_init_info.enable_raytracing = false;
        device_init_info.prefer_discrete_gpu = true;
        device_init_info.prefer_higher_api_version = true;
        device_init_info.require_shader_float64 = false;
        device_init_info.engine = "KEGE";
        device_init_info.name = "dev";

        kege::SwapchainDesc swapchain_create_info = {};
        swapchain_create_info.image_count = kege::MAX_FRAMES_IN_FLIGHT + 1;
        swapchain_create_info.debug_name = "swapchain";
        swapchain_create_info.width = window->getWidth();
        swapchain_create_info.height = window->getHeight();
        swapchain_create_info.color_format = kege::Format::bgra_u8_norm;
        swapchain_create_info.depth_format = kege::Format::depth_32;
        swapchain_create_info.present_mode = kege::PresentMode::Fifo;
        swapchain_create_info.present_queue_type = kege::QueueType::Graphics;
        swapchain_create_info.image_usage = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::CopyDst;

        kege::Ref< kege::Graphics > graphics = new kege::Graphics();
        if( !graphics->initalize( window, device_init_info, swapchain_create_info ) )
        {
            KEGE_LOG_ERROR << "Failed to initialize Graphics." <<Log::nl;
            return false;
        }
        System::setGraphics( graphics );

        string shader_file = _engine->getVirtualDirectory().fetch( "graphics-shaders/hello-triangle/shader.json" );
        if( !graphics->getShaderPipelineManager()->load( "triangle-shader", shader_file.c_str() ) )
        {
            return false;
        }
        shader_file = _engine->getVirtualDirectory().fetch( "graphics-shaders/copy/copy-color-depth.json" );
        if( !graphics->getShaderPipelineManager()->load( "copy-shader", shader_file.c_str() ) )
        {
            return false;
        }
        shader_file = _engine->getVirtualDirectory().fetch( "graphics-shaders/basic/shader.json" );
        if( !graphics->getShaderPipelineManager()->load( "basic-shader", shader_file.c_str() ) )
        {
            return false;
        }

        return true;
    }

    void GraphicsSystem::shutdown()
    {
        if ( _graphics )
        {
            _graphics->shutdown();
            _graphics.clear();
        }

        if ( _window )
        {
            _window->destroy();
            _window.clear();
        }
        System::shutdown();
    }

    KEGE_REGISTER_SYSTEM( GraphicsSystem, "graphics" );
}
