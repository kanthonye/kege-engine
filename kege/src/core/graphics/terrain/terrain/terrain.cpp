//
//  terrain.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#include "terrain.hpp"
//#include "../flat/flat-terrain.hpp"
#include "../spherical/cubesphere/physical-spherical-terrain.hpp"

namespace kege{

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

    void Terrain::render( kege::TerrainRenderer& renderer )
    {}

    void Terrain::update( const kege::dvec3& eye )
    {}

    kege::ImageLayerManager* Terrain::imageLayerManager()
    {
        return &_image_layer_manager;
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
    {}

    Terrain::Terrain()
    {}

}
