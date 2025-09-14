//
//  terrain.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#include "terrain.hpp"
#include "../flat/flat-terrain.hpp"
#include "../spherical/cubesphere/physical-spherical-terrain.hpp"

namespace kege{

    TerrainTile* Terrain::getTerrainTile( const kege::dvec3& position )const
    {
        return _physical_terrain->getTerrainTile( position );
    }

    double Terrain::getHeight( const kege::dvec3& position )const
    {
        return _physical_terrain->getHeight( position );
    }

    bool Terrain::initialize( const kege::TerrainSettings& settings )
    {
        if ( settings.type == TerrainType::SPHERICAL )
        {
            _physical_terrain = new PhysicalSphericalTerrain( settings );
        }
        else
        {
            //_physical_terrain = new PhysicalFlatTerrain( settings );
        }
        return true;
    }

    void Terrain::setOrientation( const kege::dquat& orientation )
    {
        _physical_terrain->setOrientation( orientation );
    }

    void Terrain::setPosition( const kege::dvec3& position )
    {
        _physical_terrain->setPosition( position );
    }

    void Terrain::notify( void* sender, int event, void* data )
    {}

    void Terrain::render( kege::CommandEncoder* encoder, Transform* transform )
    {
        _physical_terrain->render( encoder, transform );
    }

    void Terrain::update( const kege::dvec3& eye )
    {
        _physical_terrain->update( eye );
    }

    void Terrain::setPhysicalTerrain( kege::Ref< PhysicalTerrain > terrain )
    {
        _physical_terrain = terrain;
    }

    const kege::PhysicalTerrain* Terrain::getPhysicalTerrain()const
    {
        return _physical_terrain.ref();
    }

    Terrain:: ~Terrain()
    {
        _physical_terrain.clear();
    }

    Terrain::Terrain()
    {}

}
