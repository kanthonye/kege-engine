//
//  cylinder-spawn-volume.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#include "emitter-cylinder.hpp"

namespace kege{

    EmitterCylinderSurface::EmitterCylinderSurface
    (
        float emissions_per_second, bool burst,
        const kege::vec3 axes[2],
        float h, float r
    )
    :   Emitter( emissions_per_second, burst )
    ,   axes{ axes[0], axes[1] }
    ,   height( h )
    ,   radius( r )
    {}
    kege::point3 EmitterCylinderSurface::emit()
    {
        float radian = kege::randf(0, 6.28f);
        point3 position;
        position  = axes[1] * sinf( radian ) * radius;
        position += axes[1] * cosf( radian ) * radius;
        position += axes[0] * kege::randf(0, height);
        return position;
    }
}

namespace kege{

    EmitterCylinder::EmitterCylinder
    (
        float emissions_per_second, bool burst, 
        const kege::vec3 a[2],
        float height,
        float min_radius,
        float max_radius
    )
    :   Emitter( emissions_per_second, burst )
    ,   axes{ a[0], a[1] }
    ,   radius{ min_radius, max_radius }
    ,   height( height )
    {}
    kege::point3 EmitterCylinder::emit()
    {
        float radian = kege::randf(0.0f, 6.28f);
        float radius0 = kege::randf(0.0f, radius[0]);
        float radius1 = kege::randf(0.0f, radius[1]);

        point3 position;
        position  = axes[1] * sinf( radian ) * radius0;
        position += axes[1] * cosf( radian ) * radius1;
        position += axes[0] * kege::randf(0, height);
        return position;
    }
}




