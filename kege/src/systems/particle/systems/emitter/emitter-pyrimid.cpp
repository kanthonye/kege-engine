//
//  pyrimid-spawn-volume.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#include "emitter-pyrimid.hpp"

namespace kege{

    EmitterPyrimid::EmitterPyrimid
    (
        float emissions_per_second, bool burst,
        float w,
        float h,
        float d
    )
    :   Emitter( emissions_per_second, burst )
    ,   width(w)
    ,   height(h)
    ,   depth(d)
    {
    }
    
    kege::point3 EmitterPyrimid::emit()
    {
        float t = kege::randf(0.f, 1.f);
        float h = kege::lerp(0.f, height, t);

        float w = width*t;
        w = kege::randf(-w, w);

        float d = depth*t;
        d = kege::randf(-d, d);

        return kege::point3(w, h, d);
    }
}

