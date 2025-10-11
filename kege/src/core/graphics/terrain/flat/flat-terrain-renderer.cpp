//
//  flat-terrain-renderer.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/12/25.
//

#include "flat-terrain-renderer.hpp"

namespace kege{

    void FlatTerrainRenderer::begin( kege::CommandEncoder* encoder, Transform* transform )
    {
        _curr_batch = 0;
        _batch_count = 0;
        _encoder = encoder;
    }

    void FlatTerrainRenderer::end()
    {
        //if ( _shader_pipeline == nullptr || _terrain->imageLayerManager()->empty() ) return;

//        const ShaderResource& resource = _image_layer_manager.getShaderResource();
//
//        mat44 m[2];
//        m[1][0].x = _settings->terrain_diameter;
//        m[1][0].y = _settings->min_height;
//        m[1][0].z = _settings->max_height;
//
//        _encoder->bindGraphicsPipeline( _shader_pipeline );
//        _encoder->bindVertexBuffers(0, { _vertex_buffer }, { 0 });
//        _encoder->bindIndexBuffer( _index_buffer, 0, false );
//        _encoder->setPushConstants( kege::ShaderStage::Vertex, 0, sizeof(m), m );
//        _encoder->bindShaderResource( resource );
//
//        for ( int i=0; i<_batch_count; ++i )
//        {
//            _encoder->bindShaderResource( _batchs[i].resource_set );
//            _encoder->drawIndirect( _batchs[i].draw_buffer, 0, _batchs[i].instance_count, sizeof( VertexDrawCommand ) );
//            //_instances += _batchs[i].instance_count;
//        }
        //_drawcount = _batch_count;
    }

    kege::ImageLayerManager& FlatTerrainRenderer::getImageLayerManager()
    {
        return _image_layer_manager;
    }
    
    void FlatTerrainRenderer::submit( const FlatTerrainPatch& patch )
    {
        if ( _batch_count >= MAX_INSTANCE_BUFFER_COUNT )
        {
            _curr_batch = flush( patch.image_index );
        }

        if
        (
            _batchs[ _curr_batch ].instance_count >= MAX_INSTANCE_BUFFER_SIZE ||
            _batchs[ _curr_batch ].image_index != patch.image_index ||
            _curr_batch >= _batch_count || _batch_count == 0
        )
        {
            _curr_batch = flush( patch.image_index );
        }

        uint32_t index = _batchs[ _curr_batch ].instance_count;
        int drawcount = _index_draw_commands[ patch.index_buffer_id ].index_count;

        _batchs[ _curr_batch ].params[ index ].first_instance = index;
        _batchs[ _curr_batch ].params[ index ].instance_count = 1;
        _batchs[ _curr_batch ].params[ index ].vertex_count = drawcount;
        _batchs[ _curr_batch ].params[ index ].first_vertex = 0;
        _batchs[ _curr_batch ].patchs[ index ] = patch;

        _batchs[ _curr_batch ].instance_count++;
    }

