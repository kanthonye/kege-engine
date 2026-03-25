//
//  ecs.hpp
//  assets
//
//  Created by Kenneth Esdaile on 11/27/25.
//

#ifndef kege_ecs_hpp
#define kege_ecs_hpp

#include "ecs-system-factory.hpp"

namespace kege{

    class ECS : public kege::RefCounter
    {
    public:

        /**
         */
        template< typename... C > inline std::tuple<ecs::Entity, const C*...> fetch(ecs::Entity& entity)const
        {
            return _entity_manager.fetch<ecs::Entity, const C*...>( entity );
        }

        /**
         */
        template< typename... C > inline std::tuple<ecs::Entity, C*...> fetch(ecs::Entity& entity)
        {
            return _entity_manager.fetch<ecs::Entity, const C*...>( entity );
        }

        /**
         */
        template< typename... C > inline ecs::EntityKind* defnArchetype(uint32_t element_count)
        {
            return _entity_manager.defnArchetype<C*...>( element_count );
        }

        /**
         */
        template< typename C > inline const C* get( const ecs::Entity& entity )const
        {
            return _entity_manager.get<C>( entity );
        }

        /**
         */
        template< typename C > inline C* get( const ecs::Entity& entity )
        {
            return _entity_manager.get<C>( entity );
        }

        /**
         */
        template< typename C > uint64_t getCompId( const ecs::Entity& entity )
        {
            return _entity_manager.getCompId<C>( entity );
        }

        /**
         */
        inline void remove( int32_t component, ecs::Entity& entity )
        {
            return _entity_manager.remove( component, entity );
        }

        /**
         */
        template< typename C > inline void remove( ecs::Entity& entity )
        {
            return _entity_manager.remove<C>( entity );
        }

        /**
         */
        template< typename C > inline C* add( ecs::Entity& entity )
        {
            return _entity_manager.add<C>( entity );
        }

        /**
         */
        template< typename... C > inline ecs::Entity createWith()
        {
            return _entity_manager.createWith<C...>();
        }

        /**
         */
        inline void destroy( const ecs::Entity& entity )
        {
            return _entity_manager.destroy( entity );
        }

        /**
         */
        inline ecs::Entity create()
        {
            return _entity_manager.create();
        }

        /**
         */
        inline const ecs::Component::Layout& getLayout(const ecs::Entity& entity)const
        {
            return _entity_manager.getLayout( entity );
        }

        /**
         */
        inline const ecs::Signature& signature( const ecs::Entity& entity )const
        {
            return _entity_manager.signature( entity );
        }

        /**
         */
        inline bool valid( const ecs::Entity& entity )const
        {
            return _entity_manager.valid( entity );
        }

        /**
         */
        void attach(ecs::Entity& parent, ecs::Entity& child)
        {
            return _entity_manager.attach( parent,child );
        }

        /**
         */
        void detach(ecs::Entity& entity)
        {
            return _entity_manager.detach( entity );
        }

        /**
         * @brief Gets the ID of the parent entity of a given entity.
         * @param entity The ID of the entity whose parent is being requested.
         * @return The ID of the parent entity, or 0 if the entity has no parent.
         */
        ecs::Entity getParent( const ecs::Entity& entity )const
        {
            return _entity_manager.begin( entity );
        }

        /**
         * @brief Gets the ID of the root ancestor of a given entity in the hierarchy.
         * @param entity The ID of the entity whose root is being requested.
         * @return The ID of the root entity, or the entity's own ID if it has no parent.
         */
        ecs::Entity getRoot( const ecs::Entity& entity )const
        {
            return _entity_manager.begin( entity );
        }

        /**
         * @brief Checks if an entity has a parent in the hierarchy.
         * @param entity The ID of the entity to check.
         * @return True if the entity has a parent, false otherwise.
         */
        bool isParent( const ecs::Entity& entity )const
        {
            return _entity_manager.isParent( entity );
        }

        /**
         * @brief Checks if an entity is a child of another entity in the hierarchy.
         * @param entity The ID of the entity to check.
         * @return True if the entity has a parent, false otherwise.
         */
        bool isChild( const ecs::Entity& entity )const
        {
            return _entity_manager.isChild( entity );
        }

        /**
         * @brief Gets the ID of the first child of an entity in the hierarchy.
         * @param entity The ID of the parent entity.
         * @return The ID of the first child, or 0 if the entity has no children.
         */
        ecs::Entity begin( const ecs::Entity& entity )const
        {
            return _entity_manager.begin( entity );
        }

        /**
         * @brief Gets an invalid iterator-like value indicating the end of the children list.
         * @param entity The ID of the parent entity.
         * @return An invalid entity ID.
         */
        ecs::Entity end( const ecs::Entity& entity )const
        {
            return _entity_manager.end( entity );
        }

        /**
         * @brief Gets the ID of the next sibling of an entity in the hierarchy.
         * @param entity The ID of the current entity.
         * @return The ID of the next sibling, or 0 if there is no next sibling.
         */
        ecs::Entity next( const ecs::Entity& entity )const
        {
            return _entity_manager.next( entity );
        }

        /**
         * @brief Gets the ID of the previous sibling of an entity in the hierarchy.
         * @param entity The ID of the current entity.
         * @return The ID of the previous sibling, or 0 if there is no previous sibling.
         */
        ecs::Entity prev( const ecs::Entity& entity )const
        {
            return _entity_manager.prev( entity );
        }

        /**
         */
        inline void reset()
        {
            _entity_manager.reset();
        }

        /**
         */
        void addSystem( kege::Ref< kege::ecs::System > system );

        /**
         */
        void addSystem( const std::string& name );

        /**
         */
        const kege::ecs::EntityManager* getEntityManager()const;

        /**
         */
        kege::ecs::EntityManager* getEntityManager();

        /**
         */
        void setScene( kege::Scene* scene );

        /**
         */
        kege::Scene* getScene();

        /**
         */
        void update( double dms );

        /**
         */
        void render( double dms );

        /**
         */
        void input( double dms );

        /**
         */
        bool initialize();

        /**
         */
        void shutdown();

        /**
         */
        virtual ~ECS();

        /**
         */
        ECS();

    protected:

        std::vector< kege::Ref< kege::ecs::System > > _systems;

        /**
         * systems that require their update function to be called
         */
        std::vector< kege::ecs::System* > _system_updates;

        /**
         * systems that requires their render function to be called
         */
        std::vector< kege::ecs::System* > _system_renders;

        /**
         * systems that requires their input function to be called
         */
        std::vector< kege::ecs::System* > _system_inputs;

        //kege::InputContextManager* _input_context_manager;
        //kege::ProjectManager* _project_manager;
        //kege::RenderExecutor* _render_executor;

        kege::ecs::EntityManager _entity_manager;
        kege::Scene* _scene;
    };
}

namespace kege::ref{
    typedef kege::Ref< kege::ECS > ECS;
}

#endif /* ecs_hpp */
