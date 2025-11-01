//
//  asset-cache-table.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/1/25.
//

#ifndef asset_cache_table_hpp
#define asset_cache_table_hpp

#include "asset-cache.hpp"

namespace kege{

   class AssetManager;

    /**
     * @brief Base class for resource managers.
     *
     * This class provides a common interface for managing different types of resources.
     * It keeps track of the number of resource types and provides a virtual destructor
     * for proper cleanup in derived classes.
     */
    class AssetTable
    {
    public:

        static uint32_t getMaxTypeCount(){ return _type_enumerator; }
        virtual ~AssetTable(){}

    protected:

        static uint32_t getType(){ return _type_enumerator++; }

    protected:

        std::unordered_map< std::string, uint64_t > _lookup_table;
        static uint32_t _type_enumerator;
        friend AssetManager;
    };

}


namespace kege{

    /**
     * @brief Template class for managing resources of a specific type.
     *
     * This class provides functionality to add, retrieve, and remove resources
     * identified by unique names or handles. It utilizes a AssetCache to
     * efficiently manage memory by recycling freed entries.
     *
     * @tparam Item The type of resource being managed.
     */
    template< typename Item > class AssetCacheTable : public AssetTable
    {
    public:

        struct Asset
        {
            std::string name;
            Item item;
        };

        uint64_t add( const std::string& name, Item item );
        uint64_t getId( const std::string& name )const;

        const Item* fetch( const std::string& name )const;
        Item* fetch( const std::string& name );

        const Item* get( uint64_t handle )const;
        Item* get( uint64_t handle );

        void remove( const std::string& name );
        void erase( uint64_t handle );
        void clear();

        uint64_t next( uint64_t handle )const;
        uint64_t prev( uint64_t handle )const;

        uint64_t begin();
        uint64_t end();

        static uint32_t type();

        ~AssetCacheTable();
        AssetCacheTable();

    private:

        AssetCache< Asset > _items;
        static uint32_t _type;
        friend AssetManager;
    };

}


namespace kege{

    template< typename Item > uint32_t AssetCacheTable< Item >::_type = AssetTable::getType();

    template< typename Item > uint64_t AssetCacheTable< Item >::add( const std::string& name, Item item )
    {
        auto m = _lookup_table.find( name );
        if ( m ==  _lookup_table.end() )
        {
            uint64_t handle = _items.push( Asset{ name, item } );
            _lookup_table[ name ] = handle;
            return handle;
        }
        _items[ m->second ]->item = item;
        return m->second;
    }

    template< typename Item > const Item* AssetCacheTable< Item >::fetch( const std::string& name )const
    {
        uint64_t handle = getId( name );
        if ( handle == 0 ) return nullptr;
        return &_items[ handle ]->item;
    }

    template< typename Item > Item* AssetCacheTable< Item >::fetch( const std::string& name )
    {
        uint64_t handle = getId( name );
        if ( handle == 0 ) return nullptr;
        return &_items[ handle ]->item;
    }

    template< typename Item > uint64_t AssetCacheTable< Item >::getId( const std::string& name )const
    {
        auto m = _lookup_table.find( name );
        if ( m ==  _lookup_table.end() ) return 0;
        return m->second;
    }

    template< typename Item > const Item* AssetCacheTable< Item >::get( uint64_t handle )const
    {
        const Asset* asset = _items[ handle ];
        return (asset) ? &asset->item : nullptr;
    }

    template< typename Item > Item* AssetCacheTable< Item >::get( uint64_t handle )
    {
        Asset* asset = _items[ handle ];
        return (asset) ? &asset->item : nullptr;
    }

    template< typename Item > void AssetCacheTable< Item >::remove( const std::string& name )
    {
        auto m = _lookup_table.find( name );
        if ( m !=  _lookup_table.end() )
        {
            _items.pop( m->second );
            _lookup_table.erase( m );
        }
    }

    template< typename Item > void AssetCacheTable< Item >::erase( uint64_t handle )
    {
        Asset* item = _items[ handle ];
        if ( item )
        {
            auto m = _lookup_table.find( item->name );
            if ( m != _lookup_table.end() )
            {
                _lookup_table.erase( m );
            }
            _items.pop( handle );
        }
    }

    template< typename Item > void AssetCacheTable< Item >::clear()
    {
        _items.clear();
        _lookup_table.clear();
    }

    template< typename Item > uint64_t AssetCacheTable< Item >::next( uint64_t handle )const
    {
        return _items.next( handle );
    }

    template< typename Item > uint64_t AssetCacheTable< Item >::prev( uint64_t handle )const
    {
        return _items.prev( handle );
    }

    template< typename Item > uint64_t AssetCacheTable< Item >::begin()
    {
        return _items.begin();
    }

    template< typename Item > uint64_t AssetCacheTable< Item >::end()
    {
        return _items.end();
    }

    template< typename Item > uint32_t AssetCacheTable< Item >::type()
    {
        return _type;
    }

    template< typename Item > AssetCacheTable< Item >::~AssetCacheTable()
    {
        clear();
    }

    template< typename Item > AssetCacheTable< Item >::AssetCacheTable()
    {}
}




#endif /* asset_cache_table_hpp */
