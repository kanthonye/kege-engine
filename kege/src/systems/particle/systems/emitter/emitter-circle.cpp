//
//  circle-spawn-volume.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/28/24.
//

#include "emitter-circle.hpp"

namespace kege{

    EmitterCircleLine::EmitterCircleLine
    (
        float emissions_per_second, bool burst,
        kege::vec3 axes[ 2 ],
        float radius
    )
    :   Emitter( emissions_per_second, burst )
    {
        this->axes[ 0 ] = axes[ 0 ];
        this->axes[ 1 ] = axes[ 1 ];
        this->radius = radius;
    }
    kege::point3 EmitterCircleLine::emit()
    {
        float radian = kege::randf(0, 6.28f);
        return axes[ 0 ] * cosf( radian ) * radius + axes[ 1 ] * sinf( radian ) * radius;
    }
}

namespace kege{

    EmitterCircleArea::EmitterCircleArea
    (
        float emissions_per_second, bool burst,
        kege::vec3 axes[ 2 ],
        float min_radius,
        float max_radius
    )
    :   Emitter( emissions_per_second, burst )
    {
        this->axes[ 0 ] = axes[ 0 ];
        this->axes[ 1 ] = axes[ 1 ];
        this->radius[ 0 ] = min_radius;
        this->radius[ 1 ] = max_radius;
    }
    kege::point3 EmitterCircleArea::emit()
    {
        float radian = kege::randf(0, 6.28f);
        float len = kege::randf(radius[ 0 ], radius[ 1 ]);
        return axes[ 0 ] * cosf( radian ) * len + axes[ 1 ] * sinf( radian ) * len;
    }
}
