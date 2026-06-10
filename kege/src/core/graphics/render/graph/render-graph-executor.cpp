//
//  render-graph-executor.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/23/26.
//

#include "../renderer.hpp"
#include "render-graph-executor.hpp"

namespace kege {

    std::vector< kege::SubmitInfo > RenderGraphExecutor::execute
    (
        kege::Renderer* renderer,
        const kege::RenderGraphExecutionPlan& execution_plan
    )
    {
        std::vector< kege::SubmitInfo > submits;
        for (int exec_index = 0; exec_index < execution_plan.size(); ++exec_index)
        {
            const kege::RenderPassExecutionSequence& execution_sequence = execution_plan[ exec_index ].execution_sequence;
            const kege::RenderView* view = execution_plan[ exec_index ].render_view.ref();

            kege::RenderPassContext context;
            context.frame_index = renderer->getFrameIndex();
            context.renderer = renderer;
            context.view = view;
            
            // 1. Create one cmd + one signal semaphore per Submit_Info
            std::unordered_map< PassId, kege::ref::Semaphore > wait_semaphores;
            std::vector< kege::CommandBuffer* > command_buffers;
            std::vector< kege::Semaphore* > signal_semaphores;
            for (const auto& submit : execution_sequence.submits)
            {
                kege::CommandBuffer* cmd = renderer->acquireCommandBuffer( submit.queue_type );
                kege::Semaphore* sem = cmd->getSemaphore();

                command_buffers.push_back(cmd);
                signal_semaphores.push_back(sem);

                for (PassId pid : submit.pass_ids)
                {
                    wait_semaphores[pid] = sem;
                }
            }

            // 2. Record each submit
            for (const auto& submit : execution_sequence.submits)
            {
                context.command_buffer = command_buffers[ submit.submit_id ];
                context.command_buffer->beginCommands();
                for (PassId pass_id : submit.pass_ids)
                {
                    const RenderPassNode& pass = execution_sequence.nodes[ pass_id ];
                    context.pass_desc = pass.desc;
                    executeRenderPass( pass, context );
                }
                context.command_buffer->endCommands();
            }
            
            // 3. Build submits with wait semaphores
            for (const auto& submit : execution_sequence.submits)
            {
                kege::SubmitInfo submit_info{};
                if ( execution_sequence.submits.size() > 1 )
                {
                    submit_info.render_complete_semaphore = signal_semaphores[ submit.submit_id ];
                }
                for (PassId wait_pid : submit.wait_on_pass_ids)
                {
                    submit_info.wait_semaphores.push_back( wait_semaphores[ wait_pid ] );
                }
                submit_info.command_buffer = command_buffers[ submit.submit_id ];
                submits.push_back( submit_info );
            }
        }
        return submits;
    }

    void RenderGraphExecutor::executeRenderPass
    (
        const RenderPassNode& pass,
        kege::RenderPassContext& context
    )
    {
        applyBarriers( pass, context );
        if ( !pass.desc->writes.empty() )
        {
            if ( pass.desc->type == QueueType::Graphics )
            {
                beginRendering( pass, context );
            }
            
            if ( pass.desc->execute )
            {
                pass.desc->execute( context );
            }

            if ( pass.desc->type == QueueType::Graphics )
            {
                endRendering( context );
            }
        }
    }

    void RenderGraphExecutor::applyBarriers
    (
        const RenderPassNode& pass,
        kege::RenderPassContext& context
    )
    {
        if ( pass.barriers.empty() ) return;

        std::vector< kege::ImageMemoryBarrier >  image_barriers;
        std::vector< kege::BufferMemoryBarrier > buffer_barriers;

        for (const RgResrcBarrier& barrier : pass.barriers )
        {
            switch ( barrier.resource.type )
            {
                case RgResrcType::Buffer:
                {
                    kege::BufferMemoryBarrier bmb = {};
                    bmb.src_access = barrier.src_access;
                    bmb.dst_access = barrier.dst_access;
                    bmb.src_stage = barrier.src_stage;
                    bmb.dst_stage = barrier.dst_stage;
                    bmb.src_queue = barrier.src_queue;
                    bmb.dst_queue = barrier.dst_queue;
                    bmb.offset = barrier.offset;
                    bmb.size = barrier.size;

                    bmb.buffer = context.renderer->getBuffer( barrier.resource );
                    buffer_barriers.push_back( bmb );
                    break;
                }
                case RgResrcType::Image:
                {
                    kege::ImageMemoryBarrier imb = {};
                    imb.src_access = barrier.src_access;
                    imb.dst_access = barrier.dst_access;
                    imb.src_stage = barrier.src_stage;
                    imb.dst_stage = barrier.dst_stage;
                    imb.old_layout = barrier.old_layout;
                    imb.new_layout = barrier.new_layout;
                    imb.src_queue = barrier.src_queue; // Or kege::QUEUE_FAMILY_IGNORED
                    imb.dst_queue = barrier.dst_queue; // Or kege::QUEUE_FAMILY_IGNORED

                    imb.subresource_range =
                    {
                        .base_mip_level   = 0,
                        .level_count      = 1,
                        .base_array_layer = 0,
                        .layer_count      = 1,
                    };

                    imb.image = context.renderer->getImage( barrier.resource );
                    image_barriers.push_back(imb);
                    break;
                }

                default:
                    break;
            }
        }
        context.command_buffer->pipelineBarrier( image_barriers, buffer_barriers );
    }

    void RenderGraphExecutor::beginRendering
    (
        const RenderPassNode& pass,
        kege::RenderPassContext& context
    )
    {
        if( pass.desc->writes.empty() ) return;
        
        kege::RenderingInfo rendering_info;
        for ( auto& write : pass.desc->writes )
        {
            if ( write.type == RgResrcType::Image )
            {
                kege::Ref< kege::Image > image = context.renderer->getImage( write.resource );

                rendering_info.render_area.offset = { 0, 0 };
                rendering_info.render_area.extent = { image->getExtent().width, image->getExtent().height };
                rendering_info.layer_count = image->getArrayLayers();

                if ( hasFlag( write.access, AccessFlags::ColorWrite ) )
                {
                    rendering_info.color_attachments.push_back
                    (
                        kege::RenderingAttachmentInfo
                        {
                            .image = image,
                            .clear_value = write.clear_value,
                            .load_op = write.load_op,
                            .store_op = kege::AttachmentStoreOp::Store,
                            .image_layout = write.layout,
                        }
                    );
                }
                else if ( hasFlag( write.access, AccessFlags::DepthStencilWrite ) )
                {
                    rendering_info.depth_attachment = kege::RenderingAttachmentInfo
                    {
                        .image = image,
                        .clear_value = write.clear_value,
                        .load_op = write.load_op,
                        .store_op = kege::AttachmentStoreOp::Store,
                        .image_layout = write.layout,
                    };
                }
            }
        }

        context.command_buffer->setViewport({
            .x = 0, .y = 0,
            .width = static_cast<float>(rendering_info.render_area.extent.width),
            .height = static_cast<float>(rendering_info.render_area.extent.height),
        });

        context.command_buffer->setScissor({
            .x = 0, .y = 0,
            .width = rendering_info.render_area.extent.width,
            .height = rendering_info.render_area.extent.height,
        });

        context.command_buffer->beginRendering( rendering_info );
    }

    void RenderGraphExecutor::endRendering( kege::RenderPassContext& context )
    {
        context.command_buffer->endRendering();
    }

}

