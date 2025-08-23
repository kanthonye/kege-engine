//
//  entity-manager.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/19/25.
//

#ifndef entity_manager_hpp
#define entity_manager_hpp

#include "../memory/ref.hpp"
#include "component-manager.hpp"

namespace kege{

    /**
     * @brief Manages entities and their associated components.
     *
     * This class provides functionality to create, destroy, and manipulate entities,
     * as well as add, remove, and retrieve components for specific entities.
     * It utilizes a component-based entity system (ECS) architecture.
     */
    class EntityManager : public kege::RefCounter
    {
    public:

        template< typename Component > const ComponentCacheT< Component >* getComponentManager()const
        {
            return _component_manager.getComponentCache< Component >();
        }

        template< typename Component > ComponentCacheT< Component >* getComponentManager()
        {
            return _component_manager.getComponentCache< Component >();
        }

        /**
         * @brief Adds a component of a specific type to an entity, copying the provided component data.
         *
         * If the entity already has a component of this type, it will be overwritten.
         * @tparam Component The type of the component to add.
         * @param entity The ID of the entity to add the component to.
         * @param component A constant reference to the component data to be copied.
         * @return A pointer to the newly added component instance.
         */
        template< typename Component > Component* set( uint32_t& entity, const Component& component )
        {
            Component* cmp = _component_manager.get< Component >( _entities[ entity ].id );
            *cmp = component;
            return cmp;
        }

        /**
         * @brief Adds a component of a specific type to an entity, copying the provided component data.
         *
         * If the entity already has a component of this type, it will be overwritten.
         *
         * @tparam Component The type of the component to add.
         * @param entity The ID of the entity to add the component to.
         * @param component A constant reference to the component data to be copied.
         * @return A pointer to the newly added component instance.
         */
        template< typename Component > Component* add( uint32_t& entity, const Component& component )
        {
            Component* cmp = _component_manager.add< Component >( _entities[ entity ].id );
            *cmp = component;
            return cmp;
        }

        /**
         * @brief Adds a component of a specific type to an entity.
         *
         * If the entity already has a component of this type, this method will typically
         * return a pointer to the existing component.
         * @tparam Component The type of the component to add.
         * @param entity The ID of the entity to add the component to.
         * @return A pointer to the newly added or existing component instance.
         */
        template< typename Component > Component* add( uint32_t& entity )
        {
            return _component_manager.add< Component >( _entities[ entity ].id );
        }

        /**
         * @brief Removes a component of a specific type from an entity.
         *
         * If the entity has the specified component, it will be removed and its resources
         * will be managed by the corresponding ComponentManager.
         * @tparam Component The type of the component to remove.
         * @param entity The ID of the entity to remove the component from.
         */
        template< typename Component > void erase( uint32_t& entity )
        {
            _component_manager.erase< Component >( _entities[ entity ].id );
        }

        /**
         * @brief Gets a constant pointer to a component of a specific type for an entity.
         * @tparam Component The type of the component to retrieve.
         * @param entity The ID of the entity whose component is being requested.
         * @return A constant pointer to the component, or nullptr if the entity doesn't have this component.
         */
        template< typename Component > const Component* get( uint32_t entity, uint32_t component )const
        {
            return _component_manager.get< Component >( _entities[ entity ].id, component );
        }

        /**
         * @brief Gets a mutable pointer to a component of a specific type for an entity.
         * @tparam Component The type of the component to retrieve.
         * @param entity The ID of the entity whose component is being requested.
         * @return A mutable pointer to the component, or nullptr if the entity doesn't have this component.
         */
        template< typename Component > Component* get( uint32_t& entity, uint32_t component )
        {
            return _component_manager.get< Component >( _entities[ entity ].id, component );
        }

        /**
         * @brief Gets a constant pointer to a component of a specific type for an entity.
         * @tparam Component The type of the component to retrieve.
         * @param entity The ID of the entity whose component is being requested.
         * @return A constant pointer to the component, or nullptr if the entity doesn't have this component.
         */
        template< typename Component > const Component* get( uint32_t entity )const
        {
            return _component_manager.get< Component >( _entities[ entity ].id );
        }

        /**
         * @brief Gets a mutable pointer to a component of a specific type for an entity.
         * @tparam Component The type of the component to retrieve.
         * @param entity The ID of the entity whose component is being requested.
         * @return A mutable pointer to the component, or nullptr if the entity doesn't have this component.
         */
        template< typename Component > Component* get( uint32_t& entity )
        {
            return _component_manager.get< Component >( _entities[ entity ].id );
        }

        /**
         * @brief Checks if an entity has a component of a specific type.
         * @tparam Component The type of the component to check for.
         * @param entity The ID of the entity to check.
         * @return True if the entity has the component, false otherwise.
         */
        template< typename Component > bool has( const uint32_t& entity )const
        {
            return _component_manager.has< Component >( _entities[ entity ].id );
        }

        const EntityComponentMap& getEntityComponents( uint32_t entity )const
        {
            return _component_manager.getEntityComponents( _entities[ entity ].id );
        }

        /**
         * @brief Gets the component signature of an entity.
         *
         * The signature is a bitset representing the types of components the entity possesses.
         * @param entity The ID of the entity whose signature is being requested.
         * @return A constant reference to the entity's component signature.
         */
        const EntitySignature& signature( const uint32_t& entity )const
        {
            return _component_manager.signature( _entities[ entity ].id );
        }

