//
//  ecs.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/21/25.
//

#ifndef kege_ecs_hpp
#define kege_ecs_hpp

#include "entity-manager.hpp"

namespace kege{

    /**
     * @brief Represents an entity within the game world and manages its associated components and hierarchical relationships.
     *
     * This class provides an interface to interact with entities in a component-based entity system (ECS) architecture.
     * It allows for the dynamic addition, removal, and retrieval of components, as well as the management of parent-child
     * relationships within a scene graph. Each `Entity` object holds a unique identifier and interacts with an underlying
     * `EntityManager` to manage its data.
     */
    class Entity
    {
    public:

        friend std::ostream& operator <<(std::ostream& os, const Entity& entity);

        static kege::EntityManager& getManager();

        /**
         * @brief Set the data of a specific component type.
         *
         * If the entity already possesses a component of the same type, the existing component will be overwritten
         * with the new data.
         *
         * @tparam Component The type of the component to add.
         * @param component A constant reference to the component data that will be copied into the entity.
         * @return A pointer to the newly added component instance within the `EntityManager`. Returns `nullptr`
         * if the entity ID is invalid.
         */
        template< typename Component > Component* set( const Component& component );

        /**
         * @brief Adds a component of a specific type to this entity, copying the provided component data.
         *
         * If the entity already possesses a component of the same type, the existing component will be overwritten
         * with the new data.
         *
         * @tparam Component The type of the component to add.
         * @param component A constant reference to the component data that will be copied into the entity.
         * @return A pointer to the newly added component instance within the `EntityManager`. Returns `nullptr`
         * if the entity ID is invalid.
         */
        template< typename Component > Component* add( const Component& component );

        /**
         * @brief Adds a component of a specific type to this entity using its default constructor.
         *
         * If the entity already has a component of this type, this method will typically return a pointer
         * to the existing component instance.
         *
         * @tparam Component The type of the component to add. The component will be default-constructed.
         * @return A pointer to the newly added or existing component instance within the `EntityManager`.
         */
        template< typename Component > Component* add();

        /**
         * @brief Removes a component of a specific type from this entity.
         *
         * If the entity has the specified component, it will be removed, and its resources will be managed
         * by the corresponding `ComponentManager`.
         * @tparam Component The type of the component to remove.
         */
        template< typename Component > void erase();

        /**
         * @brief Gets a constant pointer to a component of a specific type for this entity.
         *
         * This method provides read-only access to the component data.
         *
         * @tparam Component The type of the component to retrieve.
         * @param cid The ID of the component to retrieve.
         * @return A constant pointer to the component instance, or `nullptr` if the entity does not have
         * a component of the specified type or if the entity ID is invalid.
         */
        template< typename Component > const Component* get( uint32_t cid ) const;

        /**
         * @brief Gets a mutable pointer to a component of a specific type for this entity.
         *
         * This method allows modification of the component's data.
         *
         * @tparam Component The type of the component to retrieve.
         * @param cid The ID of the component to retrieve.
         * @return A mutable pointer to the component instance, or `nullptr` if the entity does not have
         * a component of the specified type or if the entity ID is invalid.
         */
        template< typename Component > Component* get( uint32_t cid );

        /**
         * @brief Gets a constant pointer to a component of a specific type for this entity.
         *
         * This method provides read-only access to the component data.
         *
         * @tparam Component The type of the component to retrieve.
         * @return A constant pointer to the component instance, or `nullptr` if the entity does not have
         * a component of the specified type or if the entity ID is invalid.
         */
        template< typename Component > const Component* get() const;

        /**
         * @brief Gets a mutable pointer to a component of a specific type for this entity.
         *
         * This method allows modification of the component's data.
         * @tparam Component The type of the component to retrieve.
         * @return A mutable pointer to the component instance, or `nullptr` if the entity does not have
         * a component of the specified type or if the entity ID is invalid.
         */
        template< typename Component > Component* get();

        /**
         * @brief Gets a mutable pointer to a component of a specific type for this entity.
         *
         * This method allows modification of the component's data.
         * @tparam Component The type of the component to retrieve.
         * @return A mutable pointer to the component instance, or `nullptr` if the entity does not have
         * a component of the specified type or if the entity ID is invalid.
         */
        template< typename Component > uint32_t getComponentID()const;

        /**
         * @brief Checks if this entity possesses a component of a specific type.
         * @tparam Component The type of the component to check for.
         * @return `true` if the entity has the component, `false` otherwise, or if the entity ID is invalid.
         */
        template< typename Component > bool has() const;

        friend bool operator ==( const Entity& a, const Entity& b );
        friend bool operator !=( const Entity& a, const Entity& b );
        friend bool operator ==( const Entity& a, int32_t i );
        friend bool operator !=( int32_t i, const Entity& b );

        /**
         * @brief Gets the component signature of this entity.
         *
         * The signature is a bitset where each bit represents the presence or absence of a specific
         * component type on this entity. This is often used for efficient filtering of entities based
         * on their component composition.
         * @return A constant reference to the entity's component signature. The signature is managed
         * by the `EntityManager`.
         */
        const kege::EntitySignature& signature() const;

        /**
         * @brief Attaches a child entity to this entity in a hierarchical structure, making this entity the parent.
         * @param child The `Entity` object representing the child to attach. Note that the child's ID is used.
         */
        void attach( const Entity& child );

        /**
         * @brief Detaches this entity from its parent in the hierarchical structure, if it has one.
         */
        void detach();

