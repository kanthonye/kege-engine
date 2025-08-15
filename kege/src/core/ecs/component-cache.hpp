//
//  component-cache.hpp
//  physics
//
//  Created by Kenneth Esdaile on 4/20/25.
//

#ifndef component_cache_hpp
#define component_cache_hpp

#include <stdlib.h>
#include <bitset>
#include <vector>
#include <unordered_map>

namespace kege{

    enum{ MAX_COMPONENT_TYPES = 128 };

    /**
     * @brief Represents a unique identifier for an entity in the game world.
     */
    typedef uint32_t EntityID;

    /**
     * @brief Represents a unique identifier for a specific component instance.
     */
    typedef uint32_t ComponentID;

    /**
     * @brief Represents a unique type identifier for a specific component type.
     */
    typedef uint32_t ComponentType;


    typedef std::bitset< MAX_COMPONENT_TYPES > EntitySignature;


    /**
     * @brief Abstract base class for managing components.
     *
     * Defines the interface for creating, deleting, and accessing component metadata.
     * Concrete implementations will handle the storage and retrieval of specific component types.
     */
    class ComponentCache
    {
    public:

        /**
         * @brief Gets the EntityID of the entity that owns the given component.
         * @param component_id The ID of the component.
         * @return The EntityID of the owner, or potentially an invalid ID if the component is not valid.
         */
        virtual uint32_t owner( ComponentID component_id )const = 0;

        /**
         * @brief Checks if a given ComponentID is valid and currently in use.
         * @param component_id The ID of the component to check.
         * @return True if the component is valid, false otherwise.
         */
        virtual bool isvalid( ComponentID component_id )const = 0;

        /**
         * @brief Removes and invalidates a component with the given ID.
         * @param component_id The ID of the component to erase.
         */
        virtual void erase( ComponentID component_id ) = 0;

        /**
         * @brief Creates a new component instance and associates it with the given EntityID.
         * @param entity_id The ID of the entity that will own the new component.
         * @return The ID of the newly created component.
         */
        virtual ComponentID create( EntityID entity_id ) = 0;

        /**
         * @brief Removes all components managed by this manager.
         */
        virtual void purge() = 0;

        /**
         * @brief Virtual destructor to ensure proper cleanup in derived classes.
         */
        virtual ~ComponentCache(){}

        /**
         * @brief Static counter used to assign unique ComponentType values to different component types.
         */
        static uint32_t _type_counter;
    };

}


namespace kege{

    class ComponentManager;

