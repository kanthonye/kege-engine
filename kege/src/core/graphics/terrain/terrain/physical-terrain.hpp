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
#include "landscape-settings.h"

namespace kege{

    class Terrain;
    class TerrainRenderer;

    class PhysicalTerrain : public kege::RefCounter
    {
    public:

        void setOrientation( const kege::dquat& orientation );
        void setPosition( const kege::dvec3& position );

        const kege::TerrainSettings& settings()const;
        bool initialize( kege::Terrain* terrain );
        void render();
        void update();

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
