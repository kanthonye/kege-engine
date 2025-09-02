//
//  plane-spawn-volume.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#include "emitter-plane.hpp"

namespace kege{

    kege::point3 EmitterPlane::emit()
    {
        return position + (axes[ 0 ] * kege::randf(-extents.x, extents.x)) + (axes[ 1 ] * kege::randf(-extents.y, extents.y));
    }
    
    EmitterPlane::EmitterPlane
    (
        float emissions_per_second, bool burst, 
        const kege::point3& a,
        const kege::point3& b,
        const kege::point3& c,
        const kege::point2& size
    )
    :   Emitter( emissions_per_second, burst )
    {
        extents = size * 0.5f;
        position = (a + b + c) / 3.0f;
        axes[ 0 ] = normalize( a - b );
        axes[ 1 ] = normalize( c - b );

        kege::vec3 normal = cross( axes[ 0 ], axes[ 1 ] );
        axes[ 0 ] = cross( normal, axes[ 1 ] );
        axes[ 1 ] = cross( normal, axes[ 0 ] );
    }
}
