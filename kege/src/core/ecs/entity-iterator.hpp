//
//  entity-iterator.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/21/25.
//

#ifndef entity_iterator_hpp
#define entity_iterator_hpp

#include "entity.hpp"
#include "entity-view.hpp"

namespace kege{

    class EntityIterator
    {
    public:

        friend bool operator !=(const EntityIterator& a, const EntityIterator& b);
        EntityIterator& operator =(const EntityIterator& i);

        EntityIterator operator +(int i);
        EntityIterator operator -(int i);

        Entity& operator*() const;
        void operator ++(int);
        void operator --(int);
        void operator ++();
        void operator --();
        Entity* operator ->();
        operator bool()const;

        EntityIterator
        (
            EntityView* view,
            int32_t group,
            int32_t index,
            char state
        );

        EntityIterator();

    private:

        EntityView* view;
        int32_t group;
        int32_t index;
        char state;
    };
    
}


namespace kege{

    class ConstEntityIterator
    {
    public:

        friend bool operator !=(const ConstEntityIterator& a, const ConstEntityIterator& b);
        ConstEntityIterator& operator =(const ConstEntityIterator& i);
        const Entity& operator*() const;

        ConstEntityIterator operator +(int i);
        ConstEntityIterator operator -(int i);

        void operator ++(int);
        void operator --(int);
        void operator ++();
        void operator --();
        const Entity* operator ->();
        operator bool()const;

        ConstEntityIterator
        (
            const EntityView* view,
            int32_t group,
            int32_t index,
            char state
        );

        ConstEntityIterator();

    private:

        const EntityView* view;
        int32_t group;
        int32_t index;
        char state;
    };

}
#endif /* entity_iterator_hpp */
