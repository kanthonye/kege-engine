//
//  image-loader.cpp
//  physics
//
//  Created by Kenneth Esdaile on 7/29/25.
//

#include "image-loader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../../../kege/libs/third-party/stb/stb_image.h"

namespace kege{

    bool ImageLoader::load( ImageLoader::Info& info, const std::string& filename )
    {
        uint8_t* data = nullptr;
        int req_comp = 4;
        if ( !(data = stbi_load( filename.c_str(), &info.width, &info.height, &info.channels, req_comp )) )
        {
            kege::Log::error << "Failed to load font texture: " << filename;
            return {};
        }
        info.data.resize( info.width * info.height * req_comp );
        memcpy( info.data.data(), data, info.data.size() );
        stbi_image_free( data );
        
        info.channels = req_comp;
        return true;
    }

    ref::Image ImageLoader::load( Graphics* graphics, const std::string& filename )
    {
        uint8_t* data = nullptr;
        ImageLoader::Info info;

        if ( !(data = stbi_load( filename.c_str(), &info.width, &info.height, &info.channels, 4)) )
        {
            kege::Log::error << "Failed to load font texture: " << filename;
            return {};
        }
        stbi_image_free( data );

        return graphics->createImage
        ({
            .type           = ImageType::Type2D,
            .extent         = {uint32_t( info.width ),uint32_t( info.height ),1},
            .array_layers   = 1,
            .mip_levels     = 1,
            .format         = Format::undefined,
            .samples        = SampleCount::Count1,
            .usage          = ImageUsage::TransferDst,
            .memory_usage   = MemoryUsage::GpuOnly,
            .debug_name     = "image"
        });
    }

}
