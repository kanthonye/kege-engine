//
//  image-loader.cpp
//  physics
//
//  Created by Kenneth Esdaile on 7/29/25.
//

#include "image-loader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../libs/third-party/stb/stb_image.h"

namespace kege{

    std::vector<uint8_t> ImageLoader::load( int* width, int* height, int* channels, int req_chan, const std::string& filename )
    {
        uint8_t* data = nullptr;
        if ( !(data = stbi_load( filename.c_str(), width, height, channels, req_chan)) )
        {
            kege::Log::error << "Failed to load font texture: " << filename;
            return {};
        }

        std::vector< uint8_t > img((*width) * (*height) * 4);
        memcpy(img.data(), data, img.size());
        stbi_image_free( data );
        return img;
    }

    ref::Image ImageLoader::load( kege::Graphics* graphics, const std::string& filename )
    {
        int width;
        int height;
        int channels;
        uint8_t* data = nullptr;

        if ( !(data = stbi_load( filename.c_str(), &width, &height, &channels, 4)) )
        {
            kege::Log::error << "Failed to load font texture: " << filename;
            return {};
        }

        ref::Image image = graphics->createImage
        ({
            .type           = ImageType::Type2D,
            .extent         = {uint32_t( width ),uint32_t( height ),1},
            .array_layers   = 1,
            .mip_levels     = 1,
            .format         = Format::rgba_u8_norm,
            .samples        = SampleCount::Count1,
            .usage          = ImageUsage::TransferDst,
            .memory_usage   = MemoryUsage::GpuOnly,
            .debug_name     = "image",
            .data           = data
        });
        stbi_image_free( data );
        return image;
    }

    ref::Image ImageLoader::load( const std::string& filename )
    {
        return load( _graphics, filename );
    }

    void ImageLoader::operator()( kege::Graphics* graphics )
    {
        _graphics = graphics;
    }

    ImageLoader::ImageLoader( AssetManager* am )
    :   kege::AssetLoaderT< ref::Image >( am )
    {
        CallbackRequest< kege::Graphics > request(this, &ImageLoader::operator() );
        Communication::broadcast< CallbackRequest< kege::Graphics >& >( request );
    }
}
