//
//  flat-terrain-node.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/11/25.
//

#ifndef flat_terrain_node_hpp
#define flat_terrain_node_hpp

#include "../common/all.hpp"

namespace kege{

    struct FlatTerrainPatch
    {
        struct{ int x, y, radius, index_buffer_id; };
        struct{ int image_index, image_layer, tile_position[2]; };
        kege::vec4 color;
    };


    class FlatTerrainTile;
    struct FlatTerrainQuadtree;
    struct FlatTerrainQuadtreeChildren;

    struct FlatTerrainQuadtreeNeighbors
    {
        FlatTerrainQuadtreeNeighbors()
        :   north( nullptr )
        ,   south( nullptr )
        ,   east( nullptr )
        ,   west( nullptr )
        {}

        FlatTerrainQuadtree* north;
        FlatTerrainQuadtree* south;
        FlatTerrainQuadtree* east;
        FlatTerrainQuadtree* west;
    };

    struct FlatTerrainQuadtree
    {
        enum{ SOUTH_VERTEX_BIT = 1, EAST_VERTEX_BIT = 2, NORTH_VERTEX_BIT = 4, WEST_VERTEX_BIT = 8 };

        void operator()( FlatTerrainTile* tile, const dvec3& center, uint32_t diameter, uint32_t depth );

        void setNeighborNorth( FlatTerrainQuadtree* node );
        void setNeighborSouth( FlatTerrainQuadtree* node );
        void setNeighborEast( FlatTerrainQuadtree* node );
        void setNeighborWest( FlatTerrainQuadtree* node );
        void split( FlatTerrainTile* tile );
        void merge();
        
        ~FlatTerrainQuadtree();

        FlatTerrainQuadtreeChildren* children;
        FlatTerrainQuadtreeNeighbors neighbor;

        FlatTerrainPatch patch;

        dvec3 center;
        uint32_t diameter;
        uint32_t depth;

        sint2 local;
    };

    struct FlatTerrainQuadtreeChildren : public RefCounter
    {
        FlatTerrainQuadtree nw;
        FlatTerrainQuadtree ne;
        FlatTerrainQuadtree sw;
        FlatTerrainQuadtree se;
    };

}

#endif /* flat_terrain_node_hpp */
