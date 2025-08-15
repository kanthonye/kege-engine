//
//  cubic-spawn-volume.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#include "emitter-cube.hpp"

namespace kege{

    kege::point3 EmitterCube::emit()
    {
        return kege::point3
        (
            kege::randf( -extents.x, extents.x ),
            kege::randf( -extents.y, extents.y ),
            kege::randf( -extents.z, extents.z )
        );
    };
    EmitterCube::EmitterCube
    (
        float emissions_per_second, bool burst,
        float w,
        float h,
        float d
    )
    :   Emitter( emissions_per_second, burst )
    ,   extents( w * 0.5f, h * 0.5f, d * 0.5f )
    {}
}