    /**
     * @brief Template class for managing components of a specific type.
     *
     * Inherits from ComponentCache and provides concrete implementations for managing
     * components of the template parameter `Component`. Uses a free-list strategy for efficient
     * allocation and deallocation of component instances.
     *
     * @tparam Component The type of the component being managed.
     */
    template< typename Component > class ComponentCacheT : public ComponentCache
    {
    public:

        class ConstIterator
        {
        public:

            inline const Component* operator ->()
            {
                return _components->get( _component );
            }

            void operator --(int)
            {
                _component--;
                return ConstIterator( _components, _component );
            }
            void operator -()
            {
                _component--;
                return ConstIterator( _components, _component );
            }

            void operator ++(int)
            {
                _component++;
                return ConstIterator( _components, _component );
            }
            void operator ++()
            {
                _component++;
                return ConstIterator( _components, _component );
            }

            ConstIterator( ComponentCacheT< Component >* components, ComponentID component )
            :   _components( components )
            ,   _component( component )
            {}

            ConstIterator()
            :   _components( nullptr )
            ,   _component( -1 )
            {}

        private:

            ComponentID _component;
            ComponentCacheT< Component >* _components;
            friend ComponentCacheT< Component >;
        };

        class Iterator
        {
        public:

            friend inline Iterator operator -( const Iterator& other, int32_t i )
            {
                return Iterator( other._components, other._component - i );
            }

            friend inline Iterator operator +( const Iterator& other, int32_t i )
            {
                return Iterator( other._components, other._component + i );
            }

            friend inline Iterator operator -( int32_t i, const Iterator& other )
            {
                return Iterator( other._components, i - other._component );
            }

            friend inline Iterator operator +( int32_t i, const Iterator& other )
            {
                return Iterator( other._components, i + other._component + i );
            }

            friend inline bool operator !=( const Iterator& a, const Iterator& b )
            {
                return a._component != b._component;
            }
            friend inline bool operator ==( const Iterator& a, const Iterator& b )
            {
                return a._componens == b._component;
            }

            inline Component* operator ->()
            {
                return _components->get( _component );
            }

            inline Component* operator *()
            {
                return _components->get( _component );
            }

            Iterator operator --(int)
            {
                _component--;
                return Iterator( _components, _component );
            }

            Iterator operator --()
            {
                _component--;
                return Iterator( _components, _component );
            }

            Iterator operator ++(int)
            {
                _component++;
                return Iterator( _components, _component );
            }

            Iterator operator ++()
            {
                _component++;
                return Iterator( _components, _component );
            }

            Iterator( ComponentCacheT< Component >* components, ComponentID component )
            :   _components( components )
            ,   _component( component )
            {}

            Iterator()
            :   _components( nullptr )
            ,   _component( -1 )
            {}

        private:

            ComponentID _component;
            ComponentCacheT< Component >* _components;
            friend ComponentCacheT< Component >;
        };

        ConstIterator begin()const
        {
            return ConstIterator( this, _head );
        }

        ConstIterator end()const
        {
            return ConstIterator( this, _tail );
        }

        Iterator begin()
        {
            return Iterator( this, _head );
        }

        Iterator end()
        {
            return Iterator( this, _tail );
        }

    protected:

        void remove( int32_t& head, int32_t& tail, ComponentID index )
        {
            if ( head == index )
            {
                head = _components[ head ].next;
                if( head < 0 )
                {
                    tail = -1;
                }
            }
            else if ( tail == index )
            {
                tail = _components[ head ].prev;
                if( tail < 0 )
                {
                    head = -1;
                }
            }
            else
            {
                int32_t prev = _components[ index ].prev;
                int32_t next = _components[ index ].next;
                _components[ next ].prev = prev;
                _components[ prev ].next = next;
            }
            _components[ index ].owner = 0;
        }

        void insert( int32_t& head, int32_t& tail, int32_t index )
        {
            if ( head < 0 )
            {
                tail = head = index;
            }
            else
            {
                _components[ index ].prev = tail;
                _components[ tail ].next = index;
                tail = index;
            }
        }

    public:

        /**
         * @brief Marks a component as free for reuse.
         *
         * Adds the component ID to the free-list. The component's owner is reset to 0.
         * @param index The ID of the component to erase.
         */
        void erase( ComponentID index )
        {
            remove( _head, _tail, index );
            insert( _free_head, _free_tail, index );
        }

        /**
         * @brief Gets a constant pointer to the component data with the given ID.
         * @param index The ID of the component to retrieve.
         * @return A constant pointer to the component data, or nullptr if the index is invalid.
         */
        const Component* get( ComponentID index )const
        {
            return ( index < _components.size() )? &_components[ index ].data : nullptr;
        }

        /**
         * @brief Gets a mutable pointer to the component data with the given ID.
         * @param index The ID of the component to retrieve.
         * @return A mutable pointer to the component data, or nullptr if the index is invalid.
         */
        Component* get( ComponentID index )
        {
            return ( index < _components.size() )? &_components[ index ].data : nullptr;
        }

        /**
         * @brief Checks if a given ComponentID is valid and currently owned by an entity.
         * @param component The ID of the component to check.
         * @return True if the component is valid (has a non-zero owner), false otherwise.
         */
        bool isvalid( ComponentID component )const
        {
            return owner( component ) >= 0;
        }

        /**
         * @brief Gets the EntityID of the entity that owns the given component.
         * @param component The ID of the component.
         * @return The EntityID of the owner, or false (which will be implicitly cast to 0) if the component is not valid.
         */
        EntityID owner( ComponentID component )const
        {
            return ( component < _components.size() )? _components[ component ].owner : false;
        }

        /**
         * @brief Creates a new component instance and associates it with the given EntityID.
         *
         * Tries to reuse a component from the free-list. If the free-list is empty, a new
         * component is allocated.
         * @param entity The ID of the entity that will own the new component.
         * @return The ID of the newly created component.
         */
        ComponentID create( EntityID entity )
        {
            ComponentID component_id;
            if ( _free_head >= 0 )
            {
                component_id = _free_head;
                _free_head = _components[ _free_head ].next;
                if ( _free_head == _free_tail )
                {
                    _free_head = _free_tail = 0;
                }
            }
            else
            {
                component_id = _count;
                if ( _count >= _components.size() )
                {
                    _components.resize( 2 + 2 * _components.size() );
                }
                _count++;
            }

            insert( _head, _tail, component_id );
            _components[ component_id ].owner = entity;
            return component_id;
        }


        static ComponentType getType()
        {
            return _type;
        }

        /**
         * @brief Removes all component instances managed by this manager and clears the underlying storage.
         */
        void purge()
        {
            _components.clear();
        }

        /**
         * @brief Destructor for the ComponentCacheT.
         *
         * Calls the purge method to release all allocated memory.
         */
        ~ComponentCacheT()
        {
            purge();
        }

        /**
         * @brief Default constructor for ComponentCacheT.
         *
         * Initializes the internal counters and free-list pointers.
         */
        ComponentCacheT()
        :   _count( 0 )
        ,   _free_head( -1 )
        ,   _free_tail( -1 )
        ,   _head( -1 )
        ,   _tail( -1 )
        {}

    private:

        /**
         * @brief Internal structure to hold the component data and metadata.
         */
        struct ComponentInfo
        {
            /**
             * @brief The actual component data.
             */
            Component data;

            /**
             * @brief The EntityID of the entity that owns this component.
             */
            EntityID owner;

            /**
             * @brief Index of the next free component in the free-list (used when a component is erased).
             */
            int32_t next;
            int32_t prev;
        };

    private:

        /**
         * @brief Vector to store the component objects.
         */
        std::vector< ComponentInfo > _components;

        /**
         * @brief Counter for the total number of components created (including those in the free-list).
         */
        uint32_t _count;

        /**
         * @brief Index of the first free component in the free-list.
         */
        int32_t _free_head;

        /**
         * @brief Index of the last free component in the free-list.
         */
        int32_t _free_tail;


        /**
         * @brief Index of the first free component in the free-list.
         */
        int32_t _head;

        /**
         * @brief Index of the last free component in the free-list.
         */
        int32_t _tail;

        /**
         * @brief Static member to store the unique ComponentType for this component type.
         */
        static ComponentType _type;

        /**
         * @brief Declares the EntityManager as a friend class, allowing it to access private members.
         */
        friend ComponentManager;
    };

    template< typename T > uint32_t ComponentCacheT< T >::_type = ComponentCache::_type_counter++;

    enum Relationship
    {
        DISJOINT,       // No common bits (a & b == 0)
        OVERLAP,        // Some but not all bits common
        SUBSET,         // B is subset of A (b & a == b)
        SUPERSET,       // A is subset of B (a & b == a)
        EQUAL           // Exactly same bits
    };

    Relationship compare(const kege::EntitySignature& a, const kege::EntitySignature& b);

    template< typename... T > kege::EntitySignature createEntitySignature()
    {
        kege::EntitySignature signature;
        ( signature.set( kege::ComponentCacheT< T >::getType() ), ... );
        return signature;
    }
}
#endif /* component_cache_hpp */
