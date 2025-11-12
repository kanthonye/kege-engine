//
//  ui-font-loader.cpp
//  physics
//
//  Created by Kenneth Esdaile on 7/29/25.
//

#include "font-loader.hpp"
#include "image-loader.hpp"

namespace kege{

    ref::Font FontLoader::load( const std::string& filename )
    {
        FILE* file = fopen( filename.c_str(), "w+" );
        if ( file == nullptr )
        {
            kege::Log::error << "Failed to open font metadata file: " << filename;
            return {};
        }

        char name[64];
        fscanf(file, "image %s\n", name );
        std::vector< kege::Glyph > glyphs( 128 );
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

        std::filesystem::path path = filename;
        std::string imagefile = path.parent_path().string() + "/" + name;

        ref::Image image = kege::ImageLoader::load( _graphics, imagefile );

        ref::Sampler sampler = _graphics->createSampler
        ({
            .mag_filter = Filter::Linear,
            .min_filter = Filter::Linear,
            .mipmap_mode = MipmapMode::Linear,
            .address_mode_u = AddressMode::Repeat,
            .address_mode_v = AddressMode::Repeat,
            .address_mode_w = AddressMode::Repeat
        });

        return new kege::Font( glyphs, { .image = image, .sampler = sampler, .layout = kege::ImageLayout::ShaderRead });
    };

    void FontLoader::operator()( kege::Graphics* graphics )
    {
        _graphics = graphics;
    }

    FontLoader::FontLoader( AssetManager* am )
    : kege::AssetLoaderT< ref::Font >( am )
    {
        CallbackRequest< kege::Graphics > request(this, &FontLoader::operator() );
        Communication::broadcast< CallbackRequest< kege::Graphics >& >( request );
    }

}
