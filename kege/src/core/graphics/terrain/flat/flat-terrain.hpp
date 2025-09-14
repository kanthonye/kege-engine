//
//  flat-landscape-quadtree.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/30/24.
//

#ifndef flat_landscape_hpp
#define flat_landscape_hpp

#include "../terrain/physical-terrain.hpp"
#include "flat-terrain-node.hpp"
#include "flat-terrain-renderer.hpp"
#include "terrain-topography-generator.hpp"

namespace kege{

    class FlatTerrainTile;

    class PhysicalFlatTerrain : public kege::PhysicalTerrain
    {
    public:

        void render( kege::CommandEncoder* encoder, Transform* transform );
        void update( const kege::dvec3& camera_position );


        void setRotation( const kege::quat& rotation );
        void setPosition( const kege::vec3& position );

        bool initialize();
        
        PhysicalFlatTerrain( const kege::TerrainSettings& settings );
        ~PhysicalFlatTerrain();

    protected:

        enum{ ROOT_LANDSCAPE_QUADTREE, RENDER_LANDSCAPE, NEW_TERRAIN, NEW_TERRAIN_LIST, };

        typedef std::pair<Ref< FlatTerrainTile >, Ref< TerrainTopography >> GeneratedTerrainTile;

        void generateTerrainTopography( Ref< FlatTerrainTile > tile );
        void generateHeightmapTile( Ref< FlatTerrainTile > tile );

        sint2 calcTileCoord( const dvec3& tile_position );

        void insert( const sint2& coord );
        void remove( const sint2& coord );
        FlatTerrainTile* getTile( const sint2& coord );

        uint64_t calcTileIndex( const sint2& coord );

    protected:

        enum{ NORTH, SOUTH, EAST, WEST };
        
        std::map< uint64_t, FlatTerrainTile* > _tiles;
        kege::Graphics* _graphics;

        TerrainTopographyGenerator _topography_generator;

        std::vector< GeneratedTerrainTile > _generated_tiles;
        std::mutex _mutex_generated_tiles;

        Ref< FlatTerrainRenderer > _renderer;

        float _half_patch_parameter;
        int _terrain_div_patch;

        friend FlatTerrainTile;
    };

}
#endif /* flat_landscape_hpp */
