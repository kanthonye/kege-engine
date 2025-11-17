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

//    struct EntityRegistry;
////    typedef std::vector< kege::ComponentType > ComponentTypes;
//
//    struct EntityRegistryKey
//    {
//        int32_t index;
//    };

    using EntityList = std::vector< kege::Entity >;

    class EntityRegistry : public kege::RefCounter
    {
    public:

        using Signature = kege::EntitySignature;

        void insert( const kege::Entity& e );
        void remove( const kege::Entity& e );
        const EntityList& get( const Signature& query_sig );

        void clear();

    public:

        // Exact archetype groups
        std::unordered_map< Signature, EntityList > _groups;

        // Cached system queries: query_sig → all entities with archetype ⊇ query_sig
        std::unordered_map< Signature, EntityList > _system_entities;

    };

}
#endif /* entity_registry_hpp */
