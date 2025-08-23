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

    EntityView* EntityRegistry::getEntityView( const kege::EntitySignature& signature )
    {
        auto m = _entity_views.find( signature );
        if ( m == _entity_views.end() )
        {
            EntityView* view = new EntityView;
            view->_registry = this;
            for ( uint32_t i = 0; i < _entities.size(); ++i )
            {
                if ( compare( _entities[i].signature, signature ) == SUBSET )
                {
                    view->_groups.push_back(i);
                }
            }
            _entity_views[ signature ] = view;
            return view;
        }

        return m->second;
    }

    void EntityRegistry::updateViews( EntityGroup& new_group )
    {
        for ( auto& [signature, view] : _entity_views )
        {
            if ( compare( new_group.signature, signature ) == SUBSET )
            {
                view->_groups.push_back( new_group.id );
                new_group.viewers.insert( view );
            }
        }
    }

    const EntityGroup* EntityRegistry::getEntities( int index )const
    {
        return &_entities[ index ];
    }

    EntityGroup* EntityRegistry::getEntities( int index )
    {
        return &_entities[ index ];
    }

    void EntityRegistry::insert( Entity& entity )
    {
        EntityRegistryKey* registry = entity.add< EntityRegistryKey >();
        
        auto m = _entity_group_index_table.find( entity.signature() );
        if ( m == _entity_group_index_table.end() )
        {
            int index = (int) _entities.size();
            _entity_group_index_table[ entity.signature() ] = index;

            _entities.push_back({});
            EntityGroup& group = _entities[ index ];
            group.count = 0;

            registry->index = (int) group.entities.size();
            group.entities.push_back({});
            group.entities[ group.count ] = entity;
            group.signature = entity.signature();
            group.id = index;
            group.count += 1;

            updateViews( group );
        }
        else
        {
            EntityGroup& group = _entities[ m->second ];
            registry->index = group.count;
            if ( group.count >= group.entities.size()  )
            {
                group.entities.resize(1 + 2 * group.entities.size());
            }
            group.entities[ group.count ] = entity;
            group.count += 1;
        }

        for( Entity e = entity.begin(); e != 0; e = e.next() )
        {
            insert( e );
        }
    }

    void EntityRegistry::remove( Entity& entity )
    {
        for( Entity e = entity.begin(); e != 0; e = e.next() )
        {
            remove( e );
        }

        auto m = _entity_group_index_table.find( entity.signature() );
        if ( m != _entity_group_index_table.end() )
        {
            EntityRegistryKey* registry = entity.get< EntityRegistryKey >();
            if ( _entities[ m->second ].count > 0 )
            {
                std::cout << _entities[ m->second ].entities[ registry->index ] <<"\n";
                int index = _entities[ m->second ].count - 1;

                Entity& e1 = _entities[ m->second ].entities[ registry->index ];
                Entity& e2 = _entities[ m->second ].entities[ index ];
                Entity tmp = e1;
                e1 = e2;
                e2 = tmp;

                e1.get< EntityRegistryKey >()->index = registry->index;

                _entities[ m->second ].entities[ index ] = 0;
                _entities[ m->second ].count -= 1;

                if ( _entities[ m->second ].count == 0 )
                {
                    // if the entity list is empty remove it from the entity-views that are processes it.
                    for (EntityView* view : _entities[ m->second ].viewers )
                    {
                        auto it = std::find( view->_groups.begin(), view->_groups.end(), _entities[ m->second ].id );
                        if( it != view->_groups.end() )
                        {
                            view->_groups.erase(it);
                        }
                    }
                    // also remove the entity list from the this registry
                    _entities.erase(_entities.begin() + _entities[ m->second ].id );
                    _entity_group_index_table.erase(m);
                }
            }
        }
    }

    int EntityRegistry::getCount()const
    {
        return (int)_entities.size();
    }

    void EntityRegistry::clear()
    {
        for ( auto& [signature, view] : _entity_views )
        {
            delete view;
            view = nullptr;
        }
        _entity_group_index_table.clear();
        _entity_views.clear();
        _entities.clear();
    }

    EntityRegistry::~EntityRegistry()
    {
        clear();
    }
}
