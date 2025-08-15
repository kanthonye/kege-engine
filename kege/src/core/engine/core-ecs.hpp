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
    class CoreECS : public CoreSystem< kege::EntityManager >
    {
    public:

        CoreECS( kege::Engine* engine );
        bool initialize();
        void shutdown();
    };

}
#endif /* ecs_system_hpp */
