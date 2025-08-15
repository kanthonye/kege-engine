//
//  deferred-render-graph-setup.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/26/25.
//

#include "../../graphics/graph/deferred-render-graph-setup.hpp"

#include "communication.hpp"


void addDeferredGeometryRenderPass( uint32_t width, uint32_t height, uint32_t frames_in_flight, kege::RenderGraph* graph )
{
    kege::Graphics* graphics = graph->getGraphics();

    kege::RgResrcHandle swapchain_color_output = graph->defineImage
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
    kege::RgResrcHandle swapchain_depth_output = graph->defineImage
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

    kege::RgResrcHandle gbuffer_position = graph->defineImage
    ({
        .name = "gbuffer_position",
        .frames_in_flight = frames_in_flight,
        .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
        {
            .width  = width,
            .height = height,
            .depth  = 1,
            .format = kege::Format::rgba_f32,
            .type   = kege::ImageType::Type2D
        }
    });
    kege::RgResrcHandle gbuffer_normal = graph->defineImage
    ({
        .name = "gbuffer_normal",
        .frames_in_flight = frames_in_flight,
        .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
        {
            .width  = width,
            .height = height,
            .depth  = 1,
            .format = kege::Format::rgba_f16,
            .type   = kege::ImageType::Type2D
        }
    });
    kege::RgResrcHandle gbuffer_albedo = graph->defineImage
    ({
        .name = "gbuffer_albedo",
        .frames_in_flight = frames_in_flight,
        .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
        {
            .width  = width,
            .height = height,
            .depth  = 1,
            .format = kege::Format::rgba_u8_norm,
            .type   = kege::ImageType::Type2D
        }
    });
    kege::RgResrcHandle gbuffer_material = graph->defineImage
    ({
        .name = "gbuffer_material",
        .frames_in_flight = frames_in_flight,
        .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
        {
            .width  = width,
            .height = height,
            .depth  = 1,
            .format = kege::Format::rgba_u8_norm,
            .type   = kege::ImageType::Type2D
        }
    });
    kege::RgResrcHandle scene_depth = graph->defineImage
    ({
        .name = "scene_depth",
        .frames_in_flight = frames_in_flight,
        .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
        {
            .width  = width,
            .height = height,
            .depth  = 1,
            .format = kege::Format::depth_32,
            .type   = kege::ImageType::Type2D
        }
    });
    kege::RgResrcHandle lighting_output = graph->defineImage // HDR lighting output
    ({
        .name = "lighting_output",
        .frames_in_flight = frames_in_flight,
        .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
        {
            .width  = width,
            .height = height,
            .depth  = 1,
            .format = kege::Format::rgba_f16,
            .type   = kege::ImageType::Type2D
        }
    });

    kege::RgResrcHandle camera_buffer = graph->defineBuffer
    ({
        .name = "camera_buffer",
        .frames_in_flight = frames_in_flight,
        {
            .size = sizeof(int),
            .data = nullptr,
            .usage = kege::BufferUsage::UniformBuffer | kege::BufferUsage::CopyDst,
            .memory_usage = kege::MemoryUsage::CpuToGpu,
            .debug_name = "camera_buffer"
        }
    });
    kege::RgResrcHandle lighting_buffer = graph->defineBuffer
    ({
        .name = "lighting_buffer",
        .frames_in_flight = frames_in_flight,
        {
            .size = sizeof(int),
            .data = nullptr,
            .usage = kege::BufferUsage::UniformBuffer | kege::BufferUsage::CopyDst,
            .memory_usage = kege::MemoryUsage::CpuToGpu,
            .debug_name = "lighting_buffer"
        }
    });




    kege::RgResrcHandle camr_bufr_shader_resource = graph->defineShaderResource
    (
        "camera_unifrom", frames_in_flight,
        {
            kege::DescriptorSetLayoutBinding
            {
                .name = "CameraShaderData",
                .binding = 0,
                .count = 1,
                .descriptor_type = kege::DescriptorType::UniformBuffer,
                .stage_flags = kege::ShaderStage::Vertex | kege::ShaderStage::Fragment
            }
        }
    );
    graph->updateShaderResource( camr_bufr_shader_resource, { kege::RgShaderResource{{ kege::RgBufferInfo{ camera_buffer } }} } );

    kege::RgResrcHandle gbuffer_shader_resource = graph->defineShaderResource
    (
        "gbuffer_shader_resource", frames_in_flight,
        {
            kege::DescriptorSetLayoutBinding
            {
                .name = "gbuffer_position",
                .binding = 0,
                .count = 1,
                .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                .stage_flags = kege::ShaderStage::Fragment
            },
            kege::DescriptorSetLayoutBinding
            {
                .name = "gbuffer_normal",
                .binding = 1,
                .count = 1,
                .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                .stage_flags = kege::ShaderStage::Fragment
            },
            kege::DescriptorSetLayoutBinding
            {
                .name = "gbuffer_albedo",
                .binding = 2,
                .count = 1,
                .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                .stage_flags = kege::ShaderStage::Fragment
            },
            kege::DescriptorSetLayoutBinding
            {
                .name = "gbuffer_material",
                .binding = 3,
                .count = 1,
                .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                .stage_flags = kege::ShaderStage::Fragment
            },
            kege::DescriptorSetLayoutBinding
            {
                .name = "scene_depth",
                .binding = 4,
                .count = 1,
                .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                .stage_flags = kege::ShaderStage::Fragment
            },
        }
    );
    graph->updateShaderResource
    (
        gbuffer_shader_resource,
        {
            kege::RgShaderResource{{ kege::RgFetchResInfo{ "gbuffer_position", kege::RgResrcType::Image } }},
            kege::RgShaderResource{{ kege::RgFetchResInfo{ "gbuffer_normal",   kege::RgResrcType::Image } }},
            kege::RgShaderResource{{ kege::RgFetchResInfo{ "gbuffer_albedo",   kege::RgResrcType::Image } }},
            kege::RgShaderResource{{ kege::RgFetchResInfo{ "gbuffer_material", kege::RgResrcType::Image } }},
            kege::RgShaderResource{{ kege::RgFetchResInfo{ "scene_depth",      kege::RgResrcType::Image } }},
        }
    );

    kege::RgResrcHandle lbuffer_shader_resource = graph->defineShaderResource
    (
        "lighting_unifrom", frames_in_flight,
        {
            kege::DescriptorSetLayoutBinding
            {
                .name = "LightShaderData",
                .binding = 0,
                .count = 1,
                .descriptor_type = kege::DescriptorType::UniformBuffer,
                .stage_flags = kege::ShaderStage::Vertex | kege::ShaderStage::Fragment
            }
        }
    );
    graph->updateShaderResource
    (
        lbuffer_shader_resource,
        {
            kege::RgShaderResource{{ kege::RgBufferInfo{ lighting_buffer } }},
        }
    );



    graph->addGraphicsPass
    ({
        "G-Buffer Pass",
        .reads =
        {
            kege::RgReadResrcDesc
            {
                .name = "camera_unifrom",
                .type = kege::RgResrcType::ShaderResource,
                .access = kege::AccessFlags::UniformRead,
                .stage = kege::PipelineStageFlag::VertexShader | kege::PipelineStageFlag::FragmentShader
            },
        },
        .writes =
        {
            kege::RgWriteResrcDesc
            {
                .name = "gbuffer_position",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ColorAttachmentWrite,
                .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                .clear_value = kege::ClearValue{ .color = { 0.f, 0.f, 0.f, 0.f } },
                .handle = gbuffer_position,
            },
            kege::RgWriteResrcDesc
            {
                .name = "gbuffer_normal",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ColorAttachmentWrite,
                .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                .clear_value = kege::ClearValue{ .color = { 0.f, 0.f, 0.f, 0.f } },
                .handle = gbuffer_normal,
            },
            kege::RgWriteResrcDesc
            {
                .name = "gbuffer_albedo",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ColorAttachmentWrite,
                .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                .clear_value = kege::ClearValue{ .color = { 0.f, 0.f, 0.f, 0.f } },
                .handle = gbuffer_albedo,
            },
            kege::RgWriteResrcDesc
            {
                .name = "gbuffer_material",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ColorAttachmentWrite,
                .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                .clear_value = kege::ClearValue{ .color = { 0.f, 0.f, 0.f, 0.f } },
                .handle = gbuffer_material,
            },
            kege::RgWriteResrcDesc
            {
                .name = "scene_depth",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::DepthStencilAttachmentWrite,
                .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                .clear_value = kege::ClearValue{ .depth_stencil = { 1.0f } },
                .handle = scene_depth,
            }
        },
        .execute = []( kege::RenderPassContext* context )
        {
            kege::Communication::broadcast< kege::RenderPassContext* >({ context });
        }
    });

    graph->addGraphicsPass
    ({
        .name = "Lighting Pass",
        .reads =
        {
            kege::RgReadResrcDesc
            {
                .name = "lighting_buffer",
                .type = kege::RgResrcType::Buffer,
                .access = kege::AccessFlags::ShaderRead,
                .stage = kege::PipelineStageFlag::FragmentShader
            },
            kege::RgReadResrcDesc
            {
                .name = "lighting_unifrom",
                .type = kege::RgResrcType::ShaderResource,
                .access = kege::AccessFlags::ShaderRead,
                .stage = kege::PipelineStageFlag::FragmentShader
            },
            kege::RgReadResrcDesc
            {
                .name = "gbuffer_position",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ShaderRead,
                .stage = kege::PipelineStageFlag::FragmentShader
            },
            kege::RgReadResrcDesc
            {
                .name = "gbuffer_normal",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ShaderRead,
                .stage = kege::PipelineStageFlag::FragmentShader
            },
            kege::RgReadResrcDesc
            {
                .name = "gbuffer_albedo",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ShaderRead,
                .stage = kege::PipelineStageFlag::FragmentShader
            },
            kege::RgReadResrcDesc
            {
                .name = "gbuffer_material",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ShaderRead,
                .stage = kege::PipelineStageFlag::FragmentShader
            },
            kege::RgReadResrcDesc
            {
                .name = "scene_depth",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ShaderRead,
                .stage = kege::PipelineStageFlag::FragmentShader
            },
        },
        .writes =
        {
            kege::RgWriteResrcDesc
            {
                .name = "lighting_output",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ColorAttachmentWrite,
                .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                .clear_value = kege::ClearValue{ .color = { 0.f, 0.f, 0.f, 0.f } },
                .handle = lighting_output,
            }
        },
        .execute = [ lbuffer_shader_resource, gbuffer_shader_resource, graph ]( kege::RenderPassContext* context )
        {
            kege::ShaderPipelineManager* pipelines = context->getGraphics()->getShaderPipelineManager();
            kege::PipelineHandle pipeline = pipelines->get( "lighting-shader" );
            if ( !pipeline )
            {
                KEGE_LOG_ERROR << "lighting-shader does not exist." <<kege::Log::nl;
                return;
            }

            kege::CommandEncoder* encoder = context->getCommandBuffer()->createCommandEncoder();
            kege::DescriptorSetHandle gbuffer_resource = graph->getPhysicalDescriptorSet( gbuffer_shader_resource );
            kege::DescriptorSetHandle lbuffer_resource = graph->getPhysicalDescriptorSet( lbuffer_shader_resource );

            encoder->setViewport
            ({
                0, 0,
                static_cast<float>(context->getRenderArea().extent.width),
                static_cast<float>(context->getRenderArea().extent.height)
            });
            encoder->setScissor
            ({
                0, 0,
                context->getRenderArea().extent.width,
                context->getRenderArea().extent.height
            });

            encoder->bindGraphicsPipeline( pipeline );
            encoder->bindDescriptorSets( gbuffer_resource );
            encoder->bindDescriptorSets( lbuffer_resource );
            encoder->draw( 4, 1, 0, 0 );
        }
    });
}



