//
//  spherical-terrain-renderer.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#include "spherical-terrain-renderer.hpp"

namespace kege{

    uint32_t getCapSize( uint32_t count, uint32_t base, uint32_t exponent )
    {
        uint32_t exp = ceil( ::log( count ) / ::log( base ) );
        exp = kege::min( exp, exponent );
        uint32_t size = pow( base, exp );
        return ( size < base ) ? base : size;
    }

    bool SphericalTerrainRenderer::initialize()
    {
        kege::Vec4< uint32_t > cube_face_indices[ 16 ][ 24 ];
        for (int j,i = 0; i < 16; i++ )
        {
            for (j = 0; j < 24; j++ )
            {
                cube_face_indices[ i ][ j ].x = _cubemesh.face_indices[ i ].data[ j ];
            }
        }

        size_t vertices_size = sizeof( _cubemesh.face_vertices );
        size_t indices_size = sizeof( kege::Vec4< uint32_t > ) * 16 * 24;

        _vertices_uniform_buffer = _graphics->createBuffer
        ({
            .size = vertices_size,
            .data = _cubemesh.face_vertices,
            .usage = BufferUsage::UniformBuffer,
            .memory_usage = MemoryUsage::GpuOnly,
            .name = "terrain_cube_mesh_vertices"
        });
        
        _indices_uniform_buffer = _graphics->createBuffer
        ({
            .size = indices_size,
            .data = cube_face_indices,
            .usage = BufferUsage::UniformBuffer,
            .memory_usage = MemoryUsage::GpuOnly,
            .name = "terrain_cube_mesh_indices"
        });

//        _mesh_shader_resource = _graphics->allocateDescriptorSet
//        ({
//            {
//                .binding = 0,
//                .count = 1,
//                .descriptor_type = DescriptorType::UniformBuffer,
//                .stage_flags = ShaderStage::Vertex
//            },
//            {
//                .binding = 1,
//                .count = 1,
//                .descriptor_type = DescriptorType::UniformBuffer,
//                .stage_flags = ShaderStage::Vertex
//            }
//        });
//
//        _graphics->updateDescriptorSets
//        ({
//            {
//                .set = _mesh_shader_resource,
//                .binding = 0,
//                .array_element = 0,
//                .descriptor_type = DescriptorType::UniformBuffer,
//                .buffer_info =
//                {{
//                    .buffer = _vertices_uniform_buffer,
//                    .range  = vertices_size,
//                    .offset = 0,
//                }}
//            },
//            {
//                .set = _mesh_shader_resource,
//                .binding = 1,
//                .array_element = 0,
//                .descriptor_type = DescriptorType::UniformBuffer,
//                .buffer_info =
//                {{
//                    .buffer = _indices_uniform_buffer,
//                    .range  = indices_size,
//                    .offset = 0,
//                }}
//            }
//        });

        return true;
    }

    const DynamicCubeMesh* SphericalTerrainRenderer::getDynamicCubeMesh()const
    {
        return &_cubemesh;
    }
    
//    void SphericalTerrainRenderer::prepareGeometries( std::vector< char >& buffer )
//    {
//        /**
//         * Given the buffer, determine how many instance can fit into the buffer. This is done
//         * by determining how many PatchData and DrawParams pair can fit into the given buffer.
//         * Dividing the buffer size by the sum size of PatchData and DrawParams.
//         */
//        _max_instance_count = (uint32_t)(buffer.size() / ( _drawparam_stride + _patchdata_stride ));
//
//        /**
//         * Next compute the size for the DrawParams buffer. This is then use to set where the
//         * PatchData buffer start with in the given buffer
//         */
//        uint64_t draw_param_buffer_size = _max_instance_count * _drawparam_stride;
//
//        /**
//         * Next set the _patch_buffer and _draw_param_buffer pointers. The given buffer is used
//         * to temporarly store render data, Once the buffer ran out of space it is dump into a
//         * storage-buffer and draw-param-buffer and then reused.
//         */
//        _patch_buffer = reinterpret_cast< PatchData* >( &buffer[ draw_param_buffer_size ] );
//        _draw_param_buffer = reinterpret_cast< kege::DrawParams* >( &buffer[ 0 ] );
//
//        /**
//         * With the temp buffer divide update the quadtrees
//         */
//
//        _total_draws = 0;
//        _instance_count = 0;
//        _current_drawbatch = 0;
//
//        prepareGeometries( _cube_faces[ 0 ].root );
//        prepareGeometries( _cube_faces[ 1 ].root );
//        prepareGeometries( _cube_faces[ 2 ].root );
//        prepareGeometries( _cube_faces[ 3 ].root );
//        prepareGeometries( _cube_faces[ 4 ].root );
//        prepareGeometries( _cube_faces[ 5 ].root );
//
//        if ( _instance_count > 0 )
//        {
//            submitDrawParamsAndPatchBuffer();
//            _instance_count = 0;
//        }
//
//        static int tot = 0;
//        if ( tot != _total_nodes )
//        {
//            tot = _total_nodes;
//            _total_leaves = (3 * _total_nodes + 1) / 4;
//            std::cout <<"[ total-draws: " << _total_draws <<"[ total-leaves: " << _total_leaves;
//            std::cout << " ][ total-nodes: "<< _total_nodes << " ][ total-levels: " << _total_levels <<" ]\n";
//        }
//    }

