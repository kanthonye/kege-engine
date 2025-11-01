//
//  render-pass.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/20/25.
//

#include "render-stage.hpp"
#include "render-graph.hpp"
#include "../graph/render-executor.hpp"

namespace kege{

    const kege::ShaderSet* RenderStage::getShaderResource( const RgResrcHandle& handle )const
    {
        return _graph->getShaderResource( handle );
    }

    const ref::Sampler RenderStage::getSampler( const RgResrcHandle& handle )const
    {
        return _graph->getSampler( handle );
    }
    const ref::Buffer RenderStage::getBuffer( const RgResrcHandle& handle )const
    {
        return _graph->getBuffer( handle );
    }
    const ref::Image RenderStage::getImage( const RgResrcHandle& handle )const
    {
        return _graph->getImage( handle );
    }

    const kege::ShaderSet* RenderStage::fetchShaderResource( const std::string& name )const
    {
        return _graph->fetchShaderResource( name );
    }
    const ref::Sampler RenderStage::fetchSampler( const std::string& name )const
    {
        return _graph->fetchSampler( name );
    }
    const ref::Buffer RenderStage::fetchBuffer( const std::string& name )const
    {
        return _graph->fetchBuffer( name );
    }
    const ref::Image RenderStage::fetchImage( const std::string& name )const
    {
        return _graph->fetchImage( name );
    }


    const std::vector< RgResrcHandle >& RenderStage::getShaderResources()const
    {
        return _shader_resources;
    }
    
    const std::vector< ref::ShaderPipeline >& RenderStage::getShaderPipelines()const
    {
        return _fixed_shader_pipelines;
    }

    bool RenderStage::execute()
    {
        // begin the command recording
        if( getCommandBuffer()->beginCommands() )
        {
            applyBarriers( _barriers );
            if ( !_defn.writes.empty() )
            {
                beginRendering();
                if ( this->_defn.pass != RenderPassType::BarrierTransition )
                {
                    _graph->getRenderExecutor()->execute( this );
                }
                endRendering();
            }
            getCommandBuffer()->endCommands();
            return true;
        }

        return false;
    }

    void RenderStage::applyBarriers( const BarrierDescriptions& barriers )
    {
        if ( barriers.empty() ) return;

        std::vector< kege::ImageMemoryBarrier >  image_barriers;
        std::vector< kege::BufferMemoryBarrier > buffer_barriers;

        for (const RgResrcBarrierInfo& barrier : barriers )
        {
            switch ( barrier.resource_handle.type )
            {
                case RgResrcType::Buffer:
                {
                    kege::BufferMemoryBarrier bmb = {};
                    bmb.src_access = barrier.src_access_mask;
                    bmb.dst_access = barrier.dst_access_mask;
                    bmb.src_stage = barrier.src_stage_mask;
                    bmb.dst_stage = barrier.dst_stage_mask;
                    bmb.src_queue = barrier.src_queue;
                    bmb.dst_queue = barrier.dst_queue;
                    bmb.offset = barrier.offset;
                    bmb.size = barrier.size;

                    bmb.buffer = _graph->getBuffer( barrier.resource_handle );

                    buffer_barriers.push_back( bmb );
                    break;
                }
                case RgResrcType::Image:
                {
                    kege::ImageMemoryBarrier imb = {};
                    imb.src_access = barrier.src_access_mask;
                    imb.dst_access = barrier.dst_access_mask;
                    imb.src_stage = barrier.src_stage_mask;
                    imb.dst_stage = barrier.dst_stage_mask;
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

                    imb.image = _graph->getImage( barrier.resource_handle );

                    image_barriers.push_back(imb);
                    break;
                }

                default:
                    break;
            }
        }

        getCommandBuffer()->pipelineBarrier
        (
            image_barriers,
            buffer_barriers
        );
    }

    void RenderStage::beginRendering()
    {
        const int IMAGE_INDEX = _graph->_graphics->getSwapchain()->getImageIndex();

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
                if ( hasFlag( write.usage.access, AccessFlags::ColorWrite ) )
                {
                    rendering_info.color_attachments.push_back
                    (
                        kege::RenderingAttachmentInfo
                        {
                            .image = defn->physical_handle[ img_idx ],
                            .clear_value = write.clear_value,
                            .store_op = kege::AttachmentStoreOp::Store,
                            .load_op = write.usage.load_op,
                            .image_layout = write.usage.layout,
                        }
                    );
                }
                else if ( hasFlag( write.usage.access, AccessFlags::DepthStencilWrite ) )
                {
                    rendering_info.depth_attachment = kege::RenderingAttachmentInfo
                    {
                        .image = defn->physical_handle[ img_idx ],
                        .clear_value = write.clear_value,
                        .store_op = kege::AttachmentStoreOp::Store,
                        .load_op = write.usage.load_op,
                        .image_layout = write.usage.layout,
                    };
                }
            }
        }

        // begin the rendering
        getCommandBuffer()->beginRendering( rendering_info );
    }

    void RenderStage::endRendering()
    {
        getCommandBuffer()->endRendering();
    }

    const std::vector< RgWriteResrcDesc >& RenderStage::getWrites()const
    {
        return _defn.writes;
    }

    std::vector< RgWriteResrcDesc >& RenderStage::getWrites()
    {
        return _defn.writes;
    }

    const std::vector< RgReadResrcDesc >& RenderStage::getReads()const
    {
        return _defn.reads;
    }

    std::vector< RgReadResrcDesc >& RenderStage::getReads()
    {
        return _defn.reads;
    }

    CommandEncoder* RenderStage::getCommandEncoder()
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
    CommandBuffer* RenderStage::getCommandBuffer()
    {
        const int FRAME_INDEX = _graph->_graphics->getFrameIndex();
        return _submit_info[ FRAME_INDEX ].command_buffer.ref();
    }
    bool RenderStage::hasFixedPipelines()const
    {
        return !_fixed_shader_pipelines.empty();
    }

    RenderPassType RenderStage::getType()const
    {
        return _defn.pass;
    }
    int RenderStage::getPassId()const
    {
        return _id;
    }

    const SubmitInfo& RenderStage::getSubmitInfo()const
    {
        uint32_t index = _graph->_graphics->getFrameIndex();
        return _submit_info[ index ];
//        return SubmitInfo
//        {
//            .command_buffers = _command_buffers.data(),
//            .command_buffer_count = static_cast< uint32_t >( _command_buffers.size() ),
//            .wait_semaphores = _wait_semaphores.data(),
//            .wait_semaphore_count = static_cast< uint32_t >( _wait_semaphores.size() ),
//            .signal_semaphores =_render_complete_semaphores.data(),
//            .signal_semaphore_count = static_cast< uint32_t >( _render_complete_semaphores.size() ),
//            .wait_stages = _pipeline_wait_stages
//        };
    }

    void RenderStage::destroySemaphores()
    {
        for ( SubmitInfo& info : _submit_info )
        {
            info.render_complete_semaphore.clear();
//            _graph->_graphics->destroySemaphore( info.render_complete_semaphore );
//            info.render_complete_semaphore = nullptr;
//            
//            for ( Semaphore* semaphore : info.wait_semaphores )
//            {
//                _graph->_graphics->destroySemaphore( semaphore );
//            }
            info.wait_semaphores.clear();
        }
    }

    void RenderStage::destroy()
    {
        destroySemaphores();
        _submit_info.clear();
    }
    
    RenderStage::~RenderStage()
    {
        destroy();
    }

    RenderStage::RenderStage()
    {
    }

}
