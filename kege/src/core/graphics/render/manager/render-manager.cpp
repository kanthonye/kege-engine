//
//  render-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#include "../../../engine/engine.hpp"
#include "../../mesh/mesh-cuboid.hpp"
#include "../../mesh/mesh-ellipsoid.hpp"
#include "render-manager.hpp"

namespace kege{

    void RenderManager::setSceneCamera( const kege::CameraData& data )
    {
        _scene_camera_data = data;
        const BufferHandle* buffer = _graph->fetchBuffer( "camera-buffer" );
        if ( buffer )
        {
            _graphics->updateBuffer( *buffer, 0, sizeof( _scene_camera_data ), &_scene_camera_data );
        }
    }
    
    void RenderManager::setGuiCamera( const kege::CameraData& data )
    {
        _gui_camera_data = data;
        //Uniform& uniform = _camera_shader_resource[0][0].uniform;
        //_graphics->updateBuffer( uniform.buffers[0].buffer, 0, sizeof( data ), &data );
    }

    void RenderManager::updateLights( int size, void* data )
    {
        //_graphics->updateBuffer( _lights_shader_resource[0][0].uniform.buffers[0].buffer, 0, size, data );
    }

    void RenderManager::submit( const RenderObject& object )
    {
        _render_queue.submit( object );
    }
    
    RenderQueue& RenderManager::getRenderQueue()
    {
        return _render_queue;
    }

    void RenderManager::drawObjects( CommandEncoder* encoder, RenderPass* pass, const ShaderPipeline& pipeline, const std::vector< RenderObject >& objects )
    {
        encoder->bindGraphicsPipeline( pipeline.handle() );
        
        for ( const PipelineResourceBinding& binding : pipeline->global_resources )
        {
            switch ( binding.type )
            {
                case BindingType::SHADER_RESOURCE:
                {
                    const ShaderResource* resource = pass->fetchShaderResource( binding.name );
                    encoder->bindShaderResource( *resource );
                    break;
                }

                case BindingType::BUFFER:
                {
                    //const BufferHandle* buffer = pass->fetchBuffer( binding.name );
                    //encoder->bind( pipeline->link( binding.set, binding.binding, *buffer ) );
                    Log::error << "TODO: encoder->bind( pipeline->link( binding.set, binding.binding, *buffer ) )" << Log::nl;
                    return;
                    break;
                }

                case BindingType::TEXTURE:
                {
                    //const ImageHandle* texture = pass->fetchImage( binding.name );
                    //encoder->bind( pipeline->link( binding.set, binding.binding, *texture ) );
                    Log::error << "TODO: encoder->bind( pipeline->link( binding.set, binding.binding, *texture ) )" << Log::nl;
                    return;
                    break;
                }

                case BindingType::PUSH_CONSTANTS:
                {
                    //const PushConstant* constant = pass->fetchPushConstant( binding.name );
                    //encoder->setPushConstants( constant.stages, constant.offset, constants.size, constants.data );
                    Log::error << "TODO: encoder->setPushConstants( ... )" << Log::nl;
                    return;
                    break;
                }

                default:
                    Log::error << "unsupported: encoder->bind( " <<binding.name <<" )" << Log::nl;
                    return;
                    break;
            }
        }
        
        for ( const RenderObject& object : objects )
        {
            if ( object.material )
            {
                for (int i=0; i<object.material->resources.size(); ++i)
                {
                    encoder->bindShaderResource( object.material->resources[i] );
                }
            }
            encoder->setPushConstants
            (
                object.constant.stages,
                object.constant.offset,
                object.constant.size,
                object.constant.data
            );
            drawMesh( encoder, object.mesh );
        }
    }

    void RenderManager::drawMesh( CommandEncoder* encoder, const Ref< MeshSource >& mesh )
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

