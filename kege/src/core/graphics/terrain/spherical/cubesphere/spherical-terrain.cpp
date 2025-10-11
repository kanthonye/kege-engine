//
//  spherical-cube-terrain.cpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/6/24.
//

#include "virtual-directory.hpp"
#include "spherical-terrain.hpp"

namespace kege{

    const SphericalTerrainSettings& SphericalTerrain::getSettings()const
    {
        return _settings;
    }

    TerrainRenderer* SphericalTerrain::getTerrainRenderer()
    {
        return _renderer.ref();
    }

    MaterialSource* SphericalTerrain::getTerrainMaterial()
    {
        return _material.ref();
    }

    bool SphericalTerrain::initialize()
    {
        kege::string shader_file = kege::vfs( "graphics-shaders/terrain/spherical/shader.json" );
        ShaderPipeline pipeline = _settings.graphics->getShaderPipelineManager()->load( shader_file.c_str() );
        if( !pipeline )
        {
            KEGE_LOG_ERROR << "Failed to load pipeline -> " << shader_file << Log::nl;
            return false;
        }
        _material = new MaterialSource
        (
            RenderPassType::Geometry,
            pipeline, false, false
        );

        
        _renderer = new SphericalTerrainRenderer( _settings.graphics );
        _renderer->initialize();

        SphericalTerrainTile* neighbors[6][4];

        neighbors[ CUBE_FACE_FRONT ][ NORTH ] = &_faces[ CUBE_FACE_ABOVE ]._root; // NORTH
        neighbors[ CUBE_FACE_FRONT ][ EAST  ] = &_faces[ CUBE_FACE_RIGHT ]._root; // EAST
        neighbors[ CUBE_FACE_FRONT ][ SOUTH ] = &_faces[ CUBE_FACE_BELOW ]._root; // SOUTH
        neighbors[ CUBE_FACE_FRONT ][ WEST  ] = &_faces[ CUBE_FACE_LEFT  ]._root; // WEST

        neighbors[ CUBE_FACE_BACK ][ NORTH ] = &_faces[ CUBE_FACE_ABOVE ]._root; // NORTH
        neighbors[ CUBE_FACE_BACK ][ EAST  ] = &_faces[ CUBE_FACE_RIGHT ]._root; // EAST
        neighbors[ CUBE_FACE_BACK ][ SOUTH ] = &_faces[ CUBE_FACE_BELOW ]._root; // SOUTH
        neighbors[ CUBE_FACE_BACK ][ WEST  ] = &_faces[ CUBE_FACE_LEFT  ]._root; // WEST

        neighbors[ CUBE_FACE_LEFT ][ NORTH ] = &_faces[ CUBE_FACE_ABOVE ]._root; // NORTH
        neighbors[ CUBE_FACE_LEFT ][ EAST  ] = &_faces[ CUBE_FACE_BACK  ]._root; // EAST
        neighbors[ CUBE_FACE_LEFT ][ SOUTH ] = &_faces[ CUBE_FACE_BELOW ]._root; // SOUTH
        neighbors[ CUBE_FACE_LEFT ][ WEST  ] = &_faces[ CUBE_FACE_FRONT ]._root; // WEST

        neighbors[ CUBE_FACE_RIGHT ][ NORTH ] = &_faces[ CUBE_FACE_ABOVE ]._root; // NORTH
        neighbors[ CUBE_FACE_RIGHT ][ EAST  ] = &_faces[ CUBE_FACE_FRONT ]._root; // EAST
        neighbors[ CUBE_FACE_RIGHT ][ SOUTH ] = &_faces[ CUBE_FACE_BELOW ]._root; // SOUTH
        neighbors[ CUBE_FACE_RIGHT ][ WEST  ] = &_faces[ CUBE_FACE_BACK  ]._root; // WEST

        neighbors[ CUBE_FACE_ABOVE ][ NORTH ] = &_faces[ CUBE_FACE_BACK  ]._root; // NORTH
        neighbors[ CUBE_FACE_ABOVE ][ EAST  ] = &_faces[ CUBE_FACE_RIGHT ]._root; // EAST
        neighbors[ CUBE_FACE_ABOVE ][ SOUTH ] = &_faces[ CUBE_FACE_FRONT ]._root; // SOUTH
        neighbors[ CUBE_FACE_ABOVE ][ WEST  ] = &_faces[ CUBE_FACE_LEFT  ]._root; // WEST

        neighbors[ CUBE_FACE_BELOW ][ NORTH ] = &_faces[ CUBE_FACE_BACK  ]._root; // NORTH
        neighbors[ CUBE_FACE_BELOW ][ EAST  ] = &_faces[ CUBE_FACE_LEFT  ]._root; // EAST
        neighbors[ CUBE_FACE_BELOW ][ SOUTH ] = &_faces[ CUBE_FACE_FRONT ]._root; // SOUTH
        neighbors[ CUBE_FACE_BELOW ][ WEST  ] = &_faces[ CUBE_FACE_RIGHT ]._root; // WEST

        for (int face_id = 0; face_id < MAX_CUBE_FACES; face_id++ )
        {
            _faces[ face_id ].init
            (
                face_id,
                this,
                neighbors[face_id],
                _renderer->getFaceVertices( face_id ),
                _renderer->getFaceAxies( face_id ),
                1.f,
                _settings.radius,
                0
            );
        }

        return true;
    }

