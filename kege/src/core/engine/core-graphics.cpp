//
//  graphics-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#include "engine.hpp"
#include "core-graphics.hpp"

namespace kege{


    GraphicsModule::GraphicsModule( kege::Engine* engine )
    :   Module( engine, "GraphicsModule" )
    {}

    bool GraphicsModule::initialize()
    {
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

        kege::Ref< kege::GraphicsWindow > window = new kege::GlfwWindow();
        if ( !window->create( create_window_info ) )
        {
            KEGE_LOG_ERROR << "Failed to initialize GraphicsWindow."<<Log::nl;
            return false;
        }
        //System::setWindow( window );

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

        _module = new kege::Graphics();
        if( !_module->initalize( window, device_init_info, swapchain_create_info ) )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> Graphics" << Log::nl;
            return false;
        }
        //System::setGraphics( graphics );


        // shader_file = _engine->vfs()->fetch( "graphics-shaders/copy/copy-color-depth.json" );
        // if( !_module->getShaderPipelineManager()->load( "copy-shader", shader_file.c_str() ) )
        // {
        //     return false;
        // }
        // shader_file = _engine->vfs()->fetch( "graphics-shaders/basic/shader.json" );
        // if( !_module->getShaderPipelineManager()->load( "basic-shader", shader_file.c_str() ) )
        // {
        //     return false;
        // }

        return true;
    }

    kege::Graphics* GraphicsModule::get()
    {
        return _module.ref();
    }

    void GraphicsModule::shutdown()
    {
        if ( _module )
        {
            _module->shutdown();
            _module.clear();
        }
    }

    void GraphicsModule::add()
    {
        _engine->addModule( this );
        kege::Log::info << "GraphicsModule module added to engine" << Log::nl;
    }

}
