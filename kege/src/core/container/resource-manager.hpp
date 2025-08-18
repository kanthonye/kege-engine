//
//  resource-manager.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/1/24.
//

#ifndef kege_resource_manager_hpp
#define kege_resource_manager_hpp

#include "../memory/ref.hpp"
#include "../utils/log.hpp"
#include "../utils/communication.hpp"
#include "hashmap.hpp"

namespace kege{

    class ResourceManager
    {
    private:

        class Assets : public RefCounter
        {
        public:

            virtual void purge() = 0;
            virtual ~Assets(){}
        };

        template< typename Asset > struct AssetHashmap : public Assets
        {
            void purge() { assets.clear(); }
            ~AssetHashmap(){ purge(); }
            kege::Hashmap< Asset > assets;
        };

        template< typename T > static uint32_t getAssetId()
        {
            static int32_t asset_id = -1;
            if ( asset_id < 0 )
            {
                asset_id = asset_enumerator;
                asset_enumerator += 1;
            }
            return asset_id;
        }

        template< typename T > AssetHashmap< T >* getAssets()
        {
            uint32_t asset_id = getAssetId< T >();
            if ( asset_id >= _assets.size() )
            {
                _assets.resize( 10 + 2 * _assets.size() );
            }
            if ( _assets[ asset_id ] == nullptr )
            {
                _assets[ asset_id ] = new AssetHashmap< T >;
            }
            return static_cast< AssetHashmap< T >* >( _assets[ asset_id ].ref() );
        }

        template< typename T > const AssetHashmap< T >* getAssets()const
        {
            uint32_t asset_id = getAssetId< T >();
            if ( asset_id >= _assets.size() )
            {
                _assets.resize( 10 + 2 * _assets.size() );
            }
            if ( _assets[ asset_id ] == nullptr )
            {
                _assets[ asset_id ] = new AssetHashmap< T >;
            }
            return static_cast< AssetHashmap< T >* >( _assets[ asset_id ].ref() );
        }

    public:

        template< typename T > void insert( const kege::string& sid, T asset )
        {
            this->getAssets< T >()->assets.insert( sid, asset );
        }

        template< typename T > void remove( const kege::string& sid )
        {
            this->getAssets< T >()->assets.remove( sid );
        }

        template< typename T > const T* get( const kege::string& sid )const
        {
            return this->getAssets< T >()->assets.at( sid );
        }

        template< typename T > T* get( const kege::string& sid )
        {
            return this->getAssets< T >()->assets.at( sid );
        }

        bool initialize()
        {
            return true;
        }

        void shutdown()
        {
            _assets.clear();
        }

        ~ResourceManager()
        {
            shutdown();
        }

    private:

        mutable std::vector< kege::Ref< Assets > > _assets;
        static int32_t asset_enumerator;
    };
    
}

#endif /* kege_resource_manager_hpp */
