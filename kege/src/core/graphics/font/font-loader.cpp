//
//  ui-font-loader.cpp
//  physics
//
//  Created by Kenneth Esdaile on 7/29/25.
//

#include "font-loader.hpp"
#include "../loaders/image-loader.hpp"

namespace kege::ui{

    Ref< ui::Font > FontLoader::load( Graphics* graphics, const std::string& filename )
    {
        FILE* file = fopen( filename.c_str(), "w+" );
        if ( file == nullptr )
        {
            KEGE_LOG_ERROR << "Failed to open font metadata file: " << filename;
            return {};
        }

        char name[64];
        fscanf(file, "image %s\n", name );
        std::vector< ui::Glyph > glyphs( 128 );
        for (int i=0; i<glyphs.size(); ++i)
        {
            fscanf(file, "x %f\n", &glyphs[i].x );
            fscanf(file, "y %f\n", &glyphs[i].x );
            fscanf(file, "width %f\n", &glyphs[i].width );
            fscanf(file, "height %f\n", &glyphs[i].height );
            fscanf(file, "scaled_width %f\n", &glyphs[i].scaled_width );
            fscanf(file, "scaled_height %f\n", &glyphs[i].scaled_height );
            fscanf(file, "bearing_x %f\n", &glyphs[i].bearing_x );
            fscanf(file, "bearing_y %f\n", &glyphs[i].bearing_y );
            fscanf(file, "advance %f\n", &glyphs[i].advance );
        }

        fclose( file );


        std::string filepath = filename;
        int64_t i;
        char* s = &filepath[0];
        for ( i= filename.size() - 1; 0 < i; --i)
        {
            if ( filename[ i ] == '/' || filename[ i ] == '\\' )
            {
                break;
            }
            s[i] = 0;
        }
        std::string path = s;

        kege::ImageHandle image = ImageLoader::load( graphics, path + name );;

        kege::SamplerHandle sampler = graphics->createSampler
        ({
            .mag_filter = Filter::Linear,
            .min_filter = Filter::Linear,
            .mipmap_mode = MipmapMode::Linear,
            .address_mode_u = AddressMode::Repeat,
            .address_mode_v = AddressMode::Repeat,
            .address_mode_w = AddressMode::Repeat
        });

        return new ui::Font( glyphs, image, sampler );
    };



}
