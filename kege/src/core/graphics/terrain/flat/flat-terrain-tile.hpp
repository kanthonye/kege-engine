//
//  flat-terrain-tile.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/11/25.
//

#ifndef flat_terrain_tile_hpp
#define flat_terrain_tile_hpp

#include "landscape.hpp"
#include "terrain-topography.hpp"
#include "flat-terrain-node.hpp"
#include "flat-terrain-tile-node.hpp"

namespace kege{
    
    class FlatTerrainNode;

    class FlatTerrainTile : public TerrainNode
    {
    private:

        void initialize( TerrainTileNode& node, const dvec3& center, uint32_t length, uint32_t depth );

        void render( TerrainTileNode& node, FlatTerrainRenderer& renderer );

        /**
         * This funcion is use to determine if the given quadtree should split or not.
         *
         * @return True is returned if the given quadtree should split, false otherwise.
         */
        bool splitable( TerrainTileNode& node, const dvec3& eye )const;

        /**
         * Update the given quadtree relative to the eye position. This determines if the
         * given quadtree should split, merge, or remain the same.
         *
         * @param node : the quadtree to update.
         * @param eye : the postion which the quadtree should update relative to.
         */
        void update( TerrainTileNode& node, const dvec3& eye );

        /**
         * Split/sub-divide the given quadtreee
         * @param node : the quadtree to split.
         */
        void split( TerrainTileNode& node );

        /**
         * Merge the divided quadtree into one.
         * @param node : the quadtree to merge.
         */
        void merge( TerrainTileNode& node );

    private:

//        void showPatchNorthEdgeCenterVertex( TerrainTileNode& node );
//        void showPatchSouthEdgeCenterVertex( TerrainTileNode& node );
//        void showPatchEastEdgeCenterVertex( TerrainTileNode& node );
//        void showPatchWestEdgeCenterVertex( TerrainTileNode& node );
//
//        void hidePatchNorthEdgeCenterVertex( TerrainTileNode& node );
//        void hidePatchSouthEdgeCenterVertex( TerrainTileNode& node );
//        void hidePatchEastEdgeCenterVertex( TerrainTileNode& node );
//        void hidePatchWestEdgeCenterVertex( TerrainTileNode& node );

//        void setPatchIndexBufer( TerrainTileNode& node );
//        void disableVertices( TerrainTileNode& node );
//        void enableVertices( TerrainTileNode& node );
//        sint2 getLocalCoord( TerrainTileNode& node );
        
//        void disableVertex( uint32_t x, uint32_t y );
//        void enableVertex( uint32_t x, uint32_t y );
//
//        uint32_t toIndex( uint32_t& x, uint32_t& y );
        void setHeight( TerrainTileNode& node );

        void setNeighborSouth( TerrainTileNode* node );
    public:

        enum{ NORTH_VERTEX_BIT = 1, EAST_VERTEX_BIT = 2, SOUTH_VERTEX_BIT = 4, WEST_VERTEX_BIT = 8 };
        enum Status{ IDLE, PENDING, ACTIVE };
        enum{ NORTH, SOUTH, EAST, WEST };
        enum{ NW, NE, SW, SE };

        void initialize( Ref< TerrainTopography >& topography );
        void render( FlatTerrainRenderer& renderer );
        void update( const dvec3& eye );

        void merge();

        const dvec3& center()const;

        FlatTerrainTile( FlatTerrainNode* parent );
        ~FlatTerrainTile();

    public:

        TerrainTileNode _root;

        FlatTerrainNode* _parent;

        Ref< TerrainTopography > _topography;
        ImageLayer _image_layer;

        Status _status;

        sint2 _coord;

        vec4 color;
    };

}
#endif /* flat_terrain_tile_hpp */
