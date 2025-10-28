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
        uint32_t frames = MAX_FRAMES_IN_FLIGHT;
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
            .frames = graphics->getSwapchain()->getImageCount(),
            .usages = kege::ImageUsage::Color | kege::ImageUsage::Sampled,
            .info = ImageDefn::Info
            {
                .width  = graphics->getSwapchain()->getExtent().width,
                .height = graphics->getSwapchain()->getExtent().height,
                .depth  = 1,
                .format = graphics->getSwapchain()->getColorFormat(),
                .type = kege::ImageType::Type2D
            },
            //TODO: .physical_handle = graphics->getSwapchain()->getColorImages(),
            .use_swapchain_image_index = true,
        });
        _module->defnImage
        ({
            .name = "swapchain_depth_output",
            .frames = graphics->getSwapchain()->getImageCount(),
            .usages = kege::ImageUsage::DepthStencil | kege::ImageUsage::Sampled,
            .info = ImageDefn::Info
            {
                .width  = graphics->getSwapchain()->getExtent().width,
                .height = graphics->getSwapchain()->getExtent().height,
                .depth  = 1,
                .format = graphics->getSwapchain()->getDepthFormat(),
                .type = kege::ImageType::Type2D
            },
            //TODO: .physical_handle = graphics->getSwapchain()->getDepthImages(),
            .use_swapchain_image_index = true,
        });

        // ------- Scene Render Targets -------

        _module->defnImage
        ({
            .name = "scene_color",
            .frames = frames,
            .usages = kege::ImageUsage::Color | kege::ImageUsage::Sampled,
            .info = ImageDefn::Info
            {
                .width  = 1024,
                .height = 640,
                .depth  = 1,
                .format = graphics->getSwapchain()->getColorFormat(),
                .type = kege::ImageType::Type2D
            },
        });
        _module->defnImage
        ({
            .name = "scene_depth",
            .frames = frames,
            .usages = kege::ImageUsage::DepthStencil | kege::ImageUsage::Sampled,
            .info = ImageDefn::Info
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
                .frames = 1,
                .bindings =
                {
                    kege::RgShaderResrcDesc
                    {
                        .count = 1,
                        .binding = 0,
                        .stages = ShaderStageFlag::All,
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
                        .stages = ShaderStageFlag::All,
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
            .frames = frames,
            .info = BufferDesc
            {
                .name = "camera-buffer",
                .size =  2 * sizeof( mat44 ) + sizeof( vec4 ),
                .memory_usage = MemoryUsage::CpuToGpu,
                .usage = BufferUsages::UniformBuffer,
                .data = nullptr,
            },
        });
        _module->defnShaderResource
        ({
            RgShaderResrcDefn
            {
                .name = "camera-buffer",
                .set_index = 0,
                .frames = frames,
                .bindings =
                {
                    kege::RgShaderResrcDesc
                    {
                        .count = 1,
                        .binding = 0,
                        .stages = ShaderStageFlag::All,
                        .type = kege::DescriptorType::UniformBuffer,
                        .targets = {{ .type = kege::RgResrcType::Buffer, .name = "camera-buffer" }}
                    },
                }
            }
        });

        _module->defnBuffer
        ({
            .name = "light-buffer",
            .frames = frames,
            .info = BufferDesc
            {
                .name = "light-buffer",
                .size =  100 * sizeof( Light ),
                .memory_usage = MemoryUsage::CpuToGpu,
                .usage = BufferUsages::UniformBuffer,
                .data = nullptr,
            },
        });
        _module->defnShaderResource
        ({
            RgShaderResrcDefn
            {
                .name = "light-buffer",
                .set_index = 1,
                .frames = frames,
                .bindings =
                {
                    kege::RgShaderResrcDesc
                    {
                        .count = 1,
                        .binding = 0,
                        .stages = ShaderStageFlag::All,
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
                .frames = 1,
                .bindings =
                {
                    kege::RgShaderResrcDesc
                    {
                        .count = 1,
                        .binding = 0,
                        .stages = ShaderStageFlag::Fragment,
                        .type = kege::DescriptorType::CombinedImageSampler,
                        .targets = {{ .type = kege::RgResrcType::Image, .name = "scene_color" }}
                    },
                }
            }
        });

        // ------- Add Render Passes -------

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
                    .type = kege::RgResrcType::ShaderResource
                },
            },
            .writes =
            {
                kege::RgWriteResrcDesc
                {
                    .name = "scene_color",
                    .type = kege::RgResrcType::Image,
                    .usage = RgResrcUsage
                    {
                        .layout = ImageLayout::Color,
                        .access = kege::AccessFlags::ColorWrite,
                        .stage = kege::PipelineStageFlag::ColorOutput,
                        .load_op = kege::AttachmentLoadOp::Clear,
                    },
                    .clear_value = kege::ClearValue{ .color = { 0.2f, 0.2f, 0.2f, 1.0f } },
                },
                kege::RgWriteResrcDesc
                {
                    .name = "scene_depth",
                    .type = kege::RgResrcType::Image,
                    .usage = RgResrcUsage
                    {
                        .layout = ImageLayout::Depth,
                        .access = kege::AccessFlags::DepthStencilWrite,
                        .stage = kege::PipelineStageFlag::EarlyFragmentTests|kege::PipelineStageFlag::LateFragmentTests,
                        .load_op = kege::AttachmentLoadOp::Clear,
                    },
                    .clear_value = kege::ClearValue{ .depth_stencil = { 1.0f }},
                }
            }
        });

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
                    .usage = RgResrcUsage
                    {
                        .layout = ImageLayout::ShaderRead,
                        .access = kege::AccessFlags::ShaderRead,
                        .stage = kege::PipelineStageFlag::FragmentShader,
                    }
                },
                kege::RgReadResrcDesc
                {
                    .name = "scene_depth",
                    .type = kege::RgResrcType::Image,
                    .usage = RgResrcUsage
                    {
                        .layout = ImageLayout::DepthStencilRead,
                        .access = kege::AccessFlags::DepthStencilRead,
                        .stage = kege::PipelineStageFlag::FragmentShader,
                    }
                },
                kege::RgReadResrcDesc
                {
                    .name = "scene-graphics",
                    .type = kege::RgResrcType::ShaderResource,
                },
            },
            .writes =
            {
                kege::RgWriteResrcDesc
                {
                    .name = "swapchain_color_output",
                    .type = kege::RgResrcType::Image,
                    .usage = RgResrcUsage
                    {
                        .layout = ImageLayout::Color,
                        .access = kege::AccessFlags::ColorWrite,
                        .stage = kege::PipelineStageFlag::ColorOutput,
                        .load_op = kege::AttachmentLoadOp::Clear,
                    },
                    .clear_value = kege::ClearValue{ .color = { 0.2f, 0.2f, 0.2f, 1.0f } }
                },
                kege::RgWriteResrcDesc
                {
                    .name = "swapchain_depth_output",
                    .type = kege::RgResrcType::Image,
                    .usage = RgResrcUsage
                    {
                        .layout = ImageLayout::Depth,
                        .access = kege::AccessFlags::DepthStencilWrite,
                        .stage = kege::PipelineStageFlag::EarlyFragmentTests|kege::PipelineStageFlag::LateFragmentTests,
                        .load_op = kege::AttachmentLoadOp::Clear,
                    },
                    .clear_value = kege::ClearValue{ .depth_stencil = { 1.0f } }
                }
            },
        });

        _module->addPass
        ({
            "present-stage",
            .type = QueueType::Graphics,
            .pass = RenderPassType::BarrierTransition,
            .reads =
            {
                kege::RgReadResrcDesc
                {
                    .name = "swapchain_color_output",
                    .type = kege::RgResrcType::Image,
                    .usage = RgResrcUsage
                    {
                        .layout = ImageLayout::Present,
                        .access = kege::AccessFlags::ColorWrite,
                        .stage  = kege::PipelineStageFlag::ColorOutput,
                    },
                },
                kege::RgReadResrcDesc
                {
                    .name = "swapchain_depth_output",
                    .type = kege::RgResrcType::Image,
                    .usage = RgResrcUsage
                    {
                        .layout = ImageLayout::DepthStencilRead,
                        .access = kege::AccessFlags::DepthStencilRead,
                        .stage  = kege::PipelineStageFlag::FragmentShader,
                    },
                }
            },
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



//namespace kege{
//
//    RenderExecutorModule::RenderExecutorModule( kege::Engine* engine )
//    :   Module( engine, "RenderExecutorModule" )
//    {}
//
//    bool RenderExecutorModule::initialize( )
//    {
//        if ( _module != nullptr )
//        {
//            kege::Log::error << "( INITIALIZATION_FAILED ) -> RenderGraph" << Log::nl;
//            return false;
//        }
//
//        _module = new kege::RenderExecutor
//        (_engine->renderGraph().get(), MAX_FRAMES_IN_FLIGHT);
//
//        _module->initialize();
//        // ------- Setup Image Sampler Resources -------
//
//
//        return true;
//    }
//
//    kege::RenderExecutor * RenderExecutorModule::getModule()
//    {
//        return _module.ref();
//    }
//
//    void RenderExecutorModule::shutdown()
//    {
//        if ( _module )
//        {
//            _module->shutdown();
//            _module.clear();
//        }
//    }
//
//    void RenderExecutorModule::add()
//    {
//        _engine->addModule( this );
//        kege::Log::info << "RenderExecutorModule added to engine" << Log::nl;
//    }
//
//}
