//
//  render-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#include "../../mesh/mesh-cuboid.hpp"
#include "../../mesh/mesh-ellipsoid.hpp"
#include "render-graph.hpp"
#include "render-executor.hpp"

namespace kege{

    void RenderExecutor::submit( kege::RenderPassType pass, const ref::Mesh& mesh, const ref::ShaderData& shader_data, const PushConstantBlock& constant )
    {
        _render_queue.submit({ pass, mesh, shader_data.ref(), constant });
    }

    void RenderExecutor::submit( const ref::Mesh& mesh, const ref::Material& material, const PushConstantBlock& constant )
    {
        _render_queue.submit({ material->getPass(), mesh, material.ref(), constant });
    }

    void RenderExecutor::setSceneCamera( const kege::CameraData& data )
    {
        _scene_camera_data = data;
    }
    
    void RenderExecutor::setGuiCamera( const kege::CameraData& data )
    {
        _gui_camera_data = data;
        //Uniform& uniform = _camera_shader_resource[0][0].uniform;
        //_graphics->updateBuffer( uniform.buffers[0].buffer, 0, sizeof( data ), &data );
    }

    void RenderExecutor::updateLights( int size, void* data )
    {
        //_graphics->updateBuffer( _lights_shader_resource[0][0].uniform.buffers[0].buffer, 0, size, data );
    }
    
    RenderQueue& RenderExecutor::getRenderQueue()
    {
        return _render_queue;
    }

    void RenderExecutor::drawObjects( CommandEncoder* encoder, RenderStage* pass, const ref::ShaderPipeline& pipeline, const std::vector< RenderObject >& objects )
    {
        encoder->bindShaderPipeline( pipeline );

//        for ( const PipelineResourceBinding& binding : pipeline->global_resources )
//        {
//            //TODO: switch ( binding.type )
//            {
//                case BindingType::SHADER_RESOURCE:
//                {
//                    const ShaderResrc* resource = pass->fetchShaderResource( binding.name );
//                    encoder->bind( resource->getShaderBindings() );
//                    break;
//                }
//
//                case BindingType::BUFFER:
//                {
//                    //const BufferHandle* buffer = pass->fetchBuffer( binding.name );
//                    //encoder->bind( pipeline->link( binding.set, binding.binding, *buffer ) );
//                    Log::error << "TODO: encoder->bind( pipeline->link( binding.set, binding.binding, *buffer ) )" << Log::nl;
//                    return;
//                    break;
//                }
//
//                case BindingType::TEXTURE:
//                {
//                    //const ref::Image* texture = pass->fetchImage( binding.name );
//                    //encoder->bind( pipeline->link( binding.set, binding.binding, *texture ) );
//                    Log::error << "TODO: encoder->bind( pipeline->link( binding.set, binding.binding, *texture ) )" << Log::nl;
//                    return;
//                    break;
//                }
//
//                case BindingType::PUSH_CONSTANTS:
//                {
//                    //const PushConstant* constant = pass->fetchPushConstant( binding.name );
//                    //encoder->setPushBlock( constant.stages, constant.offset, constants.size, constants.data );
//                    Log::error << "TODO: encoder->setPushBlock( ... )" << Log::nl;
//                    return;
//                    break;
//                }
//
//                default:
//                    Log::error << "unsupported: encoder->bind( " <<binding.name <<" )" << Log::nl;
//                    return;
//                    break;
//            }
//        }
        
        for ( const RenderObject& object : objects )
        {
            if ( object.shader_data )
            {
                for(const auto& [set_index, set] : object.shader_data->getShaderSets())
                {
                    encoder->bind( set );
                }
            }

            encoder->setPushBlock
            (
                object.constant.stages,
                object.constant.offset,
                object.constant.size,
                object.constant.data
            );
            
            drawMesh( encoder, object.mesh );
        }
    }

    void RenderExecutor::drawMesh( CommandEncoder* encoder, const Ref< Mesh >& mesh )
    {
        if ( mesh->primative )
        {
            if ( mesh->primative->vertex_buffer )
            {
                encoder->bindVertexBuffers( 0, { mesh->primative->vertex_buffer }, { 0 });
            }

            if ( mesh->primative->index_buffer )
            {
                drawIndices( encoder, mesh );
            }
            else
            {
                drawArrays( encoder, mesh );
            }
        }
        else
        {
            drawArrays( encoder, mesh );
        }
    }

