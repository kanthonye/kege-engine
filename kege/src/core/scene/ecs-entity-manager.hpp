//
//  ecs-entity-manager.hpp
//  assets
//
//  Created by Kenneth Esdaile on 11/27/25.
//

#ifndef ecs_entity_manager_hpp
#define ecs_entity_manager_hpp

#include <tuple>
#include "ecs-entity-kind.hpp"

namespace kege::ecs{

    struct List
    {
        uint32_t head = INVALID_INDEX_U32;
        uint32_t tail = INVALID_INDEX_U32;
        int32_t count = 0;
    };

    struct Entity
    {
        constexpr operator bool()const{return id != 0;}
        union
        {
            struct
            {
                uint32_t index;
                uint32_t version;
            };
            uint64_t id = 0L;
        };
    };

    inline uint64_t to_uint64(ecs::Entity& e)
    {
        return (static_cast<uint64_t>(e.version) << 32) | e.index;
    }
    
    inline Entity from_uint64(uint64_t value)
    {
        return Entity
        {
            .index   = static_cast<uint32_t>(value & 0xFFFFFFFFull),
            .version = static_cast<uint32_t>(value >> 32)
        };
    }

    class Registry : public kege::RefCounter
    {
    public:

        virtual void insert(const ecs::Entity& e) = 0;
        virtual void remove(const ecs::Entity& e) = 0;
        virtual ~Registry(){}
    };
    
    struct Registration
    {
        Registry* registry = nullptr;
        uint32_t index = INVALID_INDEX_U32;
    };

    struct Parent
    {
        ecs::Entity head;
        ecs::Entity tail;
        uint32_t count;
    };

    struct Child
    {
        ecs::Entity parent;
        ecs::Entity prev;
        ecs::Entity next;
    };
}

namespace kege::ecs{

    class EntityManager
    {
    public:

        /**
         * @brief Structure representing an individual entity entry.
         *
         * Each entry contains the version of the entity,
         * pointers to the next and previous entities in the linked list,
         * and a flag indicating whether the entity is freed.
         */
        struct alignas(8) EntityEntry
        {
            uint32_t handle;
            uint32_t next;
            uint32_t version;
            uint32_t type;

            Registration registration;
        };


        EntityKind* createArchetype(const Component::Layout& layout)
        {
            if ( _archetype_count >= _archetypes.size() )
            {
                _archetypes.resize((_archetypes.empty())? 16 : _archetypes.size() * 2);
            }

            EntityKind* kind = new EntityKind(_archetype_count, layout, 0);
            _archetypes[ _archetype_count ] = kind;
            _archetype_count += 1;

            _archetypemap[ kind->signature() ] = kind;
            return kind;
        }

    public:

        template< typename... C > std::tuple<Entity, const C*...> fetch(Entity& entity)const;

        template< typename... C > std::tuple<Entity, C*...> fetch(Entity& entity);

        template< typename C > void remove( Entity& entity );

        template< typename C > C* add( Entity& entity );

        /**
         * @brief Retrieves a constant pointer to the component of the specified type associated with the given entity.
         * @tparam C The type of component to retrieve.
         * @param entity The handle of the entity.
         * @return A constant pointer to the component, or nullptr if not found.
         */
        template< typename C > const C* get( const Entity& entity )const;

        /**
         * @brief Retrieves a pointer to the component of the specified type associated with the given entity.
         * @tparam C The type of component to retrieve.
         * @param entity The handle of the entity.
         * @return A pointer to the component, or nullptr if not found.
         */
        template< typename C > C* get( const Entity& entity );

        template< typename C > uint64_t getCompId( const Entity& entity );

        template< typename... C > EntityKind* defnArchetype(uint32_t element_count);

        template< typename... C > Entity createWith();

        void remove( int32_t comp_type, Entity& entity );

        inline const Component::Layout& getLayout(const Entity& entity)const
        {
            return _archetypes[ _entities[ entity.index ].type ]->getLayout();
        }

        Registration* getRegistration( const Entity& entity );

        void destroy( const Entity& entity );

        Entity create();

        inline bool valid( const Entity& entity )const
        {
            return (entity.index >= _entity_count) ? false
            : _entities[ entity.index ].version == entity.version;
        }

        uint32_t getArchetypeCount()const
        {
            return _archetype_count;
        }

