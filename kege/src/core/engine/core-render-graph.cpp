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
//
//    PipelineHandle CoreRenderGraph::loadPipeline( const std::string& name, const std::string& filename )
//    {
//        kege::ShaderPipelineManager* pipeline_manager = _engine->graphics()->getShaderPipelineManager();
//        pipeline_manager->set( "copy-shader", pipeline );
//    }

    void addFinalRenderPass( kege::RenderGraph* graph )
    {
        // ------- Deferred Geometry Render Targets -------

        // ------- Deferred Lighting Render Targets -------

        // ------- Deferred Geometry Render Pass -------

        // ------- Deferred Lighting Render Pass -------

        // ------- Final Render Pass -------

    }
    
    bool CoreRenderGraph::initialize( )
    {
        if ( _module != nullptr )
        {
            kege::Log::error << "CoreRenderGraph already initialized!" << Log::nl;
            return false;
        }
        kege::Log::info << "initializing -> RenderGraph" << Log::nl;


        kege::string shader_file = _engine->vfs()->fetch( "graphics-shaders/copy/copy-color-depth.json" );
        if( !_engine->graphics()->getShaderPipelineManager()->load( "copy-shader", shader_file.c_str() ) )
        {
            KEGE_LOG_ERROR << "Failed to load pipeline -> " << shader_file << Log::nl;
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

        kege::SamplerDesc desc = {};
        desc.mag_filter = kege::Filter::Nearest;
        desc.min_filter = kege::Filter::Nearest;
        desc.mipmap_mode = kege::MipmapMode::Nearest;
        desc.address_mode_u = kege::AddressMode::ClampToEdge;
        desc.address_mode_v = kege::AddressMode::ClampToEdge;
        desc.address_mode_w = kege::AddressMode::ClampToEdge;
        _module->defineSampler( "sampler-nearest-norep", desc );

        desc.mag_filter = kege::Filter::Nearest;
        desc.min_filter = kege::Filter::Nearest;
        desc.mipmap_mode = kege::MipmapMode::Nearest;
        desc.address_mode_u = kege::AddressMode::Repeat;
        desc.address_mode_v = kege::AddressMode::Repeat;
        desc.address_mode_w = kege::AddressMode::Repeat;
        _module->defineSampler( "sampler-nearest-rep", desc );

        desc.mag_filter = kege::Filter::Linear;
        desc.min_filter = kege::Filter::Linear;
        desc.mipmap_mode = kege::MipmapMode::Linear;
        desc.address_mode_u = kege::AddressMode::ClampToEdge;
        desc.address_mode_v = kege::AddressMode::ClampToEdge;
        desc.address_mode_w = kege::AddressMode::ClampToEdge;
        _module->defineSampler( "sampler-linear-norep", desc );

        desc.mag_filter = kege::Filter::Linear;
        desc.min_filter = kege::Filter::Linear;
        desc.mipmap_mode = kege::MipmapMode::Linear;
        desc.address_mode_u = kege::AddressMode::Repeat;
        desc.address_mode_v = kege::AddressMode::Repeat;
        desc.address_mode_w = kege::AddressMode::Repeat;
        _module->defineSampler( "sampler-linear-rep", desc );

        // ------- Setup Swapchain Render Targets -------

        _module->defineImage
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
        _module->defineImage
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

        _module->defineImage
        ({
            .name = "scene_color",
            .frames_in_flight = frames_in_flight,
            .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
            {
                .width  = graphics->getSwapchainExtent().width,
                .height = graphics->getSwapchainExtent().height,
                .depth  = 1,
                .format = graphics->getSwapchainColorFormat(),
                .type = kege::ImageType::Type2D
            },
        });
        _module->defineImage
        ({
            .name = "scene_depth",
            .frames_in_flight = frames_in_flight,
            .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
            {
                .width  = graphics->getSwapchainExtent().width,
                .height = graphics->getSwapchainExtent().height,
                .depth  = 1,
                .format = kege::Format::depth_32,
                .type = kege::ImageType::Type2D
            },
        });

        // ------- Define Camera Buffer Resource -------

        uint32_t buffer_size = 2 * sizeof( kege::mat44 ) + sizeof( kege::vec4 );
        RgResrcHandle camera_buffer_resource = _module->defineBuffer
        ({
            .frames_in_flight = frames_in_flight,
            .name = "camera-buffer",
            .info =
            {
                .size = buffer_size,
                .data = nullptr,
                .usage = BufferUsage::UniformBuffer,
                .memory_usage = MemoryUsage::CpuToGpu,
            }
        });

        // ------- Define Camera Shader Resource -------

        kege::RgResrcHandle camera_descriptor_resource = _module->defineShaderResource
        (
            "camera-descriptor",
            frames_in_flight,
            {
                kege::DescriptorSetLayoutBinding
                {
                    .name = "CameraBlock",
                    .binding = 0,
                    .count = 1,
                    .descriptor_type = kege::DescriptorType::UniformBuffer,
                    .stage_flags = kege::ShaderStage::Vertex
                }
            }
        );
        _module->updateShaderResource
        (
            camera_descriptor_resource,
            std::vector< RgShaderResource >
            {
                std::vector< kege::RgBufferInfo >
                {
                    kege::RgBufferInfo
                    {
                        .offset = 0,
                        .range = buffer_size,
                        .buffer = camera_buffer_resource,
                    }
                }
            }
        );

        // ------- Add Render Passes -------

        _module->add
        ({
            .add_resources = []( kege::RenderGraph* graph )
            {
                uint32_t frames_in_flight = 2;

                kege::RgResrcHandle shader_resource = graph->defineShaderResource
                (
                    "scene-frame-descriptor", frames_in_flight,
                    {
                        kege::DescriptorSetLayoutBinding
                        {
                            .name = "src_color",
                            .binding = 0,
                            .count = 1,
                            .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                            .stage_flags = kege::ShaderStage::Fragment
                        },
                        kege::DescriptorSetLayoutBinding
                        {
                            .name = "src_depth",
                            .binding = 1,
                            .count = 1,
                            .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                            .stage_flags = kege::ShaderStage::Fragment
                        }
                    }
                );
                graph->updateShaderResource
                (
                    shader_resource,
                    {
                        kege::RgShaderResource
                        {{
                            kege::RgImageInfo
                            {
                                graph->getImageRgResrc( "scene_color" ),
                                graph->getSamplerRgResrc( "sampler-nearest-norep" )
                            }
                        }},
                        kege::RgShaderResource
                        {{
                            kege::RgImageInfo
                            {
                                graph->getImageRgResrc( "scene_depth" ),
                                graph->getSamplerRgResrc( "sampler-nearest-norep" )
                            }
                        }},
                    }
                );
            },

            .add_passes = []( kege::RenderGraph* graph )
            {
                graph->addGraphicsPass
                ({
                    .name = "final-pass",
                    .reads =
                    {
                        kege::RgReadResrcDesc
                        {
                            .name = "scene_color",
                            .type = kege::RgResrcType::Image,
                            .access = kege::AccessFlags::ShaderRead,
                            .stage = kege::PipelineStageFlag::FragmentShader,
                            //.handle = graph->getImageIndex( "scene_color" )
                        },
                        kege::RgReadResrcDesc
                        {
                            .name = "scene_depth",
                            .type = kege::RgResrcType::Image,
                            .access = kege::AccessFlags::ShaderRead,
                            .stage = kege::PipelineStageFlag::FragmentShader,
                            //.handle = graph->getImageIndex( "scene_depth" )
                        }
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
                    .execute = [ graph ]( kege::RenderPassContext* context )
                    {
                        Communication::broadcast< kege::RenderPassContext* >( context );

                        kege::ShaderPipelineManager* pipelines = graph->getGraphics()->getShaderPipelineManager();
                        kege::PipelineHandle pipeline = pipelines->get( "copy-shader" );
                        if ( !pipeline )
                        {
                            KEGE_LOG_ERROR << "copy-shader not found." <<Log::nl;
                            return;
                        }

                        kege::DescriptorSetHandle descriptor_set = context->getPhysicalDescriptorSet( "scene-frame-descriptor" );
                        if ( !descriptor_set )
                        {
                            KEGE_LOG_ERROR << "descriptor_set not found." <<Log::nl;
                            return;
                        }

                        kege::CommandEncoder* encoder = context->getCommandEncoder();
                        encoder->bindGraphicsPipeline( pipeline );
                        encoder->bindDescriptorSets( descriptor_set );
                        encoder->draw( 4, 1, 0, 0 );
                    }
                });

                graph->addGraphicsPass
                ({
                    "scene-output",
                    .reads =
                    {
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
                    },
                    .execute = []( kege::RenderPassContext* context )
                    {
                        Communication::broadcast< kege::RenderPassContext* >( context );
                    }
                });
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

//    KEGE_REGISTER_SYSTEM( RenderGraphSystem, "render-graph" );
}
