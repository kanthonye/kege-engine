//
//  render-particle-system.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/29/24.
//

#include "billboard-particle-renderer.hpp"

namespace kege{

    void BillboardParticleRenderer::operator()( kege::RenderPassContext* context )
    {
        if ( context->name() != "geometry" )
        {
            return;
        }

        DescriptorSetHandle camera_descriptor = context->getPhysicalDescriptorSet( "camera-descriptor" );
        if( !camera_descriptor ) return;

        kege::Graphics* graphics = context->getGraphics();

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

        encoder->bindGraphicsPipeline( _pipeline );
        encoder->bindDescriptorSets( camera_descriptor );

        ModelMatrices model_matrices;

        for ( kege::Entity entity : *_entities )
        {
            ParticleBuffer* buffer = entity.get< ParticleBuffer >();
            Transform* transform = entity.get< Transform >();

            if ( !_storage_buffer )
            {
                _storage_buffer = graphics->createBuffer
                ({
                    .size = buffer->particle_count * sizeof( BillboardParticleData ),
                    .data = nullptr,
                    .memory_usage = MemoryUsage::CpuToGpu,
                    .usage = BufferUsage::VertexBuffer
                });
            }

            BillboardParticleData* data = reinterpret_cast< BillboardParticleData* >( graphics->mapBuffer( _storage_buffer ) );
            for( uint32_t i = 0; i < buffer->particle_count ; i += 1 )
            {
                data[i].size     = buffer->particles[ i ].size;
                data[i].color    = buffer->particles[ i ].color;
                data[i].position = buffer->particles[ i ].position;
                data[i].rotation = buffer->particles[ i ].rotation;
                data[i].sprite   = buffer->particles[ i ].sprite;
            }
            graphics->unmapBuffer( _storage_buffer );

            model_matrices = ModelMatrices( *transform );

            encoder->bindVertexBuffers( 0, { _storage_buffer }, { 0 });
            encoder->setPushConstants(ShaderStage::Vertex, 0, sizeof( model_matrices ), &model_matrices );
            encoder->draw( 4, buffer->particle_count, 0, 0 );
        }
    }

    bool BillboardParticleRenderer::initialize()
    {
        _pipeline = _engine->graphics()->getShaderPipelineManager()->get( "billboard-particle-shader" );
        if( !_pipeline )
        {
            KEGE_LOG_ERROR << "billboard-particle-shader not loaded.";
            return false;
        }

        _comm.add< kege::RenderPassContext*, BillboardParticleRenderer>( this );
        return EntitySystem::initialize();
    }

    void BillboardParticleRenderer::shutdown()
    {
        _comm.remove< kege::RenderPassContext*, BillboardParticleRenderer >( this );
        EntitySystem::shutdown();
    }

    BillboardParticleRenderer::BillboardParticleRenderer( kege::Engine* engine )
    :   kege::EntitySystem( engine, "billboard-particle-rendering-system", 0 )
    {
        _signature = kege::createEntitySignature< kege::Transform, kege::ParticleBuffer, kege::BillboardSprite >();
    }


    KEGE_REGISTER_SYSTEM( BillboardParticleRenderer, "billboard-particle-rendering-system" );
}
