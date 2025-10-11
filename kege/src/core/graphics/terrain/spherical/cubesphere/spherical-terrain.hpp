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

namespace kege{

    class SphericalTerrain : public kege::PhysicalTerrain
    {
    public:

        const SphericalTerrainSettings& getSettings()const;
        TerrainRenderer* getTerrainRenderer();
        MaterialSource* getTerrainMaterial();

        bool initialize();
        void update( const kege::dvec3& position );
        void submitVisibleGeometries();

        void setRotation( const kege::quat& rotation );
        void setPosition( const kege::vec3& position );


        /**
         * @fn SphericalTerrain
         *
         * The constructor
         */
        SphericalTerrain( kege::Terrain* terrain, const kege::SphericalTerrainSettings& settings );

    protected:

        float getHeight( const kege::vec3& point );
        void submitVisibleNodes( SphericalTerrainTile& node );

        enum { NW, NE, SW, SE, MAX_CHILD_COUNT };
        enum { NORTH, EAST, SOUTH, WEST };

    private:

        /**
         * The 6 quadtree faces that make up the cube
         */
        SphericalTerrainFace _faces[ MAX_CUBE_FACES ];

        SphericalTerrainSettings _settings;
        Ref< SphericalTerrainRenderer > _renderer;
        Ref< MaterialSource > _material;

        /**
         * The node_count is used to track the count off all nodes in this quadtree.
         * It is just used for informational purposes.
         */
        uint32_t _total_nodes;
        int16_t _total_levels;
        uint32_t _total_leaves;

        friend SphericalTerrainFace;
        friend SphericalTerrainTile;
    };

}
#endif /* physical_spherical_terrain_hpp */
