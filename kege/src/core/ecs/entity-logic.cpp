//
//  entity-logic.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#include "entity-logic.hpp"
#include "../engine/engine.hpp"

namespace kege{

    EntityLogic::EntityLogic( kege::Engine* engine, const std::string& name )
    :   _entities( nullptr )
    ,   _engine( engine )
    ,   _name( name )
    {}

    const kege::EntitySignature& EntityLogic::getEntitySignature()const
    {
        return _signature;
    }

    const std::string& EntityLogic::getName() const
    {
        return _name;
    }

    void EntityLogic::onSceneChange()
    {
        if ( _signature.any() && _engine->scene() )
        {
            _entities = _engine->scene().getScene()->getEntityRegistry().getEntityView( _signature );
        }
    }

    EntityView* EntityLogic::getEntities()
    {
        return _entities;
    }
    
    EntityLogic:: ~EntityLogic()
    {}

}


