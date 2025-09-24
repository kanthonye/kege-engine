//
//  asset-manager.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/1/24.
//

#ifndef kege_resource_manager_hpp
#define kege_resource_manager_hpp

#include "log.hpp"

namespace kege{

    /**
     * @brief A list that allows recycling of freed entries to minimize memory allocations.
     *
     * This class maintains a doubly linked list of items, allowing for efficient
     * insertion and removal of entries. Freed entries are recycled for future use,
     * reducing the overhead of dynamic memory allocation.
     *
     * @tparam Entry The type of entries stored in the list.
     */
    template< typename Entry > class RecyclerList
    {
    private:

        struct Item
        {
            Entry   entry;
            int32_t next;
            int32_t prev;
        };

        struct List
        {
            int32_t head;
            int32_t tail;
            int32_t count;
        };

    public:

        const Entry& operator []( int32_t index )const;
        Entry& operator []( int32_t index );
        int32_t next( int32_t index )const;
        int32_t prev( int32_t index )const;

        Entry&  get( int32_t index );
        int32_t push( Entry entry );
        void pop( int32_t index );

        int32_t begin();
        int32_t end();

        size_t size()const;
        void clear();

        ~RecyclerList();
        RecyclerList();

    private:

        std::vector< Item > _items;
        List _active;
        List _freed;
    };

}


namespace kege{

    template< typename Entry > const Entry& RecyclerList< Entry >::operator []( int32_t index )const
    {
        return _items[ index ].entry;
    }
    template< typename Entry > Entry& RecyclerList< Entry >::operator []( int32_t index )
    {
        return _items[ index ].entry;
    }
    template< typename Entry > int32_t RecyclerList< Entry >::next( int32_t index )const
    {
        return _items[ index ].next;
    }
    template< typename Entry > int32_t RecyclerList< Entry >::prev( int32_t index )const
    {
        return _items[ index ].prev;
    }

    template< typename Entry > Entry&  RecyclerList< Entry >::get( int32_t index )
    {
        return _items[ index ].entry;
    }
    template< typename Entry > int32_t RecyclerList< Entry >::push( Entry entry )
    {
        int32_t index;
        if ( 0 <= _freed.head )
        {
            // remove from freed list
            index = _freed.head;
            _freed.head = _items[ index ].next;
            if ( _freed.head < 0 ) _freed.tail = -1;

            _freed.count -= 1;

            _items[ index ].entry = entry;
        }
        else
        {
            // create new entry
            index = static_cast< int32_t >( _items.size() );
            _items.push_back({});

            _items[ index ].entry = entry;
        }

        _items[ index ].next = -1;
        _items[ index ].prev = -1;

        // add to active list
        if ( _active.head < 0 )
        {
            _active.head = _active.tail = index;
        }
        else
        {
            _items[ _active.tail ].next = index;
            _items[ index ].prev = _active.tail;
            _active.tail = index;
        }
        return index;
    }

    template< typename Entry > void RecyclerList< Entry >::pop( int32_t index )
    {
        if ( 0 > index && index >= _items.size() )
        {
            return;
        }

        // remove from active list
        if ( _active.head == index )
        {
            _active.head = _items[ _active.head ].next;
            if ( _active.head < 0 ) _active.tail = -1;
            else _items[ _active.head ].prev = -1;
        }
        else if ( _active.tail == index )
        {
            _active.tail = _items[ _active.tail ].prev;
            if ( _active.tail < 0 ) _active.head = -1;
            else _items[ _active.tail ].next = -1;
        }
        else
        {
            int n = _items[ index ].next;
            int p = _items[ index ].prev;
            _items[ n ].prev = p;
            _items[ p ].next = n;
        }

        _items[ index ].prev = -1;
        _items[ index ].next = -1;

        // add to freed list
        if ( _freed.head < 0 )
        {
            _freed.head = _freed.tail = index;
        }
        else
        {
            _items[ _freed.tail ].next = index;
            _items[ index ].prev = _freed.tail;
            _freed.tail = index;
        }
    }