    void SphericalTerrainRenderer::draw( SphericalTerrainTile& node )
    {
        if ( _instance_count >= _max_instance_count )
        {
            flush();
            _instance_count = 0;
        }

        const PatchIndices* patch_indices = &_cubemesh.face_indices[ node.patch.patch_index_id ];

        /**
         * Add the current draw-command to the draw-command-buffer. The patch mesh is made up of 9 vertices,
         * and the patch-indices tell the GPU how to draw the patch. There are a total of 24 distinct patch
         * index array. each with there own index count. thus we need to tell the GPU how many indice to draw
         * @note that all the patch-index-data is already uploaded to the GPU.
         */
        _draw_param_buffer[ _instance_count ].vertex_count = patch_indices->draw_count;
        _draw_param_buffer[ _instance_count ].first_instance = _instance_count;
        _draw_param_buffer[ _instance_count ].instance_count = 1;
        _draw_param_buffer[ _instance_count ].first_vertex = 0;

        /**
         * The patch_index_id is used to tell the GPU which patch-index-data to use for draw the current instance.
         * Next we copy the required patch data to the instance buffer and increment the count.
         */
        //memcpy( &_patch_buffer[ _instance_count ], &node.patch, _patchdata_stride );
        _patch_buffer[ _instance_count ].transform = node.patch.transform;
        _patch_buffer[ _instance_count ].patch_index_id = node.patch.patch_index_id;
        _patch_buffer[ _instance_count ].patch_vertex_id = node.patch.patch_vertex_id;
        _instance_count++;
    }

    void SphericalTerrainRenderer::flush()
    {
        const uint32_t max_count = kege::getCapSize( _instance_count, _base, _exponent );
        const uint32_t max_drawcommand_buffer_size = max_count * _drawparam_stride;
        const uint32_t max_storage_buffer_size = max_count * _patchdata_stride;

        if ( _draw_buffers.empty() || _draw_buffers.size() == _current_drawbatch )
        {
            _draw_buffers.push_back({});
            PatchDrawBuffer& buffer = _draw_buffers[ _current_drawbatch ];

            buffer.instance_count = _instance_count;

            buffer.draw_buffer = _graphics->createBuffer
            ({
                .size = max_drawcommand_buffer_size,
                .data = _draw_param_buffer,
                .usage = BufferUsage::UniformBuffer,
                .memory_usage = MemoryUsage::CpuToGpu,
                .name = "terrain_draw_buffer"
            });

            buffer.patch_buffer = _graphics->createBuffer
            ({
                .size = max_storage_buffer_size,
                .data = _patch_buffer,
                .usage = BufferUsage::UniformBuffer,
                .memory_usage = MemoryUsage::CpuToGpu,
                .name = "terrain_patch_buffer"
            });

//            buffer.descriptor_set = _graphics->allocateDescriptorSet
//            ({
//                {
//                    .binding = 0,
//                    .count = 1,
//                    .descriptor_type = DescriptorType::StorageBuffer,
//                    .stage_flags = ShaderStage::Vertex
//                }
//            });
//
//            _graphics->updateDescriptorSets
//            ({
//                {
//                    .set = buffer.descriptor_set,
//                    .binding = 0,
//                    .array_element = 0,
//                    .descriptor_type = DescriptorType::StorageBuffer,
//                    .buffer_info =
//                    {{
//                        .buffer = buffer.patch_buffer,
//                        .range  = max_storage_buffer_size,
//                        .offset = 0,
//                    }}
//                }
//            });
        }
        else
        {
            PatchDrawBuffer& buffer = _draw_buffers[ _current_drawbatch ];

            if ( _graphics->bufferSize( buffer.patch_buffer ) < max_storage_buffer_size )
            {
                _graphics->destroyBuffer( buffer.patch_buffer );
                _graphics->destroyBuffer( buffer.draw_buffer );
                buffer.instance_count = _instance_count;

                buffer.draw_buffer = _graphics->createBuffer
                ({
                    .size = max_drawcommand_buffer_size,
                    .data = _draw_param_buffer,
                    .usage = BufferUsage::UniformBuffer,
                    .memory_usage = MemoryUsage::CpuToGpu,
                    .name = "terrain_draw_buffer"
                });

                buffer.patch_buffer = _graphics->createBuffer
                ({
                    .size = max_storage_buffer_size,
                    .data = _patch_buffer,
                    .usage = BufferUsage::UniformBuffer,
                    .memory_usage = MemoryUsage::CpuToGpu,
                    .name = "terrain_patch_buffer"
                });

//                _graphics->updateDescriptorSets
//                ({
//                    {
//                        .set = buffer.descriptor_set,
//                        .binding = 0,
//                        .array_element = 0,
//                        .descriptor_type = DescriptorType::StorageBuffer,
//                        .buffer_info =
//                        {{
//                            .buffer = buffer.patch_buffer,
//                            .range  = max_storage_buffer_size,
//                            .offset = 0,
//                        }}
//                    }
//                });
            }
            else
            {
                _graphics->updateBuffer( buffer.draw_buffer, 0, max_drawcommand_buffer_size, _draw_param_buffer );
                _graphics->updateBuffer( buffer.patch_buffer, 0, max_storage_buffer_size, _patch_buffer );
                //buffer.draw_buffer.updateBufferData({ 0, max_drawcommand_buffer_size, _draw_param_buffer });
                //buffer.patch_buffer.updateBufferData({ 0, max_storage_buffer_size, _patch_buffer });
                buffer.instance_count = _instance_count;
            }
        }

        _current_drawbatch++;
    }

