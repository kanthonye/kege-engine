//
//  physical-terrain.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef physical_terrain_hpp
#define physical_terrain_hpp

#include "../../../memory/ref.hpp"
#include "../../../math/algebra/vmath.hpp"
#include "../../../renderer/core/graphics.hpp"
#include "landscape-settings.h"
#include "terrain-material.hpp"

namespace kege{

    class Terrain;
    class TerrainTile;
    class TerrainRenderer;

    enum struct TerrainType{ FLAT, SPHERICAL };

    struct SphericalTerrainSettings
    {
        TerrainType type;

        /**
         * The maximum_resolution is used to control the surface level of detain.
         * If a quadtree surface is larger than the maximum resolution base on its
         * distance from the camera it will be divided into smaller surfaces.
         */
        double maximum_resolution;

        /**
         * The maximum_depth is also used to control the surface level of detain.
         * Where as the maximum_resolution regulate the level of detial base on the
         * distance from the camera, the maximum_depth control the level of detail
         * by limit the depth a quadtree node can reach.
         */
        double minimum_depth;

        /**
         * The maximum_depth is also used to control the surface level of detain.
         * Where as the maximum_resolution regulate the level of detial base on the
         * distance from the camera, the maximum_depth control the level of detail
         * by limit the depth a quadtree node can reach.
         */
        double maximum_depth;

        /**
         * The minimum_height is used set the minimum height of the height map.
         */
        double minimum_height;

        /**
         * The maximum_height is used set the maximum height of the height map.
         */
        double maximum_height;

        /**
         * This radius of the planet
         */
        double radius;

        kege::Graphics* graphics;
    };


    
    class PhysicalTerrain : public kege::RefCounter
    {
    public:

        virtual TerrainTile* getTerrainTile( const kege::dvec3& position )const;
        virtual double getHeight( const kege::dvec3& position )const;

        void setOrientation( const kege::dquat& orientation );
        void setPosition( const kege::dvec3& position );

        virtual TerrainRenderer* getTerrainRenderer() = 0;
        virtual MaterialSource* getTerrainMaterial() = 0;

        const kege::TerrainSettings& settings()const;
        
        virtual void update( const kege::dvec3& position );
        virtual void submitVisibleGeometries();
        virtual bool initialize();

        PhysicalTerrain( kege::Terrain* terrain );
        virtual ~PhysicalTerrain();

    protected:

        kege::TerrainSettings _settings;
        
        /**
         * _center_to_camera holds the terrain position minus the camera position
         */
        kege::dvec3  _center_to_camera;

        /**
         * The camera position / point of interest
         */
        kege::dvec3  _camera_position;

        /**
         * This objects orientation
         */
        kege::dquat _orientation;

        /**
         * This object's position
         */
        kege::dvec3  _position;


        Terrain* _terrain;
    };

}
#endif /* physical_terrain_hpp */
