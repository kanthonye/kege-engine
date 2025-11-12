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
#include "../graphics/render/core/graphics.hpp"

namespace kege{

    class ImageLoader : public kege::AssetLoaderT< ref::Image >
    {
    public:

        static std::vector<uint8_t> load( int* width,int* height, int* channels, int req_chan, const std::string& filename );
        static ref::Image load( kege::Graphics* graphics, const std::string& filename );

        ref::Image load( const std::string& filename );
        void operator()( kege::Graphics* response );

        ImageLoader( AssetManager* am = nullptr );
        
    private:

        kege::Graphics* _graphics;
    };

}
#endif /* image_loader_hpp */