    uint32_t FlatTerrainRenderer::flush( int image_index )
    {
        Batch& batch = _batchs[ _batch_count ];
        if ( _batchs[ _batch_count ].draw_buffer )
        {
            size_t storage_buffer_size = MAX_INSTANCE_BUFFER_SIZE * sizeof( FlatTerrainPatch );

            batch.storage_buffer = _graphics->createBuffer
            ({
                .size = storage_buffer_size,
                .data = nullptr,
                .usage = BufferUsage::StorageBuffer,
                .memory_usage = MemoryUsage::CpuToGpu,
                .name = "terrain-storage-buffer"
            });

            batch.instance_count = 0;
            batch.draw_buffer = _graphics->createBuffer
            ({
                .size = MAX_INSTANCE_BUFFER_SIZE * sizeof( IndexDrawCommand ),
                .data = nullptr,
                .usage = BufferUsage::IndirectBuffer,
                .memory_usage = MemoryUsage::CpuToGpu,
                .name = "terrain-draw-command-buffer"
            });

//TODO:            UniformLayoutDescription descriptors =
//            {
//                {
//                    .binding = 0,
//                    .count = 1,
//                    .descriptor_type = DescriptorType::StorageBuffer,
//                    .stage_flags = ShaderStage::Vertex
//                }
//            };
//            _graphics->allocateShaderResources( descriptors, 1, &batch.resource_set );
//            batch.resource_set[0] = BufferBinding
//            {
//                .binding = 0,
//                .buffers =
//                {{
//                    .buffer = batch.storage_buffer,
//                    .range  = storage_buffer_size,
//                    .offset = 0,
//                }}
//            };
//            _graphics->updateShaderResource( _batchs[ _batch_count ].resource_set );

            /*
            ( layout_config, 1, &batch.resource_set );
            const std::vector< kege::Uniform > resource_bindings =
            {{{
                {
                    .buffer = batch.storage_buffer,
                    .range  = storage_buffer_size,
                    .offset = 0,
                }
            }}};

            _graphics->updateDescriptorSets
            ({
                {
                    .set = _batchs[ _batch_count ].resource_set,
                    .binding = 0,
                    .array_element = 0,
                    .descriptor_type = DescriptorType::StorageBuffer,
                    .buffer_info =
                    {{
                        .buffer = _batchs[ _batch_count ].storage_buffer,
                        .range  = storage_buffer_size,
                        .offset = 0,
                    }}
                }
            });
             */

            _batchs[ _batch_count ].params = (VertexDrawCommand*) _graphics->mapBuffer( _batchs[ _batch_count ].draw_buffer );
            _batchs[ _batch_count ].patchs = (FlatTerrainPatch*) _graphics->mapBuffer( _batchs[ _batch_count ].storage_buffer );
        }

        //_batchs[ _batch_count ].params.mapBuffer( _batchs[ _batch_count ].draw_buffer );
        //_batchs[ _batch_count ].patchs.mapBuffer( _batchs[ _batch_count ].patch_buffer->_shader_resources );
        _batchs[ _batch_count ].image_index = image_index;
        _batchs[ _batch_count ].instance_count = 0;
        _batch_count++;
        
        return _batch_count;
    }

    bool FlatTerrainRenderer::initialize( const kege::TerrainSettings* settings )
    {
//TODO:        _shader_pipeline = PipelineLoader::load( _graphics, kege::vfs( "graphics-shaders/terrain/flat/shader.json" ).c_str() );
//        if ( !_shader_pipeline )
//        {
//            return false;
//        }
//
//        _batchs.resize( MAX_INSTANCE_BUFFER_COUNT );
//        _batchs[0].instance_count = 0;
//        _batchs[0].image_index = 0;
//
        bool init = _image_layer_manager.initialize
        (
            _settings->heightmap_diameter,
            _settings->heightmap_diameter,
            _settings->max_image_array_layers
        );
        return init && initializeMesh();
    }