    template< typename Entry > int32_t RecyclerList< Entry >::begin()
    {
        return _active.head;
    }
    template< typename Entry > int32_t RecyclerList< Entry >::end()
    {
        return _active.tail;
    }

    template< typename Entry > void RecyclerList< Entry >::clear()
    {
        _items.clear();
    }

    template< typename Entry > size_t RecyclerList< Entry >::size()const
    {
        return _items.size();
    }

    template< typename Entry > RecyclerList< Entry >::~RecyclerList()
    {
        clear();
    }

    template< typename Entry > RecyclerList< Entry >::RecyclerList()
    :   _active({ -1, -1, 0 })
    ,   _freed({ -1, -1, 0 })
    {}
}





namespace kege{

    /**
     * @brief Base class for resource managers.
     *
     * This class provides a common interface for managing different types of resources.
     * It keeps track of the number of resource types and provides a virtual destructor
     * for proper cleanup in derived classes.
     */
    class ResourceManager
    {
    public:

        static uint32_t getMaxTypeCount(){ return _enumerator; }
        virtual ~ResourceManager(){}

    protected:

        static uint32_t getType(){ return _enumerator++; }

    protected:

        static uint32_t _enumerator;
    };

}


namespace kege{

    /**
     * @brief Template class for managing resources of a specific type.
     *
     * This class provides functionality to add, retrieve, and remove resources
     * identified by unique names or handles. It utilizes a RecyclerList to
     * efficiently manage memory by recycling freed entries.
     *
     * @tparam Item The type of resource being managed.
     */
    template< typename Item > class ResourceManagerT : public ResourceManager
    {
    public:

        struct NamedItem
        {
            std::string name;
            Item item;
        };

        int32_t add( const std::string& name, Item item );
        int32_t getID( const std::string& name )const;

        const Item* fetch( const std::string& name )const;
        Item* fetch( const std::string& name );

        const Item& get( int32_t id )const;
        Item& get( int32_t id );

        void removeItemByName( const std::string& name );
        void removeItemByID( uint32_t handle );
        void clear();

        int32_t next( int32_t index )const;
        int32_t prev( int32_t index )const;

        int32_t begin();
        int32_t end();

        static uint32_t type();

        ~ResourceManagerT();
        ResourceManagerT();

    private:

        std::unordered_map< std::string, int32_t > _table;
        RecyclerList< NamedItem > _items;
        static uint32_t _type;
    };

}


namespace kege{

    template< typename Item > uint32_t ResourceManagerT< Item >::_type = ResourceManager::getType();

    template< typename Item > int32_t ResourceManagerT< Item >::add( const std::string& name, Item item )
    {
        auto m = _table.find( name );
        if ( m ==  _table.end() )
        {
            int32_t index = _items.push({ name, item });
            _table[ name ] = index;
            return index;
        }
        _items[ m->second ].item = item;
        return m->second;
    }

    template< typename Item > int32_t ResourceManagerT< Item >::getID( const std::string& name )const
    {
        auto m = _table.find( name );
        if ( m ==  _table.end() ) return -1;
        return m->second;
    }

    template< typename Item > const Item* ResourceManagerT< Item >::fetch( const std::string& name )const
    {
        uint32_t handle = getID( name );
        if ( handle >= _items.size() ) return nullptr;
        return &_items[ handle ].item;
    }

    template< typename Item > Item* ResourceManagerT< Item >::fetch( const std::string& name )
    {
        uint32_t handle = getID( name );
        if ( handle >= _items.size() ) return nullptr;
        return &_items[ handle ].item;
    }

    template< typename Item > const Item& ResourceManagerT< Item >::get( int32_t id )const
    {
        return _items[ id ].item;
    }

    template< typename Item > Item& ResourceManagerT< Item >::get( int32_t id )
    {
        return _items[ id ].item;
    }

