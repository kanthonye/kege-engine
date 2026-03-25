//
//  ecs-entity-manager.cpp
//  assets
//
//  Created by Kenneth Esdaile on 11/27/25.
//

#include <iostream>
#include "ecs-entity-manager.hpp"

namespace kege::ecs{

    void EntityManager::remove( int32_t comp_type, Entity& entity )
    {
        EntityEntry& entry = _entities[ entity.index ];
        EntityKind* current = _archetypes[ entry.type ].ref();
        EntityKind::Edge& edge = current->_edges[ comp_type ];
        if (edge.remove == nullptr)
        {
            Signature signature = current->signature();
            signature.clear( comp_type );

            // check if archetype exist in archetype map, if not create the new archetype
            auto src = _archetypemap.find( signature );
            if (src == _archetypemap.end())
            {
                _archetypes[_archetype_count] = current->dupWithout( _archetype_count, comp_type );
                edge.remove = _archetypes[ _archetype_count ].ref();
                _archetypemap[ signature ] = edge.add;
                _archetype_count += 1;
            }
            else
            {
                edge.remove = src->second;
            }
        }

        if ( entry.registration.registry )
        {
            entry.registration.registry->remove( entity );
        }

        uint32_t src_index = entry.handle;
        entry.handle = edge.remove->move(current, src_index);
        entry.type = edge.remove->type();
        current->erase(src_index);

        if ( entry.registration.registry )
        {
            entry.registration.registry->insert( entity );
        }
    }

    Registration* EntityManager::getRegistration( const Entity& entity )
    {
        if ( !valid( entity ) ) return nullptr;
        return &_entities[ entity.index ].registration;
    }


    void EntityManager::destroy( const Entity& entity )
    {
        if ( entity.id == 0 ) return;

        for (ecs::Entity child = begin( entity ); valid(child); child = next( entity ) )
        {
            destroy( child );
        }
        
        if ( _freed.count == 0 )
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
            _freed.count -= 1;
            index = _freed.head;
            _freed.head = _entities[ _freed.head ].next;
            if ( _freed.count == 0 ) _freed.head = _freed.tail = 0;

            entry = &_entities[ index ];
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

        c->parent = parent;
        c->prev = {};
        c->next = {};

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

    void EntityManager::detach(ecs::Entity& entity, ecs::Parent& parent, ecs::Child& child)
    {
        if ( parent.head.id == entity.id )
        {
            ecs::Child* head = get< ecs::Child >( parent.head );
            if ( valid( head->next ) )
            {
                parent.head = head->next;
                get< ecs::Child >( head->next )->prev.id = 0;
            }
            else parent.head.id = parent.tail.id = 0;
        }
        else if ( parent.tail.id == entity.id )
        {
            ecs::Child* tail = get< ecs::Child >( parent.tail );
            if ( valid( tail->prev ) )
            {
                parent.tail = tail->prev;
                get< ecs::Child >( tail->prev )->next.id = 0;
            }
            else parent.tail.id = parent.head.id = 0;
        }
        else
        {

            ecs::Child* p = get< ecs::Child >( child.prev );
            ecs::Child* n = get< ecs::Child >( child.next );
            p->next = child.next;
            n->prev = child.prev;
        }
        parent.count -= 1;
    }

    void EntityManager::detach(ecs::Entity& entity)
    {
        ecs::Child* c = get< ecs::Child >( entity );
        if( c == nullptr )
        {
            return;
        }
        ecs::Parent* p = get< ecs::Parent >( c->parent );
        if( p == nullptr )
        {
            return;
        }
        if ( p->head.version == entity.version )
            detach( entity, *p, *c );
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
