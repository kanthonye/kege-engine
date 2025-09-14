//
//  render-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#include "../engine/engine.hpp"
#include "render-manager.hpp"

namespace kege{


    void RenderManager::submit( const RenderObject& object )
    {
        _render_queue.submit( object );
    }
    
    RenderQueue& RenderManager::getRenderQueue()
    {
        return _render_queue;
    }

    void RenderManager::operator()( kege::RenderPassContext* context )
    {
        switch ( context->getRenderPassType() )
        {
            case RenderPassType::ShadowMap:
            {
                break;
            }

            default: break;
        };

        const RenderPassQueue& queue = _render_queue.getQueueForPass( context->getRenderPassType() );
        CommandEncoder* encoder = context->getCommandEncoder();

        ModelMatrices model;
        ShaderPipeline* shader = 0;

        for (const kege::RenderObject& object : queue.objects )
        {
            encoder->bindGraphicsPipeline( object.material->pipeline );

            if ( shader->require( ShaderRequirement::PUSH_CONSTANTS ) )
            {
                model( object.transform.position, object.transform.orientation, object.transform.scale );
                encoder->setPushConstants( ShaderStage::Vertex, 0, sizeof( model ), &model );
            }

            // bind the vertex buffers
            if ( !shader->states().vertex_input_state.attributes.empty() )
            {
                if ( !object.mesh->primative->vertex_buffer )
                {
                    continue;
                }
                encoder->bindVertexBuffers( 0, { object.mesh->primative->vertex_buffer }, { 0 });
            }

            if ( !object.mesh->indirect_draw_buffer_list->buffers.empty() && !object.mesh->instance_buffer_list->buffers.empty() )
            {
                for (int i=0; i<object.mesh->indirect_draw_buffer_list->buffers.size(); ++i )
                {
                    const IndirectDrawBuffer& indirect_buffer = object.mesh->indirect_draw_buffer_list->buffers[i];
                    const InstanceBuffer& instance_buffer = object.mesh->instance_buffer_list->buffers[i];
                }

            }
            if ( !object.mesh->indirect_draw_buffer_list->buffers.empty() )
            {
                for (const IndirectDrawBuffer& indirect_buffer : object.mesh->indirect_draw_buffer_list->buffers )
                {
                    // indirect index buffer draw
                    if ( object.mesh->primative->index_buffer )
                    {
                        encoder->bindIndexBuffer( object.mesh->primative->index_buffer, 0, false );
                        encoder->drawIndexIndirect
                        (
                            indirect_buffer.buffer,
                            indirect_buffer.offset,
                            indirect_buffer.count,
                            indirect_buffer.stride
                        );
                    }
                    // indirect vertex buffer draw
                    else
                    {
                        encoder->drawIndirect
                        (
                            indirect_buffer.buffer,
                            indirect_buffer.offset,
                            indirect_buffer.count,
                            indirect_buffer.stride
                        );
                    }
                }
            }
            else
            {
                if ( object.mesh->primative->index_buffer )
                {
                    encoder->bindIndexBuffer( object.mesh->primative->index_buffer, 0, false );
                    encoder->drawIndexed
                    (
                        object.mesh->index_count,
                        object.mesh->instance_count,
                        object.mesh->first_index, 0,
                        object.mesh->first_instance
                    );
                }
                else
                {
                    encoder->draw
                    (
                        object.mesh->index_count,
                        object.mesh->instance_count,
                        object.mesh->first_index,
                        object.mesh->first_instance
                    );
                }
            }
        }
    }

    void RenderManager::execute( double dms )
    {
        _graph->execute();
        clear();
    }

    bool RenderManager::initialize()
    {
        Communication::add< kege::RenderPassContext*, RenderManager >( this );
        return true;
    }

    void RenderManager::shutdown()
    {
        Communication::remove< kege::RenderPassContext*, RenderManager >( this );
        _render_queue.clear();
    }

    void RenderManager::clear()
    {
        _render_queue.clear();
    }

    RenderManager::RenderManager( CreateInfo info )
    :   _engine( info.engine )
    ,   _graphics( info.graphics )
    ,   _graph( info.graph )
    ,   _frames_in_flight( info.frames_in_flight )
    {}

    RenderManager::~RenderManager()
    {
        shutdown();
    }

}
