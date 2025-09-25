//
//  render-graph-system.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/19/25.
//

#include "engine.hpp"
#include "core-render-graph.hpp"

namespace kege{

    CoreRenderGraph::CoreRenderGraph( kege::Engine* engine )
    :   Module( engine, "RenderGraph" )
    {}
    
    bool CoreRenderGraph::initialize( )
    {
        if ( _module != nullptr )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> RenderGraph" << Log::nl;
            return false;
        }

        kege::string shader_file = kege::vfs( "graphics-shaders/copy/copy-color-depth.json" );
        if( !_engine->graphics()->getShaderPipelineManager()->load( shader_file.c_str() ) )
        {
            kege::Log::error << "( LOADING_FAILED ) -> graphics-shaders/copy/copy-color-depth.json" << Log::nl;
            return false;
        }

        // Create the render graph with the graphics system
        // Use the graphics system to get the swapchain and other graphics resources
        // This will allow the render graph to manage rendering operations
        // across multiple frames and handle resources efficiently.
        uint32_t frames_in_flight = 2;
        kege::Graphics* graphics = _engine->graphics().get();
        _module = new kege::RenderGraph( graphics );

        // ------- Setup Image Sampler Resources -------

        _module->defnSampler
        ({
            .name =  "sampler-nearest-norep",
            .desc = kege::SamplerDesc
            {
                .mag_filter = kege::Filter::Nearest,
                .min_filter = kege::Filter::Nearest,
                .mipmap_mode = kege::MipmapMode::Nearest,
                .address_mode_u = kege::AddressMode::ClampToEdge,
                .address_mode_v = kege::AddressMode::ClampToEdge,
                .address_mode_w = kege::AddressMode::ClampToEdge,
            }
        });

        _module->defnSampler
        ({
            .name =  "sampler-nearest-rep",
            .desc = kege::SamplerDesc
            {
                .mag_filter = kege::Filter::Nearest,
                .min_filter = kege::Filter::Nearest,
                .mipmap_mode = kege::MipmapMode::Nearest,
                .address_mode_u = kege::AddressMode::Repeat,
                .address_mode_v = kege::AddressMode::Repeat,
                .address_mode_w = kege::AddressMode::Repeat,
            }
        });

        _module->defnSampler
        ({
            .name =  "sampler-linear-rep",
            .desc = kege::SamplerDesc
            {
                .mag_filter = kege::Filter::Linear,
                .min_filter = kege::Filter::Linear,
                .mipmap_mode = kege::MipmapMode::Nearest,
                .address_mode_u = kege::AddressMode::Repeat,
                .address_mode_v = kege::AddressMode::Repeat,
                .address_mode_w = kege::AddressMode::Repeat,
            }
        });

        _module->defnSampler
        ({
            .name =  "sampler-linear-norep",
            .desc = kege::SamplerDesc
            {
                .mag_filter = kege::Filter::Linear,
                .min_filter = kege::Filter::Linear,
                .mipmap_mode = kege::MipmapMode::Nearest,
                .address_mode_u = kege::AddressMode::ClampToEdge,
                .address_mode_v = kege::AddressMode::ClampToEdge,
                .address_mode_w = kege::AddressMode::ClampToEdge,
            }
        });

        // ------- Setup Swapchain Render Targets -------

        _module->defnImage
        ({
            .name = "swapchain_color_output",
            .frames_in_flight = graphics->getSwapchainImageCount(),
            .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
            {
                .width  = graphics->getSwapchainExtent().width,
                .height = graphics->getSwapchainExtent().height,
                .depth  = 1,
                .format = graphics->getSwapchainColorFormat(),
                .type = kege::ImageType::Type2D
            },
            .physical_handle = graphics->getSwapchainColorImages(),
        });
        _module->defnImage
        ({
            .name = "swapchain_depth_output",
            .frames_in_flight = graphics->getSwapchainImageCount(),
            .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
            {
                .width  = graphics->getSwapchainExtent().width,
                .height = graphics->getSwapchainExtent().height,
                .depth  = 1,
                .format = graphics->getSwapchainDepthFormat(),
                .type = kege::ImageType::Type2D
            },
            .physical_handle = graphics->getSwapchainDepthImages(),
        });