kege::RenderPassDefn copyRenderPass( uint32_t width, uint32_t height, uint32_t frames_in_flight, kege::RenderGraph* graph )
{
    kege::RgResrcHandle scene_color = graph->defineImage
    ({
        .name = "scene_color",
        .frames_in_flight = frames_in_flight,
        .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
        {
            .width  = width,
            .height = height,
            .depth  = 1,
            .format = kege::Format::rgba_f32,
            .type   = kege::ImageType::Type2D
        }
    });

    kege::RgResrcHandle scene_depth = graph->defineImage
    ({
        .name = "scene_depth",
        .frames_in_flight = frames_in_flight,
        .usages = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::ShaderResource,
        {
            .width  = width,
            .height = height,
            .depth  = 1,
            .format = kege::Format::depth_32,
            .type   = kege::ImageType::Type2D
        }
    });

    kege::RgResrcHandle shader_resource = graph->defineShaderResource
    (
        "scene_frame", frames_in_flight,
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

    kege::SamplerDesc desc = {};
    desc.mag_filter = kege::Filter::Nearest;
    desc.min_filter = kege::Filter::Nearest;
    desc.mipmap_mode = kege::MipmapMode::Nearest; ///< Mipmap filtering
    desc.address_mode_u = kege::AddressMode::ClampToEdge; ///< U-coordinate addressing
    desc.address_mode_v = kege::AddressMode::ClampToEdge; ///< V-coordinate addressing
    desc.address_mode_w = kege::AddressMode::ClampToEdge; ///< W-coordinate addressing
    kege::RgResrcHandle sampler = graph->defineSampler( "sampler-nearest-norep", desc );

    graph->updateShaderResource
    (
        shader_resource,
        {
            kege::RgShaderResource{{ kege::RgImageInfo{ scene_color, sampler } }},
            kege::RgShaderResource{{ kege::RgImageInfo{ scene_depth, sampler } }},
        }
    );

    return kege::RenderPassDefn
    {
        .name = "copy-pass",
        .reads =
        {
            kege::RgReadResrcDesc
            {
                .name = "scene_color",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ShaderRead,
                .stage = kege::PipelineStageFlag::FragmentShader,
                .handle = scene_color
            },
            kege::RgReadResrcDesc
            {
                .name = "scene_depth",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ShaderRead,
                .stage = kege::PipelineStageFlag::FragmentShader,
                .handle = scene_depth
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
        .execute = [shader_resource, graph]( kege::RenderPassContext* context )
        {
            const int FRAME_INDEX = graph->getGraphics()->getCurrFrameIndex();

            kege::ShaderPipelineManager* pipelines = graph->getGraphics()->getShaderPipelineManager();

            kege::PipelineHandle pipeline = pipelines->get( "copy-shader" );
            if ( !pipeline )
            {
                KEGE_LOG_ERROR << "copy-shader not found." <<kege::Log::nl;
                return;
            }

            kege::DescriptorSetHandle descriptor_set = graph->getPhysicalDescriptorSet( shader_resource );
            kege::CommandEncoder* encoder = context->getCommandBuffer()->createCommandEncoder();

            encoder->setScissor({ 0, 0, context->getRenderArea().extent.width, context->getRenderArea().extent.height });
            encoder->setViewport({ 0, 0, static_cast<float>(context->getRenderArea().extent.width), static_cast<float>(context->getRenderArea().extent.height) });

            encoder->bindGraphicsPipeline( pipeline );
            encoder->bindDescriptorSets( descriptor_set );
            encoder->draw( 4, 1, 0, 0 );
        }
    };
}

kege::RenderPassDefn displayTriangleRenderPass( kege::Graphics* graphics, kege::RenderGraph* graph )
{
    return kege::RenderPassDefn
    {
        "render-triangle",
        .reads = {},
        .writes =
        {
            kege::RgWriteResrcDesc
            {
                .name = "scene_color",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::ColorAttachmentWrite,
                .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                .clear_value = kege::ClearValue{ .color = { 0.2f, 0.2f, 0.2f, 1.0f } }
            },
            kege::RgWriteResrcDesc
            {
                .name = "scene_depth",
                .type = kege::RgResrcType::Image,
                .access = kege::AccessFlags::DepthStencilAttachmentWrite,
                .stage = kege::PipelineStageFlag::ColorAttachmentOutput,
                .clear_value = kege::ClearValue{ .depth_stencil = { 1.0f }}
            }
        },
        .execute = []( kege::RenderPassContext* context )
        {
            kege::PipelineHandle pipeline = context->getGraphics()->getShaderPipelineManager()->get( "triangle-shader" );
            if ( pipeline )
            {
                kege::CommandEncoder* encoder = context->getCommandBuffer()->createCommandEncoder();

                encoder->setScissor
                ({
                    0, 0,
                    context->getRenderArea().extent.width,
                    context->getRenderArea().extent.height
                });
                encoder->setViewport
                ({
                    0, 0,
                    static_cast<float>(context->getRenderArea().extent.width),
                    static_cast<float>(context->getRenderArea().extent.height)
                });

                encoder->bindGraphicsPipeline( pipeline );
                encoder->draw( 3, 1, 0, 0 );
            }
        }
    };
}