    void SphericalTerrainRenderer::begin( kege::CommandEncoder* encoder, Transform* transform )
    {
        /**
         * Given the buffer, determine how many instance can fit into the buffer. This is done
         * by determining how many PatchData and DrawParams pair can fit into the given buffer.
         * Dividing the buffer size by the sum size of PatchData and DrawParams.
         */
        _max_instance_count = (uint32_t)(_temp_buffer.size() / ( _drawparam_stride + _patchdata_stride ));

        /**
         * Next compute the size for the DrawParams buffer. This is then use to set where the
         * PatchData buffer start with in the given buffer
         */
        uint64_t draw_param_buffer_size = _max_instance_count * _drawparam_stride;

        /**
         * Next set the _patch_buffer and _draw_param_buffer pointers. The given buffer is used
         * to temporarly store render data, Once the buffer ran out of space it is dump into a
         * storage-buffer and draw-param-buffer and then reused.
         */
        _patch_buffer = reinterpret_cast< PatchData* >( &_temp_buffer[ draw_param_buffer_size ] );
        _draw_param_buffer = reinterpret_cast< kege::VertexDrawCommand* >( &_temp_buffer[ 0 ] );

        /**
         * With the temp buffer divide update the quadtrees
         */

        _total_draws = 0;
        _instance_count = 0;
        _current_drawbatch = 0;
    }

    void SphericalTerrainRenderer::end()
    {
        if ( _instance_count > 0 )
        {
            flush();
            _instance_count = 0;
        }
    }

//    void SphericalTerrainRenderer::draw( kege::CommandBuffer* command_buffer )const
//    {
//        /*
//        ShaderPipeline* pipeline = ShaderPipelineLibrary::get( "spherical-terrain-shader" );
//        command_buffer->bindShaderResource( *pipeline, _cubemesh->mesh_shader_resource );
//
//        for ( const PatchDrawBuffer& buffer : _draw_buffers )
//        {
//            command_buffer->bindShaderResource( *pipeline, buffer.descriptor_set );
//            command_buffer->drawIndirect( buffer.draw_buffer, 0, buffer.instance_count, _drawparam_stride );
//        }
//         */
//    }
//
//    void SphericalTerrainRenderer::bind( kege::CommandBuffer* command_buffer )const
//    {
////        uint32_t set_index = 1;
////        command_buffer->bindShaderResource( _cubemesh->mesh_shader_resource, set_index );
//    }

    SphericalTerrainRenderer::~SphericalTerrainRenderer()
    {
    }

    SphericalTerrainRenderer::SphericalTerrainRenderer()
    :   _terrain( nullptr )
    ,   _graphics( nullptr )
    ,   _cubemesh( 1.0 )
    ,   _patch_buffer( nullptr )
    ,   _draw_param_buffer( nullptr )
    ,   _max_instance_count( 0 )
    ,   _instance_count( 0 )
    ,   _total_draws( 0 )
    ,   _exponent( 6 )
    ,   _base( 8 )
    ,   _drawparam_stride( (uint32_t)sizeof( VertexDrawCommand ) )
    ,   _patchdata_stride( (uint32_t)sizeof( PatchData ) )
    {}
}