        // ------- Scene Render Targets -------

        _module->defnImage
        ({
            .name = "scene_color",
            .frames_in_flight = frames_in_flight,
            .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
            {
                .width  = 1024,
                .height = 640,
                .depth  = 1,
                .format = graphics->getSwapchainColorFormat(),
                .type = kege::ImageType::Type2D
            },
        });
        _module->defnImage
        ({
            .name = "scene_depth",
            .frames_in_flight = frames_in_flight,
            .usages = kege::ImageUsageFlags::DepthStencilAttachment | kege::ImageUsageFlags::ShaderResource,
            {
                .width  = 1024,
                .height = 640,
                .depth  = 1,
                .format = kege::Format::depth_32,
                .type = kege::ImageType::Type2D
            },
        });


        _module->defnShaderResource
        ({
            RgShaderResrcDefn
            {
                .name = "scene-graphics",
                .frames_in_flight = 1,
                .bindings =
                {
                    kege::RgShaderResrcDesc
                    {
                        .count = 1,
                        .binding = 0,
                        .stages = ShaderStage::All,
                        .type = kege::DescriptorType::CombinedImageSampler,
                        .targets =
                        {
                            { .type = kege::RgResrcType::Image, .name = "scene_color", .sampler = "sampler-nearest-norep" }
                        }
                    },
                    kege::RgShaderResrcDesc
                    {
                        .count = 1,
                        .binding = 1,
                        .stages = ShaderStage::All,
                        .type = kege::DescriptorType::CombinedImageSampler,
                        .targets =
                        {
                            { .type = kege::RgResrcType::Image, .name = "scene_depth", .sampler = "sampler-nearest-norep" }
                        }
                    },
                }
            }
        });



        _module->defnBuffer
        ({
            .name = "camera-buffer",
            .frames_in_flight = frames_in_flight,
            .info = BufferDesc
            {
                .name = "camera-buffer",
                .size =  2 * sizeof( mat44 ) + sizeof( vec4 ),
                .memory_usage = MemoryUsage::CpuToGpu,
                .usage = BufferUsage::UniformBuffer,
                .data = nullptr,
            },
        });
        _module->defnShaderResource
        ({
            RgShaderResrcDefn
            {
                .name = "camera-buffer",
                .set_index = 0,
                .frames_in_flight = frames_in_flight,
                .bindings =
                {
                    kege::RgShaderResrcDesc
                    {
                        .count = 1,
                        .binding = 0,
                        .stages = ShaderStage::All,
                        .type = kege::DescriptorType::UniformBuffer,
                        .targets = {{ .type = kege::RgResrcType::Buffer, .name = "camera-buffer" }}
                    },
                }
            }
        });

        _module->defnBuffer
        ({
            .name = "light-buffer",
            .frames_in_flight = frames_in_flight,
            .info = BufferDesc
            {
                .name = "light-buffer",
                .size =  100 * sizeof( Light ),
                .memory_usage = MemoryUsage::CpuToGpu,
                .usage = BufferUsage::UniformBuffer,
                .data = nullptr,
            },
        });
        _module->defnShaderResource
        ({
            RgShaderResrcDefn
            {
                .name = "light-buffer",
                .set_index = 1,
                .frames_in_flight = frames_in_flight,
                .bindings =
                {
                    kege::RgShaderResrcDesc
                    {
                        .count = 1,
                        .binding = 0,
                        .stages = ShaderStage::All,
                        .type = kege::DescriptorType::UniformBuffer,
                        .targets = {{ .type = kege::RgResrcType::Buffer, .name = "light-buffer" }}
                    },
                }
            }
        });






        _module->defnShaderResource
        ({
            RgShaderResrcDefn
            {
                .name = "scene-color",
                .frames_in_flight = 1,
                .bindings =
                {
                    kege::RgShaderResrcDesc
                    {
                        .count = 1,
                        .binding = 0,
                        .stages = ShaderStage::Fragment,
                        .type = kege::DescriptorType::CombinedImageSampler,
                        .targets = {{ .type = kege::RgResrcType::Image, .name = "scene_color" }}
                    },
                }
            }
        });

