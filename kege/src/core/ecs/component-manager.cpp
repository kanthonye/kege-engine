//
//  component-manager.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/19/25.
//

#include "component-manager.hpp"

namespace kege{

    const EntityComponentMap& ComponentManager::getEntityComponents( uint32_t entity )const
    {
        return _component_containers[ entity ].components;
    }

    const EntitySignature& ComponentManager::signature( uint32_t entity )const
    {
        return _component_containers[ entity ].signature;
    }

    void ComponentManager::purge( uint32_t& entity )
    {
        for(const auto& [type, id] : _component_containers[ entity ].components)
        {
            _component_caches[ type ]->erase( id );
        }
        _component_containers[ entity ].components.clear();
        _component_containers[ entity ].signature.reset();

        if ( _head == 0 )
        {
            _head = _tail = entity;
        }
        else
        {
            _component_containers[ entity ].next = _head;
            _head = entity;
        }
    }

    uint32_t ComponentManager::create()
    {
        uint32_t index;
        if ( _head != 0 )
        {
            index = _head;
            _head = _component_containers[ _head ].next;
            if ( _head == _tail )
            {
                _head = _tail = 0;
            }
        }
        else
        {
            index = _count;
            if ( _count >= _component_containers.size() )
            {
                _component_containers.resize( 2 + 2 * _component_containers.size() );
            }
            _count++;
        }

        return index;
    }

    bool ComponentManager::initialize()
    {
        _component_caches = std::vector< ComponentCache* >( MAX_COMPONENT_TYPES, nullptr );
        return true;
    }

    void ComponentManager::shutdown()
    {
        for (ComponentCache* cache : _component_caches )
        {
            if ( cache )
            {
                delete cache;
                cache = nullptr;
            }
        }
        _component_caches.clear();
    }

    std::ostream& ComponentManager::print(std::ostream& os, uint32_t entity)
    {
        os<<"  container: " << entity <<"\n";
        os<<"    components:\n";
        for(const auto& [type, id] : _component_containers[ entity ].components)
        {
            os<<"      type: " << type <<"\n";
            os<<"      id:   " << id <<"\n";
        }
        return os;
    }

    ComponentManager::~ComponentManager()
    {
        shutdown();
    }

    ComponentManager::ComponentManager()
    :   _count( 1 )
    ,   _head( 0 )
    ,   _tail( 0 )
    {}

}

