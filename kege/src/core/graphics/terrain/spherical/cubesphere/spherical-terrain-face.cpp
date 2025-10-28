//
//  spherical-terrain-face.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/4/25.
//

#include "spherical-terrain.hpp"
#include "spherical-terrain-face.hpp"

namespace kege{

    void SphericalTerrainFace::init
    (
        int16_t face_id,
        SphericalTerrain* terrain,
        SphericalTerrainTile* neighbors[4],
        const kege::vec4* vertices,
        const kege::mat33& axes,
        float scale,
        double radius,
        int depth
    )
    {
        _face_id = face_id;
        _terrain = terrain;
        _vertices = vertices;
        _neighbors[0] = neighbors[0];
        _neighbors[1] = neighbors[1];
        _neighbors[2] = neighbors[2];
        _neighbors[3] = neighbors[3];
        _surface_axies[ 0 ] = axes[ 0 ];
        _surface_axies[ 1 ] = axes[ 1 ];
        operator()( _root, axes[ 2 ], scale, radius, depth );
    }

    void SphericalTerrainFace::operator()( SphericalTerrainTile& node, const kege::vec3& position, float scale, float radius, int depth )
    {
        kege::fvec3 sphere_position = _terrain->getRadius() * normalize( fvec3( position ) );

        node.sphere = { sphere_position, radius };
        node.patch.transform = { position, scale };
        node.face_id = _face_id;
        node.children = nullptr;
        node.depth = depth;

//        node.patch.elevations[ 0 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 0 ].xyz ) );
//        node.patch.elevations[ 1 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 1 ].xyz ) );
//        node.patch.elevations[ 2 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 2 ].xyz ) );
//        node.patch.elevations[ 3 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 3 ].xyz ) );
//        node.patch.elevations[ 4 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 4 ].xyz ) );
//        node.patch.elevations[ 5 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 5 ].xyz ) );
//        node.patch.elevations[ 6 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 6 ].xyz ) );
//        node.patch.elevations[ 7 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 7 ].xyz ) );
//        node.patch.elevations[ 8 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 8 ].xyz ) );
//
//        float normal_strength = 16.f;
//        node.normal = {0.f, 0.f, 0.f};
//        node.normal.z = 1.0 / normal_strength;
//        node.normal.x =
//        {
//            node.patch.elevations[0] + 2.f * node.patch.elevations[3] + node.patch.elevations[5] -
//            node.patch.elevations[2] - 2.f * node.patch.elevations[4] - node.patch.elevations[7]
//        };
//        node.normal.y =
//        {
//            node.patch.elevations[0] + 2.f * node.patch.elevations[1] + node.patch.elevations[2] -
//            node.patch.elevations[5] - 2.f * node.patch.elevations[6] - node.patch.elevations[7]
//        };
//        node.normal = normalize( node.normal );
//        _terrain->_total_nodes++;
    }

    void SphericalTerrainFace::update( SphericalTerrainTile& node, SphericalTerrainTile* neighbors[4] )
    {
//        _terrain->_total_levels = kege::max( _terrain->_total_levels, node.depth );

        node.index_buffer_id = 0;
//        if ( neighbors[ NORTH ]) node.patch.patch_index_id |= 1;
//        if ( neighbors[ EAST  ]) node.patch.patch_index_id |= 2;
//        if ( neighbors[ SOUTH ]) node.patch.patch_index_id |= 4;
//        if ( neighbors[ WEST  ]) node.patch.patch_index_id |= 8;
//        node.patch.patch_vertex_id = face_id;

        if( canSubDivide( node ) )
        {
            if ( splitable( node ) )
            {
                split( node );
            }

            if( node.children )
            {
                SphericalTerrainTile* nw_neighbors[4] =
                {
                    (neighbors[ NORTH ]) ? ((neighbors[ NORTH ]->children) ? &neighbors[ NORTH ]->children->nodes[ SW ] : nullptr) : nullptr, // NORTH
                    &node.children->nodes[ NE ], // EAST
                    &node.children->nodes[ SW ], // SOUTH
                    (neighbors[ WEST ]) ? ((neighbors[ WEST ]->children) ? &neighbors[ WEST ]->children->nodes[ NE ] : nullptr) : nullptr  // WEST
                };

                SphericalTerrainTile* ne_neighbors[4] =
                {
                    (neighbors[ NORTH ]) ? ((neighbors[ NORTH ]->children) ? &neighbors[ NORTH ]->children->nodes[ SE ] : nullptr) : nullptr, // NORTH
                    (neighbors[ EAST  ]) ? ((neighbors[ EAST  ]->children) ? &neighbors[ EAST  ]->children->nodes[ NW ] : nullptr) : nullptr, // EAST
                    &node.children->nodes[ SE ], // SOUTH
                    &node.children->nodes[ NW ], // WEST
                };

                SphericalTerrainTile* sw_neighbors[4] =
                {
                    &node.children->nodes[ NW ], // NORTH
                    &node.children->nodes[ SE ], // EAST
                    (neighbors[ SOUTH ]) ? ((neighbors[ SOUTH ]->children) ? &neighbors[ SOUTH ]->children->nodes[ NW ] : nullptr) : nullptr, // NORTH
                    (neighbors[ WEST  ]) ? ((neighbors[ WEST  ]->children) ? &neighbors[ WEST  ]->children->nodes[ SE ] : nullptr) : nullptr,  // WEST
                };

                SphericalTerrainTile* se_neighbors[4] =
                {
                    &node.children->nodes[ NE ], // NORTH
                    (neighbors[ EAST  ]) ? ((neighbors[ EAST  ]->children) ? &neighbors[ EAST  ]->children->nodes[ SW ] : nullptr) : nullptr, // EAST
                    (neighbors[ SOUTH ]) ? ((neighbors[ SOUTH ]->children) ? &neighbors[ SOUTH ]->children->nodes[ NE ] : nullptr) : nullptr, // SOUTH
                    &node.children->nodes[ SW ], // WEST
                };

                update( node.children->nodes[ 0 ], nw_neighbors );
                update( node.children->nodes[ 1 ], ne_neighbors );
                update( node.children->nodes[ 2 ], sw_neighbors );
                update( node.children->nodes[ 3 ], se_neighbors );
            }
        }
        else if ( node.children )
        {
            merge( node );
        }
    }
    
