//
//  spherical-cube-terrain.cpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/6/24.
//

#include "virtual-directory.hpp"
#include "spherical-terrain.hpp"

namespace kege{

    void setFrontPlaneNeighbor( TerrainPlane& plane, std::vector< TerrainPlane >& planes )
    {
        plane.neighbors.north = &planes[ CUBE_FACE_ABOVE ]; // NORTH
        plane.neighbors.east  = &planes[ CUBE_FACE_RIGHT ]; // EAST
        plane.neighbors.south = &planes[ CUBE_FACE_BELOW ]; // SOUTH
        plane.neighbors.west  = &planes[ CUBE_FACE_LEFT  ]; // WEST
    }

    void setBackPlaneNeighbor( TerrainPlane& plane, std::vector< TerrainPlane >& planes )
    {
        plane.neighbors.north = &planes[ CUBE_FACE_ABOVE ]; // NORTH
        plane.neighbors.east  = &planes[ CUBE_FACE_RIGHT ]; // EAST
        plane.neighbors.south = &planes[ CUBE_FACE_BELOW ]; // SOUTH
        plane.neighbors.west  = &planes[ CUBE_FACE_LEFT  ]; // WEST
    }

    void setEastPlaneNeighbor( TerrainPlane& plane, std::vector< TerrainPlane >& planes )
    {
        plane.neighbors.north = &planes[ CUBE_FACE_ABOVE ]; // NORTH
        plane.neighbors.east  = &planes[ CUBE_FACE_FRONT ]; // EAST
        plane.neighbors.south = &planes[ CUBE_FACE_BELOW ]; // SOUTH
        plane.neighbors.west  = &planes[ CUBE_FACE_BACK  ]; // WEST
    }

    void setWestPlaneNeighbor( TerrainPlane& plane, std::vector< TerrainPlane >& planes )
    {
        plane.neighbors.north = &planes[ CUBE_FACE_ABOVE ]; // NORTH
        plane.neighbors.east  = &planes[ CUBE_FACE_BACK  ]; // EAST
        plane.neighbors.south = &planes[ CUBE_FACE_BELOW ]; // SOUTH
        plane.neighbors.west  = &planes[ CUBE_FACE_FRONT ]; // WEST
    }


    void setNorthPlaneNeighbor(  TerrainPlane& plane, std::vector< TerrainPlane >& planes )
    {
        plane.neighbors.north = &planes[ CUBE_FACE_BACK  ]; // NORTH
        plane.neighbors.east  = &planes[ CUBE_FACE_RIGHT ]; // EAST
        plane.neighbors.south = &planes[ CUBE_FACE_FRONT ]; // SOUTH
        plane.neighbors.west  = &planes[ CUBE_FACE_LEFT  ]; // WEST
    }

    void setSouthPlaneNeighbor( TerrainPlane& plane, std::vector< TerrainPlane >& planes )
    {
        plane.neighbors.north = &planes[ CUBE_FACE_BACK  ]; // NORTH
        plane.neighbors.east  = &planes[ CUBE_FACE_LEFT  ]; // EAST
        plane.neighbors.south = &planes[ CUBE_FACE_FRONT ]; // SOUTH
        plane.neighbors.west  = &planes[ CUBE_FACE_RIGHT ]; // WEST
    }

    bool SphericalTerrain::initialize( kege::Graphics* graphics )
    {
//        kege::string shader_file = kege::vfs( "graphics-shaders/terrain/spherical/shader.json" );
//        ShaderPipeline pipeline = graphics->getShaderPipelineManager()->load( shader_file.c_str() );
//        if( !pipeline )
//        {
//            kege::Log::error << "Failed to load pipeline -> " << shader_file << Log::nl;
//            return false;
//        }
//
//        _material = new MaterialSource
//        (
//            RenderPassType::Geometry,
//            pipeline, false, false
//        );

        _renderer = new SphericalTerrainRenderer( graphics );
        _renderer->initialize();
        _planes.resize( 6 );

        _maximum_resolution = 512;
        _maximum_depth = 12;
        _minimum_depth = 0;
        _chuck_size = 256;
        _radius = 1024;

        for (int face_id = 0; face_id < MAX_CUBE_FACES; face_id++ )
        {
            const kege::mat33& axes = _renderer->getFaceAxies( face_id );
            _planes[ face_id ].face_id = face_id;
            _planes[ face_id ].vertices = _renderer->getFaceVertices( face_id );
            _planes[ face_id ].neighbors = _planes[ face_id ].neighbors;
            _planes[ face_id ].axies[ 0 ] = axes[ 0 ];
            _planes[ face_id ].axies[ 1 ] = axes[ 1 ];
            initTerrainTile( this, _planes[ face_id ].root, axes[ 2 ], 1.0, _radius, 0, face_id );
        }

        setFrontPlaneNeighbor( _planes[ CUBE_FACE_FRONT ], _planes );
        setBackPlaneNeighbor( _planes[ CUBE_FACE_BACK ], _planes );
        setEastPlaneNeighbor( _planes[ CUBE_FACE_RIGHT ], _planes );
        setWestPlaneNeighbor( _planes[ CUBE_FACE_LEFT ], _planes );
        setNorthPlaneNeighbor( _planes[ CUBE_FACE_ABOVE ], _planes );
        setSouthPlaneNeighbor( _planes[ CUBE_FACE_BELOW ], _planes );

        return true;
    }

    void SphericalTerrain::submitVisibleTiles( TerrainTile& tile )
    {
        if( tile.children )
        {
            submitVisibleTiles( tile.children->subtiles[ 0 ] );
            submitVisibleTiles( tile.children->subtiles[ 1 ] );
            submitVisibleTiles( tile.children->subtiles[ 2 ] );
            submitVisibleTiles( tile.children->subtiles[ 3 ] );
        }
        else if( tile.visible )
        {
            getRenderer()->submit( tile.face_id, 0, tile.quad );
            //submitVisibleChunklet( this, tile.chunk->chunklet );
        }
    }

    void SphericalTerrain::submitVisibleGeometries()
    {
        _renderer->begin();
        submitVisibleTiles( _planes[ 0 ].root );
        submitVisibleTiles( _planes[ 1 ].root );
        submitVisibleTiles( _planes[ 2 ].root );
        submitVisibleTiles( _planes[ 3 ].root );
        submitVisibleTiles( _planes[ 4 ].root );
        submitVisibleTiles( _planes[ 5 ].root );
        _renderer->end();
    }

    float SphericalTerrain::getHeight( const kege::vec3& point )
    {
        return 0.f;
    }

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

    void SphericalTerrain::update( const kege::fvec3& position )
    {
        _camera_position = position;
        _center_to_camera = normalize( _camera_position - _position );

        for (int i=0; i<_planes.size(); ++i)
        {
            updateTile( this, _planes[i].root );
        }
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
    {
    }

    SphericalTerrain::~SphericalTerrain()
    {
        for (int face_id = 0; face_id < MAX_CUBE_FACES; face_id++ )
        {
            mergeTerrainTile( _planes[ face_id ].root );
        }
    }
}