    void RenderExecutor::drawIndices( CommandEncoder* encoder, const Ref< Mesh >& mesh )
    {
        encoder->bindIndexBuffer( mesh->primative->index_buffer, 0, false );
        if ( mesh->indirect_draw_object && mesh->shader_data )
        {
            for(const auto& [set_index, set] : mesh->shader_data->getShaderSets())
            {
                encoder->bind( set );
            }
            for (int i=0; i<mesh->indirect_draw_object->size(); ++i )
            {
                const IndirectDrawCommandBuffer& indirect_buffer = mesh->indirect_draw_object->at(i);
                encoder->drawIndexIndirect
                (
                    indirect_buffer.buffer,
                    indirect_buffer.offset,
                    indirect_buffer.count,
                    indirect_buffer.stride
                );
            }
        }
        else if ( mesh->indirect_draw_object )
        {
            for (int i=0; i<mesh->indirect_draw_object->size(); ++i )
            {
                const IndirectDrawCommandBuffer& indirect_buffer = mesh->indirect_draw_object->at(i);
                encoder->drawIndexIndirect
                (
                    indirect_buffer.buffer,
                    indirect_buffer.offset,
                    indirect_buffer.count,
                    indirect_buffer.stride
                );
            }
        }
        else if ( mesh->shader_data )
        {
            for(const auto& [set_index, set] : mesh->shader_data->getShaderSets())
            {
                encoder->bind( set );
            }
            encoder->drawIndexed
            (
                mesh->index_count,
                mesh->instance_count,
                mesh->first_index,
                0,
                mesh->first_instance
            );
        }
        else
        {
            encoder->drawIndexed
            (
                mesh->index_count,
                mesh->instance_count,
                mesh->first_index,
                0,
                mesh->first_instance
            );
        }
    }
    
    void RenderExecutor::drawArrays( CommandEncoder* encoder, const Ref< Mesh >& mesh )
    {
        if ( mesh->indirect_draw_object && mesh->shader_data )
        {
            for(const auto& [set_index, set] : mesh->shader_data->getShaderSets())
            {
                encoder->bind( set );
            }
            for (int i=0; i<mesh->indirect_draw_object->size(); ++i )
            {
                const IndirectDrawCommandBuffer& indirect_buffer = mesh->indirect_draw_object->at(i);
                encoder->drawIndirect
                (
                    indirect_buffer.buffer,
                    indirect_buffer.offset,
                    indirect_buffer.count,
                    indirect_buffer.stride
                );
            }
        }
        else if ( mesh->indirect_draw_object )
        {
            for (int i=0; i<mesh->indirect_draw_object->size(); ++i )
            {
                const IndirectDrawCommandBuffer& indirect_buffer = mesh->indirect_draw_object->at(i);
                encoder->drawIndirect
                (
                    indirect_buffer.buffer,
                    indirect_buffer.offset,
                    indirect_buffer.count,
                    indirect_buffer.stride
                );
            }
        }
        else if ( mesh->shader_data )
        {
            for(const auto& [set_index, set] : mesh->shader_data->getShaderSets())
            {
                encoder->bind( set );
            }
            encoder->draw
            (
                mesh->index_count,
                mesh->instance_count,
                mesh->first_index,
                mesh->first_instance
            );
        }
        else
        {
            encoder->draw
            (
                mesh->index_count,
                mesh->instance_count,
                mesh->first_index,
                mesh->first_instance
            );
        }
    }

    void RenderExecutor::execute( RenderStage* pass )
    {
        const RenderPassQueue& queue = _render_queue.getQueueForPass( pass->getType() );
        CommandEncoder* encoder = pass->getCommandEncoder();

        if ( pass->hasFixedPipelines() )
        {
            const std::vector< ref::ShaderPipeline >& pipelines = pass->getShaderPipelines();
            for ( const ref::ShaderPipeline& pipeline : pipelines )
            {
                for ( kege::RenderPassQueue::iterator i = queue.objects.begin(); i != queue.objects.end(); i++ )
                {
                    drawObjects( encoder, pass, pipeline, i->second );
                }
            }
        }
        else
        {
            for ( kege::RenderPassQueue::iterator i = queue.objects.begin(); i != queue.objects.end(); i++ )
            {
                const RenderPassQueue::RenderObjects& objects = i->second;
                drawObjects( encoder, pass, i->first, objects );
            }
        }
    }

