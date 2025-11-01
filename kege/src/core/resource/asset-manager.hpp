//
//  asset-manager.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/1/24.
//

#ifndef kege_resource_manager_hpp
#define kege_resource_manager_hpp

#include "asset-cache-table.hpp"
#include "asset-loader.hpp"

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

        template< typename Item, typename Loader > Loader* addLoader( const std::string& ext )
        {
            Loader* loader = new Loader( this );
            this->getAssetCacheTable< Ref< AssetLoaderT< Item > > >()->add( ext, loader );
            return loader;
        }

        template< typename Item > const Item* load( const std::string& filename )const
        {
            std::string ext;
            for(int i = 1; filename[ filename.length() - i ] != '.'; ++i )
            {
                ext += filename[ filename.length() - i ];
            }
            reverse(ext.begin(), ext.end());

            Ref< AssetLoaderT< Item > >* loader = this->fetch< Ref< AssetLoaderT< Item > > >( ext );
            if( loader == nullptr ) return nullptr;

            return loader->load( filename );
        }

        void shutdown();

        ~AssetManager();
        AssetManager();

    private:

        mutable std::vector< AssetTable* > _assets;

        using AssetLoaderMap = std::unordered_map< std::string, ref::AssetLoader >;
        mutable std::vector< AssetLoaderMap > _loaders;
    };

}

namespace kege::ref{
    typedef kege::Ref< kege::AssetManager > AssetManager;
}
#endif /* kege_resource_manager_hpp */
