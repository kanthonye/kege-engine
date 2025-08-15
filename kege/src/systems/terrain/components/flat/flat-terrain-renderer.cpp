//
//  flat-terrain-renderer.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/12/25.
//

#include "flat-terrain.hpp"
#include "flat-terrain-renderer.hpp"

namespace kege{

    void FlatTerrainRenderer::submit( const FlatTerrainPatch& patch )
    {
        if ( _batch_count >= MAX_INSTANCE_BUFFER_COUNT )
        {
            return;
        }

        if
        (
            _batchs[ _curr_batch ].instance_count >= MAX_INSTANCE_BUFFER_SIZE ||
            _batchs[ _curr_batch ].image_index != patch.image_index ||
            _curr_batch >= _batch_count || _batch_count == 0
        )
        {
            _curr_batch = newBatch( patch.image_index );
        }

        uint32_t index = _batchs[ _curr_batch ].instance_count;
        int drawcount = _quad_mesh_index_buffers.getIndexBufferDrawCount( patch.index_buffer_id );

        _batchs[ _curr_batch ].params[ index ].first_instance = index;
        _batchs[ _curr_batch ].params[ index ].instance_count = 1;
        _batchs[ _curr_batch ].params[ index ].vertex_count = drawcount;
        _batchs[ _curr_batch ].params[ index ].first_vertex = 0;
        _batchs[ _curr_batch ].patchs[ index ] = patch;

        _batchs[ _curr_batch ].instance_count++;
    }

    uint32_t FlatTerrainRenderer::newBatch( uint32_t image_index )
    {
        if ( _batchs[ _batch_count ].draw_buffer == nullptr )
        {
            kege::Buffer draw_buffer = kege::Graphics::createIndirectBuffer
            (
                kege::DYNAMIC_BUFFER,
                MAX_INSTANCE_BUFFER_SIZE * sizeof( DrawParams ),
                nullptr
            );

            kege::Buffer storage_buffer = kege::Graphics::createStorageBuffer
            (
                kege::DYNAMIC_BUFFER,
                MAX_INSTANCE_BUFFER_SIZE * sizeof( FlatTerrainPatch ),
                nullptr
            );

            _batchs[ _batch_count ].resource_set = kege::Graphics::createResourceSet({{ 0, "PatchBuffer" }}, { storage_buffer });
//            _batchs[ _batch_count ].patch_buffer = kege::ShaderResourceLayout::create
//            (
//                {
//                    0, "PatchBuffer",
//                    1, graphicsDESCRIPTOR_TYPE_STORAGE_BUFFER,
//                    0, graphicsVERTEX_SHADER
//                },
//                { storage_buffer }
//            );
            _batchs[ _batch_count ].draw_buffer = draw_buffer;
            _batchs[ _batch_count ].instance_count = 0;

            _batchs[ _batch_count ].params.mapBuffer( draw_buffer );
            _batchs[ _batch_count ].patchs.mapBuffer( storage_buffer );
        }

        //_batchs[ _batch_count ].params.mapBuffer( _batchs[ _batch_count ].draw_buffer );
        //_batchs[ _batch_count ].patchs.mapBuffer( _batchs[ _batch_count ].patch_buffer->_shader_resources );
        _batchs[ _batch_count ].image_index = image_index;
        _batchs[ _batch_count ].instance_count = 0;

        return _batch_count++;
    }

    void FlatTerrainRenderer::beginRender( kege::CommandBuffer& command_buffer )
    {
        _curr_batch = 0;
        _batch_count = 0;
        _terrain->stats.instances  = 0;
    }

    void FlatTerrainRenderer::endRender( kege::CommandBuffer& command_buffer )
    {
        if ( _shader_pipeline == nullptr || _terrain->imageLayerManager()->empty() ) return;

        const ResourceSet& resource = _terrain->imageLayerManager()->getShaderResourceLayout();

        mat44 m[2];
        m[1][0].x = _terrain->settings()->terrain_diameter;
        m[1][0].y = _terrain->settings()->min_height;
        m[1][0].z = _terrain->settings()->max_height;

        command_buffer->bindShaderPipeline( _shader_pipeline );
        command_buffer->bindResource( _shader_pipeline, _quad_mesh_index_buffers.getGeometryBuffer() );
        command_buffer->bindResource( _shader_pipeline, resource );
        command_buffer->pushConstants
        (
            _shader_pipeline,
            {
                .info = { kege::VERTEX_SHADER, 0, sizeof(m) },
                .data = m
            }
        );

        for ( int i=0; i<_batch_count; ++i )
        {
            command_buffer->bindResource( _shader_pipeline, _batchs[i].resource_set );
            command_buffer->drawIndirect( _batchs[i].draw_buffer, 0, _batchs[i].instance_count, sizeof( DrawParams ) );

            _terrain->stats.instances += _batchs[i].instance_count;
        }
        _terrain->stats.drawcount = _batch_count;
    }

    bool FlatTerrainRenderer::initialize()
    {
        _shader_pipeline = ShaderPipelineLibrary::load( "/Users/kae/Projects/game-engine-projects/kege/kege/assets/shaders/glsl/graphics/terrain/flat/shader.json" );
        if ( !_shader_pipeline )
        {
            return false;
        }

        _batchs.resize( MAX_INSTANCE_BUFFER_COUNT );
        _batchs[0].instance_count = 0;
        _batchs[0].image_index = 0;

        _quad_mesh_index_buffers.initialize();
        return true;
    }

    void FlatTerrainRenderer::shutdown()
    {
        for (int i=0; i<_batchs.size(); ++i)
        {
            Graphics::freeResourceSet( _batchs[i].resource_set );
            _batchs[i].draw_buffer.clear();
        }
        _quad_mesh_index_buffers.destroy();
        _shader_pipeline = nullptr;
        _batchs.clear();
    }

    FlatTerrainRenderer::~FlatTerrainRenderer()
    {
        shutdown();
    }

    FlatTerrainRenderer::FlatTerrainRenderer( FlatTerrain* terrain )
    :   _shader_pipeline( nullptr )
    ,   _curr_batch( 0 )
    ,   _batch_count( 0 )
    ,   _terrain( terrain )
    {}
}

