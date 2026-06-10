//
//  spherical-terrain-renderer.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#include "spherical-terrain-renderer.hpp"

namespace kege{

    const kege::mat33& SphericalTerrainRenderer::getFaceAxies( int face_index )const
    {
        return _face_axies[ face_index ];
    }
    
    const kege::vec4* SphericalTerrainRenderer::getFaceVertices( int face_index )const
    {
        return &_face_vertices[ face_index ];
    }

    uint32_t getCapSize( uint32_t count, uint32_t base, uint32_t exponent )
    {
        uint32_t exp = ceil( ::log( count ) / ::log( base ) );
        exp = kege::min( exp, exponent );
        uint32_t size = pow( base, exp );
        return ( size < base ) ? base : size;
    }
    
    bool SphericalTerrainRenderer::initialize()
    {
        std::vector< int > face_indices;
        generateCubeVertces( 1.0, _face_axies, _face_vertices );
        generateCubeIndices( face_indices, _draw_commands );
/*
        _mesh_primitive = new MeshPrimitive;
        _mesh_primitive->vertex_buffer = _graphics->createBuffer
        ({
            .size = _face_vertices.size() * sizeof( _face_vertices[0] ),
            .data = _face_vertices.data(),
            .usage = BufferUsages::VertexBuffer,
            .memory_usage = MemoryUsage::GpuOnly,
            .name = "terrain_cube_mesh_vertices"
        });
        _mesh_primitive->index_buffer = _graphics->createBuffer
        ({
            .size = face_indices.size() * sizeof( face_indices[0] ),
            .data = face_indices.data(),
            .usage = BufferUsages::IndexBuffer,
            .memory_usage = MemoryUsage::GpuOnly,
            .name = "terrain_cube_mesh_indices"
        });
*/

        return true;
    }

    void SphericalTerrainRenderer::submit( int face_id, int index_buffer_id, vec4& patch )
    {
/*
        if( this->sources.empty() )
        {
            _current_batch = newBatch();
            _batch_count += 1;
        }
        else if( _current_batch.capacity <= _patch_count )
        {
            if( this->sources.size() <= _batch_count )
            {
                _current_batch = newBatch();
            }
            else
            {
                _current_batch = mapBatch( _batch_count );
            }
            _patch_count = 0;
            _batch_count += 1;
        }
*/
        //_current_batch.patches[ _patch_count ] = patch;
        _current_batch.patchs[ _patch_count ] = patch;
    
//        _current_batch.draw_commands[ _patch_count ] = _draw_commands[ index_buffer_id ];
//        _current_batch.draw_commands[ _patch_count ].first_instance = _patch_count;
//        _current_batch.draw_commands[ _patch_count ].vertex_offset = 9 * face_id;


        _current_batch.draw_commands[ _patch_count ].first_index = _draw_commands[ index_buffer_id ].first_index;
        _current_batch.draw_commands[ _patch_count ].index_count = _draw_commands[ index_buffer_id ].index_count;
        _current_batch.draw_commands[ _patch_count ].first_instance = _patch_count;
        _current_batch.draw_commands[ _patch_count ].instance_count = 1;
        _current_batch.draw_commands[ _patch_count ].vertex_offset = 9 * face_id;


        _patch_count += 1;
    }

    SphericalTerrainRenderer::Batch SphericalTerrainRenderer::mapBatch( int index )
    {
//        kege::Ref< ShaderResrc >& sr = this->sources[ index ]->instance_buffer_list->buffers[0].resource;
//        void* patchs = (*sr)[0][0].uniform.buffers[0].buffer->map();
//        void* draw_commands = this->sources[ index ]->indirect_draw_buffer_list->buffers[0].buffer->map();
//
//        return Batch
//        {
//            .draw_commands = reinterpret_cast< kege::IndexDrawCommand* >( draw_commands ),
//            .patchs = reinterpret_cast< kege::vec4* >( patchs ),
//            .capacity = _max_draw_count,
//            .index = index
//        };
        return {};
    }

