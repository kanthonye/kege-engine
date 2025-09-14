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

namespace kege{

    class Terrain;
    class TerrainTile;
    class TerrainRenderer;

    class PhysicalTerrain : public kege::RefCounter
    {
    public:

        virtual TerrainTile* getTerrainTile( const kege::dvec3& position )const;
        virtual double getHeight( const kege::dvec3& position )const;

        void setOrientation( const kege::dquat& orientation );
        void setPosition( const kege::dvec3& position );

        const kege::TerrainSettings& settings()const;
        bool initialize( kege::Terrain* terrain );
        void render( kege::CommandEncoder* encoder, Transform* transform );
        void update( const kege::dvec3& position );

        PhysicalTerrain( const kege::TerrainSettings& settings );
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
