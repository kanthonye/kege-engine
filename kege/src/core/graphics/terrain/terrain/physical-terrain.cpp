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

    void PhysicalTerrain::render( kege::CommandEncoder* encoder, Transform* transform )
    {}

    void PhysicalTerrain::update( const kege::dvec3& position )
    {}

    PhysicalTerrain:: ~PhysicalTerrain()
    {}

}