        uint32_t getFreedCount()const
        {
            return _freed.count;
        }

        uint32_t getMaxCount()const
        {
            return _entity_count;
        }

        inline const Signature& signature( const Entity& entity )const
        {
            return _archetypes[ type( entity ) ]->signature();
        }

        inline uint32_t type( const Entity& entity )const
        {
            return _entities[ entity.index ].type;
        }

        inline const Signature& getArchetypeSignature(int type )
        {
            return _archetypes[ type ]->signature();
        }

        uint32_t signatureToType( const Signature& signature )const;
        void printMemoryUsage()const;
        void reset();

        void attach(ecs::Entity& parent, ecs::Entity& child);
        void detach(ecs::Entity& child);

        /**
         * @brief Gets the ID of the parent entity of a given entity.
         * @param entity The ID of the entity whose parent is being requested.
         * @return The ID of the parent entity, or 0 if the entity has no parent.
         */
        ecs::Entity getParent( const ecs::Entity& entity )const;

        /**
         * @brief Gets the ID of the root ancestor of a given entity in the hierarchy.
         * @param entity The ID of the entity whose root is being requested.
         * @return The ID of the root entity, or the entity's own ID if it has no parent.
         */
        ecs::Entity getRoot( const ecs::Entity& entity )const;

        /**
         * @brief Checks if an entity has a parent in the hierarchy.
         * @param entity The ID of the entity to check.
         * @return True if the entity has a parent, false otherwise.
         */
        bool isParent( const ecs::Entity& entity )const;

        /**
         * @brief Checks if an entity is a child of another entity in the hierarchy.
         * @param entity The ID of the entity to check.
         * @return True if the entity has a parent, false otherwise.
         */
        bool isChild( const ecs::Entity& entity )const;

        /**
         * @brief Gets the ID of the first child of an entity in the hierarchy.
         * @param entity The ID of the parent entity.
         * @return The ID of the first child, or 0 if the entity has no children.
         */
        ecs::Entity begin( const ecs::Entity& entity )const;

        /**
         * @brief Gets an invalid iterator-like value indicating the end of the children list.
         * @param entity The ID of the parent entity.
         * @return An invalid entity ID.
         */
        ecs::Entity end( const ecs::Entity& entity )const;

        /**
         * @brief Gets the ID of the next sibling of an entity in the hierarchy.
         * @param entity The ID of the current entity.
         * @return The ID of the next sibling, or 0 if there is no next sibling.
         */
        ecs::Entity next( const ecs::Entity& entity )const;

        /**
         * @brief Gets the ID of the previous sibling of an entity in the hierarchy.
         * @param entity The ID of the current entity.
         * @return The ID of the previous sibling, or 0 if there is no previous sibling.
         */
        ecs::Entity prev( const ecs::Entity& entity )const;

        EntityManager( size_t size = 0 );
        ~EntityManager();

    private:

        void detach(ecs::Entity& child, ecs::Parent& p, ecs::Child& c);

    private:

        std::unordered_map<Signature, EntityKind*, SignatureHash, SignatureEq> _archetypemap;
        kege::array<kege::Ref<EntityKind>> _archetypes;
        uint32_t _archetype_count;

        kege::array< EntityEntry > _entities;
        uint32_t _entity_count;
        List _freed;
    };

}

namespace kege::ecs{

    template< typename... C > std::tuple<Entity, const C*...> EntityManager::fetch(Entity& entity)const
    {
        if ( !valid( entity ) ) return std::tuple<Entity, C*...>{};
        const EntityEntry& entry = _entities[ entity.index ];
        const EntityKind* archetype = _archetypes[ entry.type ].ref();

        return std::tuple<Entity, const C*...>
        ( entity, reinterpret_cast<const C*>( archetype->getComponent( entry.handle, Component::type<C>() ) )... );
    }

    template< typename... C > std::tuple<Entity, C*...> EntityManager::fetch(Entity& entity)
    {
        if ( !valid( entity ) ) return std::tuple<Entity, C*...>{};
        const EntityEntry& entry = _entities[ entity.index ];
        EntityKind* archetype = _archetypes[ entry.type ].ref();

        return std::tuple<Entity, C*...>
        ( entity, reinterpret_cast<C*>( archetype->getComponent( entry.handle, Component::type<C>() ) )... );
    }

