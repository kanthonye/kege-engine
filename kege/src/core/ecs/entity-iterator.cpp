//
//  entity-iterator.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/21/25.
//

#include "entity-registry.hpp"
#include "entity-iterator.hpp"

namespace kege{

    void calcPlusPosition(int32_t& index, int32_t& group, int32_t amount, const EntityView* view)
    {
        index = index + amount;
        while( index >= view->getEntityCount( group ) )
        {
            index -= view->getEntityCount( group );
            group += 1;
            if ( group >= view->_groups.size() )
            {
                break;
            }
        }
    }

    void calcMinusPosition(int32_t& index, int32_t& group, int32_t amount, const EntityView* view)
    {
        index = index - amount;
        while( index < 0 && group > 0 )
        {
            index += view->getEntityCount( group );
            group -= 1;
        }
    }

    bool operator !=(const EntityIterator& a, const EntityIterator& b)
    {
        if ( a.view == nullptr || b.view == nullptr ) return false;

        if ( a.state == 0 && b.state == 2 )
        {
            return a.view->entityIsAccessible( a.group, a.index );
        }
        else if ( a.state == 2 && b.state == 0 )
        {
            return b.view->entityIsAccessible( b.group, b.index ) != 0;
        }
        else if ( a.state == 0 && b.state == 1 )
        {
            return !a.view->isEqual( a.group, a.index, b.group, b.index );
        }
        return false;
    }

    EntityIterator& EntityIterator::operator =(const EntityIterator& i)
    {
        state = 1;
        view = i.view;
        group = i.group;
        index = i.index;
        return *this;
    }

    EntityIterator EntityIterator::operator +(int i)
    {
        EntityIterator itr;
        itr.state = state;
        itr.view = view;
        itr.group = group;
        itr.index = index;
        calcPlusPosition(itr.index, itr.group, i, view);
        return itr;
    }

    EntityIterator EntityIterator::operator -(int i)
    {
        EntityIterator itr;
        itr.state = state;
        itr.view = view;
        itr.group = group;
        itr.index = index;
        calcMinusPosition(itr.index, itr.group, i, view);
        if (itr.group < view->_groups.size() && itr.index >= 0)
        {
            if ( itr.index >= view->getEntityCount( itr.group ) )
            {
                itr.index = 0;
                itr.group += 1;
            }
        }
        return itr;
    }

    void EntityIterator::operator ++(int)
    {
        if ( view )
        {
            index++;
            if (index >= view->getEntityCount( group ) )
            {
                index = 0;
                group += 1;
            }
        }
    }

    void EntityIterator::operator --(int)
    {
        if ( view )
        {
            index--;
            if (index < 0)
            {
                index = view->getEntityCount( group ) - 1;
                group -= 1;
            }
        }
    }

    void EntityIterator::operator ++()
    {
        if ( view )
        {
            index++;
            if (index >= view->getEntityCount( group ))
            {
                index = 0;
                group += 1;
            }
        }
    }

    void EntityIterator::operator --()
    {
        if ( view )
        {
            index--;
            if (index < 0)
            {
                index = int(view->getEntityCount( group ) - 1);
                group -= 1;
            }
        }
    }

    Entity* EntityIterator::operator ->()
    {
        return &view->getEntity( group, index );
    }

    Entity& EntityIterator::operator*() const
    {
        return view->getEntity( group, index );
    }

    EntityIterator::operator bool()const
    {
        if ( view )
        {
            if (group >= view->_groups.size())
            {
                return false;
            }
            if (index >= view->getEntityCount( group ))
            {
                return false;
            }
            return true;
        }
        return false;
    }

    EntityIterator::EntityIterator
    (
        EntityView* view,
        int32_t group,
        int32_t index,
        char state
    )
    :   view( view )
    ,   group( group )
    ,   index( index )
    ,   state( state )
    {}

    EntityIterator::EntityIterator()
    :   view( nullptr )
    ,   group( 0 )
    ,   index( 0 )
    ,   state( 0 )
    {}

}




namespace kege{

    bool operator !=(const ConstEntityIterator& a, const ConstEntityIterator& b)
    {
        if ( a.view == nullptr || b.view == nullptr ) return false;

        if ( a.state == 0 && b.state == 2 )
        {
            return a.view->entityIsAccessible( a.group, a.index );
        }
        else if ( a.state == 2 && b.state == 0 )
        {
            return b.view->entityIsAccessible( b.group, b.index ) != 0;
        }
        else if ( a.state == 0 && b.state == 1 )
        {
            return !a.view->isEqual( a.group, a.index, b.group, b.index );
        }
        return false;
    }

    ConstEntityIterator& ConstEntityIterator::operator =(const ConstEntityIterator& i)
    {
        state = 1;
        view = i.view;
        group = i.group;
        index = i.index;
        return *this;
    }

    ConstEntityIterator ConstEntityIterator::operator +(int i)
    {
        ConstEntityIterator itr;
        itr.state = state;
        itr.view = view;
        itr.group = group;
        itr.index = index;
        calcPlusPosition(itr.index, itr.group, i, view);
        return itr;
    }

    ConstEntityIterator ConstEntityIterator::operator -(int i)
    {
        ConstEntityIterator itr;
        itr.state = state;
        itr.view = view;
        itr.group = group;
        itr.index = index;
        calcMinusPosition(itr.index, itr.group, i, view);
        return itr;
    }

    void ConstEntityIterator::operator ++(int)
    {
        if ( view )
        {
            index++;
            if (index >= view->getEntityCount( group ))
            {
                index = 0;
                group += 1;
            }
        }
    }

    void ConstEntityIterator::operator --(int)
    {
        if ( view )
        {
            index--;
            if (index <= 0)
            {
                index = int(view->getEntityCount( group ) - 1);
                group -= 1;
            }
        }
    }

    void ConstEntityIterator::operator ++()
    {
        if ( view )
        {
            index++;
            if (index >= view->getEntityCount( group ))
            {
                index = 0;
                group += 1;
            }
        }
    }

    void ConstEntityIterator::operator --()
    {
        if ( view )
        {
            index--;
            if (index <= 0)
            {
                index = view->getEntityCount( group ) - 1;
                group -= 1;
            }
        }
    }

    const Entity* ConstEntityIterator::operator ->()
    {
        return &view->getEntity( group, index );
    }

    const Entity& ConstEntityIterator::operator*() const
    {
        return view->getEntity( group, index );
    }

    ConstEntityIterator::operator bool()const
    {
        if ( view )
        {
            if (group >= view->_groups.size())
            {
                return false;
            }
            if (index >= view->getEntityCount( group ))
            {
                return false;
            }
            return true;
        }
        return false;
    }

    ConstEntityIterator::ConstEntityIterator
    (
        const EntityView* view,
        int32_t group,
        int32_t index,
        char state
    )
    :   view( view )
    ,   group( group )
    ,   index( index )
    ,   state( state )
    {}

    ConstEntityIterator::ConstEntityIterator()
    :   view( nullptr )
    ,   group( 0 )
    ,   index( 0 )
    ,   state( 0 )
    {}

}
