//
//  height-map-generator.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/15/24.
//

#include "height-map-generator.hpp"

namespace kege{

//    std::mutex global_mutex;
    void HeightmapGenerator::generate( double tx, double ty, Ref< TerrainTopography >& topography ) const
    {
//        {
//            std::lock_guard< std::mutex > lock( global_mutex );
//            std::cout <<"terrain_coord "<<dvec2(tx, ty)<<"\n";
//        }
        Ref< TopographicLayer > layer = new TopographicLayer( _width, _width );
        double scale = double( _terrain_width ) / double( _width - 1);
        //int k = 0;

        for ( int i=0; i<_settings.size(); ++i )
        {
            // Generate fractal noise
            for (int y=0.0; y<_width; ++y)
            {
                for (int x=0.0; x<_width; ++x)
                {
                    double nx = tx + x * scale;
                    double ny = ty + y * scale;
                    double n  = noise( nx, ny, &_settings[0] );

                    layer->data[ x + y * _width ] = n;
                    //layer->data[ k ] = _heightmapOpFunct( layer->data[ k ], n );
                    //k++;
                }
            }
        }

        topography->heightmap = layer;
    }

    double HeightmapGenerator::noise( double x, double y, const HeightmapLayerSetting* settings )const
    {
//        x = x + settings->heightmap.offset.x;
//        y = y + settings->heightmap.offset.y;
        //std::cout <<"{"<< x <<", " << y <<"}, ";

        double amplitude = 1.0; // Reduces the impact of each octave as the detail increases.
        double frequency = 1.0; // Scales the input for higher detail in subsequent octaves.
        double max_amplitude = 0.0f;

        double n, sum = 0.0f, scale = 1.0  / settings->heightmap.scale;

        double a = 3, b = 1.3;
        for (int i = 0; i < settings->heightmap.octaves; ++i)
        {
            n = amplitude * settings->noiseFunct
            (
                (x * frequency) * scale,
                (y * frequency) * scale,
                settings->permutation->table
            );

            double k = pow(n, a);
            n = k / (k + pow(b - b * n, a));

            sum += n;

            max_amplitude += amplitude;
            amplitude *= settings->heightmap.persistance;
            frequency *= settings->heightmap.lacunarity;
        }

        return sum / max_amplitude; // Normalize to [-1, 1]
    }

    HeightmapGenerator::HeightmapGenerator
    (
     int width, int terrain_width,
     const std::vector< HeightmapLayerSetting >& settings,
     HeightmapOpFunct heightmapOpFunct
    )
    :   _settings( settings )
    ,   _heightmapOpFunct( heightmapOpFunct )
    ,   _terrain_width( terrain_width )
    ,   _width( width )
    {}

    HeightmapGenerator::HeightmapGenerator()
    :   _settings({})
    ,   _heightmapOpFunct( nullptr )
    ,   _terrain_width( 0 )
    ,   _width( 0 )
    {}

}
