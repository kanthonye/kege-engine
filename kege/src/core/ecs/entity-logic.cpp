//
//  entity-logic.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#include "entity-logic.hpp"

namespace kege{

    EntityLogic::EntityLogic( const std::string& name )
    :   _entities( nullptr )
    ,   _scene( nullptr )
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

    void EntityLogic::setScene( kege::Scene* scene )
    {
        _scene = scene;
        if ( _signature.any() && _scene )
        {
            _entities = _scene->getEntityRegistry().getEntityView( _signature );
        }
    }
    kege::Scene* EntityLogic::scene()
    {
        return _scene;
    }

    EntityView* EntityLogic::getEntities()
    {
        return _entities;
    }
    
    EntityLogic:: ~EntityLogic()
    {}

}