    void SphericalTerrainRenderer::endBatch()
    {
//        if ( _current_batch.draw_commands != nullptr )
//        {
//            this->sources[ _current_batch.index ]->indirect_draw_buffer_list->buffers[0].offset = 0;
//            this->sources[ _current_batch.index ]->indirect_draw_buffer_list->buffers[0].count = _patch_count;
//
//            kege::Ref< ShaderResrc >& sr =this->sources[ _current_batch.index ]->instance_buffer_list->buffers[0].resource;
//            (*sr)[0][0].uniform.buffers[0].buffer->unmap();
//
//            this->sources[ _current_batch.index ]->indirect_draw_buffer_list->buffers[0].buffer->unmap();
//
//            _current_batch.draw_commands = nullptr;
//            _current_batch.patchs = nullptr;
//            _current_batch.capacity = 0;
//            _current_batch.index = 0;
//        }
    }

    SphericalTerrainRenderer::Batch SphericalTerrainRenderer::newBatch()
    {
        /*
        kege::IndirectDrawCommandBuffer draw_commands =
        {
            .count  = 0,
            .offset = 0,
            .stride = _draw_command_stride,
            .buffer = _graphics->createBuffer
            ({
                .size  = _max_draw_count * _draw_command_stride,
                .data  = nullptr,
                .usage = kege::BufferUsages::IndirectBuffer,
                .memory_usage = kege::MemoryUsage::CpuToGpu,
                .name  = "terrain_draw_buffer"
            })
        };
        _mesh->setIndirectDrawCommandBuffer(0, draw_commands);


        _mesh->setInstanceBufferBinding(0, kege::BufferBindings{});
         */
//        _mesh->instance_draw_object->resize(_mesh->instance_draw_object->size() + 1);
//        _mesh->instance_draw_object->at(0) = InstanceDrawBuffer{};

//        new IndirectDrawObject
//        ({
//        }),


//        endBatch();
//
//        int index = static_cast< int >( this->sources.size() );
//        size_t patch_buffer_size = _max_draw_count * _patch_stride;
//
//        MeshSourceRef mesh_source = new Mesh
//        (
//            _mesh_primitive,
//            new IndirectDrawBufferList
//            ({
//                IndirectDrawBuffer
//                {
//                    .count = 0,
//                    .offset = 0,
//                    .stride = _draw_command_stride,
//                    .buffer = _graphics->createBuffer
//                    ({
//                        .size = _max_draw_count * _draw_command_stride,
//                        .data = nullptr,
//                        .usage = BufferUsages::IndirectBuffer,
//                        .memory_usage = MemoryUsage::CpuToGpu,
//                        .name = "terrain_draw_buffer"
//                    })
//                }
//            }),
//            new InstanceBufferList
//            ({
//                InstanceBuffer
//                {
//                    .resource = new ShaderResrc(UniformResourceLayout{
//                        .descriptors = UniformDescriptorSets
//                        {
//                            UniformDescriptorSet
//                            {
//                                .set = 1,
//                                .descriptors =
//                                {
//                                    UniformDescriptor
//                                    {
//                                        .count = 1,
//                                        .binding = 0,
//                                        .name = "terrain_draw_buffer",
//                                        .descriptor_type = DescriptorType::StorageBuffer,
//                                    }
//                                }
//                            }
//                        },
//                        .resources = UniformResourceSets
//                        {
//                            UniformResourceSet
//                            {
//                                UniformResource
//                                {
//                                    .binding = 0,
//                                    .uniform = BufferBindings
//                                    {
//                                        BufferInfo
//                                        {
//                                            .offset = 0,
//                                            .range = patch_buffer_size,
//                                            .buffer = _graphics->createBuffer
//                                            ({
//                                                .size = patch_buffer_size,
//                                                .data = nullptr,
//                                                .usage = BufferUsages::StorageBuffer,
//                                                .memory_usage = MemoryUsage::CpuToGpu,
//                                                .name = "terrain_patch_buffer"
//                                            })
//                                        }
//                                    }
//                                }
//                            }
//                        },
//                        .graphics = _graphics
//                    })
//                }
//            })
//        );
//        mesh_source->material_index = 0;
//        this->sources.push_back( mesh_source );
//
//        return mapBatch( index );
        return {};
    }

