//
//  sphere-spawn-volume.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#include "emitter-sphere.hpp"

namespace kege{

    EmitterSphereArea::EmitterSphereArea
    (
        float emissions_per_second,
        bool burst,
        float minr,
        float maxr
    )
    :   Emitter( emissions_per_second, burst )
    ,   min_radius(minr)
    ,   max_radius(maxr)
    {
    }

    kege::point3 EmitterSphereArea::emit()
    {
        float radianA = kege::randf(0, 6.28f);
        float radianB = kege::randf(0, 6.28f);
        float radius = kege::randf(min_radius, max_radius);
        float y = sinf(radianB) * radius;
        float r = cosf(radianB) * radius;
        float z = sinf(radianA) * r;
        float x = cosf(radianA) * r;
        return kege::point3(x, y, z);
    }
}

namespace kege{

    EmitterSphere::EmitterSphere( float emissions_per_second, bool burst, float r )
    :   Emitter( emissions_per_second, burst )
    ,   radius(r)
    {}
    kege::point3 EmitterSphere::emit()
    {
        float radianA = kege::randf(0, 6.28f);
        float radianB = kege::randf(0, 6.28f);
        float y = sinf(radianB) * radius;
        float r = cosf(radianB) * radius;
        float z = sinf(radianA) * r;
        float x = cosf(radianA) * r;
        return kege::point3(x, y, z) ;
    }
}

