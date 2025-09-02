//
//  line-spawn-volume.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#include "emitter-line.hpp"

namespace kege{

    EmitterLine::EmitterLine
    (
        float emissions_per_second, bool burst,
        const kege::point3& start,
        const kege::point3& end
    )
    :   Emitter( emissions_per_second, burst )
    {
        origin = start;
        direction = end - start;
        length = magn(direction);
        direction /= length;
    }
    kege::point3 EmitterLine::emit()
    {
        return origin + direction * kege::randf(0.f, length);
    }
}

