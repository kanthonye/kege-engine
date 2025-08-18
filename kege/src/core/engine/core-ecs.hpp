//
//  ecs-system.hpp
//  physics
//
//  Created by Kenneth Esdaile on 4/19/25.
//

#ifndef ecs_system_hpp
#define ecs_system_hpp

#include "../ecs/entity.hpp"
#include "module.hpp"

namespace kege{

    /**
     * System use to initialize and shutdown the ECS (Entity Component System)
     */
    class ECSModule : public Module
    {
    public:

        kege::Ref< kege::EntityManager > operator ->() { return _module; }
        const kege::Ref< kege::EntityManager > operator ->() const { return _module; }
        operator bool()const override{ return _module != nullptr; }

        bool initialize()override;
        void shutdown()override;
        void add()override;

        ECSModule( kege::Engine* engine );

    private:

        kege::Ref< kege::EntityManager > _module;
        // This is the Entity Manager that will be used to manage entities and components
        // It will be used to add, remove and update entities and components across multiple frames
        // It will also be used to handle entity creation and destruction.
        // The Entity Manager will be used to manage entities and components efficiently and handle resources efficiently.
        // It will also be used to handle entity and component serialization and deserialization.
    };

}
#endif /* ecs_system_hpp */