    void SphericalTerrainFace::update()
    {
        update( _root, _neighbors );
    }

    bool SphericalTerrainFace::canSubDivide( SphericalTerrainTile& node )
    {
        kege::fvec3 world_position = _terrain->getPosition() + rotate( _terrain->getOrientation(), node.sphere.xyz );
        kege::fvec3 v = _terrain->getCameraPosition() - world_position;

        double a = dot( _terrain->getCenterToCamera(), world_position);
        if (a < 0 )
        {
            node.visible = false;
        }
        else
        {
            node.visible = true;
        }

        double radius_sq = kege::sq( node.sphere.w + node.sphere.w );
        double dist = magnSq( v );
        double resolution = (dist / radius_sq);
        return ( node.depth < _terrain->getMinimumDepth() ) ? true : resolution < _terrain->getMaximumResolution();
    }

    bool SphericalTerrainFace::splitable( SphericalTerrainTile& node )
    {
        return !node.children && node.depth < _terrain->getMaximumDepth();
    }

    void SphericalTerrainFace::split( SphericalTerrainTile& node )
    {
        const int    DEPTH  = node.depth    + 1;
        const double RADIUS = node.sphere.w * 0.5;
        const float  SCALE  = node.patch.transform.w   * 0.5;
        const kege::vec3& CENTER = node.patch.transform.xyz;

        /*
         * compute child quadtree center positions
         */
        kege::fvec3 child_center[ MAX_CHILD_COUNT ];
        child_center[ NW ] = CENTER + _surface_axies[ 0 ] * SCALE + _surface_axies[ 1 ] * SCALE;
        child_center[ NE ] = CENTER - _surface_axies[ 0 ] * SCALE + _surface_axies[ 1 ] * SCALE;
        child_center[ SW ] = CENTER - _surface_axies[ 0 ] * SCALE - _surface_axies[ 1 ] * SCALE;
        child_center[ SE ] = CENTER + _surface_axies[ 0 ] * SCALE - _surface_axies[ 1 ] * SCALE;

        node.children = new SphericalTerrainTileChildren;
        operator()( node.children->nodes[ NW ], child_center[ NW ], SCALE, RADIUS, DEPTH );
        operator()( node.children->nodes[ NE ], child_center[ NE ], SCALE, RADIUS, DEPTH );
        operator()( node.children->nodes[ SW ], child_center[ SW ], SCALE, RADIUS, DEPTH );
        operator()( node.children->nodes[ SE ], child_center[ SE ], SCALE, RADIUS, DEPTH );
    }

    void SphericalTerrainFace::merge( SphericalTerrainTile& node )
    {
        if ( node.children )
        {
            merge( node.children->nodes[ 0 ] );
            merge( node.children->nodes[ 1 ] );
            merge( node.children->nodes[ 2 ] );
            merge( node.children->nodes[ 3 ] );

            delete node.children;
            node.children = nullptr;
            //_terrain->_total_nodes--;
        }
    }

}
