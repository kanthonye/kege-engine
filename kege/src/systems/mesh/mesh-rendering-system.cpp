//
//  mesh-rendering-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/18/25.
//

#include "mesh-rendering-system.hpp"

namespace kege{

    void MeshRenderingSystem::operator()( kege::RenderPassContext* context )
    {
//        kege::PipelineHandle pipeline = context->graphics()->getShaderPipelineManager()->get( "triangle-shader" );
//        if ( pipeline )
//        {
//            kege::CommandEncoder* encoder = context->getCommandBuffer()->createCommandEncoder();
//
//            encoder->setScissor
//            ({
//                0, 0,
//                context->getRenderArea().extent.width,
//                context->getRenderArea().extent.height
//            });
//            encoder->setViewport
//            ({
//                0, 0,
//                static_cast<float>(context->getRenderArea().extent.width),
//                static_cast<float>(context->getRenderArea().extent.height)
//            });
//
//            encoder->bindGraphicsPipeline( pipeline );
//            encoder->draw( 3, 1, 0, 0 );
//        }


        CommandEncoder* encoder = context->getCommandBuffer()->createCommandEncoder();
        encoder->setScissor
        ({
            0, 0,
            context->getRenderArea().extent.width,
            context->getRenderArea().extent.height
        });
        encoder->setViewport
        ({
            0, 0,
            static_cast<float>( context->getRenderArea().extent.width ),
            static_cast<float>( context->getRenderArea().extent.height )
        });

        kege::PipelineHandle pipeline = context->getGraphics()->getShaderPipelineManager()->get( "basic-shader" );
        if( !pipeline ) return;

        DescriptorSetHandle camera_descriptor = context->getPhysicalDescriptorSet( "camera-descriptor" );
        if( !camera_descriptor ) return;

        kege::AssetSystem* assets = &_engine->scene()->getAssetSystem();

        encoder->bindGraphicsPipeline( pipeline );
        encoder->bindDescriptorSets( camera_descriptor );

        for (kege::Entity entity : *_entities )
        {
            kege::Ref< kege::Mesh >* mesh = entity.get< kege::Ref< kege::Mesh > >();
            Transform* transform = entity.get< Transform >();

            if ( mesh == nullptr ) continue;

            kege::Ref< kege::Mesh > resmesh = *mesh;//assets->get< kege::Ref< kege::Mesh > >( mesh->resource );
            if ( resmesh == nullptr ) continue;;

            if( !resmesh->vertex_buffer )
            {
                resmesh->init( context->getGraphics() );
            }

            encoder->bindVertexBuffers( 0, { resmesh->vertex_buffer }, { 0 });
            encoder->bindIndexBuffer( resmesh->index_buffer, 0, false );

            ModelMatrices model;
            model(transform->position, transform->orientation, transform->scale);
            encoder->setPushConstants(ShaderStage::Vertex, 0, sizeof( model ), &model );

            for (int i=0; i<resmesh->primatives.size(); ++i)
            {
                resmesh->primatives[i]->draw( encoder );
            }
        }
        
    }

    bool MeshRenderingSystem::initialize()
    {
        _comm.add< kege::RenderPassContext*, kege::MeshRenderingSystem >( this );
        return EntitySystem::initialize();
    }

    void MeshRenderingSystem::shutdown()
    {
        _comm.remove< kege::RenderPassContext*, kege::MeshRenderingSystem >( this );
        EntitySystem::shutdown();
    }

    MeshRenderingSystem::MeshRenderingSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "mesh-rendering-system", 0 )
    {
        _signature = createEntitySignature< kege::Ref< kege::Mesh >, kege::Transform >();
    }


    KEGE_REGISTER_SYSTEM( MeshRenderingSystem, "mesh-rendering" );
}