        // ------- Add Render Passes -------
        _module->addPass
        ({
            .type = QueueType::Graphics,
            .pass = RenderPassType::UI,
            .name = "final-pass",
            .reads =
            {
                kege::RgReadResrcDesc
                {
                    .name = "scene_color",
                    .type = kege::RgResrcType::Image,
                    .access = kege::AccessFlags::ShaderRead,
                    .stage = kege::PipelineStageFlag::FragmentShader,
                },
                kege::RgReadResrcDesc
                {
                    .name = "scene_depth",
                    .type = kege::RgResrcType::Image,
                    .access = kege::AccessFlags::ShaderRead,
                    .stage = kege::PipelineStageFlag::FragmentShader,
                },
                kege::RgReadResrcDesc
                {
                    .name = "scene-graphics",
                    .type = kege::RgResrcType::ShaderResource,
                    .access = kege::AccessFlags::ShaderRead,
                    .stage = kege::PipelineStageFlag::FragmentShader,
                },
            },
            .writes =
            {
                kege::RgWriteResrcDesc
                {
                    .name = "swapchain_color_output",
                    .type = kege::RgResrcType::Image,
                    .access = kege::AccessFlags::ColorAttachmentWrite,
                    .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                    .clear_value = kege::ClearValue{ .color = { 0.2f, 0.2f, 0.2f, 1.0f } }
                },
                kege::RgWriteResrcDesc
                {
                    .name = "swapchain_depth_output",
                    .type = kege::RgResrcType::Image,
                    .access = kege::AccessFlags::DepthStencilAttachmentWrite,
                    .stage = kege::PipelineStageFlag::FragmentShader,
                    .clear_value = kege::ClearValue{ .depth_stencil = { 1.0f } }
                }
            },
        });


        _module->addPass
        ({
            "scene-output",
            .type = QueueType::Graphics,
            .pass = RenderPassType::Geometry,
            .reads =
            {
                kege::RgReadResrcDesc
                {
                    .name = "camera-buffer",
                    .type = kege::RgResrcType::ShaderResource,
                    .access = kege::AccessFlags::ShaderRead,
                    .stage = kege::PipelineStageFlag::AllGraphics,
                },
            },
            .writes =
            {
                kege::RgWriteResrcDesc
                {
                    .name = "scene_color",
                    .type = kege::RgResrcType::Image,
                    .access = kege::AccessFlags::ColorAttachmentWrite,
                    .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                    .clear_value = kege::ClearValue{ .color = { 0.2f, 0.2f, 0.2f, 1.0f } },
                },
                kege::RgWriteResrcDesc
                {
                    .name = "scene_depth",
                    .type = kege::RgResrcType::Image,
                    .access = kege::AccessFlags::DepthStencilAttachmentWrite,
                    .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                    .clear_value = kege::ClearValue{ .depth_stencil = { 1.0f }},
                }
            }
        });

        if( !_module->compile() )
        {
            return false;
        }

        return true;
    }

    void CoreRenderGraph::shutdown()
    {
        if ( _module )
        {
            _module.clear();
        }
    }

    void CoreRenderGraph::add()
    {
        _engine->addModule( this );
        kege::Log::info << "CoreRenderGraph module added to engine" << Log::nl;
    }

}



namespace kege{

    RenderManagerModule::RenderManagerModule( kege::Engine* engine )
    :   Module( engine, "RenderManagerModule" )
    {}

    bool RenderManagerModule::initialize( )
    {
        if ( _module != nullptr )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> RenderGraph" << Log::nl;
            return false;
        }

        _module = new kege::RenderManager
        ({
            .engine = _engine,
            .graphics = _engine->graphics().get(),
            .graph = _engine->renderGraph().get(),
            .frames_in_flight = MAX_FRAMES_IN_FLIGHT,
        });

        _module->initialize();
        // ------- Setup Image Sampler Resources -------


        return true;
    }

    kege::RenderManager * RenderManagerModule::getModule()
    {
        return _module.ref();
    }

    void RenderManagerModule::shutdown()
    {
        if ( _module )
        {
            _module->shutdown();
            _module.clear();
        }
    }

    void RenderManagerModule::add()
    {
        _engine->addModule( this );
        kege::Log::info << "RenderManagerModule added to engine" << Log::nl;
    }

}
