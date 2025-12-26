//
//  ecs-entity-manager.cpp
//  assets
//
//  Created by Kenneth Esdaile on 11/27/25.
//

#include <iostream>
#include "ecs-entity-manager.hpp"

namespace kege::ecs{

    Registration* EntityManager::getRegistration( const Entity& entity )
    {
        if ( !valid( entity ) ) return nullptr;
        return &_entities[ entity.index ].registration;
    }


    void EntityManager::destroy( const Entity& entity )
    {
        for (ecs::Entity child = begin( entity ); valid(child); child = next( entity ) )
        {
            destroy( child );
        }
        
        if ( _freed.head >= INVALID_INDEX_U32 )
        {
            _freed.tail = _freed.head = entity.index;
        }
        else
        {
            _entities[ _freed.tail ].next = entity.index;
            _freed.tail = entity.index;
        }

        _freed.count += 1;

        EntityEntry& entry = _entities[ entity.index ];
        _archetypes[ entry.type ]->erase( entry.handle );
        entry.handle = INVALID_INDEX_U32;
        entry.next = INVALID_INDEX_U32;
        entry.version += 1;

        if ( entry.registration.registry != nullptr )
        {
            entry.registration.registry->remove( entity );
            entry.registration.registry = nullptr;
        }
    }

    Entity EntityManager::create()
    {
        uint32_t index;
        EntityEntry* entry;
        if ( _freed.count != 0 )
        {
            index = _freed.head;
            entry = &_entities[ index ];
            _freed.count -= 1;

            _freed.head = _entities[ _freed.head ].next;
            if ( _freed.head >= _freed.count ) _freed.tail = INVALID_INDEX_U32;
        }
        else
        {
            if ( _entity_count >= _entities.size() )
            {
                size_t size = (!_entities.empty())? 2 * _entity_count: 16;
                _entities.resize( size );
            }
            index = _entity_count;
            _entity_count += 1;

            entry = &_entities[ index ];
            entry->version = 1;
        }

        entry->type = 0;

        return Entity
        {
            .index = index,
            .version = entry->version
        };
    }

    uint32_t EntityManager::signatureToType( const Signature& signature )const
    {
        auto i = _archetypemap.find( signature );
        if ( i == _archetypemap.end()) return INVALID_INDEX_U32;
        return i->second->type();
    }

    void EntityManager::printMemoryUsage()const
    {
        size_t size = 0;
        for (int i=0; i<_archetypes.size(); ++i)
        {
            if(_archetypes[i])
            {
                std::cout << "type: "<<_archetypes[i]->type() <<" -> " <<_archetypes[i]->getBufferSize() <<"\n";
                size += _archetypes[i]->getBufferSize();
            }
        }
        std::cout << "total size: "<<size <<"\n";
    }

    void EntityManager::reset()
    {
        for (int i = 0; i < _archetypes.size(); ++i)
        {
            if (_archetypes[i]) {
                _archetypes[i]->reset();
            }
        }
        _entity_count = 0;
        _freed = {};
    }

    void EntityManager::attach(ecs::Entity& parent, ecs::Entity& child)
    {
        ecs::Parent* p = get< ecs::Parent >( parent );
        if( p == nullptr )
        {
            p = add< ecs::Parent >(parent);
            p->count = 0;
        }

        ecs::Child* c = get< ecs::Child >( child );
        if( c == nullptr )
        {
            c = add< ecs::Child >( child );
        }

        c->prev.index = INVALID_INDEX_U32;
        c->next.index = INVALID_INDEX_U32;
        c->parent = parent;

        if ( p->count == 0 )
        {
            p->tail = p->head = child;
        }
        else
        {
            ecs::Child* t = get< ecs::Child >( p->tail );
            t->next = child;
            c->prev = p->tail;
            p->tail = child;
        }
        p->count += 1;
    }

    void EntityManager::detach(ecs::Entity& child, ecs::Parent* p, ecs::Child* c)
    {
        if( p == nullptr || c == nullptr )
        {
            return;
        }

        if ( p->head.version == child.version )
        {
            if ( p->head.index == child.index )
            {
                ecs::Child* c = get< ecs::Child >( p->head );
                p->head = c->next;
                if ( valid( p->head ) )
                {
                    get< ecs::Child >( p->head )->prev.index = INVALID_INDEX_U32;
                }
                else p->tail.index = INVALID_INDEX_U32;
            }
            else if ( p->tail.index == child.index )
            {
                ecs::Child* c = get< ecs::Child >( p->tail );
                p->tail = c->prev;
                if ( valid( p->tail ) )
                {
                    get< ecs::Child >( p->tail )->next.index = INVALID_INDEX_U32;
                }
                else p->head.index = INVALID_INDEX_U32;
            }
            else
            {

                ecs::Child* c = get< ecs::Child >( child );
                ecs::Child* p = get< ecs::Child >( c->prev );
                ecs::Child* n = get< ecs::Child >( c->next );
                p->next = c->next;
                n->prev = c->prev;
            }
            p->count -= 1;
        }
    }

    void EntityManager::detach(ecs::Entity& child)
    {
        ecs::Child* c = get< ecs::Child >( child );
        if( c == nullptr )
        {
            return;
        }
        ecs::Parent* p = get< ecs::Parent >( c->parent );
        if( p == nullptr )
        {
            return;
        }
        detach( child, p, c );
    }

    ecs::Entity EntityManager::getParent( const ecs::Entity& entity )const
    {
        const ecs::Child* c = get< ecs::Child >( entity );
        return (c != nullptr)? c->parent : ecs::Entity{};
    }

    ecs::Entity EntityManager::getRoot( const ecs::Entity& entity )const
    {
        const ecs::Child* c = get< ecs::Child >( entity );
        return (c != nullptr)? getRoot( c->parent ) : entity;
    }

    bool EntityManager::isParent( const ecs::Entity& entity )const
    {
        return get< ecs::Parent >( entity ) != nullptr;
    }

    bool EntityManager::isChild( const ecs::Entity& entity )const
    {
        return get< ecs::Child >( entity ) != nullptr;
    }

    ecs::Entity EntityManager::begin( const ecs::Entity& entity )const
    {
        const ecs::Parent* p = get< ecs::Parent >( entity );
        return (p != nullptr)? (p->count != 0 )? p->head : ecs::Entity{}: ecs::Entity{};
    }

    ecs::Entity EntityManager::end( const ecs::Entity& entity )const
    {
        const ecs::Parent* p = get< ecs::Parent >( entity );
        return (p != nullptr)? (p->count != 0 )? p->tail : ecs::Entity{}: ecs::Entity{};
    }

    ecs::Entity EntityManager::next( const ecs::Entity& entity )const
    {
        const ecs::Child* c = get< ecs::Child >( entity );
        return (c != nullptr)? c->next : ecs::Entity{};
    }

    ecs::Entity EntityManager::prev( const ecs::Entity& entity )const
    {
        const ecs::Child* c = get< ecs::Child >( entity );
        return (c != nullptr)? c->prev : ecs::Entity{};
    }

    EntityManager::EntityManager( size_t size )
    :   _archetype_count(0)
    ,   _entity_count(0)
    ,   _freed{}
    {
        defnArchetype(0);
        if( size != 0 )
        {
            _entities.resize( size );
        }
    }

    EntityManager::~EntityManager()
    {
        _archetypemap.clear();
        _archetypes.clear();
        _entities.clear();
    }
}
