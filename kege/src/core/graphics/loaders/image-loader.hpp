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
#include "../../utils/log.hpp"
#include "../../graphics/core/graphics.hpp"

namespace kege{

    class ImageLoader
    {
    public:

        struct Info
        {
            int width;
            int height;
            int channels;
            std::vector< uint8_t > data;
        };

        static bool load( ImageLoader::Info& info, const std::string& filename );
        static ImageHandle load( Graphics* graphics, const std::string& filename );
    };

}

#endif /* image_loader_hpp */
