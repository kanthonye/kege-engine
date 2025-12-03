//
//  ecs-entity-registry.hpp
//  assets
//
//  Created by Kenneth Esdaile on 11/27/25.
//

#ifndef ecs_entity_registry_hpp
#define ecs_entity_registry_hpp

#include "ecs-view.hpp"
#include "ecs-entity-manager.hpp"

namespace kege::ecs{

    class EntityRegistry : public Registry
    {
    private:

        using RegisteredEntities = kege::vector< kege::Ref<EntityStorage> >;
        using CacheView = std::unordered_map<Signature, kege::Ref< View >, SignatureHash, SignatureEq>;
        EntityStorage* createEntityStorage( uint32_t type, const Signature& signature );

    public:

        template<typename... C> ViewT<C...>& view();

        template< typename... C > void reserve( size_t size );

        virtual void insert(const Entity& e);
        virtual void remove(const Entity& e);
        virtual void purge();

        explicit EntityRegistry(EntityManager& ecs);
        virtual ~EntityRegistry();

    protected:

        RegisteredEntities _registered_entities;
        mutable CacheView _cached_views;
        EntityManager& _ecs;
    };

}

namespace kege::ecs{

    template< typename... C > void EntityRegistry::reserve( size_t size )
    {
        static Signature signature;
        if ( signature.empty() ) (signature.set( Component::type<C>()),...);

        int32_t kind_index = _ecs.signatureToType( signature );
        if ( kind_index >= _registered_entities.size())
        {
            _registered_entities.resize( kind_index + 1 );
            _registered_entities[ kind_index ] = new EntityStorage;
            _registered_entities[ kind_index ]->entities.resize( size );
        }
    }

    template<typename... C> ViewT<C...>& EntityRegistry::view()
    {
        static Signature signature;
        if ( signature.empty() ) (signature.set( Component::type<C>()),...);

        auto it = _cached_views.find(signature);
        if (it != _cached_views.end())
        {
            return *static_cast< ViewT<C...>* >(it->second.ref());
        }

//        int32_t type = _ecs.signatureToType( signature );
//        //TODO: type return -1 because of that we got EXE_BAD_ACCESS with _registered_entities[ type ]
//        if ( type >= _registered_entities.size())
//        {
//            _registered_entities.resize( type + 1 );
//            EntityStorage* storage = new EntityStorage;
//            _registered_entities[ type ] = storage;
//        }

        kege::vector< EntityStorage* > entity_storages;
        for (int type = 0; type < _registered_entities.size(); ++type )
        {
            if ( _registered_entities[ type ] )
            {
                if ( signature.isSubsetOf( _registered_entities[ type ]->signature ) )
                {
                    entity_storages.push_back( _registered_entities[ type ].ref() );
                }
            }
        }

        ViewT<C...>* view = new ViewT<C...>(&_ecs, entity_storages);
        _cached_views[ signature ] = view;
        return *view;
    }
}
#endif /* ecs_entity_registry_hpp */
