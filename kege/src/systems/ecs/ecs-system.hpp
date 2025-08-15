//
//  ecs-system.hpp
//  physics
//
//  Created by Kenneth Esdaile on 4/19/25.
//

#ifndef ecs_system_hpp
#define ecs_system_hpp

#include "../../core/ecs/entity.hpp"
#include "../../core/system/system.hpp"

namespace kege{

    /**
     * System use to initialize and shutdown the ECS (Entity Component System)
     */
    class ECSystem : public System
    {
    public:

        bool initialize()override;
        void shutdown()override;

        ECSystem( kege::Engine* engine );
    };

}
#endif /* ecs_system_hpp */
