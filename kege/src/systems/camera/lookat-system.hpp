//
//  lookat-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/20/25.
//

#ifndef lookat_system_hpp
#define lookat_system_hpp

#include "scene.hpp"
#include "camera.hpp"
#include "entity-system.hpp"

namespace kege{

    class LookAtSystem : public kege::EntitySystem
    {
    public:

        LookAtSystem( kege::Engine* engine );
        void update( double dms )override;
        bool initialize()override;
        void shutdown()override;
    };

    struct LookAt
    {
        Entity target; // The entity to follow
        vec3 up = vec3(0, 1, 0); // Camera offset from the target
    };

}

#endif /* lookat_system_hpp */
