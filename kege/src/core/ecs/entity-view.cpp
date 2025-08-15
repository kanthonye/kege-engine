//
//  entity-view.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/21/25.
//

#include "entity-view.hpp"
#include "entity-iterator.hpp"
#include "entity-registry.hpp"

namespace kege{


    const std::vector< Entity >& EntityView::getEntities( uint32_t group )const
    {
        return _registry->_entities[ _groups[ group ] ].entities;
    }

    std::vector< Entity >& EntityView::getEntities( uint32_t group )
    {
        return _registry->_entities[ _groups[ group ] ].entities;
    }

    uint32_t EntityView::getEntityCount( uint32_t group )const
    {
        return _registry->_entities[ _groups[ group ] ].count;
    }

    const Entity& EntityView::getEntity( uint32_t group, uint32_t index )const
    {
        return _registry->_entities[ _groups[ group ] ].entities[ index ];
    }

    Entity& EntityView::getEntity( uint32_t group, uint32_t index )
    {
        return _registry->_entities[ _groups[ group ] ].entities[ index ];
    }

    bool EntityView::entityIsAccessible( uint32_t group, uint32_t index )const
    {
        if ( group >= _groups.size() ) return false;
        if ( index >= _registry->_entities[ _groups[ group ] ].count ) return false;
        return _registry->_entities[ _groups[ group ] ].entities[ index ] != 0;
    }

    bool EntityView::isEqual( uint32_t group1, uint32_t index1, uint32_t group2, uint32_t index2 )const
    {
        if ( group1 >= _groups.size() || group2 >= _groups.size() ) return false;
        if ( index1 >= _registry->_entities[ _groups[ group1 ] ].count ) return false;
        if ( index2 >= _registry->_entities[ _groups[ group2 ] ].count ) return false;

        return
        _registry->_entities[ _groups[ group1 ] ].entities[ index1 ] !=
        _registry->_entities[ _groups[ group2 ] ].entities[ index2 ];
    }

    ConstEntityIterator EntityView::begin()const
    {
        return ConstEntityIterator
        {
            this,
            0,
            0,
            0
        };
    }

    ConstEntityIterator EntityView::end()const
    {
        int32_t index = uint32_t(_groups.size() - 1);
        return ConstEntityIterator
        {
            this,
            index,
            int32_t( getEntityCount( index ) - 1 ),
            2
        };
    }

    EntityIterator EntityView::begin()
    {
        return
        {
            this,
            0,
            0,
            0
        };
    }

    EntityIterator EntityView::end()
    {
        if ( !_groups.empty() )
        {
            int32_t index = uint32_t(_groups.size() - 1);
            return EntityIterator
            {
                this,
                index,
                int32_t( getEntityCount( index ) - 1 ),
                2
            };
        }
        return {};
    }

}
