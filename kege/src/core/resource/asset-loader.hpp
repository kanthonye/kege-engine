//
//  asset-loader.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/1/25.
//

#ifndef asset_loader_hpp
#define asset_loader_hpp

#include "../memory/ref.hpp"
#include "../utils/log.hpp"
#include <filesystem>

namespace kege{

    class AssetManager;

    class AssetLoader : public kege::RefCounter
    {
    public:

        AssetLoader( AssetManager* am ): _manager( am ) {}
        virtual ~AssetLoader(){ _manager = nullptr; }

    protected:

        AssetManager* _manager;
    };


    template< typename AssetType > class AssetLoaderT : public kege::AssetLoader
    {
    public:

        virtual AssetType load( const std::string& filename ) = 0;
        AssetLoaderT( AssetManager* am ): AssetLoader( am ) {}
        virtual ~AssetLoaderT(){}
    };

}
namespace kege::ref{
    typedef kege::Ref< kege::AssetLoader > AssetLoader;
}
#endif /* asset_loader_hpp */
