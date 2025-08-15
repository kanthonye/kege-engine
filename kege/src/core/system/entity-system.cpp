//
//  entity-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#include "../../core/scene/scene.hpp"
#include "../../core/system/entity-system.hpp"

namespace kege{

    EntitySystem::EntitySystem( kege::Engine* engine, const std::string& name, uint32_t requirements )
    :   kege::System( engine, name, requirements )
    {}

    void EntitySystem::onSceneChange()
    {
        if ( _signature.any() && _engine->getScene() )
        {
            _entities = _engine->getScene()->getEntityRegistry().getEntityView( _signature );
        }
    }
    
    bool EntitySystem::initialize()
    {
        onSceneChange();
        return System::initialize();
    }

    void EntitySystem::shutdown()
    {
        System::shutdown();
    }

    EntitySystem:: ~EntitySystem()
    {}


}