    void SphericalTerrain::submitVisibleGeometries()
    {
        _renderer->begin();
        submitVisibleNodes( _faces[ 0 ]._root );
        submitVisibleNodes( _faces[ 1 ]._root );
        submitVisibleNodes( _faces[ 2 ]._root );
        submitVisibleNodes( _faces[ 3 ]._root );
        submitVisibleNodes( _faces[ 4 ]._root );
        submitVisibleNodes( _faces[ 5 ]._root );
        _renderer->end();


        static int tot = 0;
        if ( tot != _total_nodes )
        {
            tot = _total_nodes;
            _total_leaves = (3 * _total_nodes + 1) / 4;
            //std::cout <<"[ total-draws: " << _total_draws <<"[ total-leaves: " << _total_leaves;
            std::cout << " ][ total-nodes: "<< _total_nodes << " ][ total-levels: " << _total_levels <<" ]\n";
        }
    }

    void SphericalTerrain::submitVisibleNodes( SphericalTerrainTile& node )
    {
        if( node.children )
        {
            submitVisibleNodes( node.children->nodes[ 0 ] );
            submitVisibleNodes( node.children->nodes[ 1 ] );
            submitVisibleNodes( node.children->nodes[ 2 ] );
            submitVisibleNodes( node.children->nodes[ 3 ] );
        }
        else if( node.visible )
        {
            _renderer->submit( node.face_id, node.index_buffer_id, node.patch );
        }
    }
//    
//    void SphericalTerrain::init( QuadtreePatchNode& node, const kege::vec3& position, float scale, double radius, int depth, int face_id )
//    {
//        kege::dvec3 sphere_position = _planet_radius * normalize( dvec3( position ) );
//
//        node.sphere = { sphere_position, radius };
//        node.patch.transform = { position, scale };
//        node.patch.patch_vertex_id = face_id;
//        node.children = nullptr;
//        node.depth = depth;
//
////        node.patch.elevations[ 0 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 0 ].xyz ) );
////        node.patch.elevations[ 1 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 1 ].xyz ) );
////        node.patch.elevations[ 2 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 2 ].xyz ) );
////        node.patch.elevations[ 3 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 3 ].xyz ) );
////        node.patch.elevations[ 4 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 4 ].xyz ) );
////        node.patch.elevations[ 5 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 5 ].xyz ) );
////        node.patch.elevations[ 6 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 6 ].xyz ) );
////        node.patch.elevations[ 7 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 7 ].xyz ) );
////        node.patch.elevations[ 8 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 8 ].xyz ) );
////
////        float normal_strength = 16.f;
////        node.normal = {0.f, 0.f, 0.f};
////        node.normal.z = 1.0 / normal_strength;
////        node.normal.x =
////        {
////            node.patch.elevations[0] + 2.f * node.patch.elevations[3] + node.patch.elevations[5] -
////            node.patch.elevations[2] - 2.f * node.patch.elevations[4] - node.patch.elevations[7]
////        };
////        node.normal.y =
////        {
////            node.patch.elevations[0] + 2.f * node.patch.elevations[1] + node.patch.elevations[2] -
////            node.patch.elevations[5] - 2.f * node.patch.elevations[6] - node.patch.elevations[7]
////        };
////        node.normal = normalize( node.normal );
//        _total_nodes++;
//    }
//
//    void SphericalTerrain::prepareGeometries( std::vector< char >& buffer )
//    {
////        /**
////         * Given the buffer, determine how many instance can fit into the buffer. This is done
////         * by determining how many PatchData and DrawParams pair can fit into the given buffer.
////         * Dividing the buffer size by the sum size of PatchData and DrawParams.
////         */
////        _max_instance_count = (uint32_t)(buffer.size() / ( _drawparam_stride + _patchdata_stride ));
////
////        /**
////         * Next compute the size for the DrawParams buffer. This is then use to set where the
////         * PatchData buffer start with in the given buffer
////         */
////        uint64_t draw_param_buffer_size = _max_instance_count * _drawparam_stride;
////
////        /**
////         * Next set the _patch_buffer and _draw_param_buffer pointers. The given buffer is used
////         * to temporarly store render data, Once the buffer ran out of space it is dump into a
////         * storage-buffer and draw-param-buffer and then reused.
////         */
////        _patch_buffer = reinterpret_cast< PatchData* >( &buffer[ draw_param_buffer_size ] );
////        _draw_param_buffer = reinterpret_cast< kege::DrawParams* >( &buffer[ 0 ] );
////
////        /**
////         * With the temp buffer divide update the quadtrees
////         */
////
////        _total_draws = 0;
////        _instance_count = 0;
////        _current_drawbatch = 0;
////
////        prepareGeometries( _cube_faces[ 0 ]._root );
////        prepareGeometries( _cube_faces[ 1 ]._root );
////        prepareGeometries( _cube_faces[ 2 ]._root );
////        prepareGeometries( _cube_faces[ 3 ]._root );
////        prepareGeometries( _cube_faces[ 4 ]._root );
////        prepareGeometries( _cube_faces[ 5 ]._root );
////
////        if ( _instance_count > 0 )
////        {
////            submitDrawParamsAndPatchBuffer();
////            _instance_count = 0;
////        }
////
////        static int tot = 0;
////        if ( tot != _total_nodes )
////        {
////            tot = _total_nodes;
////            _total_leaves = (3 * _total_nodes + 1) / 4;
////            std::cout <<"[ total-draws: " << _total_draws <<"[ total-leaves: " << _total_leaves;
////            std::cout << " ][ total-nodes: "<< _total_nodes << " ][ total-levels: " << _total_levels <<" ]\n";
////        }
//    }
//
//    void SphericalTerrain::prepareGeometries( QuadtreePatchNode& node )
//    {
//        if( node.children )
//        {
//            prepareGeometries( node.children->nodes[ 0 ] );
//            prepareGeometries( node.children->nodes[ 1 ] );
//            prepareGeometries( node.children->nodes[ 2 ] );
//            prepareGeometries( node.children->nodes[ 3 ] );
//        }
//        else if( node.visible )
//        {
//            if ( _instance_count >= _max_instance_count )
//            {
//                submitDrawParamsAndPatchBuffer();
//                _instance_count = 0;
//            }
//
//            const PatchIndices* patch_indices = &_cubemesh->composite_indices[ node.patch.patch_index_id ];
//
//            /**
//             * Add the current draw-command to the draw-command-buffer. The patch mesh is made up of 9 vertices,
//             * and the patch-indices tell the GPU how to draw the patch. There are a total of 24 distinct patch
//             * index array. each with there own index count. thus we need to tell the GPU how many indice to draw
//             * @note that all the patch-index-data is already uploaded to the GPU.
//             */
//            _draw_param_buffer[ _instance_count ].vertex_count = patch_indices->draw_count;
//            _draw_param_buffer[ _instance_count ].first_instance = _instance_count;
//            _draw_param_buffer[ _instance_count ].instance_count = 1;
//            _draw_param_buffer[ _instance_count ].first_vertex = 0;
//
//            /**
//             * The patch_index_id is used to tell the GPU which patch-index-data to use for draw the current instance.
//             * Next we copy the required patch data to the instance buffer and increment the count.
//             */
//            //memcpy( &_patch_buffer[ _instance_count ], &node.patch, _patchdata_stride );
//            _patch_buffer[ _instance_count ].transform = node.patch.transform;
//            _patch_buffer[ _instance_count ].patch_index_id = node.patch.patch_index_id;
//            _patch_buffer[ _instance_count ].patch_vertex_id = node.patch.patch_vertex_id;
//            _instance_count++;
//        }
//    }
//
//    void SphericalTerrain::submitDrawParamsAndPatchBuffer()
//    {
//        /*
//        const uint32_t max_count = kege::getCapSize( _instance_count, _base, _exponent );
//        const uint32_t max_drawcommand_buffer_size = max_count * _drawparam_stride;
//        const uint32_t max_storage_buffer_size = max_count * _patchdata_stride;
//
//        if ( _draw_buffers.empty() || _draw_buffers.size() == _current_drawbatch )
//        {
//            _draw_buffers.push_back({});
//            PatchDrawBuffer& buffer = _draw_buffers[ _current_drawbatch ];
//
//            buffer.instance_count = _instance_count;
//
//            buffer.draw_buffer = kege::DrawParamBuffer
//            ({
//                max_drawcommand_buffer_size,
//                _draw_param_buffer,
//                kege::DYNAMIC_BUFFER
//            });
//
//            buffer.patch_buffer = kege::StorageBuffer
//            ({
//                max_storage_buffer_size,
//                _patch_buffer,
//                kege::DYNAMIC_BUFFER
//            });
//
//            ShaderResourceLayout::create( buffer.descriptor_set, {{
//                {
//                    {
//                        DESCRIPTOR_TYPE_STORAGE_BUFFER,
//                        "PatchDataBuffer", 1, 0,
//                        VERTEX_SHADER, 1, 0
//                    },
//                    &buffer.patch_buffer
//                }
//            }});
////            buffer.descriptor_set = ShaderResourceLayout
////            ({
////                {
////                    "PatchDataBuffer", // string name
////                    buffer.patch_buffer.id(), // resource_id
////                    DESCRIPTOR_TYPE_STORAGE_BUFFER, // DescriptorType
////                    0, // uint32_t binding
////                    1, // uint32_t count
////                    2, // uint32_t set
////                },
////            });
//        }
//        else
//        {
//            PatchDrawBuffer& buffer = _draw_buffers[ _current_drawbatch ];
//
//            if ( buffer.patch_buffer.size() < max_storage_buffer_size )
//            {
//                buffer.patch_buffer.destroy();
//                buffer.draw_buffer.destroy();
//
//                buffer.instance_count = _instance_count;
//
//                buffer.draw_buffer = kege::DrawParamBuffer
//                ({
//                    max_drawcommand_buffer_size,
//                    _draw_param_buffer,
//                    kege::DYNAMIC_BUFFER
//                });
//
//                buffer.descriptor_set->buffers[0] = kege::StorageBuffer
//                ({
//                    max_storage_buffer_size,
//                    _patch_buffer,
//                    kege::DYNAMIC_BUFFER
//                });
//                buffer.descriptor_set.update();
////                buffer.descriptor_set.update
////                ({
////                    {
////                        "PatchDataBuffer", // string name
////                        buffer.patch_buffer.id(), // resource_id
////                        DESCRIPTOR_TYPE_STORAGE_BUFFER, // DescriptorType
////                        0, // uint32_t binding
////                        1, // uint32_t count
////                        2, // uint32_t set
////                    },
////                });
//            }
//            else
//            {
//                buffer.draw_buffer.updateBufferData({ 0, max_drawcommand_buffer_size, _draw_param_buffer });
//                buffer.patch_buffer.updateBufferData({ 0, max_storage_buffer_size, _patch_buffer });
//                buffer.instance_count = _instance_count;
//            }
//        }
//
//        _current_drawbatch++;
//             */
//    }
//
//    void SphericalTerrain::draw( kege::CommandBuffer* command_buffer )const
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
//    void SphericalTerrain::bind( kege::CommandBuffer* command_buffer )const
//    {
////        uint32_t set_index = 1;
////        command_buffer->bindShaderResource( _cubemesh->mesh_shader_resource, set_index );
//    }

