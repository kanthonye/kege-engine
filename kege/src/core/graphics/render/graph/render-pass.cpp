//
//  render-pass.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/20/25.
//

#include "render-pass.hpp"
#include "render-graph.hpp"
#include "../manager/render-manager.hpp"

namespace kege{

    const kege::ShaderResrc* RenderPass::getShaderResource( const RgResrcHandle& handle )const
    {
        return _graph->getShaderResource( handle );
    }

    const kege::SamplerHandle* RenderPass::getSampler( const RgResrcHandle& handle )const
    {
        return _graph->getSampler( handle );
    }
    const kege::BufferHandle* RenderPass::getBuffer( const RgResrcHandle& handle )const
    {
        return _graph->getBuffer( handle );
    }
    const kege::ImageHandle* RenderPass::getImage( const RgResrcHandle& handle )const
    {
        return _graph->getImage( handle );
    }

    const kege::ShaderResrc* RenderPass::fetchShaderResource( const std::string& name )const
    {
        return _graph->fetchShaderResource( name );
    }
    const kege::SamplerHandle* RenderPass::fetchSampler( const std::string& name )const
    {
        return _graph->fetchSampler( name );
    }
    const kege::BufferHandle* RenderPass::fetchBuffer( const std::string& name )const
    {
        return _graph->fetchBuffer( name );
    }
    const kege::ImageHandle* RenderPass::fetchImage( const std::string& name )const
    {
        return _graph->fetchImage( name );
    }


    const std::vector< RgResrcHandle >& RenderPass::getShaderResources()const
    {
        return _shader_resources;
    }
    
    const std::vector< ShaderPipeline >& RenderPass::getShaderPipelines()const
    {
        return _fixed_shader_pipelines;
    }

    bool RenderPass::execute( RenderManager& render_manager )
    {
        //const int FRAME_INDEX = _graph->_graphics->getCurrFrameIndex();
        const int IMAGE_INDEX = _graph->_graphics->getSwapchainImageIndex();

        // select frame command-buffer
//        _context._command_buffer = _command_buffers[ FRAME_INDEX ];

        // begin the command recording
        if( !getCommandBuffer()->beginCommands() )
        {
            return false;
        }

        if ( !_barriers.resource_barriers.empty() )
        {
            std::vector< kege::ImageMemoryBarrier > image_barriers;
            std::vector< kege::BufferMemoryBarrier > buffer_barriers;
            //std::vector< kege::MemoryBarrier > memory_barriers; // For global barriers if you add them

            // Determine overall source and destination stages for this batch of barriers
            // For simplicity, often a broad stage is used, or you can try to be more precise.
            // Vulkan recommends using the logically earliest src stage and latest dst stage.
            PipelineStageFlag overall_src_stage_mask_rg = PipelineStageFlag::None;
            PipelineStageFlag overall_dst_stage_mask_rg = PipelineStageFlag::None;

            for (const RgResrcBarrierInfo& rb_info : _barriers.resource_barriers )
            {
                overall_src_stage_mask_rg = overall_src_stage_mask_rg | rb_info.src_stage_mask;
                overall_dst_stage_mask_rg = overall_dst_stage_mask_rg | rb_info.dst_stage_mask;

                if ( rb_info.image_handle )  // Check if it's an image barrier
                {
                    kege::ImageMemoryBarrier imb = {};
                    imb.src_access = rb_info.src_access_mask;
                    imb.dst_access = rb_info.dst_access_mask;
                    imb.old_layout = rb_info.old_layout;
                    imb.new_layout = rb_info.new_layout;
                    imb.image = rb_info.image_handle;
                    imb.src_queue = rb_info.src_queue; // Or kege::QUEUE_FAMILY_IGNORED
                    imb.dst_queue = rb_info.dst_queue; // Or kege::QUEUE_FAMILY_IGNORED
                    imb.subresource_range =
                    {
                        .base_mip_level   = 0,
                        .level_count      = 1,
                        .base_array_layer = 0,
                        .layer_count      = 1,
                    };
                    image_barriers.push_back(imb);
                }
                else if ( rb_info.buffer_handle ) // Check if it's a buffer barrier
                {
                    kege::BufferMemoryBarrier bmb = {};
                    bmb.src_access = rb_info.src_access_mask;
                    bmb.dst_access = rb_info.dst_access_mask;
                    bmb.buffer = rb_info.buffer_handle;
                    bmb.src_queue = rb_info.src_queue;
                    bmb.dst_queue = rb_info.dst_queue;
                    bmb.offset = rb_info.offset;
                    bmb.size = rb_info.size;
                    buffer_barriers.push_back( bmb );
                }
                // TODO: Handle global memory barriers if you add them to BarrierDescription
            }

            kege::PipelineStageFlag kege_src_stage = overall_src_stage_mask_rg;
            kege::PipelineStageFlag kege_dst_stage = overall_dst_stage_mask_rg;

            // Ensure TOP_OF_PIPE for empty source, BOTTOM_OF_PIPE for empty dest (common practice)
            if (kege_src_stage == 0) kege_src_stage = kege::PipelineStageFlag::TopOfPipe;
            if (kege_dst_stage == 0) kege_dst_stage = kege::PipelineStageFlag::BottomOfPipe;

            getCommandBuffer()->pipelineBarrier
            (
                kege_src_stage,
                kege_dst_stage,
                image_barriers,
                buffer_barriers
            );
        }


        beginRendering( IMAGE_INDEX );
        render_manager.execute( this );
        endRendering();

        getCommandBuffer()->endCommands();
        return true;
    }