    template< typename Item > void ResourceManagerT< Item >::removeItemByName( const std::string& name )
    {
        auto m = _table.find( name );
        if ( m !=  _table.end() )
        {
            _items.pop( m->second );
            _table.erase( m );
        }
    }

    template< typename Item > void ResourceManagerT< Item >::removeItemByID( uint32_t id )
    {
        if ( id < _table.size() )
        {
            NamedItem& item = _items[ id ];

            auto m = _table.find( item.name );
            if ( m !=  _table.end() )_table.erase( m );

            _items.pop( id );
        }
    }

    template< typename Item > void ResourceManagerT< Item >::clear()
    {
        _items.clear();
        _table.clear();
    }

    template< typename Item > int32_t ResourceManagerT< Item >::next( int32_t index )const
    {
        return _items.next( index );
    }

    template< typename Item > int32_t ResourceManagerT< Item >::prev( int32_t index )const
    {
        return _items.prev( index );
    }

    template< typename Item > int32_t ResourceManagerT< Item >::begin()
    {
        return _items.begin();
    }

    template< typename Item > int32_t ResourceManagerT< Item >::end()
    {
        return _items.end();
    }

    template< typename Item > uint32_t ResourceManagerT< Item >::type()
    {
        return _type;
    }

    template< typename Item > ResourceManagerT< Item >::~ResourceManagerT()
    {
        clear();
    }

    template< typename Item > ResourceManagerT< Item >::ResourceManagerT()
    {}
}






namespace kege{

    /**
     * @brief Manages multiple resource managers for different asset types.
     *
     * This class provides a unified interface to manage various types of resources,
     * such as textures, shaders, and models. It maintains a collection of
     * ResourceManagerT instances, each responsible for a specific resource type.
     */
    class AssetManager
    {
    public:

        template< typename Item > int32_t add( const std::string& name, Item res )
        {
            return this->get< Item >()->add( name, res );
        }

        template< typename Item > const Item* fetch( const std::string& name )const
        {
            return this->get< Item >()->fetch(name);
        }

        template< typename Item > Item* fetch( const std::string& name )
        {
            return this->get< Item >()->fetch( name );
        }

        template< typename Item > const Item* get( int32_t id )const
        {
            return &this->get< Item >()->get( id );
        }

        template< typename Item > Item* get( int32_t id )
        {
            return &this->get< Item >()->get( id );
        }

        template< typename Item > int32_t getID( const std::string& name )const
        {
            return this->get< Item >()->getID( name );
        }

        template< typename Item > void remove( const std::string& name )
        {
            return this->get< Item >()->removeItemByName( name );
        }

        template< typename Item > void remove( int32_t handle )
        {
            return this->get< Item >()->removeItemByID( handle );
        }

        template< typename Item > void clear()
        {
            return this->get< Item >()->clear();
        }

        template<typename Item> ResourceManagerT<Item>* get()
        {
            if ( _managers[ ResourceManagerT< Item >::type() ] == nullptr )
            {
                _managers[ ResourceManagerT< Item >::type() ] = new ResourceManagerT< Item >;
            }
            return reinterpret_cast< ResourceManagerT< Item >* >( _managers[ ResourceManagerT< Item >::type() ] );
        }

        template<typename Item> const ResourceManagerT<Item>* get()const
        {
            if ( _managers[ ResourceManagerT< Item >::type() ] == nullptr )
            {
                _managers[ ResourceManagerT< Item >::type() ] = new ResourceManagerT< Item >;
            }
            return reinterpret_cast< ResourceManagerT< Item >* >( _managers[ ResourceManagerT< Item >::type() ] );
        }

        void initalize();
        void shutdown();

        ~AssetManager();
        AssetManager();

    private:

        mutable std::vector< ResourceManager* > _managers;
    };

}

#endif /* kege_resource_manager_hpp */
