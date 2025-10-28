//
//  terrain-chunklet.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/12/25.
//

#include "spherical-terrain.hpp"
#include "terrain-chunklet.hpp"

namespace kege{

    void initChucklet
    (
        SphericalTerrain* terrain,
        TerrainChucklet& chunklet,
        const kege::vec3& position,
        float scale,
        float radius,
        int depth,
        int face_id
    )
    {
        kege::fvec3 sphere_position = terrain->getRadius() * normalize( fvec3( position ) );

        chunklet.sphere = { sphere_position, radius };
        chunklet.quad = { position, scale };
        chunklet.face_id = face_id;
        chunklet.children = nullptr;
        chunklet.depth = depth;
        chunklet.visible = false;
        chunklet.index_buffer_id = 0;

        /*
        node.patch.elevations[ 0 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 0 ].xyz ) );
        node.patch.elevations[ 1 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 1 ].xyz ) );
        node.patch.elevations[ 2 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 2 ].xyz ) );
        node.patch.elevations[ 3 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 3 ].xyz ) );
        node.patch.elevations[ 4 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 4 ].xyz ) );
        node.patch.elevations[ 5 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 5 ].xyz ) );
        node.patch.elevations[ 6 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 6 ].xyz ) );
        node.patch.elevations[ 7 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 7 ].xyz ) );
        node.patch.elevations[ 8 ] = getHeight( kege::normalize( position + scale * _cube_faces[face_id].vertices->data[ 8 ].xyz ) );

        float normal_strength = 16.f;
        node.normal = {0.f, 0.f, 0.f};
        node.normal.z = 1.0 / normal_strength;
        node.normal.x =
        {
            node.patch.elevations[0] + 2.f * node.patch.elevations[3] + node.patch.elevations[5] -
            node.patch.elevations[2] - 2.f * node.patch.elevations[4] - node.patch.elevations[7]
        };
        node.normal.y =
        {
            node.patch.elevations[0] + 2.f * node.patch.elevations[1] + node.patch.elevations[2] -
            node.patch.elevations[5] - 2.f * node.patch.elevations[6] - node.patch.elevations[7]
        };
        node.normal = normalize( node.normal );
        */
    }

    void submitVisibleChunklet( SphericalTerrain* terrain, TerrainChucklet& chunklet )
    {
        if( chunklet.children )
        {
            submitVisibleChunklet( terrain, chunklet.children->chucklets[ 0 ] );
            submitVisibleChunklet( terrain, chunklet.children->chucklets[ 1 ] );
            submitVisibleChunklet( terrain, chunklet.children->chucklets[ 2 ] );
            submitVisibleChunklet( terrain, chunklet.children->chucklets[ 3 ] );
        }
        else if( chunklet.visible )
        {
            terrain->getRenderer()->submit( chunklet.face_id, chunklet.index_buffer_id, chunklet.quad );
        }
    }

    bool chuckletIsSplitable( SphericalTerrain* terrain, TerrainChucklet& chucklet )
    {
        kege::fvec3 chucklet_position = terrain->getPosition() + rotate( terrain->getOrientation(), chucklet.sphere.xyz );
        kege::fvec3 v = terrain->getCameraPosition() - chucklet_position;

        double a = dot( terrain->getCenterToCamera(), chucklet_position );
        if (a < 0 ) chucklet.visible = false;
        else chucklet.visible = true;

        double radius_sq = kege::sq( chucklet.sphere.w + chucklet.sphere.w );
        double dist = magnSq( v );
        double resolution = (dist / radius_sq);
        return ( chucklet.depth < terrain->getMinimumDepth() ) ? true : resolution < terrain->getMaximumResolution();
    }

    bool chuckletIsDivisible( SphericalTerrain* terrain, TerrainChucklet& chucklet )
    {
        return !chucklet.children && chucklet.depth < terrain->getMaximumDepth();
    }

