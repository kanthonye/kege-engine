//
//  flat-landscape-quadtree.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/30/24.
//

#ifndef flat_landscape_hpp
#define flat_landscape_hpp

#include "flat-terrain-node.hpp"
#include "terrain-topography-generator.hpp"

namespace kege{

    class FlatTerrainTile;

    class FlatTerrain : public Landscape
    {
    public:

        enum{ ROOT_LANDSCAPE_QUADTREE, RENDER_LANDSCAPE, NEW_TERRAIN, NEW_TERRAIN_LIST, };

        typedef std::pair<Ref< FlatTerrainTile >, Ref< TerrainTopography >> GeneratedTerrainTile;

        void generateTerrainTopography( Ref< FlatTerrainTile > tile );
        void generateHeightmapTile( Ref< FlatTerrainTile > tile );

        bool initialize( const kege::LandscapeSettings& settings );
        sint2 calcTileCoord( const dvec3& tile_position );
        
        void render( kege::CommandBuffer& command_buffer );
        void update( const dvec3& eye );

        void insert( const sint2& coord, FlatTerrainTile* tile );
        void remove( const sint2& coord );
        FlatTerrainTile* getTile( const sint2& coord );

        uint64_t calcTileIndex( const sint2& coord );

        ~FlatTerrain();
        FlatTerrain();

    

        enum{ NORTH, SOUTH, EAST, WEST };
        
        std::map< uint64_t, FlatTerrainTile* > _tiles;

        TerrainTopographyGenerator _topography_generator;

        std::vector< GeneratedTerrainTile > _generated_tiles;
        std::mutex _mutex_generated_tiles;

        FlatTerrainRenderer _renderer;
        FlatTerrainNode _root;

        float _half_patch_parameter;
        int _terrain_div_patch;
    };

}
#endif /* flat_landscape_hpp */