    void SphericalTerrainRenderer::bind( kege::CommandBuffer* cmd ) const
    {}

    void SphericalTerrainRenderer::begin()
    {
        _batch_count = 0;
        _patch_count = 0;
        _total_draws = 0;
    }

    void SphericalTerrainRenderer::end()
    {
        endBatch();
    }

    void SphericalTerrainRenderer::generateCubeVertces
    (
        float scale,
        std::array< kege::mat33, MAX_CUBE_FACES >& face_axies,
        std::vector< kege::vec4 >& face_vertices
    )
    {
        kege::vec3 points[ 8 ];
        points[ 0 ] = kege::vec3(  scale,  scale,  scale );
        points[ 1 ] = kege::vec3( -scale,  scale,  scale );
        points[ 2 ] = kege::vec3( -scale, -scale,  scale );
        points[ 3 ] = kege::vec3(  scale, -scale,  scale );

        points[ 4 ] = kege::vec3(  scale,  scale, -scale );
        points[ 5 ] = kege::vec3( -scale,  scale, -scale );
        points[ 6 ] = kege::vec3( -scale, -scale, -scale );
        points[ 7 ] = kege::vec3(  scale, -scale, -scale );

        std::vector< kege::vec3 > cube_face_quads[ 6 ];
        cube_face_quads[ CUBE_FACE_FRONT ] = { points[ 0 ], points[ 1 ], points[ 2 ], points[ 3 ] };
        cube_face_quads[ CUBE_FACE_BACK  ] = { points[ 5 ], points[ 4 ], points[ 7 ], points[ 6 ] };
        cube_face_quads[ CUBE_FACE_LEFT  ] = { points[ 1 ], points[ 5 ], points[ 6 ], points[ 2 ] };
        cube_face_quads[ CUBE_FACE_RIGHT ] = { points[ 4 ], points[ 0 ], points[ 3 ], points[ 7 ] };
        cube_face_quads[ CUBE_FACE_ABOVE ] = { points[ 2 ], points[ 6 ], points[ 7 ], points[ 3 ] };
        cube_face_quads[ CUBE_FACE_BELOW ] = { points[ 5 ], points[ 1 ], points[ 0 ], points[ 4 ] };

        face_vertices.resize( 9 * MAX_CUBE_FACES );
        for (int face = 0; face < MAX_CUBE_FACES; face++ )
        {
            face_axies[ face ][ 0 ] = kege::normalize( cube_face_quads[ face ][ 3 ] - cube_face_quads[ face ][ 2 ] );
            face_axies[ face ][ 1 ] = kege::normalize( cube_face_quads[ face ][ 1 ] - cube_face_quads[ face ][ 2 ] );
            face_axies[ face ][ 2 ] = kege::cross( face_axies[ face ][ 0 ], face_axies[ face ][ 1 ] );

            int vertex_index = face * 9;
            for (float y = -1.f; y <= 1.f; y += 1.f )
            {
                for (float x = -1.f; x <= 1.f; x += 1.f )
                {
                    face_vertices[ vertex_index ].x = x * face_axies[ face ][ 0 ].x + y * face_axies[ face ][ 1 ].x;
                    face_vertices[ vertex_index ].y = x * face_axies[ face ][ 0 ].y + y * face_axies[ face ][ 1 ].y;
                    face_vertices[ vertex_index ].z = x * face_axies[ face ][ 0 ].z + y * face_axies[ face ][ 1 ].z;
                    face_vertices[ vertex_index ].w = scale;
                    std::cout <<face_vertices[ vertex_index ] <<"\n";
                    vertex_index++;
                }
                std::cout <<"\n";
            }
            std::cout <<"\n";
        }

    }