    void RenderExecutor::execute( RenderGraph* graph, const ref::Semaphore& image_available, const ref::Semaphore& render_complete )
    {
        if( !graph->_execution_order.empty() )
        {
            ref::Buffer buffer = graph->fetchBuffer( "camera-buffer" );
            if ( buffer )
            {
                buffer->copyFrom( &_scene_camera_data, sizeof( _scene_camera_data ) );
            }

            std::vector< kege::SubmitInfo > submits;
            for (int i = 0; i < graph->_execution_order.size(); ++i)
            {
                RenderStage* pass = graph->_execution_order[i];
                if( pass->execute() )
                {
                    submits.push_back( pass->getSubmitInfo() );
                }
            }
            _graphics->submit( submits, image_available, render_complete );
        }
        _render_queue.clear();
    }

    bool RenderExecutor::initialize()
    {
//        uint32_t buffer_size = 2 * sizeof( kege::mat44 ) + sizeof( kege::vec4 );
//        kege::mat44 m[3] = {kege::mat44(1.0), kege::mat44(1.0), kege::mat44(0.0)};
//        _camera_shader_resource = _graphics->allocateUniformSet
//        ({
//            UniformDesc
//            {
//                .count = 1,
//                .binding = 0,
//                .descriptor_type = kege::DescriptorType::UniformBuffer,
//                .stage_flags = ShaderStageFlag::Vertex,
//                .name = "Camera"
//            }
//        });
//        _camera_shader_resource[0][0] = UniformBinding
//        {
//            .binding = 0,
//            .uniform =
//            {{
//                BufferInfo
//                {
//                    .range = 2 * sizeof( mat44 ) + sizeof( vec4 ),
//                    .offset = 0,
//                    .buffer = _graphics->createBuffer
//                    ({
//                        .size = buffer_size,
//                        .data = m,
//                        .usage = BufferUsages::UniformBuffer,
//                        .memory_usage = MemoryUsage::CpuToGpu,
//                    })
//                }
//            }}
//        };
//        _camera_shader_resource.update({});
//
//        _lights_shader_resource = _graphics->allocateUniformSet
//        ({
//            UniformDesc
//            {
//                .count = 1,
//                .binding = 0,
//                .descriptor_type = kege::DescriptorType::StorageBuffer,
//                .stage_flags = ShaderStageFlag::Fragment,
//                .name = "Lights"
//            }
//        });
//        _lights_shader_resource[0][0] = UniformBinding
//        {
//            .binding = 0,
//            .uniform =
//            {{
//                BufferInfo
//                {
//                    .range = 10 * sizeof( Light ),
//                    .offset = 0,
//                    .buffer = _graphics->createBuffer
//                    ({
//                        .size = buffer_size,
//                        .data = nullptr,
//                        .usage = BufferUsages::StorageBuffer,
//                        .memory_usage = MemoryUsage::CpuToGpu,
//                    })
//                }
//            }}
//        };
//        _lights_shader_resource.update({});


        _sphere = new Mesh;
        _sphere->primative = new EllipsoidMesh(1, 1, 16, 16);
        _sphere->upload( getGraphics() );

        _cube = new Mesh;
        _cube->primative = new CuboidMesh(0, 1);
        _cube->upload( getGraphics() );

        _cube = new Mesh;
        _cube->first_instance = 0;
        _cube->instance_count = 1;
        _cube->first_index = 0;
        _cube->index_count = 4;

        return true;
    }

    void RenderExecutor::shutdown()
    {
        _render_queue.clear();
    }

    void RenderExecutor::clearRenderQueue()
    {
        _render_queue.clear();
    }

    kege::Graphics* RenderExecutor::getGraphics()
    {
        return _graphics;
    }

    RenderExecutor::RenderExecutor( kege::Graphics* graphics, uint32_t frames_in_flight )
    :   _graphics( graphics )
    ,   _frames_in_flight( frames_in_flight )
    {}

    RenderExecutor::~RenderExecutor()
    {
        shutdown();
    }

}