    void RenderPass::beginRendering( const int IMAGE_INDEX )
    {
        kege::RenderingInfo rendering_info;
        const ImageDefn* defn = nullptr;
        
        for ( int i=0; i<_defn.writes.size(); ++i )
        {
            const RgWriteResrcDesc& write = _defn.writes[i];
            if ( write.type == RgResrcType::Image )
            {
                defn = _graph->_asset_manager.get< ImageDefn >( write.handle );
                int img_idx = IMAGE_INDEX % int( defn->physical_handle.size() );

                _render_area = { 0, 0, defn->info.width, defn->info.height };

                rendering_info.render_area = _render_area;
                rendering_info.layer_count = defn->info.depth;
                if ( write.access == AccessFlags::ColorAttachmentWrite )
                {
                    rendering_info.color_attachments.push_back
                    (
                        kege::RenderingAttachmentInfo
                        {
                            .image_view_handle = defn->physical_handle[ img_idx ],
                            .clear_value = write.clear_value,
                            .load_op = kege::AttachmentLoadOp::Clear,
                            .store_op = kege::AttachmentStoreOp::Store,
                            .image_layout = kege::ImageLayout::ColorAttachment,
                        }
                    );
                }
                else if ( write.access == AccessFlags::DepthStencilAttachmentWrite )
                {
                    rendering_info.depth_attachment = kege::RenderingAttachmentInfo
                    {
                        .image_view_handle = defn->physical_handle[ img_idx ],
                        .clear_value = write.clear_value,
                        .load_op = kege::AttachmentLoadOp::Clear,
                        .store_op = kege::AttachmentStoreOp::Store,
                        .image_layout = kege::ImageLayout::DepthStencilAttachment,
                    };
                }
            }
        }

        // begin the rendering
        getCommandBuffer()->beginRendering( rendering_info );
    }

    void RenderPass::endRendering()
    {
        getCommandBuffer()->endRendering();
    }

    const std::vector< RgWriteResrcDesc >& RenderPass::getWrites()const
    {
        return _defn.writes;
    }

    std::vector< RgWriteResrcDesc >& RenderPass::getWrites()
    {
        return _defn.writes;
    }

    const std::vector< RgReadResrcDesc >& RenderPass::getReads()const
    {
        return _defn.reads;
    }

    std::vector< RgReadResrcDesc >& RenderPass::getReads()
    {
        return _defn.reads;
    }

//    RenderPassContext& RenderPass::getContext()
//    {
//        return _context;
//    }

    CommandEncoder* RenderPass::getCommandEncoder()
    {
        kege::CommandEncoder* encoder = getCommandBuffer()->createCommandEncoder();
        encoder->setScissor
        ({
            0, 0,
            _render_area.extent.width,
            _render_area.extent.height
        });
        encoder->setViewport
        ({
            0, 0,
            static_cast<float>( _render_area.extent.width ),
            static_cast<float>( _render_area.extent.height )
        });
        return encoder;
    }
    CommandBuffer* RenderPass::getCommandBuffer()
    {
        const int FRAME_INDEX = _graph->_graphics->getCurrFrameIndex();
        return _command_buffers[ FRAME_INDEX ];
    }
    bool RenderPass::hasFixedPipelines()const
    {
        return !_fixed_shader_pipelines.empty();
    }

    RenderPassType RenderPass::getType()const
    {
        return _defn.pass;
    }
    RenderPass::~RenderPass()
    {
        if ( _graph && !_command_buffers.empty() )
        {
            for (int i=0; i<_command_buffers.size(); ++i)
            {
                _graph->getGraphics()->destroyCommandBuffer( _command_buffers[i] );
            }
            _command_buffers.clear();
            _graph = nullptr;
        }
    }

}