    void SphericalTerrainRenderer::generateCubeIndices
    (
        std::vector< int >& face_indices,
        std::array< IndexDrawCommand, 16 >& draw_commands
    )
    {
        const uint16_t index_buffer_element_count = 24;
        const uint16_t index_buffer_count = 16;

        uint16_t vertex_offset = 0;
        uint16_t index_offset = 0;
        uint16_t i;

        face_indices.resize( index_buffer_element_count * index_buffer_count );
        // zero composite indices
        memset( face_indices.data(), 0, face_indices.size() *sizeof(face_indices[0]) );

        i = 0; // 0000 = 0
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\       /|
        face_indices[ index_offset +  2 ] =  2; // | \     / |
        face_indices[ index_offset +  3 ] =  2; // |  \   /  |
        face_indices[ index_offset +  4 ] =  4; // |   \ /   |
        face_indices[ index_offset +  5 ] =  8; // 3    4    5
        face_indices[ index_offset +  6 ] =  8; // |   / \   |
        face_indices[ index_offset +  7 ] =  4; // |  /   \  |
        face_indices[ index_offset +  8 ] =  6; // | /     \ |
        face_indices[ index_offset +  9 ] =  6; // |/       \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 12;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 1; // 0001 = 1
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\   |   /|
        face_indices[ index_offset +  2 ] =  1; // | \  |  / |
        face_indices[ index_offset +  3 ] =  1; // |  \ | /  |
        face_indices[ index_offset +  4 ] =  4; // |   \|/   |
        face_indices[ index_offset +  5 ] =  2; // 3    4    5
        face_indices[ index_offset +  6 ] =  2; // |   / \   |
        face_indices[ index_offset +  7 ] =  4; // |  /   \  |
        face_indices[ index_offset +  8 ] =  8; // | /     \ |
        face_indices[ index_offset +  9 ] =  8; // |/       \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  6;
        face_indices[ index_offset + 12 ] =  6;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 15;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 2; // 0010 = 2
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\       /|
        face_indices[ index_offset +  2 ] =  2; // | \     / |
        face_indices[ index_offset +  3 ] =  2; // |  \   /  |
        face_indices[ index_offset +  4 ] =  4; // |   \ /   |
        face_indices[ index_offset +  5 ] =  5; // 3    4----5
        face_indices[ index_offset +  6 ] =  5; // |   / \   |
        face_indices[ index_offset +  7 ] =  4; // |  /   \  |
        face_indices[ index_offset +  8 ] =  8; // | /     \ |
        face_indices[ index_offset +  9 ] =  8; // |/       \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  6;
        face_indices[ index_offset + 12 ] =  6;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 15;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 3; // 0011 = 3
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\   |   /|
        face_indices[ index_offset +  2 ] =  1; // | \  |  / |
        face_indices[ index_offset +  3 ] =  1; // |  \ | /  |
        face_indices[ index_offset +  4 ] =  4; // |   \|/   |
        face_indices[ index_offset +  5 ] =  2; // 3    4----5
        face_indices[ index_offset +  6 ] =  2; // |   / \   |
        face_indices[ index_offset +  7 ] =  4; // |  /   \  |
        face_indices[ index_offset +  8 ] =  4; // | /     \ |
        face_indices[ index_offset +  9 ] =  5; // |/       \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  8;
        face_indices[ index_offset + 12 ] =  8;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  6;
        face_indices[ index_offset + 15 ] =  6;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 18;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 4; // 0100 = 4
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\       /|
        face_indices[ index_offset +  2 ] =  2; // | \     / |
        face_indices[ index_offset +  3 ] =  2; // |  \   /  |
        face_indices[ index_offset +  4 ] =  4; // |   \ /   |
        face_indices[ index_offset +  5 ] =  8; // 3    4    5
        face_indices[ index_offset +  6 ] =  8; // |   /|\   |
        face_indices[ index_offset +  7 ] =  4; // |  / | \  |
        face_indices[ index_offset +  8 ] =  7; // | /  |  \ |
        face_indices[ index_offset +  9 ] =  7; // |/   |   \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  6;
        face_indices[ index_offset + 12 ] =  6;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 15;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 5; // 0101 = 5
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\   |   /|
        face_indices[ index_offset +  2 ] =  1; // | \  |  / |
        face_indices[ index_offset +  3 ] =  1; // |  \ | /  |
        face_indices[ index_offset +  4 ] =  4; // |   \|/   |
        face_indices[ index_offset +  5 ] =  2; // 3    4    5
        face_indices[ index_offset +  6 ] =  2; // |   /|\   |
        face_indices[ index_offset +  7 ] =  4; // |  / | \  |
        face_indices[ index_offset +  8 ] =  8; // | /  |  \ |
        face_indices[ index_offset +  9 ] =  8; // |/   |   \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  7;
        face_indices[ index_offset + 12 ] =  7;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  6;
        face_indices[ index_offset + 15 ] =  6;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 18;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 6; // 0110 = 6
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\       /|
        face_indices[ index_offset +  2 ] =  2; // | \     / |
        face_indices[ index_offset +  3 ] =  2; // |  \   /  |
        face_indices[ index_offset +  4 ] =  4; // |   \ /   |
        face_indices[ index_offset +  5 ] =  5; // 3    4----5
        face_indices[ index_offset +  6 ] =  5; // |   /|\   |
        face_indices[ index_offset +  7 ] =  4; // |  / | \  |
        face_indices[ index_offset +  8 ] =  8; // | /  |  \ |
        face_indices[ index_offset +  9 ] =  8; // |/   |   \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  7;
        face_indices[ index_offset + 12 ] =  7;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  6;
        face_indices[ index_offset + 15 ] =  6;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 18;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 7; // 0111 = 7
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\   |   /|
        face_indices[ index_offset +  2 ] =  1; // | \  |  / |
        face_indices[ index_offset +  3 ] =  1; // |  \ | /  |
        face_indices[ index_offset +  4 ] =  4; // |   \|/   |
        face_indices[ index_offset +  5 ] =  2; // 3    4----5
        face_indices[ index_offset +  6 ] =  2; // |   /|\   |
        face_indices[ index_offset +  7 ] =  4; // |  / | \  |
        face_indices[ index_offset +  8 ] =  5; // | /  |  \ |
        face_indices[ index_offset +  9 ] =  5; // |/   |   \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  8;
        face_indices[ index_offset + 12 ] =  8;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  7;
        face_indices[ index_offset + 15 ] =  7;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  6;
        face_indices[ index_offset + 18 ] =  6;
        face_indices[ index_offset + 19 ] =  4;
        face_indices[ index_offset + 20 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 21;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 8; // 1000 = 8
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\       /|
        face_indices[ index_offset +  2 ] =  2; // | \     / |
        face_indices[ index_offset +  3 ] =  2; // |  \   /  |
        face_indices[ index_offset +  4 ] =  4; // |   \ /   |
        face_indices[ index_offset +  5 ] =  8; // 3----4    5
        face_indices[ index_offset +  6 ] =  8; // |   / \   |
        face_indices[ index_offset +  7 ] =  4; // |  /   \  |
        face_indices[ index_offset +  8 ] =  6; // | /     \ |
        face_indices[ index_offset +  9 ] =  6; // |/       \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  3;
        face_indices[ index_offset + 12 ] =  3;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  0;
        face_indices[ index_offset + 15 ] =  0;
        face_indices[ index_offset + 16 ] =  0;
        face_indices[ index_offset + 17 ] =  0;
        face_indices[ index_offset + 18 ] =  0;
        face_indices[ index_offset + 19 ] =  0;
        face_indices[ index_offset + 20 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 15;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 9; // 1001 = 9
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\   |   /|
        face_indices[ index_offset +  2 ] =  1; // | \  |  / |
        face_indices[ index_offset +  3 ] =  1; // |  \ | /  |
        face_indices[ index_offset +  4 ] =  4; // |   \|/   |
        face_indices[ index_offset +  5 ] =  2; // 3----4    5
        face_indices[ index_offset +  6 ] =  2; // |   / \   |
        face_indices[ index_offset +  7 ] =  4; // |  /   \  |
        face_indices[ index_offset +  8 ] =  8; // | /     \ |
        face_indices[ index_offset +  9 ] =  8; // |/       \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  6;
        face_indices[ index_offset + 12 ] =  6;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  3;
        face_indices[ index_offset + 15 ] =  3;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  0;
        face_indices[ index_offset + 18 ] =  0;
        face_indices[ index_offset + 19 ] =  0;
        face_indices[ index_offset + 20 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 18;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 10; // 1010 = 10
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\       /|
        face_indices[ index_offset +  2 ] =  2; // | \     / |
        face_indices[ index_offset +  3 ] =  2; // |  \   /  |
        face_indices[ index_offset +  4 ] =  4; // |   \ /   |
        face_indices[ index_offset +  5 ] =  5; // 3----4----5
        face_indices[ index_offset +  6 ] =  5; // |   / \   |
        face_indices[ index_offset +  7 ] =  4; // |  /   \  |
        face_indices[ index_offset +  8 ] =  8; // | /     \ |
        face_indices[ index_offset +  9 ] =  8; // |/       \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  6;
        face_indices[ index_offset + 12 ] =  6;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  3;
        face_indices[ index_offset + 15 ] =  3;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  0;
        face_indices[ index_offset + 18 ] =  0;
        face_indices[ index_offset + 19 ] =  0;
        face_indices[ index_offset + 20 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 18;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 11; // 1011 = 11
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\   |   /|
        face_indices[ index_offset +  2 ] =  1; // | \  |  / |
        face_indices[ index_offset +  3 ] =  1; // |  \ | /  |
        face_indices[ index_offset +  4 ] =  4; // |   \|/   |
        face_indices[ index_offset +  5 ] =  2; // 3----4----5
        face_indices[ index_offset +  6 ] =  2; // |   / \   |
        face_indices[ index_offset +  7 ] =  4; // |  /   \  |
        face_indices[ index_offset +  8 ] =  5; // | /     \ |
        face_indices[ index_offset +  9 ] =  5; // |/       \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  8;
        face_indices[ index_offset + 12 ] =  8;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  6;
        face_indices[ index_offset + 15 ] =  6;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  3;
        face_indices[ index_offset + 18 ] =  3;
        face_indices[ index_offset + 19 ] =  4;
        face_indices[ index_offset + 20 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 21;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 12; // 1100 = 12
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\       /|
        face_indices[ index_offset +  2 ] =  2; // | \     / |
        face_indices[ index_offset +  3 ] =  2; // |  \   /  |
        face_indices[ index_offset +  4 ] =  4; // |   \ /   |
        face_indices[ index_offset +  5 ] =  8; // 3----4    5
        face_indices[ index_offset +  6 ] =  8; // |   /|\   |
        face_indices[ index_offset +  7 ] =  4; // |  / | \  |
        face_indices[ index_offset +  8 ] =  7; // | /  |  \ |
        face_indices[ index_offset +  9 ] =  7; // |/   |   \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  6;
        face_indices[ index_offset + 12 ] =  6;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  3;
        face_indices[ index_offset + 15 ] =  3;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  0;
        face_indices[ index_offset + 18 ] =  0;
        face_indices[ index_offset + 19 ] =  0;
        face_indices[ index_offset + 20 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 18;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 13; // 1101 = 13
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\   |   /|
        face_indices[ index_offset +  2 ] =  1; // | \  |  / |
        face_indices[ index_offset +  3 ] =  1; // |  \ | /  |
        face_indices[ index_offset +  4 ] =  4; // |   \|/   |
        face_indices[ index_offset +  5 ] =  2; // 3----4    5
        face_indices[ index_offset +  6 ] =  2; // |   /|\   |
        face_indices[ index_offset +  7 ] =  4; // |  / | \  |
        face_indices[ index_offset +  8 ] =  8; // | /  |  \ |
        face_indices[ index_offset +  9 ] =  8; // |/   |   \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  7;
        face_indices[ index_offset + 12 ] =  7;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  6;
        face_indices[ index_offset + 15 ] =  6;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  3;
        face_indices[ index_offset + 18 ] =  3;
        face_indices[ index_offset + 19 ] =  4;
        face_indices[ index_offset + 20 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 21;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 14; // 1110 = 14
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\       /|
        face_indices[ index_offset +  2 ] =  2; // | \     / |
        face_indices[ index_offset +  3 ] =  2; // |  \   /  |
        face_indices[ index_offset +  4 ] =  4; // |   \ /   |
        face_indices[ index_offset +  5 ] =  5; // 3----4----5
        face_indices[ index_offset +  6 ] =  5; // |   /|\   |
        face_indices[ index_offset +  7 ] =  4; // |  / | \  |
        face_indices[ index_offset +  8 ] =  8; // | /  |  \ |
        face_indices[ index_offset +  9 ] =  8; // |/   |   \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  7;
        face_indices[ index_offset + 12 ] =  7;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  6;
        face_indices[ index_offset + 15 ] =  6;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  3;
        face_indices[ index_offset + 18 ] =  3;
        face_indices[ index_offset + 19 ] =  4;
        face_indices[ index_offset + 20 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 21;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        i = 15; // 1111 = 15
        index_offset  = i * index_buffer_element_count;
        face_indices[ index_offset +  0 ] =  0; // 0----1----2
        face_indices[ index_offset +  1 ] =  4; // |\   |   /|
        face_indices[ index_offset +  2 ] =  1; // | \  |  / |
        face_indices[ index_offset +  3 ] =  1; // |  \ | /  |
        face_indices[ index_offset +  4 ] =  4; // |   \|/   |
        face_indices[ index_offset +  5 ] =  2; // 3----4----5
        face_indices[ index_offset +  6 ] =  2; // |   /|\   |
        face_indices[ index_offset +  7 ] =  4; // |  / | \  |
        face_indices[ index_offset +  8 ] =  5; // | /  |  \ |
        face_indices[ index_offset +  9 ] =  5; // |/   |   \|
        face_indices[ index_offset + 10 ] =  4; // 6----7----8
        face_indices[ index_offset + 11 ] =  8;
        face_indices[ index_offset + 12 ] =  8;
        face_indices[ index_offset + 13 ] =  4;
        face_indices[ index_offset + 14 ] =  7;
        face_indices[ index_offset + 15 ] =  7;
        face_indices[ index_offset + 16 ] =  4;
        face_indices[ index_offset + 17 ] =  6;
        face_indices[ index_offset + 18 ] =  6;
        face_indices[ index_offset + 19 ] =  4;
        face_indices[ index_offset + 20 ] =  3;
        face_indices[ index_offset + 21 ] =  3;
        face_indices[ index_offset + 22 ] =  4;
        face_indices[ index_offset + 23 ] =  0;

        draw_commands[ i ].first_instance = 0;
        draw_commands[ i ].instance_count = 1;
        draw_commands[ i ].first_index    = index_offset;
        draw_commands[ i ].index_count    = 24;
        draw_commands[ i ].vertex_offset  = vertex_offset;

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

    }

    SphericalTerrainRenderer::~SphericalTerrainRenderer()
    {
    }

    SphericalTerrainRenderer::SphericalTerrainRenderer( GraphicsDevice* graphics )
    :   _terrain( nullptr )
    ,   _graphics( graphics )
    ,   _batch_count( 0 )
    ,   _patch_count( 0 )
    ,   _total_draws( 0 )
    ,   _exponent( 6 )
    ,   _base( 8 )
    ,   _draw_command_stride( (uint32_t)sizeof( IndexDrawCommand ) )
    ,   _patch_stride( (uint32_t)sizeof( PatchData ) )
    ,   _max_draw_count( 16000 )
    {}
}
