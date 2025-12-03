//
//  ecs-entity-registry.cpp
//  assets
//
//  Created by Kenneth Esdaile on 11/27/25.
//

#include "ecs-entity-registry.hpp"

namespace kege::ecs{

    EntityStorage* EntityRegistry::createEntityStorage( uint32_t type, const Signature& signature )
    {
        _registered_entities.resize( type + 1 );
        EntityStorage* storage = new EntityStorage;
        storage->signature = signature;
        storage->count = 0;
        _registered_entities[ type ] = storage;

        for (auto& sig : _cached_views )
        {
            if ( sig.first.isSubsetOf( signature ) )
            {
                sig.second->add( storage );
            }
        }
        return storage;
    }

    void EntityRegistry::insert(const Entity& handle)
    {
        EntityStorage* storage;
        uint32_t type = _ecs.type( handle );
        if ( type >= _registered_entities.size())
        {
            storage = createEntityStorage( type, _ecs.signature( handle ) );
        }
        else if ( !_registered_entities[ type ] )
        {
            storage = createEntityStorage( type, _ecs.signature( handle ) );
        }
        else
        {
            storage = _registered_entities[ type ].ref();
        }

        Registration* registration = _ecs.getRegistration( handle );
        registration->index = storage->count;
        registration->registry = this;
        storage->push( handle );
    }

    void EntityRegistry::remove(const Entity& handle)
    {
        uint32_t type = _ecs.type( handle );
        if ( type >= _registered_entities.size()) return;

        EntityStorage* storage = _registered_entities[ type ].ref();
        if ( !_registered_entities[ type ] )
            return;

        Registration* registration = _ecs.getRegistration( handle );
        if (registration)
        {
            if ( registration->registry != nullptr )
            {
                storage->erase( registration->index );
            }
        }

        if ( storage->count == 0 )
        {
            const Signature& signature = _ecs.signature( handle );
            /**
             every view that reference the current entity storage should remove it from their storage list.
             */
            for (auto& sig : _cached_views )
            {
                if ( sig.first.isSubsetOf( signature ) )
                {
                    sig.second->remove( storage );
                }
            }
            _registered_entities[ type ].clear();
        }
    }

    void EntityRegistry::purge()
    {
        _registered_entities.clear();
        _cached_views.clear();
    }

    EntityRegistry::EntityRegistry(EntityManager& ecs)
    :   _ecs(ecs)
    {}

    EntityRegistry::~EntityRegistry()
    {
        purge();
    }

}