        /**
         * @brief Attaches a child entity to a parent entity in a hierarchical structure.
         * @param parent The ID of the parent entity.
         * @param child The ID of the child entity to attach.
         */
        void attach( uint32_t& parent, const uint32_t& child );

        /**
         * @brief Detaches an entity from its parent in the hierarchical structure.
         * @param entity The ID of the entity to detach.
         */
        void detach( const uint32_t& entity );

        /**
         * @brief Gets the ID of the parent entity of a given entity.
         * @param entity The ID of the entity whose parent is being requested.
         * @return The ID of the parent entity, or 0 if the entity has no parent.
         */
        uint32_t getParent( const uint32_t& entity )const;

        /**
         * @brief Gets the ID of the root ancestor of a given entity in the hierarchy.
         * @param entity The ID of the entity whose root is being requested.
         * @return The ID of the root entity, or the entity's own ID if it has no parent.
         */
        uint32_t getRoot( const uint32_t& entity )const;

        /**
         * @brief Checks if an entity has a parent in the hierarchy.
         * @param entity The ID of the entity to check.
         * @return True if the entity has a parent, false otherwise.
         */
        bool isParent( const uint32_t& entity )const;

        /**
         * @brief Checks if an entity is a child of another entity in the hierarchy.
         * @param entity The ID of the entity to check.
         * @return True if the entity has a parent, false otherwise.
         */
        bool isChild( const uint32_t& entity )const;

        /**
         * @brief Gets the ID of the first child of an entity in the hierarchy.
         * @param entity The ID of the parent entity.
         * @return The ID of the first child, or 0 if the entity has no children.
         */
        uint32_t begin( const uint32_t& entity )const;

        /**
         * @brief Gets an invalid iterator-like value indicating the end of the children list.
         * @param entity The ID of the parent entity.
         * @return An invalid entity ID.
         */
        uint32_t end( const uint32_t& entity )const;

        /**
         * @brief Gets the ID of the next sibling of an entity in the hierarchy.
         * @param entity The ID of the current entity.
         * @return The ID of the next sibling, or 0 if there is no next sibling.
         */
        uint32_t next( const uint32_t& entity )const;

        /**
         * @brief Gets the ID of the previous sibling of an entity in the hierarchy.
         * @param entity The ID of the current entity.
         * @return The ID of the previous sibling, or 0 if there is no previous sibling.
         */
        uint32_t prev( const uint32_t& entity )const;

        /**
         * @brief Checks if an entity ID is valid and currently in use.
         * @param entity The ID of the entity to check.
         * @return True if the entity is valid, false otherwise.
         */
        bool isvalid( const uint32_t& entity )const;

        /**
         * @brief Removes all components from an entity and potentially detaches it from the hierarchy.
         * @param entity The ID of the entity to purge.
         */
        void destroy( uint32_t& entity );

        /**
         * @brief Creates a new entity and returns its unique ID.
         * @return The ID of the newly created entity.
         */
        uint32_t create();

        /**
         * @brief Initializes the entity manager.
         * @return True if initialization was successful, false otherwise.
         */
        bool initialize();

        /**
         * @brief Shuts down the entity manager, releasing all resources.
         */
        void shutdown();

        /**
         * @brief Destructor for the EntityManager.
         */
        ~EntityManager();

        /**
         * @brief Default constructor for the EntityManager.
         */
        EntityManager();


        std::ostream& print(std::ostream& os, uint32_t entity);
    private:

        /**
         * @brief Internal structure representing an entity in the game world.
         */
        struct EntityData
        {
            /**
             * @brief The ID of the entity info in the component manager.
             */
            uint32_t id;

            /**
             * @brief The ID of the parent entity in the hierarchy (0 indicates no parent).
             */
            uint32_t parent;

            /**
             * @brief The ID of the first child entity in the child list (0 indicates no children).
             */
            uint32_t head;

            /**
             * @brief The ID of the last child entity in the child list (0 indicates no children).
             */
            uint32_t tail;

            /**
             * @brief The ID of the next sibling entity in the parent's child list (0 indicates no next sibling).
             */
            uint32_t next;

            /**
             * @brief The ID of the previous sibling entity in the parent's child list (0 indicates no previous sibling).
             */
            uint32_t prev;

            /**
             * @brief The number of direct child entities.
             */
            uint32_t count;

            /**
             * @brief Pointer to the next free entity slot (used for efficient entity creation and deletion).
             */
            uint32_t next_free;
        };

    private:

        /**
         * @brief Vector to store pointers to the ComponentManager instances for each component type.
         *
         * The index of the vector corresponds to the ComponentType.
         */
        ComponentManager _component_manager;
        
        /**
         * @brief Vector to store the entity data.
         */
        std::vector< EntityData > _entities;

        /**
         * @brief Counter for the total number of entities currently alive.
         */
        int32_t _count;

        /**
         * @brief Index of the first free entity slot in the _entities vector.
         */
        int32_t _head;

        /**
         * @brief Index of the last free entity slot in the _entities vector.
         */
        int32_t _tail;
    };

}

#endif /* entity_manager_hpp */
