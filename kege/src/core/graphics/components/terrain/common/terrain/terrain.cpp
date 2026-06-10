//
//  terrain.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#include "terrain.hpp"
#include "../../flat/flat-terrain.hpp"
#include "../../spherical/spherical-terrain.hpp"

namespace kege{

    TerrainTile* Terrain::getTerrainTile( const kege::fvec3& position )const
    {
        return _physical_terrain->getTerrainTile( position );
    }

    double Terrain::getHeight( const kege::fvec3& position )const
    {
        return _physical_terrain->getHeight( position );
    }

    bool Terrain::initialize( const kege::SphericalTerrainSettings& settings )
    {
        if ( settings.type == TerrainType::SPHERICAL )
        {
            _physical_terrain = new SphericalTerrain( this, settings );
            return _physical_terrain->initialize( settings.graphics );
        }
        else
        {
            //_physical_terrain = new FlatTerrain( settings );
        }
        return true;
    }

    void Terrain::setOrientation( const kege::fquat& orientation )
    {
        _physical_terrain->setOrientation( orientation );
    }

    void Terrain::setPosition( const kege::fvec3& position )
    {
        _physical_terrain->setPosition( position );
    }

    void Terrain::notify( void* sender, int event, void* data )
    {}

    void Terrain::submitVisibleGeometries()
    {
        _physical_terrain->submitVisibleGeometries();
    }

    void Terrain::update( const kege::fvec3& eye )
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

    TerrainRenderer* Terrain::getTerrainRenderer()
    {
        return _physical_terrain->getRenderer();
    }
    kege::Material* Terrain::getTerrainMaterial()
    {
        return _physical_terrain->getMaterial();
    }

    Terrain:: ~Terrain()
    {
        _physical_terrain.clear();
    }

    Terrain::Terrain()
    {}

}
