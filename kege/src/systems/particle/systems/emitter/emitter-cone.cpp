//
//  cone-spawn-volume.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#include "emitter-cone.hpp"

namespace kege{

    EmitterCone::EmitterCone
    (
        float emissions_per_second, bool burst,
        kege::vec3 a[2],
        float h,
        float minr,
        float maxr
    )
    :   Emitter( emissions_per_second, burst )
    ,   axes{ a[0], a[1] }
    ,   radius{ minr, maxr }
    ,   height( h )
    {
    }
    kege::point3 EmitterCone::emit()
    {
        float t = kege::randf( 0.f, 1.f );
        float radian = kege::randf( 0.f, 6.28f );
        float z = sinf( radian ) * t * radius[1];
        float x = cosf( radian ) * t * radius[0];
        float y = kege::lerp( -height, height, t );
        return axes[1] * x + axes[0] * y + axes[1] * z;
    }
}
