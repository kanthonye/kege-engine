//
//  heightmap-generator.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/15/24.
//

#include "terrain-topography-generator.hpp"

namespace kege{

    Ref< TerrainTopography > TerrainTopographyGenerator::generate( double x, double y ) const
    {
        Ref< TerrainTopography > topography = new TerrainTopography;
        for (const Ref< TopographicLayerGenerator >& layer : _topographic_layer_generators )
        {
            layer->generate( x, y, topography );
        }
        return topography;
    }

    void TerrainTopographyGenerator::addSurfaceGenerator( Ref< TopographicLayerGenerator > generator )
    {
        _topographic_layer_generators.push_back( generator );
    }
    
    TerrainTopographyGenerator::~TerrainTopographyGenerator()
    {
        _topographic_layer_generators.clear();
    }
}