    bool FlatTerrainRenderer::initializeMesh()
    {
        uint1 index_array_size = 24;
        uint1 index_array_count = 16;
        uint1 index_buffer_size = index_array_size * index_array_count;

        _index_draw_commands.resize( index_array_count );
        std::vector< uint32_t > indices( index_buffer_size );
        memset( indices.data(), 0, indices.size() * sizeof( indices[0] ) );
        {
            uint16_t i = 0, j = 0; // 0000 = 0
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 12;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  8; // 3    4    5
            indices[  6 + i ] =  8; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  6; // | /     \ |
            indices[  9 + i ] =  6; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  0;

            j = 1; // 0001 = 1
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 15;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3    4    5
            indices[  6 + i ] =  2; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  8; // | /     \ |
            indices[  9 + i ] =  8; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  0;

            j = 2; // 0010 = 2
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 15;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  5; // 3    4----5
            indices[  6 + i ] =  5; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  8; // | /     \ |
            indices[  9 + i ] =  8; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  0;

            j = 3; // 0011 = 3
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3    4----5
            indices[  6 + i ] =  2; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  4; // | /     \ |
            indices[  9 + i ] =  5; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  8;
            indices[ 12 + i ] =  8;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;

            j = 4; // 0100 = 4
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 15;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  8; // 3    4    5
            indices[  6 + i ] =  8; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  7; // | /  |  \ |
            indices[  9 + i ] =  7; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  0;

            j = 5; // 0101 = 5
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3    4    5
            indices[  6 + i ] =  2; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  8; // | /  |  \ |
            indices[  9 + i ] =  8; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  7;
            indices[ 12 + i ] =  7;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;

            j = 6; // 0110 = 6
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  5; // 3    4----5
            indices[  6 + i ] =  5; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  8; // | /  |  \ |
            indices[  9 + i ] =  8; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  7;
            indices[ 12 + i ] =  7;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;

            j = 7; // 0111 = 7
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 21;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3    4----5
            indices[  6 + i ] =  2; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  5; // | /  |  \ |
            indices[  9 + i ] =  5; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  8;
            indices[ 12 + i ] =  8;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  7;
            indices[ 15 + i ] =  7;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  6;
            indices[ 18 + i ] =  6;
            indices[ 19 + i ] =  4;
            indices[ 20 + i ] =  0;

            j = 8; // 1000 = 8
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 15;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  8; // 3----4    5
            indices[  6 + i ] =  8; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  6; // | /     \ |
            indices[  9 + i ] =  6; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  3;
            indices[ 12 + i ] =  3;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  0;
            indices[ 15 + i ] =  0;
            indices[ 16 + i ] =  0;
            indices[ 17 + i ] =  0;
            indices[ 18 + i ] =  0;
            indices[ 19 + i ] =  0;
            indices[ 20 + i ] =  0;

            j = 9; // 1001 = 9
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3----4    5
            indices[  6 + i ] =  2; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  8; // | /     \ |
            indices[  9 + i ] =  8; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  3;
            indices[ 15 + i ] =  3;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;
            indices[ 18 + i ] =  0;
            indices[ 19 + i ] =  0;
            indices[ 20 + i ] =  0;

            j = 10; // 1010 = 10
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  5; // 3----4----5
            indices[  6 + i ] =  5; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  8; // | /     \ |
            indices[  9 + i ] =  8; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  3;
            indices[ 15 + i ] =  3;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;
            indices[ 18 + i ] =  0;
            indices[ 19 + i ] =  0;
            indices[ 20 + i ] =  0;

            j = 11; // 1011 = 11
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 21;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3----4----5
            indices[  6 + i ] =  2; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  5; // | /     \ |
            indices[  9 + i ] =  5; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  8;
            indices[ 12 + i ] =  8;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  3;
            indices[ 18 + i ] =  3;
            indices[ 19 + i ] =  4;
            indices[ 20 + i ] =  0;

            j = 12; // 1100 = 12
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  8; // 3----4    5
            indices[  6 + i ] =  8; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  7; // | /  |  \ |
            indices[  9 + i ] =  7; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  3;
            indices[ 15 + i ] =  3;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;
            indices[ 18 + i ] =  0;
            indices[ 19 + i ] =  0;
            indices[ 20 + i ] =  0;

            j = 13; // 1101 = 13
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 21;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3----4    5
            indices[  6 + i ] =  2; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  8; // | /  |  \ |
            indices[  9 + i ] =  8; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  7;
            indices[ 12 + i ] =  7;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  3;
            indices[ 18 + i ] =  3;
            indices[ 19 + i ] =  4;
            indices[ 20 + i ] =  0;

            j = 14; // 1110 = 14
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 21;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  5; // 3----4----5
            indices[  6 + i ] =  5; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  8; // | /  |  \ |
            indices[  9 + i ] =  8; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  7;
            indices[ 12 + i ] =  7;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  3;
            indices[ 18 + i ] =  3;
            indices[ 19 + i ] =  4;
            indices[ 20 + i ] =  0;

            j = 15; // 1111 = 15
            i = j * index_array_size;
            _index_draw_commands[ j ].vertex_offset  = 0;
            _index_draw_commands[ j ].first_instance = 0;
            _index_draw_commands[ j ].instance_count = 1;
            _index_draw_commands[ j ].first_index    = i;
            _index_draw_commands[ j ].index_count    = 24;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3----4----5
            indices[  6 + i ] =  2; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  5; // | /  |  \ |
            indices[  9 + i ] =  5; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  8;
            indices[ 12 + i ] =  8;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  7;
            indices[ 15 + i ] =  7;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  6;
            indices[ 18 + i ] =  6;
            indices[ 19 + i ] =  4;
            indices[ 20 + i ] =  3;
            indices[ 21 + i ] =  3;
            indices[ 22 + i ] =  4;
            indices[ 23 + i ] =  0;
        }

        size_t indices_arraysize = indices.size() * sizeof( indices[0] );
        _index_buffer = _graphics->createBuffer
        ({
            .size = indices_arraysize,
            .data = indices.data(),
            .usage = BufferUsage::IndexBuffer,
            .memory_usage = MemoryUsage::GpuOnly,
            .name = "IndexBuffers"
        });

        const uint1 vertex_count = 9;
        const vec4 vertex[ vertex_count ]
        {
            vec4( -1.f,-1.f   , 0.f, 0.f ),
            vec4(  0.f,-1.f   , 0.f, 0.f ),
            vec4(  1.f,-1.f   , 0.f, 0.f ),
            vec4( -1.f, 0.f   , 0.f, 0.f ),
            vec4(  0.f, 0.f   , 0.f, 0.f ),
            vec4(  1.f, 0.f   , 0.f, 0.f ),
            vec4( -1.f, 1.f   , 0.f, 0.f ),
            vec4(  0.f, 1.f   , 0.f, 0.f ),
            vec4(  1.f, 1.f   , 0.f, 0.f )
        };
        _vertex_buffer = _graphics->createBuffer
        ({
            .size = vertex_count * sizeof( vertex[0] ),
            .data = vertex,
            .usage = BufferUsage::VertexBuffer,
            .memory_usage = MemoryUsage::GpuOnly,
            .name = "IndexBuffers"
        });

        return true;
    }
//    uint32_t FlatTerrainRenderer::newBatch( uint32_t image_index )
//    {
//        if ( _batchs[ _batch_count ].draw_buffer == nullptr )
//        {
//            kege::Buffer draw_buffer = kege::Graphics::createIndirectBuffer
//            (
//                kege::DYNAMIC_BUFFER,
//                MAX_INSTANCE_BUFFER_SIZE * sizeof( DrawParams ),
//                nullptr
//            );
//
//            kege::Buffer storage_buffer = kege::Graphics::createStorageBuffer
//            (
//                kege::DYNAMIC_BUFFER,
//                MAX_INSTANCE_BUFFER_SIZE * sizeof( FlatTerrainPatch ),
//                nullptr
//            );
//
//            _batchs[ _batch_count ].resource_set = kege::Graphics::createResourceSet({{ 0, "PatchBuffer" }}, { storage_buffer });
////            _batchs[ _batch_count ].patch_buffer = kege::ShaderResourceLayout::create
////            (
////                {
////                    0, "PatchBuffer",
////                    1, graphicsDESCRIPTOR_TYPE_STORAGE_BUFFER,
////                    0, graphicsVERTEX_SHADER
////                },
////                { storage_buffer }
////            );
//            _batchs[ _batch_count ].draw_buffer = draw_buffer;
//            _batchs[ _batch_count ].instance_count = 0;
//
//            _batchs[ _batch_count ].params.mapBuffer( draw_buffer );
//            _batchs[ _batch_count ].patchs.mapBuffer( storage_buffer );
//        }
//
//        //_batchs[ _batch_count ].params.mapBuffer( _batchs[ _batch_count ].draw_buffer );
//        //_batchs[ _batch_count ].patchs.mapBuffer( _batchs[ _batch_count ].patch_buffer->_shader_resources );
//        _batchs[ _batch_count ].image_index = image_index;
//        _batchs[ _batch_count ].instance_count = 0;
//
//        return _batch_count++;
//    }

//    void FlatTerrainRenderer::beginRender( kege::CommandBuffer& command_buffer )
//    {
//        _curr_batch = 0;
//        _batch_count = 0;
//        _terrain->stats.instances  = 0;
//    }

//    void FlatTerrainRenderer::endRender( kege::CommandBuffer& command_buffer )
//    {
//    }


    void FlatTerrainRenderer::shutdown()
    {
//        for (int i=0; i<_batchs.size(); ++i)
//        {
//            Graphics::freeResourceSet( _batchs[i].resource_set );
//            _batchs[i].draw_buffer.clear();
//        }
//        _quad_mesh_index_buffers.destroy();
//        _shader_pipeline = nullptr;
//        _batchs.clear();
    }

    FlatTerrainRenderer::~FlatTerrainRenderer()
    {
        shutdown();
    }

    FlatTerrainRenderer::FlatTerrainRenderer( kege::Graphics* graphics )
    :   _settings( nullptr )
    ,   _graphics( graphics )
    ,   _encoder( nullptr )
    ,   _curr_batch( 0 )
    ,   _batch_count( 0 )
    {}
}

