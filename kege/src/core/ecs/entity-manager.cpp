//
//  entity-manager.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/19/25.
//

#include "entity-manager.hpp"

namespace kege{

    void EntityManager::attach( uint32_t& parent, const uint32_t& child )
    {
        detach( child );

        if ( _entities[ parent ].head == 0 )
        {
            _entities[ parent ].tail = _entities[ parent ].head = child;
        }
        else
        {
            _entities[ _entities[ parent ].tail ].next = child;
            _entities[ child ].prev = _entities[ parent ].tail;
            _entities[ parent ].tail = child;
        }
        _entities[ parent ].count++;
        _entities[ child ].parent = parent;
    }

    void EntityManager::detach( const uint32_t& entity )
    {
        uint32_t parent = _entities[ entity ].parent;

        if ( _entities[ parent ].head == entity )
        {
            _entities[ parent ].head = _entities[ _entities[ parent ].head ].next;
            if ( _entities[ parent ].head != 0 )
            {
                _entities[ _entities[ parent ].head ].prev = 0;
            }
        }
        else if ( _entities[ parent ].tail == entity )
        {
            _entities[ parent ].tail = _entities[ _entities[ parent ].tail ].prev;
            if ( _entities[ parent ].tail != 0 )
            {
                _entities[ _entities[ parent ].tail ].next = 0;
            }
        }
        else
        {
            _entities[ _entities[ entity ].prev ].next = _entities[ entity ].next;
            _entities[ _entities[ entity ].next ].prev = _entities[ entity ].prev;
        }

        if ( _entities[ parent ].head == 0 || _entities[ parent ].tail == 0 )
        {
            _entities[ parent ].tail = _entities[ parent ].head = 0;
        }

        _entities[ parent ].count--;
        _entities[ entity ].parent = 0;
    }

    uint32_t EntityManager::getParent( const uint32_t& entity )const
    {
        return (entity != 0) ? _entities[ entity ].parent : 0;
    }

    uint32_t EntityManager::getRoot( const uint32_t& entity )const
    {
        return ( _entities[ entity ].parent != 0 ) ? getRoot( _entities[ entity ].parent ) : entity;
    }

    bool EntityManager::isParent( const uint32_t& entity )const
    {
        return (entity != 0) ? _entities[ entity ].head != 0 : false;
    }

    bool EntityManager::isChild( const uint32_t& entity )const 
    {
        return (entity != 0) ? _entities[ entity ].parent != 0 : false;
    }

    uint32_t EntityManager::begin( const uint32_t& entity )const
    {
        return (entity != 0) ? _entities[ entity ].head : 0;
    }

    uint32_t EntityManager::end( const uint32_t& entity )const
    {
        return (entity != 0) ? _entities[ entity ].tail : 0;
    }

    uint32_t EntityManager::next( const uint32_t& entity )const
    {
        return (entity != 0) ? _entities[ entity ].next : 0;
    }

    uint32_t EntityManager::prev( const uint32_t& entity )const
    {
        return (entity != 0) ? _entities[ entity ].prev : 0;
    }

    bool EntityManager::isvalid( const uint32_t& entity )const
    {
        return (entity != 0) ? _entities[ entity ].id != 0 : false;
    }

    void EntityManager::destroy( uint32_t& entity )
    {
        if ( entity == 0 || entity >= _entities.size() )
            return;
        
        for ( uint32_t e = begin( entity ); e != 0; e = next( entity ) )
        {
            destroy( e );
        }

        detach( entity );
        
        if ( _head == 0 )
        {
            _head = _tail = entity;
        }
        else
        {
            _entities[ entity ].next_free = _head;
            _head = entity;
        }
        _component_manager.purge( _entities[ entity ].id );
        _entities[ entity ].parent = 0;
        _entities[ entity ].count = 0;
        _entities[ entity ].head = 0;
        _entities[ entity ].tail = 0;
        _entities[ entity ].prev = 0;
        _entities[ entity ].next = 0;
        _entities[ entity ].id = 0;
    }

    uint32_t EntityManager::create()
    {
        uint32_t index;
        if ( _head != 0 )
        {
            index = _head;
            _head = _entities[ _head ].next_free;
            if ( _head == _tail )
            {
                _head = _tail = 0;
            }
        }
        else
        {
            index = _count;
            if ( _count >= _entities.size() )
            {
                _entities.resize( 2 + 2 * _entities.size() );
            }
            _count++;
        }
        _entities[ index ].id = _component_manager.create();
        _entities[ index ].parent = 0;
        _entities[ index ].count = 0;
        _entities[ index ].head = 0;
        _entities[ index ].tail = 0;
        _entities[ index ].prev = 0;
        _entities[ index ].next = 0;
        return index;
    }

    bool EntityManager::initialize()
    {
        return _component_manager.initialize();
    }

    void EntityManager::shutdown()
    {
        _component_manager.shutdown();
        _entities.clear();
    }

    std::ostream& EntityManager::print(std::ostream& os, uint32_t entity)
    {
        os << "entity: " << entity <<"\n";
        _component_manager.print( os, _entities[ entity].id);
        return os;
    }

    EntityManager::~EntityManager()
    {
        shutdown();
    }
    
    EntityManager::EntityManager()
    :   _count( 1 )
    ,   _head( 0 )
    ,   _tail( 0 )
    {}

}