    void SphericalTerrain::setRotation( const kege::quat& rotation )
    {
        _orientation = dquat(rotation.x, rotation.y, rotation.z, rotation.w);
    }

    void SphericalTerrain::setPosition( const kege::vec3& position )
    {
        _position.x = position.x;
        _position.y = position.y;
        _position.z = position.z;
    }

    float SphericalTerrain::getHeight( const kege::vec3& point )
    {
        return 0.f;
    }

//    bool SphericalTerrain::canSubDivide( QuadtreePatchNode& node )
//    {
//        kege::dvec3 world_position = _position + rotate( _orientation, node.sphere.xyz );
//        kege::dvec3 v = _camera_position - world_position;
//
//        double a = dot( _center_to_camera, world_position);
//        if (a < 0 )
//        {
//            node.visible = false;
//        }
//        else
//        {
//            node.visible = true;
//        }
//
//        double radius_sq = kege::sq( node.sphere.w + node.sphere.w );
//        double dist = magnSq( v );
//        double resolution = (dist / radius_sq);
//        return ( node.depth < _minimum_depth ) ? true : resolution < _maximum_resolution;
//    }
//
//    bool SphericalTerrain::splitable( QuadtreePatchNode& node )
//    {
//        return !node.children && node.depth < _maximum_depth;
//    }
//
//    void SphericalTerrain::split( int face_id, QuadtreePatchNode& node )
//    {
//        const int    DEPTH  = node.depth    + 1;
//        const double RADIUS = node.sphere.w * 0.5;
//        const float  SCALE  = node.patch.transform.w   * 0.5;
//        const kege::vec3& CENTER = node.patch.transform.xyz;
//
//        /*
//         * compute child quadtree center positions
//         */
//        kege::fvec3 child_center[ MAX_CHILD_COUNT ];
//        child_center[ NW ] = CENTER + _cube_faces[ face_id ].surface_axies[ 0 ] * SCALE + _cube_faces[ face_id ].surface_axies[ 1 ] * SCALE;
//        child_center[ NE ] = CENTER - _cube_faces[ face_id ].surface_axies[ 0 ] * SCALE + _cube_faces[ face_id ].surface_axies[ 1 ] * SCALE;
//        child_center[ SW ] = CENTER - _cube_faces[ face_id ].surface_axies[ 0 ] * SCALE - _cube_faces[ face_id ].surface_axies[ 1 ] * SCALE;
//        child_center[ SE ] = CENTER + _cube_faces[ face_id ].surface_axies[ 0 ] * SCALE - _cube_faces[ face_id ].surface_axies[ 1 ] * SCALE;
//
//        node.children = new QuadtreePatchChildren;
//        init( node.children->nodes[ NW ], child_center[ NW ], SCALE, RADIUS, DEPTH, face_id );
//        init( node.children->nodes[ NE ], child_center[ NE ], SCALE, RADIUS, DEPTH, face_id );
//        init( node.children->nodes[ SW ], child_center[ SW ], SCALE, RADIUS, DEPTH, face_id );
//        init( node.children->nodes[ SE ], child_center[ SE ], SCALE, RADIUS, DEPTH, face_id );
//    }
//
//    void SphericalTerrain::merge( QuadtreePatchNode& node )
//    {
//        if ( node.children )
//        {
//            merge( node.children->nodes[ 0 ] );
//            merge( node.children->nodes[ 1 ] );
//            merge( node.children->nodes[ 2 ] );
//            merge( node.children->nodes[ 3 ] );
//
//            delete node.children;
//            node.children = nullptr;
//            _total_nodes--;
//        }
//    }
//
//    void SphericalTerrain::update( int16_t face_id, QuadtreePatchNode& node, QuadtreePatchNode* neighbors[4] )
//    {
//        _total_levels = kege::max( _total_levels, node.depth );
//
//        node.patch.patch_index_id = 0;
////        if ( neighbors[ NORTH ]) node.patch.patch_index_id |= 1;
////        if ( neighbors[ EAST  ]) node.patch.patch_index_id |= 2;
////        if ( neighbors[ SOUTH ]) node.patch.patch_index_id |= 4;
////        if ( neighbors[ WEST  ]) node.patch.patch_index_id |= 8;
////        node.patch.patch_vertex_id = face_id;
//
//        if( canSubDivide( node ) )
//        {
//            if ( splitable( node ) )
//            {
//                split( face_id, node );
//            }
//
//            if( node.children )
//            {
//                QuadtreePatchNode* nw_neighbors[4] =
//                {
//                    (neighbors[ NORTH ]) ? ((neighbors[ NORTH ]->children) ? &neighbors[ NORTH ]->children->nodes[ SW ] : nullptr) : nullptr, // NORTH
//                    &node.children->nodes[ NE ], // EAST
//                    &node.children->nodes[ SW ], // SOUTH
//                    (neighbors[ WEST ]) ? ((neighbors[ WEST ]->children) ? &neighbors[ WEST ]->children->nodes[ NE ] : nullptr) : nullptr  // WEST
//                };
//
//                QuadtreePatchNode* ne_neighbors[4] =
//                {
//                    (neighbors[ NORTH ]) ? ((neighbors[ NORTH ]->children) ? &neighbors[ NORTH ]->children->nodes[ SE ] : nullptr) : nullptr, // NORTH
//                    (neighbors[ EAST  ]) ? ((neighbors[ EAST  ]->children) ? &neighbors[ EAST  ]->children->nodes[ NW ] : nullptr) : nullptr, // EAST
//                    &node.children->nodes[ SE ], // SOUTH
//                    &node.children->nodes[ NW ], // WEST
//                };
//
//                QuadtreePatchNode* sw_neighbors[4] =
//                {
//                    &node.children->nodes[ NW ], // NORTH
//                    &node.children->nodes[ SE ], // EAST
//                    (neighbors[ SOUTH ]) ? ((neighbors[ SOUTH ]->children) ? &neighbors[ SOUTH ]->children->nodes[ NW ] : nullptr) : nullptr, // NORTH
//                    (neighbors[ WEST  ]) ? ((neighbors[ WEST  ]->children) ? &neighbors[ WEST  ]->children->nodes[ SE ] : nullptr) : nullptr,  // WEST
//                };
//
//                QuadtreePatchNode* se_neighbors[4] =
//                {
//                    &node.children->nodes[ NE ], // NORTH
//                    (neighbors[ EAST  ]) ? ((neighbors[ EAST  ]->children) ? &neighbors[ EAST  ]->children->nodes[ SW ] : nullptr) : nullptr, // EAST
//                    (neighbors[ SOUTH ]) ? ((neighbors[ SOUTH ]->children) ? &neighbors[ SOUTH ]->children->nodes[ NE ] : nullptr) : nullptr, // SOUTH
//                    &node.children->nodes[ SW ], // WEST
//                };
//
//                update( face_id, node.children->nodes[ 0 ], nw_neighbors );
//                update( face_id, node.children->nodes[ 1 ], ne_neighbors );
//                update( face_id, node.children->nodes[ 2 ], sw_neighbors );
//                update( face_id, node.children->nodes[ 3 ], se_neighbors );
//            }
//        }
//        else if ( node.children )
//        {
//            merge( node );
//        }
//    }

