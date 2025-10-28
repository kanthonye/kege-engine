//
//  spherical-cube-terrain.hpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/6/24.
//

#ifndef physical_spherical_terrain_hpp
#define physical_spherical_terrain_hpp

#include "../../terrain/physical-terrain.hpp"
#include "spherical-terrain-renderer.hpp"
#include "cube-mesh-shader-resource.hpp"
#include "spherical-terrain-face.hpp"

#include "terrain-plane.hpp"

namespace kege{

    class SphericalTerrain : public kege::PhysicalTerrain
    {
    public:

        /**
         * @fn SphericalTerrain
         *
         * The constructor
         */
        SphericalTerrain( kege::Terrain* terrain, const kege::SphericalTerrainSettings& settings );

        // Getters
        const TerrainRenderer* getRenderer() const { return _renderer.ref(); }
        TerrainRenderer* getRenderer() { return _renderer.ref(); }

        const MaterialSource* getMaterial() const { return _material.ref(); }
        MaterialSource* getMaterial() { return _material.ref(); }

        const TerrainPlane& getPlanes( int i ) const { return _planes[i]; }
        TerrainPlane& getPlanes( int i ) { return _planes[i]; }

        // Setters
        void setRenderer( const kege::Ref< SphericalTerrainRenderer >& r ) { _renderer = r; }
        void setMaterial( const kege::Ref< MaterialSource >& m ) { _material = m; }
        void setPlanes( const std::vector< TerrainPlane >& p ) { _planes = p; }

        void update( const kege::fvec3& position );

        void submitVisibleTiles( TerrainTile& tile );
        void submitVisibleGeometries();
        
        bool initialize( kege::Graphics* graphics );

        ~SphericalTerrain();
        
    protected:

        float getHeight( const kege::vec3& point );

        enum { NW, NE, SW, SE, MAX_CHILD_COUNT };
        enum { NORTH, EAST, SOUTH, WEST };

    private:

        kege::Ref< SphericalTerrainRenderer > _renderer;
        kege::Ref< MaterialSource > _material;

        std::vector< TerrainPlane > _planes;

//        /**
//         * The 6 quadtree faces that make up the cube
//         */
//        SphericalTerrainFace _faces[ MAX_CUBE_FACES ];
//
//        SphericalTerrainSettings _settings;
//        Ref< SphericalTerrainRenderer > _renderer;
//        Ref< MaterialSource > _material;
//
//        /**
//         * The node_count is used to track the count off all nodes in this quadtree.
//         * It is just used for informational purposes.
//         */
//        uint32_t _total_nodes;
//        int16_t _total_levels;
//        uint32_t _total_leaves;
//
//        friend SphericalTerrainFace;
//        friend SphericalTerrainTile;
    };

}
#endif /* physical_spherical_terrain_hpp */
