//
//  triangle-spawn-volume.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#include "emitter-triangle.hpp"

namespace kege{

    EmitterTriangle::EmitterTriangle
    (
        float emissions_per_second, bool burst, 
        const kege::point3& a,
        const kege::point3& b,
        const kege::point3& c,
        float height,
        float base
    )
    :   Emitter( emissions_per_second, burst )
    {
        this->base = base * 0.5;
        this->height = height * 0.5;
        position = (a + b + c) / 3.0f;
        axes[ 0 ] = normalize( a - b );
        axes[ 1 ] = normalize( c - b );

        kege::vec3 normal = cross( axes[ 0 ], axes[ 1 ] );
        axes[ 0 ] = cross( normal, axes[ 1 ] );
        axes[ 1 ] = cross( normal, axes[ 0 ] );
    }
    kege::point3 EmitterTriangle::emit()
    {
        const float t = kege::randf(0, 1.0f);

        float b = base * t;
        b = kege::randf(-b, b);
        float h = kege::lerp(-height, height, t);

        return position + axes[ 0 ] * b + axes[ 1 ] * h;
    }
}


