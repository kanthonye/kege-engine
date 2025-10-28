//
//  asset-manager.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/1/24.
//

#ifndef kege_resource_manager_hpp
#define kege_resource_manager_hpp

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






namespace kege{

    /**
     * @brief Manages multiple resource managers for different asset types.
     *
     * This class provides a unified interface to manage various types of resources,
     * such as textures, shaders, and models. It maintains a collection of
     * AssetCacheTable instances, each responsible for a specific resource type.
     */
    class AssetManager : public kege::RefCounter
    {
    public:

        template< typename Item > uint64_t add( const std::string& name, Item res )
        {
            return this->getAssetCacheTable< Item >()->add( name, res );
        }

        template< typename Item > const Item* fetch( const std::string& name )const
        {
            return this->getAssetCacheTable< Item >()->fetch(name);
        }

        template< typename Item > Item* fetch( const std::string& name )
        {
            return this->getAssetCacheTable< Item >()->fetch( name );
        }

        template< typename Item > const Item* get( uint64_t handle )const
        {
            return this->getAssetCacheTable< Item >()->get( handle );
        }

        template< typename Item > Item* get( uint64_t handle )
        {
            return this->getAssetCacheTable< Item >()->get( handle );
        }

        template< typename Item > uint64_t getId( const std::string& name )const
        {
            return this->getAssetCacheTable< Item >()->getId( name );
        }

        template< typename Item > void remove( const std::string& name )
        {
            return this->getAssetCacheTable< Item >()->remove( name );
        }

        template< typename Item > void erase( uint64_t handle )
        {
            return this->getAssetCacheTable< Item >()->erase( handle );
        }

        template< typename Item > void clear()
        {
            return this->getAssetCacheTable< Item >()->clear();
        }

        template<typename Item> const AssetCacheTable<Item>* getAssetCacheTable()const
        {
            if ( _assets.empty() ) _assets.resize( AssetTable::_type_enumerator );
            if ( _assets[ AssetCacheTable< Item >::_type ] == nullptr )
            {
                _assets[ AssetCacheTable< Item >::_type ] = new AssetCacheTable< Item >;
            }
            return reinterpret_cast< AssetCacheTable< Item >* >( _assets[ AssetCacheTable< Item >::_type ] );
        }

        template<typename Item> AssetCacheTable< Item >* getAssetCacheTable()
        {
            if ( _assets.empty() ) _assets.resize( AssetTable::_type_enumerator );
            if ( _assets[ AssetCacheTable< Item >::_type ] == nullptr )
            {
                _assets[ AssetCacheTable< Item >::_type ] = new AssetCacheTable< Item >;
            }
            return reinterpret_cast< AssetCacheTable< Item >* >( _assets[ AssetCacheTable< Item >::_type ] );
        }

        void shutdown();

        ~AssetManager();
        AssetManager();

    private:

        mutable std::vector< AssetTable* > _assets;
    };

}

namespace kege::ref{
    typedef kege::Ref< kege::AssetManager > AssetManager;
}
#endif /* kege_resource_manager_hpp */
