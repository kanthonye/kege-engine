//
//  terrain-tile.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/12/25.
//

#include "terrain-tile.hpp"
#include "spherical-terrain.hpp"

namespace kege{

    void initTerrainTile( SphericalTerrain* terrain, TerrainTile& tile, const kege::fvec3& position, float scale, float radius, int depth, int face_id )
    {
        kege::fvec3 normal;
        float xx = position.x * position.x;
        float yy = position.y * position.y;
        float zz = position.z * position.z;
        normal.x = position.x * sqrt(1.0 - (yy + zz) / 2.0 + (yy * zz) / 3);
        normal.y = position.y * sqrt(1.0 - (zz + xx) / 2.0 + (zz * xx) / 3);
        normal.z = position.z * sqrt(1.0 - (xx + yy) / 2.0 + (xx * yy) / 3);

        tile.sphere.x = normal.x * terrain->getRadius();
        tile.sphere.y = normal.y * terrain->getRadius();
        tile.sphere.z = normal.z * terrain->getRadius();
        tile.sphere.w = radius;

        tile.quad.x = position.x;
        tile.quad.y = position.y;
        tile.quad.z = position.z;
        tile.quad.w = scale;

        tile.face_id = face_id;
        tile.children = nullptr;
        tile.depth = depth;
    }

    void createTileChunk( SphericalTerrain* terrain, TerrainTile& tile )
    {
        tile.chunk = new TerrainTileChuck;
        initChucklet( terrain, tile.chunk->chunklet, tile.quad.xyz, tile.quad.w, tile.sphere.w, tile.depth, tile.face_id );
    }

    bool tileCanSplit( SphericalTerrain* terrain, TerrainTile& tile )
    {
        kege::fvec3 chucklet_position = terrain->getPosition() + rotate( terrain->getOrientation(), tile.sphere.xyz );
        kege::fvec3 v = terrain->getCameraPosition() - chucklet_position;

        double a = dot( terrain->getCenterToCamera(), chucklet_position );
        if (a < 0 ) tile.visible = false;
        else tile.visible = true;

        double radius_sq = kege::sq( tile.sphere.w );
        double dist = magnSq( v );
        double resolution = (dist / radius_sq);

        return tile.depth < terrain->getMaximumDepth() ? resolution < terrain->getMaximumResolution() :  false;
    }

    bool tileIsDivisible( SphericalTerrain* terrain, TerrainTile& tile )
    {
        return !tile.children && tile.depth < terrain->getMaximumDepth();
    }

