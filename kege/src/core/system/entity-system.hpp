//
//  entity-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#ifndef entity_system_hpp
#define entity_system_hpp

#include "../../core/engine/engine.hpp"
#include "../../core/ecs/entity.hpp"
#include "../../core/ecs/entity-registry.hpp"
#include "../../core/system/system.hpp"

namespace kege{

    class SystemManager;

    class EntitySystem : public kege::System
    {
    public:

        EntitySystem( kege::Engine* engine, const std::string& name, uint32_t requirements );
        virtual void onSceneChange()override;
        virtual bool initialize()override;
        virtual void shutdown()override;

        EntityView* getEntities(){return _entities;}
        virtual ~EntitySystem();

    protected:

        kege::EntitySignature _signature;
        EntityView* _entities;
    };

}

#endif /* entity_system_hpp */
