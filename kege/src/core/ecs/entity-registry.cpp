//
//  entity-registry.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/21/25.
//

#include <ostream>
#include "entity-registry.hpp"
#include "entity-manager.hpp"

namespace kege{
    /**
     * @brief Checks if signature A is a subset of signature B.
     *
     * Returns true if all bits set in A are also set in B.
     * I.e., (A & B) == A, meaning every required component in A exists in B.
     * Used for archetype queries: query_sig ⊆ archetype_sig.
     *
     * @param a The potential subset signature (e.g., query).
     * @param b The potential superset signature (e.g., archetype).
     * @return True if A ⊆ B.
     */
    inline bool is_subset(const kege::EntitySignature& a, const kege::EntitySignature& b) noexcept {
        return (a & b) == a;
    }

    void EntityRegistry::insert( const kege::Entity& e )
    {
        const Signature& sig = e.signature();
        _groups[sig].push_back(e);

        // Update all cached system queries
        for (auto& [query_sig, list] : _system_entities)
        {
            if ( is_subset(query_sig, sig) )
            {
                list.push_back(e);
            }
        }
    }

    void EntityRegistry::remove( const kege::Entity& e )
    {
        const Signature& sig = e.signature();

        // Remove from exact group
        auto& group = _groups[sig];
        group.erase( std::remove( group.begin(), group.end(), e ), group.end() );
        if ( group.empty() ) _groups.erase(sig);

        // Remove from all system caches
        for (auto& [_, list] : _system_entities)
        {
            list.erase( std::remove( list.begin(), list.end(), e ), list.end() );
        }
    }

    const EntityList& EntityRegistry::get( const Signature& query_sig )
    {
        auto it = _system_entities.find(query_sig);
        if (it != _system_entities.end())
        {
            return it->second;
        }

        // Build cache
        EntityList& result = _system_entities[query_sig];
        for (const auto& [arch_sig, entities] : _groups)
        {
            if ( is_subset(query_sig, arch_sig) )
            {
                result.insert( result.end(), entities.begin(), entities.end() );
            }
        }
        return result;
    }

    void EntityRegistry::clear() {
        _system_entities.clear();
        _groups.clear();
    }
//    EntityView* EntityRegistry::getEntityView( const kege::EntitySignature& signature )
//    {
//        auto m = _entity_views.find( signature );
//        if ( m == _entity_views.end() )
//        {
//            EntityView* view = new EntityView;
//            view->_registry = this;
//            for ( uint32_t i = 0; i < _entities.size(); ++i )
//            {
//                if ( compare( _entities[i].signature, signature ) == SUBSET )
//                {
//                    view->_groups.push_back(i);
//                }
//            }
//            _entity_views[ signature ] = view;
//            return view;
//        }
//
//        return m->second;
//    }
//
//    const std::vector< EntityGroup >& EntityRegistry::entities()const
//    {
//        return _entities;
//    }
//
//    void EntityRegistry::updateViews( EntityGroup& new_group )
//    {
//        for ( auto& [signature, view] : _entity_views )
//        {
//            if ( compare( new_group.signature, signature ) == SUBSET )
//            {
//                view->_groups.push_back( new_group.id );
//                new_group.viewers.insert( view );
//            }
//        }
//    }
//
//    const EntityGroup* EntityRegistry::getEntities( int index )const
//    {
//        return &_entities[ index ];
//    }
//
//    EntityGroup* EntityRegistry::getEntities( int index )
//    {
//        return &_entities[ index ];
//    }
//
//    void EntityRegistry::insert( Entity& entity )
//    {
//        EntityRegistryKey* registry = entity.add< EntityRegistryKey >();
//        
//        auto m = _entity_group_index_table.find( entity.signature() );
//        if ( m == _entity_group_index_table.end() )
//        {
//            int index = (int) _entities.size();
//            _entity_group_index_table[ entity.signature() ] = index;
//
//            _entities.push_back({});
//            EntityGroup& group = _entities[ index ];
//            group.count = 0;
//
//            registry->index = (int) group.entities.size();
//            group.entities.push_back({});
//            group.entities[ group.count ] = entity;
//            group.signature = entity.signature();
//            group.id = index;
//            group.count += 1;
//
//            updateViews( group );
//        }
//        else
//        {
//            EntityGroup& group = _entities[ m->second ];
//            registry->index = group.count;
//            if ( group.count >= group.entities.size()  )
//            {
//                group.entities.resize(1 + 2 * group.entities.size());
//            }
//            group.entities[ group.count ] = entity;
//            group.count += 1;
//        }
//
//        for( Entity e = entity.begin(); e != 0; e = e.next() )
//        {
//            insert( e );
//        }
//    }
//
//    void EntityRegistry::remove( Entity& entity )
//    {
//        for( Entity e = entity.begin(); e != 0; e = e.next() )
//        {
//            remove( e );
//        }
//
//        auto m = _entity_group_index_table.find( entity.signature() );
//        if ( m != _entity_group_index_table.end() )
//        {
//            EntityRegistryKey* registry = entity.get< EntityRegistryKey >();
//            if ( _entities[ m->second ].count > 0 )
//            {
//                std::cout << _entities[ m->second ].entities[ registry->index ] <<"\n";
//                int index = _entities[ m->second ].count - 1;
//
//                Entity& e1 = _entities[ m->second ].entities[ registry->index ];
//                Entity& e2 = _entities[ m->second ].entities[ index ];
//                Entity tmp = e1;
//                e1 = e2;
//                e2 = tmp;
//
//                e1.get< EntityRegistryKey >()->index = registry->index;
//
//                _entities[ m->second ].entities[ index ] = 0;
//                _entities[ m->second ].count -= 1;
//
//                if ( _entities[ m->second ].count == 0 )
//                {
//                    // if the entity list is empty remove it from the entity-views that are processes it.
//                    for (EntityView* view : _entities[ m->second ].viewers )
//                    {
//                        auto it = std::find( view->_groups.begin(), view->_groups.end(), _entities[ m->second ].id );
//                        if( it != view->_groups.end() )
//                        {
//                            view->_groups.erase(it);
//                        }
//                    }
//                    // also remove the entity list from the this registry
//                    _entities.erase(_entities.begin() + _entities[ m->second ].id );
//                    _entity_group_index_table.erase(m);
//                }
//            }
//        }
//    }
//
//    int EntityRegistry::getCount()const
//    {
//        return (int)_entities.size();
//    }
//
//    void EntityRegistry::clear()
//    {
//        for ( auto& [signature, view] : _entity_views )
//        {
//            delete view;
//            view = nullptr;
//        }
//        _entity_group_index_table.clear();
//        _entity_views.clear();
//        _entities.clear();
//    }
//
//    EntityRegistry::~EntityRegistry()
//    {
//        clear();
//    }
}
