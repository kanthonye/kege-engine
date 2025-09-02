//
//  physical-terrain.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#include "physical-terrain.hpp"


namespace kege{

    PhysicalTerrain::PhysicalTerrain( const kege::TerrainSettings& settings )
    :   _settings( settings )
    {}

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

    bool PhysicalTerrain::initialize( kege::Terrain* terrain )
    {
        return false;
    }

    void PhysicalTerrain::render()
    {}

    void PhysicalTerrain::update()
    {}

    PhysicalTerrain:: ~PhysicalTerrain()
    {}

}
