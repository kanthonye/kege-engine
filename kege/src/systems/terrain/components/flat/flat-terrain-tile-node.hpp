//
//  flat-terrain-tile-node.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/11/25.
//

#ifndef flat_terrain_tile_node_hpp
#define flat_terrain_tile_node_hpp

#include "vectors.hpp"
#include "landscape-settings.h"
#include "flat-terrain-renderer.hpp"

namespace kege{

    struct TerrainTileNodeChildren;
    struct TerrainTileNode;

    struct TerrainTileNodeNeighbors
    {
        TerrainTileNodeNeighbors()
        :   north( nullptr )
        ,   south( nullptr )
        ,   east( nullptr )
        ,   west( nullptr )
        {}

        TerrainTileNode* north;
        TerrainTileNode* south;
        TerrainTileNode* east;
        TerrainTileNode* west;
    };

    struct TerrainTileNode
    {
        enum{ SOUTH_VERTEX_BIT = 1, EAST_VERTEX_BIT = 2, NORTH_VERTEX_BIT = 4, WEST_VERTEX_BIT = 8 };

        void setNeighborNorth( TerrainTileNode* node );
        void setNeighborSouth( TerrainTileNode* node );
        void setNeighborEast( TerrainTileNode* node );
        void setNeighborWest( TerrainTileNode* node );

        ~TerrainTileNode();

        FlatTerrainPatch patch;
        TerrainTileNodeNeighbors neighbor;

        dvec3 center;
        uint1 diameter;
        uint1 depth;

        sint2 local;

        TerrainTileNodeChildren* children;
    };

    struct TerrainTileNodeChildren
    {
        TerrainTileNode nw;
        TerrainTileNode ne;
        TerrainTileNode sw;
        TerrainTileNode se;
    };
}

#endif /* flat_terrain_tile_node_hpp */