        /**
         * @brief Gets the parent entity of this entity in the hierarchy.
         * @return An `Entity` object representing the parent. The returned entity will have an ID of 0
         * if this entity has no parent or if this entity's ID is invalid.
         */
        Entity getParent() const;

        /**
         * @brief Gets the root ancestor of this entity in the hierarchical structure.
         *
         * This traverses up the parent chain until it reaches an entity with no parent.
         * @return An `Entity` object representing the root ancestor. If this entity has no parent,
         * it will return itself. Returns an entity with ID 0 if the current entity's ID is invalid.
         */
        Entity getRoot() const;

        /**
         * @brief Checks if this entity has any children in the hierarchical structure.
         * @return `true` if the entity has at least one child, `false` otherwise, or if the entity ID is invalid.
         */
        bool isParent() const;

        /**
         * @brief Checks if this entity has a parent in the hierarchical structure.
         * @return `true` if the entity has a parent, `false` otherwise, or if the entity ID is invalid.
         */
        bool isChild() const;

        /**
         * @brief Gets the first child of this entity in the hierarchical structure for iteration.
         * @return An `Entity` object representing the first child. Returns an entity with ID 0 if
         * this entity has no children or if this entity's ID is invalid. This is the starting
         * point for traversing the children using `next()`.
         */
        Entity begin() const;

        /**
         * @brief Gets an invalid `Entity` object (with ID 0) that serves as an end iterator for the children list.
         * @return An `Entity` object with an ID of 0, indicating the end of the children.
         */
        Entity end() const;

        /**
         * @brief Gets the next sibling of this entity in the hierarchical structure.
         * @return An `Entity` object representing the next sibling. Returns an entity with ID 0 if
         * there is no next sibling or if this entity's ID is invalid.
         */
        Entity next() const;

        /**
         * @brief Gets the previous sibling of this entity in the hierarchical structure.
         * @return An `Entity` object representing the previous sibling. Returns an entity with ID 0 if
         * there is no previous sibling or if this entity's ID is invalid.
         */
        Entity prev() const;

        /**
         * @brief Checks if this entity's ID is valid and if the entity is currently active in the system.
         * @return `true` if the entity is valid, `false` otherwise.
         */
        bool isvalid() const;

        /**
         * @brief Checks if this entity's ID is valid and if the entity is currently active in the system.
         * @return `true` if the entity is valid, `false` otherwise.
         */
        operator bool()const;

        /**
         * @brief Removes all components from this entity and detaches it from its parent in the hierarchy.
         *
         * After calling this method, the entity's ID might be reused by the `EntityManager` in the future.
         */
        void destroy();

        /**
         * @brief Creates a new entity and returns an `Entity` object representing it.
         *
         * This is a static factory method that interacts with the global `EntityManager` to allocate
         * a new entity ID.
         * @return An `Entity` object representing the newly created entity.
         */
        static Entity create();

        /**
         * @brief Initializes the entity manager.
         * @return True if initialization was successful, false otherwise.
         */
        static bool initialize();

        /**
         * @brief Shuts down the entity manager, releasing all resources.
         */
        static void shutdown();

        /**
         * @brief Virtual destructor for the `Entity` class.
         *
         * While the `Entity` class itself doesn't manage any resources directly, a virtual destructor
         * is good practice in case you inherit from this class in the future and those derived classes
         * might need to perform cleanup.
         */
        virtual ~Entity();

        /**
         * @brief Constructor for the `Entity` class, used internally to wrap an entity ID.
         * @param id The unique identifier of the entity.
         */
        Entity(uint32_t id);

        /**
         * @brief Default constructor for the `Entity` class, creating an invalid entity (ID 0).
         */
        Entity();

    private:

        /**
         * @brief Static instance of the `EntityManager` that manages all entities and their components.
         *
         * This single instance is shared across all `Entity` objects.
         */
        static kege::EntityManager _mgr;

        /**
         * @brief The unique identifier of this entity.
         */
        uint32_t _id;
    };


    template< typename Component >
    Component* Entity::set( const Component& component )
    {
        return ( _mgr.isvalid( _id ) ) ? _mgr.set< Component >( _id, component ) : nullptr;
    }


    template< typename Component >
    Component* Entity::add( const Component& component )
    {
        return ( _mgr.isvalid( _id ) ) ? _mgr.add< Component >( _id, component ) : nullptr;
    }

    template< typename Component >
    Component* Entity::add()
    {
        return _mgr.add< Component >( _id );
    }

    template< typename Component >
    void Entity::erase()
    {
        if ( _mgr.isvalid( _id ) ) _mgr.erase< Component >( _id );
    }

    template< typename Component >
    const Component* Entity::get( uint32_t cid ) const
    {
        return ( _mgr.isvalid( _id ) ) ? _mgr.get< Component >( _id, cid ) : nullptr;
    }

    template< typename Component >
    Component* Entity::get( uint32_t cid )
    {
        return ( _mgr.isvalid( _id ) ) ? _mgr.get< Component >( _id, cid ) : nullptr;
    }

    template< typename Component >
    const Component* Entity::get() const
    {
        return ( _mgr.isvalid( _id ) ) ? _mgr.get< Component >( _id ) : nullptr;
    }

    template< typename Component >
    Component* Entity::get()
    {
        return ( _mgr.isvalid( _id ) ) ? _mgr.get< Component >( _id ) : nullptr;
    }

    template< typename Component >
    bool Entity::has() const
    {
        return ( _mgr.isvalid( _id ) ) ? _mgr.has< Component >( _id ) : false;
    }

}

#endif /* ecs_hpp */
