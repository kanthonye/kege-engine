//
//  entity-view.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/21/25.
//

#ifndef entity_view_hpp
#define entity_view_hpp

#include <set>
#include "entity.hpp"

namespace kege{

    struct EntityRegistry;
    class EntityIterator;
    class ConstEntityIterator;
    class EntityView;

    struct EntityGroup
    {
        std::set< EntityView* > viewers;
        std::vector< Entity > entities;
        kege::EntitySignature signature;
        uint32_t count = 0;
        uint32_t id = 0;
    };

    class EntityView
    {
    public:

        ConstEntityIterator begin()const;
        ConstEntityIterator end()const;
        EntityIterator begin();
        EntityIterator end();
        EntityView():_registry(nullptr){}

    private:

        friend bool operator !=(const ConstEntityIterator& a, const ConstEntityIterator& b);
        friend bool operator !=(const EntityIterator& a, const EntityIterator& b);
        friend void calcPlusPosition(int32_t& index, int32_t& group, int32_t amount, const EntityView* view);
        friend void calcMinusPosition(int32_t& index, int32_t& group, int32_t amount, const EntityView* view);


        const std::vector< Entity >& getEntities( uint32_t group )const;
        std::vector< Entity >& getEntities( uint32_t group );
        uint32_t getEntityCount( uint32_t group )const;

        const Entity& getEntity( uint32_t group, uint32_t index )const;
        Entity& getEntity( uint32_t group, uint32_t index );

        bool entityIsAccessible( uint32_t group, uint32_t index )const;
        bool isEqual( uint32_t group1, uint32_t index1, uint32_t group2, uint32_t index2 )const;

        std::vector< uint32_t > _groups;
        EntityRegistry* _registry;

        friend ConstEntityIterator;
        friend EntityIterator;
        friend EntityRegistry;
    };

}
#endif /* entity_view_hpp */
