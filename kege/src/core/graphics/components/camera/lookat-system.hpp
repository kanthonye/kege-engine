//
//  lookat-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/20/25.
//

#ifndef lookat_system_hpp
#define lookat_system_hpp

#include "../../../math/algebra/vmath.hpp"
#include "../../../scene/ecs.hpp"
#include "../../../scene/ecs-system-factory.hpp"

namespace kege{

    class LookAtSystem : public kege::ecs::System
    {
    public:

        LookAtSystem( kege::ECS* esm );
        void update( double dms )override;
    };

    struct LookAt
    {
        ecs::Entity target; // The entity to follow
        vec3 up = vec3(0, 1, 0); // Camera offset from the target
    };

}

#endif /* lookat_system_hpp */
