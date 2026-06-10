//
//  image-loader.hpp
//  physics
//
//  Created by Kenneth Esdaile on 7/29/25.
//

#ifndef image_loader_hpp
#define image_loader_hpp

#include <string>
#include <vector>
#include "asset-manager.hpp"
#include "../utils/communication.hpp"
#include "../graphics/core/all.hpp"

namespace kege{

    class ImageLoader : public kege::AssetLoaderT< ref::Image >
    {
    public:

        static std::vector<uint8_t> load( int* width,int* height, int* channels, int req_chan, const std::string& filename );
        static ref::Image load( kege::GraphicsDevice* graphics, const std::string& filename );

        ref::Image load( const std::string& filename );
        void operator()( kege::GraphicsDevice* response );

        ImageLoader( AssetManager* am = nullptr );
        
    private:

        kege::GraphicsDevice* _graphics;
    };

}
#endif /* image_loader_hpp */
