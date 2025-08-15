//
//  component-manager.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/19/25.
//

#ifndef component_manager_hpp
#define component_manager_hpp

#include <iostream>
#include "component-cache.hpp"

namespace kege{

    /**
     * @brief Internal structure representing an component object in the game world.
     */
    struct ComponentContainer
    {
        /**
         * @brief Stores the component IDs for each component type associated with this container.
         *
         * The key is the ComponentType, and the value is the ComponentID within the respective ComponentManager.
         */
        std::unordered_map< ComponentType, ComponentID > components;

        /**
         * @brief Bitset representing the signature of components this container possesses.
         *
         * Each bit corresponds to a specific ComponentType.
         */
        EntitySignature signature;

        /**
         * @brief Pointer to the next free container slot (used for efficient container creation and deletion).
         */
        uint32_t next;
    };


    /**
     * @brief Manages entities and their associated components.
     *
     * This class provides functionality to create, destroy, and manipulate entities,
     * as well as add, remove, and retrieve components for specific entities.
     * It utilizes a component-based container system (ECS) architecture.
     */
    class ComponentManager
    {
    public:

        /**
         * @brief Retrieves a constant pointer to the ComponentCacheT for a given component type.
         *
         * This is a private helper function used to access the component manager responsible
         * for components of type `Component`.
         * @tparam Component The type of the component.
         * @return A constant pointer to the ComponentCacheT for the given component type.
         */
        template< typename Component > const ComponentCacheT< Component >* getComponentCache()const
        {
            const uint32_t& TYPE = ComponentCacheT< Component >::_type;
            return reinterpret_cast< ComponentCacheT< Component >* >( _component_caches[ TYPE ] );
        }

        /**
         * @brief Retrieves a pointer to the ComponentCacheT for a given component type.
         *
         * This is a private helper function used to access or create the component manager
         * responsible for components of type `Component`. If a manager for this type
         * does not exist, it will be created and added to the internal storage.
         * @tparam Component The type of the component.
         * @return A pointer to the ComponentCacheT for the given component type.
         */
        template< typename Component > ComponentCacheT< Component >* getComponentCache()
        {
            ComponentCache*& cache = _component_caches[ ComponentCacheT< Component >::_type ];
            ComponentCacheT< Component >* cmgr = reinterpret_cast< ComponentCacheT< Component >* >( cache );

            if ( cmgr == nullptr )
            {
                cmgr = new ComponentCacheT< Component >();
                cache = cmgr;
            }
            return cmgr;
        }

        /**
         * @brief Adds a component of a specific type to an container, copying the provided component data.
         *
         * If the container already has a component of this type, it will be overwritten.
         * @tparam Component The type of the component to add.
         * @param container The ID of the container to add the component to.
         * @param component A constant reference to the component data to be copied.
         * @return A pointer to the newly added component instance.
         */
        template< typename Component > Component* set( uint32_t& container, const Component& component )
        {
            Component* cmp = add< Component >( container );
            *cmp = component;
            return cmp;
        }

        /**
         * @brief Adds a component of a specific type to an container.
         *
         * If the container already has a component of this type, this method will typically
         * return a pointer to the existing component.
         * @tparam Component The type of the component to add.
         * @param container The ID of the container to add the component to.
         * @return A pointer to the newly added or existing component instance.
         */
        template< typename Component > Component* add( uint32_t& container )
        {
            ComponentCacheT< Component >* cmgr = getComponentCache< Component >();

            if ( !_component_containers[ container ].signature.test( cmgr->_type ) )
            {
                _component_containers[ container ].components[ cmgr->_type ] = cmgr->create( container );
                _component_containers[ container ].signature.set( cmgr->_type );
            }

            return cmgr->get( _component_containers[ container ].components[ cmgr->_type ] );
        }

        /**
         * @brief Removes a component of a specific type from an container.
         *
         * If the container has the specified component, it will be removed and its resources
         * will be managed by the corresponding ComponentManager.
         * @tparam Component The type of the component to remove.
         * @param container The ID of the container to remove the component from.
         */
        template< typename Component > void erase( uint32_t& container )
        {
            ComponentCacheT< Component >* cmgr = getComponentCache< Component >();
            if ( _component_containers[ container ].signature.test( cmgr->_type ) )
            {
                cmgr->erase( _component_containers[ container ].components[ cmgr->_type ] );
                _component_containers[ container ].signature.reset( cmgr->_type );
                _component_containers[ container ].components[ cmgr->_type ] = 0;
            }
        }

