//
//  follow-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/20/25.
//

#ifndef follow_system_hpp
#define follow_system_hpp

#include "../../../scene/ecs.hpp"
#include "../../../math/algebra/vmath.hpp"

namespace kege{

    class FollowSystem : public kege::ecs::System
    {
    public:

        FollowSystem( kege::ECS* ecs );
        void update( double dms )override;
    };

    struct Follow
    {
        ecs::Entity target; // The entity to follow
        vec3   offset = vec3(0, 0, 0); // Camera offset from the target
        float  distance = 10.f;
    };

}

#endif /* follow_system_hpp */