    void subdivideChucklet( SphericalTerrain* terrain, TerrainChucklet& chucklet )
    {
        const int    DEPTH  = chucklet.depth    + 1;
        const double RADIUS = chucklet.sphere.w * 0.5;
        const float  SCALE  = chucklet.quad.w   * 0.5;
        const kege::vec3& CENTER = chucklet.quad.xyz;

        TerrainPlane& plane = terrain->getPlanes( chucklet.face_id );
        /*
         * compute child quadtree center positions
         */
        kege::fvec3 child_center[ MAX_CHILD_COUNT ];
        child_center[ NW ] = CENTER + plane.axies[ 0 ] * SCALE + plane.axies[ 1 ] * SCALE;
        child_center[ NE ] = CENTER - plane.axies[ 0 ] * SCALE + plane.axies[ 1 ] * SCALE;
        child_center[ SW ] = CENTER - plane.axies[ 0 ] * SCALE - plane.axies[ 1 ] * SCALE;
        child_center[ SE ] = CENTER + plane.axies[ 0 ] * SCALE - plane.axies[ 1 ] * SCALE;

        chucklet.children = new TerrainChucklets;
        initChucklet( terrain, chucklet.children->chucklets[ NW ], child_center[ NW ], SCALE, RADIUS, DEPTH, chucklet.face_id );
        initChucklet( terrain, chucklet.children->chucklets[ NE ], child_center[ NE ], SCALE, RADIUS, DEPTH, chucklet.face_id );
        initChucklet( terrain, chucklet.children->chucklets[ SW ], child_center[ SW ], SCALE, RADIUS, DEPTH, chucklet.face_id );
        initChucklet( terrain, chucklet.children->chucklets[ SE ], child_center[ SE ], SCALE, RADIUS, DEPTH, chucklet.face_id );
    }
    void mergeChucklet( TerrainChucklet& chucklet )
    {
        if ( chucklet.children )
        {
            mergeChucklet( chucklet.children->chucklets[ 0 ] );
            mergeChucklet( chucklet.children->chucklets[ 1 ] );
            mergeChucklet( chucklet.children->chucklets[ 2 ] );
            mergeChucklet( chucklet.children->chucklets[ 3 ] );

            delete chucklet.children;
            chucklet.children = nullptr;
        }
    }
    void updateChucklet( SphericalTerrain* terrain, TerrainChucklet& chucklet )
    {
        chucklet.index_buffer_id = 0;
        //if ( neighbors[ NORTH ]) node.patch.patch_index_id |= 1;
        //if ( neighbors[ EAST  ]) node.patch.patch_index_id |= 2;
        //if ( neighbors[ SOUTH ]) node.patch.patch_index_id |= 4;
        //if ( neighbors[ WEST  ]) node.patch.patch_index_id |= 8;
        //node.patch.patch_vertex_id = face_id;

        if( chuckletIsSplitable( terrain, chucklet ) )
        {
            if ( chuckletIsDivisible( terrain, chucklet ) )
            {
                subdivideChucklet( terrain, chucklet );
            }

            if( chucklet.children )
            {
//                TerrainPlaneNeighbors nw_neighbors;
//                {
//                    .north = (neighbors[ NORTH ]) ? ((neighbors[ NORTH ]->children) ? &neighbors[ NORTH ]->children->nodes[ SW ] : nullptr) : nullptr, // NORTH
//                    .east = &node.children->nodes[ NE ], // EAST
//                    .south = &node.children->nodes[ SW ], // SOUTH
//                    .west = (neighbors[ WEST ]) ? ((neighbors[ WEST ]->children) ? &neighbors[ WEST ]->children->nodes[ NE ] : nullptr) : nullptr  // WEST
//                };
//
//                TerrainPlaneNeighbors ne_neighbors;
//                {
//                    (neighbors[ NORTH ]) ? ((neighbors[ NORTH ]->children) ? &neighbors[ NORTH ]->children->nodes[ SE ] : nullptr) : nullptr, // NORTH
//                    (neighbors[ EAST  ]) ? ((neighbors[ EAST  ]->children) ? &neighbors[ EAST  ]->children->nodes[ NW ] : nullptr) : nullptr, // EAST
//                    &node.children->nodes[ SE ], // SOUTH
//                    &node.children->nodes[ NW ], // WEST
//                };
//
//                TerrainPlaneNeighbors sw_neighbors;
//                {
//                    &node.children->nodes[ NW ], // NORTH
//                    &node.children->nodes[ SE ], // EAST
//                    (neighbors[ SOUTH ]) ? ((neighbors[ SOUTH ]->children) ? &neighbors[ SOUTH ]->children->nodes[ NW ] : nullptr) : nullptr, // NORTH
//                    (neighbors[ WEST  ]) ? ((neighbors[ WEST  ]->children) ? &neighbors[ WEST  ]->children->nodes[ SE ] : nullptr) : nullptr,  // WEST
//                };
//
//                TerrainPlaneNeighbors se_neighbors;
//                {
//                    &node.children->nodes[ NE ], // NORTH
//                    (neighbors[ EAST  ]) ? ((neighbors[ EAST  ]->children) ? &neighbors[ EAST  ]->children->nodes[ SW ] : nullptr) : nullptr, // EAST
//                    (neighbors[ SOUTH ]) ? ((neighbors[ SOUTH ]->children) ? &neighbors[ SOUTH ]->children->nodes[ NE ] : nullptr) : nullptr, // SOUTH
//                    &node.children->nodes[ SW ], // WEST
//                };

                updateChucklet( terrain, chucklet.children->chucklets[ 0 ] );
                updateChucklet( terrain, chucklet.children->chucklets[ 1 ] );
                updateChucklet( terrain, chucklet.children->chucklets[ 2 ] );
                updateChucklet( terrain, chucklet.children->chucklets[ 3 ] );
            }
        }
        else if ( chucklet.children )
        {
            mergeChucklet( chucklet );
        }
    }

}