    template< typename C > C* EntityManager::add( Entity& entity )
    {
        ComponentType comp_type = Component::type<C>();
        EntityEntry& entry = _entities[ entity.index ];
        EntityKind* current = _archetypes[ entry.type ].ref();
        EntityKind::Edge& edge = current->_edges[ comp_type ];
        if (edge.add == nullptr)
        {
            Signature signature = current->signature();
            signature.set( comp_type );

            // check if archetype exist in archetype map, if not create the new archetype
            auto src = _archetypemap.find( signature );
            if (src == _archetypemap.end())
            {
                _archetypes[_archetype_count] = current->dupWith( _archetype_count, Component::getInfo<C>() );
                edge.add = _archetypes[_archetype_count].ref();
                _archetypemap[signature] = edge.add;
                _archetype_count += 1;
            }
            else
            {
                edge.add = src->second;
            }
        }

        if ( entry.registration.registry )
        {
            entry.registration.registry->remove( entity );
        }

        uint32_t src_index = entry.handle;
        entry.handle = edge.add->move(current, src_index);
        entry.type = edge.add->type();
        current->erase(src_index);


        if ( entry.registration.registry )
        {
            entry.registration.registry->insert( entity );
        }

        return static_cast<C*>(edge.add->getComponent(entry.handle, comp_type));
    }

    template< typename C > void EntityManager::remove( Entity& entity )
    {
        remove( Component::type<C>(), entity );
    }

    /**
     * @brief Retrieves a constant pointer to the component of the specified type associated with the given entity.
     * @tparam C The type of component to retrieve.
     * @param entity The handle of the entity.
     * @return A constant pointer to the component, or nullptr if not found.
     */
    template< typename C > const C* EntityManager::get( const Entity& entity )const
    {
        if ( !valid( entity ) ) return nullptr;
        const EntityEntry& entry = _entities[ entity.index ];
        return (C*) _archetypes[ entry.type ]->getComponent( entry.handle, Component::type<C>() );
    }

    /**
     * @brief Retrieves a pointer to the component of the specified type associated with the given entity.
     * @tparam C The type of component to retrieve.
     * @param entity The handle of the entity.
     * @return A pointer to the component, or nullptr if not found.
     */
    template< typename C > C* EntityManager::get( const Entity& entity )
    {
        if ( !valid( entity ) ) return nullptr;
        EntityEntry& entry = _entities[ entity.index ];
        return (C*) _archetypes[ entry.type ]->getComponent( entry.handle, Component::type<C>() );
    }

   template< typename C > uint64_t EntityManager::getCompId( const Entity& entity )
   {
       if ( !valid( entity ) ) return 0;
       EntityEntry& entry = _entities[ entity.index ];

       //uint64_t component_id;
       uint32_t eid = entry.handle;
       uint32_t typ = Component::type<C>();

       return (uint64_t(eid) << 32) | typ;
   }

    template< typename... C > EntityKind* EntityManager::defnArchetype(uint32_t element_count)
    {
        if ( _archetype_count >= _archetypes.size() )
        {
            _archetypes.resize((_archetypes.empty())? 16 : _archetypes.size() * 2);
        }
        EntityKind* archetype = new EntityKind(_archetype_count, Component::getLayout<C...>(), element_count);
        _archetypemap[ archetype->signature() ] = archetype;
        _archetypes[ _archetype_count ] = archetype;
        _archetype_count += 1;
        return archetype;
    }

    template< typename... C > Entity EntityManager::createWith()
    {
        EntityKind* archetype;

        /*
         STEP 1: create entity component signature. This is necessary looking up
         the archetype they entity belongs to.
         */
        static Signature signature;
        if(signature.empty())( signature.set( Component::type<C>() ), ...);

        /*
         STEP 2: find the archetype the entity belongs to. if the archetype doesn't
         exist, create a new archetype using the available parameters.
         */
        auto i = _archetypemap.find( signature );
        if (i == _archetypemap.end())
        {
            archetype = defnArchetype<C...>(0);
        }
        else
        {
            archetype = i->second;
        }

        /*
         STEP 3: Aquire an entity block, and set its archetype parameters
         */
        Entity entity = create();

        EntityEntry& entry = _entities[ entity.index ];
        entry.handle = archetype->create();
        entry.type = archetype->type();

        return entity;
    }
}
#endif /* ecs_entity_manager_hpp */
