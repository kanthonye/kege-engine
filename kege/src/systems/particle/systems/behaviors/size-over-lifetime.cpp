//
//  size-over-lifetime.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#include "size-over-lifetime.hpp"
namespace kege{

    void SizeOverLifetime::update( double dms, Particle& particle )
    {
        if ( curve.size() == 0) return;

        float t = particle.health / particle.max_health;

        if (t <= 0.0f)
        {
            particle.size = curve[0];
            return;
        }

        if (t >= 1.0f)
        {
            particle.size = curve[ curve.size() - 1 ];
            return;
        }

        int min = int( curve.size() - 1 ) * t;
        int max = min + 1;

        particle.size = lerp( curve[ min ], curve[ max ], t );
    }

    SizeOverLifetime::SizeOverLifetime( const array< float >& curve )
    :   curve( curve )
    {}

}