    void RenderManager::drawIndices( CommandEncoder* encoder, const Ref< MeshSource >& mesh )
    {
        encoder->bindIndexBuffer( mesh->primative->index_buffer, 0, false );
        if ( mesh->indirect_draw_buffer_list && mesh->instance_buffer_list )
        {
            for (int i=0; i<mesh->indirect_draw_buffer_list->buffers.size(); ++i )
            {
                const IndirectDrawBuffer& indirect_buffer = mesh->indirect_draw_buffer_list->buffers[i];
                const InstanceBuffer& instance_buffer = mesh->instance_buffer_list->buffers[i];

                encoder->bindShaderResource( instance_buffer.shader_resource );
                encoder->drawIndexIndirect
                (
                    indirect_buffer.buffer,
                    indirect_buffer.offset,
                    indirect_buffer.count,
                    indirect_buffer.stride
                );
            }
        }
        else if ( mesh->indirect_draw_buffer_list )
        {
            for (const IndirectDrawBuffer& indirect_buffer : mesh->indirect_draw_buffer_list->buffers )
            {
                encoder->drawIndexIndirect
                (
                    indirect_buffer.buffer,
                    indirect_buffer.offset,
                    indirect_buffer.count,
                    indirect_buffer.stride
                );
            }
        }
        else if ( mesh->instance_buffer_list )
        {
            for (const InstanceBuffer& instance : mesh->instance_buffer_list->buffers )
            {
                encoder->bindShaderResource( instance.shader_resource );
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
    
    void RenderManager::drawArrays( CommandEncoder* encoder, const Ref< MeshSource >& mesh )
    {
        if ( mesh->indirect_draw_buffer_list && mesh->instance_buffer_list )
        {
            for (int i=0; i<mesh->indirect_draw_buffer_list->buffers.size(); ++i )
            {
                const IndirectDrawBuffer& indirect_buffer = mesh->indirect_draw_buffer_list->buffers[i];
                const InstanceBuffer& instance_buffer = mesh->instance_buffer_list->buffers[i];
                encoder->bindShaderResource( instance_buffer.shader_resource );
                encoder->drawIndirect
                (
                    indirect_buffer.buffer,
                    indirect_buffer.offset,
                    indirect_buffer.count,
                    indirect_buffer.stride
                );
            }
        }
        else if ( mesh->indirect_draw_buffer_list )
        {
            for (const IndirectDrawBuffer& indirect_buffer : mesh->indirect_draw_buffer_list->buffers )
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
        else if ( mesh->instance_buffer_list )
        {
            for (const InstanceBuffer& instance : mesh->instance_buffer_list->buffers )
            {
                encoder->bindShaderResource( instance.shader_resource );
                encoder->draw
                (
                    mesh->index_count,
                    instance.instance_count,
                    mesh->first_index,
                    instance.first_instance
                );
            }
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

//    void executeRenderPass( kege::RenderPassContext* context )
//    {
//        RenderGraph* graph = 0;
//        graph->defnShaderResource
//        ({
//            .name = "frames_in_flight",
//            .frames_in_flight = 1,
//            .bindings =
//            {
//                RgShaderResrcDesc
//                {
//                    .targets = {{RgResrcType::Image, "color"}},
//                    .type    = DescriptorType::CombinedImageSampler,
//                    .name    = "",
//                    .binding = 0,
//                    .count   = 1,
//                }
//            }
//        });
//    }
    
    void RenderManager::execute( RenderPass* pass )
    {
        const RenderPassQueue& queue = _render_queue.getQueueForPass( pass->getType() );
        CommandEncoder* encoder = pass->getCommandEncoder();

        if ( pass->hasFixedPipelines() )
        {
            const std::vector< ShaderPipeline >& pipelines = pass->getShaderPipelines();
            for ( const ShaderPipeline& pipeline : pipelines )
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
                const std::vector< RenderObject >& objects = i->second;
                const ShaderPipeline& pipeline = i->first;
                drawObjects( encoder, pass, pipeline, objects );
            }
        }
    }

    bool RenderManager::initialize()
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
//                .stage_flags = ShaderStage::Vertex,
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
//                        .usage = BufferUsage::UniformBuffer,
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
//                .stage_flags = ShaderStage::Fragment,
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
//                        .usage = BufferUsage::StorageBuffer,
//                        .memory_usage = MemoryUsage::CpuToGpu,
//                    })
//                }
//            }}
//        };
//        _lights_shader_resource.update({});


        _sphere = new MeshSource;
        _sphere->primative = new EllipsoidMesh(1, 1, 16, 16);
        _sphere->upload( _graphics );

        _cube = new MeshSource;
        _cube->primative = new CuboidMesh(0, 1);
        _cube->upload( _graphics );

        _cube = new MeshSource;
        _cube->first_instance = 0;
        _cube->instance_count = 1;
        _cube->first_index = 0;
        _cube->index_count = 4;

        return true;
    }

    void RenderManager::shutdown()
    {
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
