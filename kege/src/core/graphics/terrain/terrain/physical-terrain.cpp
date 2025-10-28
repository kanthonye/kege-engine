//
//  physical-terrain.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#include "physical-terrain.hpp"


namespace kege{

    TerrainTile* PhysicalTerrain::getTerrainTile( const kege::fvec3& position )const
    {
        return nullptr;
    }
    double PhysicalTerrain::getHeight( const kege::fvec3& position )const
    {
        return 0.0;
    }

    const kege::TerrainSettings& PhysicalTerrain::settings()const
    {
        return _settings;
    }

    bool PhysicalTerrain::initialize( kege::Graphics* graphics )
    {
        return false;
    }

    void PhysicalTerrain::submitVisibleGeometries()
    {}

    void PhysicalTerrain::update( const kege::fvec3& position )
    {}

    PhysicalTerrain::PhysicalTerrain( kege::Terrain* terrain )
    :   _terrain( terrain )
    ,   _maximum_resolution(1)
    ,   _minimum_depth(0)
    ,   _maximum_depth(0)
    ,   _chuck_size(2)
    ,   _radius(1)
    {}
    
    PhysicalTerrain:: ~PhysicalTerrain()
    {}

}
