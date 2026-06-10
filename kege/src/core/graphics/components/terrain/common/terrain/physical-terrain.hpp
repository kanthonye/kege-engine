//
//  physical-terrain.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef physical_terrain_hpp
#define physical_terrain_hpp

#include "terrain-settings.hpp"
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

        kege::GraphicsDevice* graphics;
    };


    
    class PhysicalTerrain : public kege::RefCounter
    {
    public:

        virtual TerrainTile* getTerrainTile( const kege::fvec3& position )const;
        virtual double getHeight( const kege::fvec3& position )const;

        const kege::fvec3& getCenterToCamera() const { return _center_to_camera; }
        const kege::fvec3& getCameraPosition() const { return _camera_position; }
        const kege::fquat& getOrientation() const { return _orientation; }
        const kege::fvec3& getPosition() const { return _position; }

        void setCenterToCamera( const kege::fvec3& v ) { _center_to_camera = v; }
        void setCameraPosition( const kege::fvec3& v ) { _camera_position = v; }
        void setOrientation( const kege::fquat& q ) { _orientation = q; }
        void setPosition( const kege::fvec3& v ) { _position = v; }

        void setMaximumResolution( float v ) { _maximum_resolution = v; }
        void setMinimumDepth( float v ) { _minimum_depth = v; }
        void setMaximumDepth( float v ) { _maximum_depth = v; }
        void setChuckSize( float v ) { _chuck_size = v; }
        void setRadius( float v ) { _radius = v; }

        float getMaximumResolution() const { return _maximum_resolution; }
        float getMinimumDepth() const { return _minimum_depth; }
        float getMaximumDepth() const { return _maximum_depth; }
        float getChuckSize() const { return _chuck_size; }
        float getRadius() const { return _radius; }

        virtual TerrainRenderer* getRenderer() = 0;
        virtual kege::Material* getMaterial() = 0;

        const kege::TerrainSettings& settings()const;
        
        virtual void update( const kege::fvec3& position );
        virtual void submitVisibleGeometries();
        virtual bool initialize( kege::GraphicsDevice* graphics );

        PhysicalTerrain( kege::Terrain* terrain );
        virtual ~PhysicalTerrain();

    protected:

        kege::TerrainSettings _settings;
        
        /**
         * _center_to_camera holds the terrain position minus the camera position
         */
        kege::fvec3  _center_to_camera;

        /**
         * The camera position / point of interest
         */
        kege::fvec3  _camera_position;

        /**
         * This objects orientation
         */
        kege::fquat _orientation;

        /**
         * This object's position
         */
        kege::fvec3  _position;

        float _maximum_resolution;
        float _minimum_depth;
        float _maximum_depth;
        float _chuck_size;
        float _radius;

        Terrain* _terrain;
    };

}
#endif /* physical_terrain_hpp */
