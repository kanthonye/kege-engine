//
//  render-pass-render-functions.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 4/24/26.
//

#include "command-buffer.hpp"
#include "render-functions.hpp"
#include "mesh.hpp"
#include "../renderer.hpp"

namespace kege{

    void drawMesh
    (
        kege::CommandBuffer* command_buffer,
        const kege::Mesh* mesh,
        uint32_t first_element,
        uint32_t element_count,
        uint32_t first_instance,
        uint32_t instance_count,
        int32_t vertex_offset
    )
    {
        if ( !mesh->getIndirectDrawCommandBuffers().empty() && mesh->getVertexBuffer() != nullptr )
        {
            if ( mesh->getIndexBuffer() != nullptr )
            {
                const IndirectDrawCommandBuffers& draw_commands = mesh->getIndirectDrawCommandBuffers();
                for (int i=0; i<draw_commands.size(); ++i )
                {
                    const IndirectDrawCommandBuffer& indirect_buffer = draw_commands.at(i);
                    command_buffer->drawIndexIndirect
                    (
                        indirect_buffer.buffer,
                        indirect_buffer.offset,
                        indirect_buffer.count,
                        indirect_buffer.stride
                    );
                }
            }
            else
            {
                const IndirectDrawCommandBuffers& draw_commands = mesh->getIndirectDrawCommandBuffers();
                for (int i=0; i<draw_commands.size(); ++i )
                {
                    const IndirectDrawCommandBuffer& indirect_buffer = draw_commands.at(i);
                    command_buffer->drawIndirect
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
            if ( mesh->getIndexBuffer() != nullptr )
            {
                command_buffer->drawIndexed
                (
                    element_count,
                    instance_count,
                    first_element,
                    vertex_offset,
                    first_instance
                );
            }
            else
            {
                command_buffer->draw
                (
                    element_count,
                    instance_count,
                    first_element,
                    first_instance
                );
            }
        }
    }

    void renderDeferredStaticGeometry(kege::RenderPassContext& context)
    {
        const RenderBatchPtrs& batchs = context.view->render_pass_batchs[ (int)context.pass_desc->pass ];
        if ( batchs.empty() ) return;

        kege::CommandBuffer* cmd = context.command_buffer.ref();
        kege::AssetManager* asset_manager = context.renderer->getAssetManager();

        for (RenderBatch* batch : batchs)
        {
            kege::ref::ShaderPipeline& pipeline = *asset_manager->get< kege::ref::ShaderPipeline >( batch->pipeline_id );
            //kege::ref::Material* material = asset_manager->get< kege::ref::Material >( batch->material_id );
            kege::ref::Mesh& mesh = *asset_manager->get< kege::ref::Mesh >( batch->mesh_id );
            
            cmd->bindShaderPipeline( pipeline );
            mesh->bind( cmd );

            if ( !context.pass_desc->reads.empty() )
            {
                // bind global render pass read resources such as Camera, Lights, Textures
                kege::ShaderResourceBinder* binder = context.renderer->getShaderResourceBinder
                ( pipeline.ref(), context.pass_desc );
                binder->bind( cmd );
            }

            //material_manager->resource_binder->bind( cmd );
            //texture_manager->resource_binder->bind( cmd );
            //if( material ) (*material)->bind( cmd );
            
            cmd->setPushBlock
            (
                kege::ShaderStageFlag::Vertex | kege::ShaderStageFlag::Fragment,
                0, sizeof(context.view->camera), &context.view->camera
            );
            drawMesh
            (
                cmd,
                mesh.ref() ,
                batch->first_element,
                batch->element_count,
                batch->first_instance,
                batch->instance_count,
                batch->vertex_offset
            );
        }
    }

    void renderUI(kege::RenderPassContext& context)
    {
//        const RenderBatchPtrs& batchs = context.view->render_pass_batchs[ (int)context.pass_type ];
//        if ( batchs.empty() ) return;
//
//        kege::CommandBuffer* cmd = context.command_buffer.ref();
//        kege::AssetManager* asset_manager = context.renderer->getAssetManager();
//
//        for (RenderBatch* batch : batchs)
//        {
//            kege::ref::ShaderPipeline& pipeline = *asset_manager->get< kege::ref::ShaderPipeline >( batch->pipeline_id );
//            kege::ref::Mesh& mesh = *asset_manager->get< kege::ref::Mesh >( batch->mesh_id );
//
//            cmd->bindShaderPipeline( pipeline );
//            mesh->bind( cmd );
//
//            if( context.pass_bindings != nullptr )
//            {
//                kege::ShaderResourceBinder* binder = context.renderer->getShaderResourceBinder
//                ( pipeline.ref(), context.pass_bindings );
//                binder->bind( cmd );
//            }
//
//            if( pipeline->checkFeature( kege::FeatureFlag::INSTANCED_RENDERING ) )
//            {}
//            else
//            {
//                if( context.pass_type == kege::RenderPassType::UI )
//                {
//                    cmd->setPushBlock
//                    (
//                        kege::ShaderStageFlag::Vertex | kege::ShaderStageFlag::Fragment,
//                        0, 64, &context.view->camera.projection
//                    );
//                }
//                else
//                {
//                    for( int i = 0; i < batch->instances.size(); ++i )
//                    {
//                        drawMesh
//                        (
//                            cmd,
//                            mesh.ref() ,
//                            batch->first_element,
//                            batch->element_count,
//                            batch->first_instance,
//                            batch->instance_count,
//                            batch->vertex_offset
//                        );
//                    }
//                }
//            }
//        }
    }


    void forwardRenderingStaticMesh(kege::RenderPassContext& context)
    {
    }

}
