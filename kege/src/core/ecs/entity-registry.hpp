//
//  entity-registry.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/21/25.
//

#ifndef entity_registry_hpp
#define entity_registry_hpp

#include "entity.hpp"
#include "entity-view.hpp"
#include "entity-iterator.hpp"

namespace kege{

    struct EntityRegistry;
//    typedef std::vector< kege::ComponentType > ComponentTypes;

    struct EntityRegistryKey
    {
        int32_t index;
    };

    struct EntityRegistry
    {
    public:

        template< typename... T > kege::EntityView* getEntityView()
        {
            kege::EntitySignature signature;
            ( signature.set( kege::ComponentCacheT< T >::getType() ), ... );
            return getEntityView( signature );
        }

        EntityView* getEntityView( const kege::EntitySignature& signature );
        void updateViews( EntityGroup& new_group );
        const EntityGroup* getEntities( int index )const;
        EntityGroup* getEntities( int index );
        void insert( Entity& entity );
        void remove( Entity& entity );

        int getCount()const;
        void clear();
        ~EntityRegistry();

    private:

        std::unordered_map< kege::EntitySignature, EntityView* > _entity_views;
        std::unordered_map< kege::EntitySignature, uint32_t > _entity_group_index_table;
        std::vector< EntityGroup > _entities;
        friend EntityView;
    };

}
#endif /* entity_registry_hpp */