    void subDivideTile( SphericalTerrain* terrain, TerrainTile& tile )
    {
        const int    DEPTH  = tile.depth    + 1;
        const double RADIUS = tile.sphere.w * 0.5;
        const float  SCALE  = tile.quad.w   * 0.5;
        const kege::fvec3& CENTER = tile.quad.xyz;

        TerrainPlane& plane = terrain->getPlanes( tile.face_id );
        /*
         * compute child quadtree center positions
         */
        kege::fvec3 child_center[ MAX_CHILD_COUNT ];
        child_center[ NW ] = CENTER + plane.axies[ 0 ] * SCALE + plane.axies[ 1 ] * SCALE;
        child_center[ NE ] = CENTER - plane.axies[ 0 ] * SCALE + plane.axies[ 1 ] * SCALE;
        child_center[ SW ] = CENTER - plane.axies[ 0 ] * SCALE - plane.axies[ 1 ] * SCALE;
        child_center[ SE ] = CENTER + plane.axies[ 0 ] * SCALE - plane.axies[ 1 ] * SCALE;

        tile.children = new TerrainTiles;
        initTerrainTile( terrain, tile.children->subtiles[ NW ], child_center[ NW ], SCALE, RADIUS, DEPTH, tile.face_id );
        initTerrainTile( terrain, tile.children->subtiles[ NE ], child_center[ NE ], SCALE, RADIUS, DEPTH, tile.face_id );
        initTerrainTile( terrain, tile.children->subtiles[ SW ], child_center[ SW ], SCALE, RADIUS, DEPTH, tile.face_id );
        initTerrainTile( terrain, tile.children->subtiles[ SE ], child_center[ SE ], SCALE, RADIUS, DEPTH, tile.face_id );
        /*
        TerrainPlaneNeighbors nw_neighbors;
        {
            .north = (neighbors[ NORTH ]) ? ((neighbors[ NORTH ]->children) ? &neighbors[ NORTH ]->children->nodes[ SW ] : nullptr) : nullptr, // NORTH
            .east = &node.children->nodes[ NE ], // EAST
            .south = &node.children->nodes[ SW ], // SOUTH
            .west = (neighbors[ WEST ]) ? ((neighbors[ WEST ]->children) ? &neighbors[ WEST ]->children->nodes[ NE ] : nullptr) : nullptr  // WEST
        };

        TerrainPlaneNeighbors ne_neighbors;
        {
            (neighbors[ NORTH ]) ? ((neighbors[ NORTH ]->children) ? &neighbors[ NORTH ]->children->nodes[ SE ] : nullptr) : nullptr, // NORTH
            (neighbors[ EAST  ]) ? ((neighbors[ EAST  ]->children) ? &neighbors[ EAST  ]->children->nodes[ NW ] : nullptr) : nullptr, // EAST
            &node.children->nodes[ SE ], // SOUTH
            &node.children->nodes[ NW ], // WEST
        };

        TerrainPlaneNeighbors sw_neighbors;
        {
            &node.children->nodes[ NW ], // NORTH
            &node.children->nodes[ SE ], // EAST
            (neighbors[ SOUTH ]) ? ((neighbors[ SOUTH ]->children) ? &neighbors[ SOUTH ]->children->nodes[ NW ] : nullptr) : nullptr, // NORTH
            (neighbors[ WEST  ]) ? ((neighbors[ WEST  ]->children) ? &neighbors[ WEST  ]->children->nodes[ SE ] : nullptr) : nullptr,  // WEST
        };

        TerrainPlaneNeighbors se_neighbors;
        {
            &node.children->nodes[ NE ], // NORTH
            (neighbors[ EAST  ]) ? ((neighbors[ EAST  ]->children) ? &neighbors[ EAST  ]->children->nodes[ SW ] : nullptr) : nullptr, // EAST
            (neighbors[ SOUTH ]) ? ((neighbors[ SOUTH ]->children) ? &neighbors[ SOUTH ]->children->nodes[ NE ] : nullptr) : nullptr, // SOUTH
            &node.children->nodes[ SW ], // WEST
        };
         */
    }

    void mergeTerrainTile( TerrainTile& tile )
    {
        if ( tile.children )
        {
            mergeTerrainTile( tile.children->subtiles[ 0 ] );
            mergeTerrainTile( tile.children->subtiles[ 1 ] );
            mergeTerrainTile( tile.children->subtiles[ 2 ] );
            mergeTerrainTile( tile.children->subtiles[ 3 ] );

            delete tile.children;
            tile.children = nullptr;
        }
        else if ( tile.chunk )
        {
            mergeChucklet( tile.chunk->chunklet );
            delete tile.chunk;
            tile.chunk = nullptr;
        }
    }

    void updateTile( SphericalTerrain* terrain, TerrainTile& tile )
    {
        if( tileCanSplit( terrain, tile ) )
        {
            if ( tileIsDivisible( terrain, tile ) )
            {
                subDivideTile( terrain, tile );
//                if ( tile.sphere.w <= terrain->getChuckSize() )
//                {
//                    createTileChunk( terrain, tile );
//                }
//                else
//                {
//                    subDivideTile( terrain, tile );
//                }
            }

//            if( tile.chunk )
//            {
//                updateChucklet( terrain, tile.chunk->chunklet );
//            }
//            else
            if( tile.children )
            {
                updateTile( terrain, tile.children->subtiles[ 0 ] );
                updateTile( terrain, tile.children->subtiles[ 1 ] );
                updateTile( terrain, tile.children->subtiles[ 2 ] );
                updateTile( terrain, tile.children->subtiles[ 3 ] );
            }
        }
        else if ( tile.children )
        {
            mergeTerrainTile( tile );
        }
    }

}
