//
//  normal-map-generator.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/16/25.
//

#include "normal-map-generator.hpp"

namespace kege{

    void NormalmapGenerator::generate( double tx, double ty, Ref< TerrainTopography >& topography ) const
    {
        Ref< Normalmap > layer = new Normalmap( _width, _width );
        kege::vec3 normal;

        int i = 0, w = _width - 1;
        for (int y=0.0; y<_width; ++y)
        {
            for (int x=0.0; x<_width; ++x)
            {
                float h[9];

                const int xmin = clamp( x - 1, 0, w );
                const int xmax = clamp( x + 1, 0, w );
                const int ymin = clamp( y - 1, 0, w );
                const int ymax = clamp( y + 1, 0, w );

                h[0] = topography->heightmap->get( xmin, ymin );
                h[1] = topography->heightmap->get( x   , ymin );
                h[2] = topography->heightmap->get( xmax, ymin );
                h[3] = topography->heightmap->get( xmin, y    );
                h[4] = topography->heightmap->get( x   , y    );
                h[5] = topography->heightmap->get( xmax, y    );
                h[6] = topography->heightmap->get( xmin, ymax );
                h[7] = topography->heightmap->get( x   , ymax );
                h[8] = topography->heightmap->get( xmax, ymax );

                normal.z = 1.0 / _strength;
                normal.x = h[0] + 2.0*h[3] + h[5] - h[2] - 2.0*h[4] - h[7];
                normal.y = h[0] + 2.0*h[1] + h[2] - h[5] - 2.0*h[6] - h[7];

                normal = normalize( normal ) * 0.5 + 0.5;

                i = x + y * _width;
                
                layer->data[i].x = normal.x * 255;
                layer->data[i].y = normal.y * 255;
                layer->data[i].z = normal.z * 255;
            }
            //std::cout <<"\n";
        }
        //std::cout <<"\n";
        topography->normalmap = layer;
    }

    NormalmapGenerator::NormalmapGenerator(double strength, int width)
    :   _strength( strength )
    ,   _width( width )
    {}
}