        /**
         * @brief Gets a constant pointer to a component of a specific type for an container.
         * @tparam Component The type of the component to retrieve.
         * @param container The ID of the container whose component is being requested.
         * @return A constant pointer to the component, or nullptr if the container doesn't have this component.
         */
        template< typename Component > const Component* get( const uint32_t& container, const uint32_t& component )const
        {
            return getComponentCache< Component >()->get( component );
        }

        /**
         * @brief Gets a mutable pointer to a component of a specific type for an container.
         * @tparam Component The type of the component to retrieve.
         * @param container The ID of the container whose component is being requested.
         * @return A mutable pointer to the component, or nullptr if the container doesn't have this component.
         */
        template< typename Component > Component* get( uint32_t& container, const uint32_t& component )
        {
            return getComponentCache< Component >()->get( component );
        }

        /**
         * @brief Gets a constant pointer to a component of a specific type for an container.
         * @tparam Component The type of the component to retrieve.
         * @param container The ID of the container whose component is being requested.
         * @return A constant pointer to the component, or nullptr if the container doesn't have this component.
         */
        template< typename Component > const Component* get( const uint32_t& container )const
        {
            const ComponentCacheT< Component >* cmgr = getComponentCache< Component >();
            if ( cmgr == nullptr || !_component_containers[ container ].signature.test( cmgr->_type ) ) return nullptr;
            auto m = _component_containers[ container ].components.find( cmgr->_type );
            return cmgr->get( m->second );
        }

        /**
         * @brief Gets a mutable pointer to a component of a specific type for an container.
         * @tparam Component The type of the component to retrieve.
         * @param container The ID of the container whose component is being requested.
         * @return A mutable pointer to the component, or nullptr if the container doesn't have this component.
         */
        template< typename Component > Component* get( uint32_t& container )
        {
            ComponentCacheT< Component >* cmgr = getComponentCache< Component >();
            if ( !_component_containers[ container ].signature.test( cmgr->_type ) ) return nullptr;
            return cmgr->get( _component_containers[ container ].components[ cmgr->_type ] );
        }

        /**
         * @brief Checks if an container has a component of a specific type.
         * @tparam Component The type of the component to check for.
         * @param container The ID of the container to check.
         * @return True if the container has the component, false otherwise.
         */
        template< typename Component > bool has( const uint32_t& container )const
        {
            const uint32_t& TYPE = ComponentCacheT< Component >::_type;
            return _component_containers[ container ].signature.test( TYPE );
        }

        /**
         * @brief Gets the component signature of an container.
         *
         * The signature is a bitset representing the types of components the container possesses.
         * @param container The ID of the container whose signature is being requested.
         * @return A constant reference to the container's component signature.
         */
        const EntitySignature& signature( uint32_t container )const;

        /**
         * @brief Removes all components from an container and potentially detaches it from the hierarchy.
         * @param container The ID of the container to purge.
         */
        void purge( uint32_t& container );

        /**
         * @brief Creates a new container and returns its unique ID.
         * @return The ID of the newly created container.
         */
        uint32_t create();

        /**
         * @brief Initializes the container manager.
         * @return True if initialization was successful, false otherwise.
         */
        bool initialize();

        /**
         * @brief Shuts down the container manager, releasing all resources.
         */
        void shutdown();

        std::ostream& print(std::ostream& os, uint32_t entity);

        /**
         * @brief Destructor for the EntityManager.
         */
        ~ComponentManager();

        /**
         * @brief Default constructor for the EntityManager.
         */
        ComponentManager();

    private:

        /**
         * @brief Vector to store pointers to the ComponentManager instances for each component type.
         *
         * The index of the vector corresponds to the ComponentType.
         */
        std::vector< ComponentCache* > _component_caches;

        /**
         * @brief Vector to store the container data.
         */
        std::vector< ComponentContainer > _component_containers;

        /**
         * @brief Counter for the total number of entities currently alive.
         */
        int32_t _count;

        /**
         * @brief Index of the first free container slot in the _component_containers vector.
         */
        int32_t _head;

        /**
         * @brief Index of the last free container slot in the _component_containers vector.
         */
        int32_t _tail;
    };

}

#endif /* component_manager_hpp */