    void SphericalTerrain::update( const kege::dvec3& position )
    {
        _total_levels = 0;
        _camera_position = position;
        _center_to_camera = normalize( _camera_position - _position );

        _faces[0].update();
        _faces[1].update();
        _faces[2].update();
        _faces[3].update();
        _faces[4].update();
        _faces[5].update();
//        QuadtreePatchNode* front_neighbors[4] =
//        {
//            &_faces[ CUBE_FACE_ABOVE ]._root, // NORTH
//            &_faces[ CUBE_FACE_RIGHT ]._root, // EAST
//            &_faces[ CUBE_FACE_BELOW ]._root, // SOUTH
//            &_faces[ CUBE_FACE_LEFT  ]._root, // WEST
//        };
//
//        QuadtreePatchNode* back_neighbors[4] =
//        {
//            &_faces[ CUBE_FACE_ABOVE ]._root, // NORTH
//            &_faces[ CUBE_FACE_RIGHT ]._root, // EAST
//            &_faces[ CUBE_FACE_BELOW ]._root, // SOUTH
//            &_faces[ CUBE_FACE_LEFT  ]._root, // WEST
//        };
//
//        QuadtreePatchNode* left_neighbors[4] =
//        {
//            &_faces[ CUBE_FACE_ABOVE ]._root, // NORTH
//            &_faces[ CUBE_FACE_BACK  ]._root, // EAST
//            &_faces[ CUBE_FACE_BELOW ]._root, // SOUTH
//            &_faces[ CUBE_FACE_FRONT ]._root, // WEST
//        };
//
//        QuadtreePatchNode* right_neighbors[4] =
//        {
//            &_faces[ CUBE_FACE_ABOVE ]._root, // NORTH
//            &_faces[ CUBE_FACE_FRONT ]._root, // EAST
//            &_faces[ CUBE_FACE_BELOW ]._root, // SOUTH
//            &_faces[ CUBE_FACE_BACK  ]._root, // WEST
//        };;
//
//        QuadtreePatchNode* above_neighbors[4] =
//        {
//            &_faces[ CUBE_FACE_BACK  ]._root, // NORTH
//            &_faces[ CUBE_FACE_RIGHT ]._root, // EAST
//            &_faces[ CUBE_FACE_FRONT ]._root, // SOUTH
//            &_faces[ CUBE_FACE_LEFT  ]._root, // WEST
//        };
//
//        QuadtreePatchNode* below_neighbors[4] =
//        {
//            &_faces[ CUBE_FACE_BACK  ]._root, // NORTH
//            &_faces[ CUBE_FACE_LEFT  ]._root, // EAST
//            &_faces[ CUBE_FACE_FRONT ]._root, // SOUTH
//            &_faces[ CUBE_FACE_RIGHT ]._root, // WEST
//        };
//

//        update( CUBE_FACE_FRONT, _cube_faces[ CUBE_FACE_FRONT ]._root, front_neighbors );
//        update( CUBE_FACE_BACK,  _cube_faces[ CUBE_FACE_BACK  ]._root, back_neighbors );
//        update( CUBE_FACE_LEFT,  _cube_faces[ CUBE_FACE_LEFT  ]._root, left_neighbors );
//        update( CUBE_FACE_RIGHT, _cube_faces[ CUBE_FACE_RIGHT ]._root, right_neighbors );
//        update( CUBE_FACE_ABOVE, _cube_faces[ CUBE_FACE_ABOVE ]._root, above_neighbors );
//        update( CUBE_FACE_BELOW, _cube_faces[ CUBE_FACE_BELOW ]._root, below_neighbors );
    }

    SphericalTerrain::SphericalTerrain( kege::Terrain* terrain, const kege::SphericalTerrainSettings& settings )
    :   PhysicalTerrain( terrain )
    ,   _settings( settings )
//    ,   _maximum_resolution( resolution )
//    ,   _minimum_depth( min_depth )
//    ,   _maximum_depth( max_depth )
//    ,   _minimum_height( min_height )
//    ,   _maximum_height( max_height )
//    ,   _total_nodes( 0 )
//    ,   _total_levels( 0 )
//    ,   _total_leaves( 0 )
//    ,   _total_draws( 0 )
//    ,   _exponent( 6 )
//    ,   _base( 8 )
//    ,   _drawparam_stride( (uint32_t)sizeof( DrawParams ) )
//    ,   _patchdata_stride( (uint32_t)sizeof( PatchData ) )
    {
//        _max_draw_capacity = computeDrawCapacity( pow( _base, _exponent ) );
//        initialize();
    }
}
