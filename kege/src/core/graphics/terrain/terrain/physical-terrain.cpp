//
//  physical-terrain.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#include "physical-terrain.hpp"


namespace kege{

    TerrainTile* PhysicalTerrain::getTerrainTile( const kege::dvec3& position )const
    {
        return nullptr;
    }
    double PhysicalTerrain::getHeight( const kege::dvec3& position )const
    {
        return 0.0;
    }

    void PhysicalTerrain::setOrientation( const kege::dquat& orientation )
    {
        _orientation = orientation;
    }

    void PhysicalTerrain::setPosition( const kege::dvec3& position )
    {
        _position = position;
    }

    const kege::TerrainSettings& PhysicalTerrain::settings()const
    {
        return _settings;
    }

    bool PhysicalTerrain::initialize()
    {
        return false;
    }

    void PhysicalTerrain::submitVisibleGeometries()
    {}

    void PhysicalTerrain::update( const kege::dvec3& position )
    {}

    PhysicalTerrain::PhysicalTerrain( kege::Terrain* terrain )
    :   _terrain( terrain )
    {}
    
    PhysicalTerrain:: ~PhysicalTerrain()
    {}

}
