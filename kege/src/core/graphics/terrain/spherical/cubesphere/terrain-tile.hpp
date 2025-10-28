//
//  terrain-tile.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/12/25.
//

#ifndef terrain_tile_hpp
#define terrain_tile_hpp

#include "terrain-chunk.hpp"

namespace kege{

    struct TerrainTileChuck
    {
        TerrainChucklet chunklet;
    };
}


namespace kege{

    struct TerrainTile;
    struct TerrainTiles;

    struct TerrainTileNeighbors
    {
        TerrainTile* north;
        TerrainTile* south;
        TerrainTile* east;
        TerrainTile* west;
    };

    struct TerrainTile
    {
        TerrainTileNeighbors neighbors;
        TerrainTileChuck* chunk;
        TerrainTiles* children;

        /**
         * The spherical volume that encapsulate this quadtree patch node. This
         * member is necessary for culling and computing the level of detail of
         * this QuadtreePatchNode.
         */
        kege::fvec4 sphere;

        /**
         * The PatchData to load into the render buffer when rendering this patch.
         */
        kege::fvec4 quad;

        int16_t depth;
        int16_t face_id;

        bool visible;
    };

    struct TerrainTiles
    {
        TerrainTile subtiles[4];
    };

    void initTerrainTile( SphericalTerrain* terrain, TerrainTile& tile, const kege::fvec3& position, float scale, float radius, int depth, int face_id );
    void createTileChunk( SphericalTerrain* terrain, TerrainTile& tile );
    bool tileCanSplit( SphericalTerrain* terrain, TerrainTile& tile );
    bool tileIsDivisible( SphericalTerrain* terrain, TerrainTile& tile );
    void subDivideTile( SphericalTerrain* terrain, TerrainTile& tile );
    void updateTile( SphericalTerrain* terrain, TerrainTile& tile );
    void mergeTerrainTile( TerrainTile& chucklet );
}

#endif /* terrain_tile_hpp */
