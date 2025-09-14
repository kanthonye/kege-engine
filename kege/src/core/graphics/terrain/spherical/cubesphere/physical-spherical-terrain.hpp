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

    class PhysicalSphericalTerrain : public kege::PhysicalTerrain
    {
    public:

        bool initialize( SphericalTerrainRenderer* renderer );
        void render( kege::CommandEncoder* encoder, Transform* transform );
        void update( const kege::dvec3& position );

        void setRotation( const kege::quat& rotation );
        void setPosition( const kege::vec3& position );


        /**
         * @fn PhysicalSphericalTerrain
         *
         * The constructor
         */
        PhysicalSphericalTerrain( const kege::TerrainSettings& settings );

    protected:


        float getHeight( const kege::vec3& point );
        void render( SphericalTerrainTile& node );

        enum { NW, NE, SW, SE, MAX_CHILD_COUNT };
        enum { NORTH, EAST, SOUTH, WEST };

        double radius()const;

    private:

        /**
         * The 6 quadtree faces that make up the cube
         */
        SphericalTerrainFace _faces[ MAX_CUBE_FACES ];

        Ref< SphericalTerrainRenderer > _renderer;

        kege::Terrain* _terrain;

        /**
         * This radius of the planet
         */
        double _planet_radius;

        /**
         * The maximum_resolution is used to control the surface level of detain.
         * If a quadtree surface is larger than the maximum resolution base on its
         * distance from the camera it will be divided into smaller surfaces.
         */
        double _maximum_resolution;

        /**
         * The maximum_depth is also used to control the surface level of detain.
         * Where as the maximum_resolution regulate the level of detial base on the
         * distance from the camera, the maximum_depth control the level of detail
         * by limit the depth a quadtree node can reach.
         */
        double _minimum_depth;

        /**
         * The maximum_depth is also used to control the surface level of detain.
         * Where as the maximum_resolution regulate the level of detial base on the
         * distance from the camera, the maximum_depth control the level of detail
         * by limit the depth a quadtree node can reach.
         */
        double _maximum_depth;

        /**
         * The minimum_height is used set the minimum height of the height map.
         */
        double _minimum_height;

        /**
         * The maximum_height is used set the maximum height of the height map.
         */
        double _maximum_height;

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
