//
//  ui-font-creator.cpp
//  physics
//
//  Created by Kenneth Esdaile on 7/29/25.
//

#include "font-creator.hpp"

namespace kege{

    std::vector< Glyph > FontCreator::createGlyphs( const FontCreator::Info& info )
    {
        int threshold = 20;
        int char_width  = info.width / info.columns;
        int char_height = info.height / info.rows;

        std::vector< Glyph > glyphs( 128 );

        for (int i = 0; i < 128; ++i) // ASCII range 0-127
        {
            int col = i % info.columns;
            int row = i / info.columns;
            int start_x = col * char_width;
            int start_y = row * char_height;

            int min_x = char_width,  max_x = 0;
            int min_y = char_height, max_y = 0;
            int min_w = char_width;
            int min_h = char_width;
            // Detect the actual character width by scanning columns
            for (int x = 0; x < char_width; ++x)
            {
                for (int y = 0; y < char_height; ++y)
                {
                    int pixel_index = ((start_y + y) * info.width + (start_x + x)) * 4; // RGBA

                    if ( info.image_data[ pixel_index ] > threshold )  // If alpha > threshold, mark it as non-empty
                    {
                        min_x = std::min(min_x, x);
                        max_x = std::max(max_x, x);

                        min_y = std::min(min_y, y);
                        max_y = std::max(max_y, y);
                    }

                    /**
                     * what we want to do here is calculate the width from where the since distance
                     * field start to the where the actual glyph starts. this will allow us to calc
                     * the bearing_x offset. which is why the threshold is 200, you can increase or
                     * reduce to adjust the offset.
                     */
                    if ( info.image_data[ pixel_index ] >= 200 )
                    {
                        min_w = std::min(x, min_w);
                        min_h = std::min(y, min_h);
                    }
                }
            }

            if (min_x > max_x) // No visible pixels found (empty character)
            {
                glyphs[i] = Glyph
                {
                    .x = float(start_x),
                    .y = float(start_y),
                    .width = 0.f,
                    .height = float(char_height),
                    .advance = 16
                };
            }
            else // visible pixels found
            {
                Glyph g;
                g.x         = float(start_x + min_x);
                g.y         = float(start_y + min_y);
                g.width     = float(max_x - min_x + 1);
                g.height    = float(max_y - min_y + 1);
                g.bearing_x = float(min_x);
                g.bearing_y = float(min_y);

                // Normalize UV
                glyphs[i].x              = g.x / float( info.width );
                glyphs[i].y              = g.y / float( info.height );
                glyphs[i].width          = g.width / float( info.width );
                glyphs[i].height         = g.height / float( info.height );

                // Normalize glyph size
                glyphs[i].scaled_width   = g.width / float(char_width);
                glyphs[i].scaled_height  = g.height / float(char_height);


                // Calculate Bearing X and Bearing Y
                glyphs[i].bearing_x = float(min_w - min_x) / float(char_width); // Distance from left edge of bounding box to left edge of glyph
                glyphs[i].bearing_y = float(min_y) / float(char_height); // Distance from baseline to top of glyph
                glyphs[i].offset_y  = float(min_y - min_h) / float(char_height);
                /**
                 * Calculate Advance (width of the glyph plus some spacing). You can increase or
                 * reduce the percentage value to adjust the spacing between letters.
                 */
                glyphs[i].advance = (g.width * 0.55) / float(char_width); // Add 2 pixels for spacing

            }
        }
        glyphs[32].advance = glyphs['T'].advance;
        return glyphs;
    }

    ref::Font FontCreator::create( Graphics* graphics, int char_per_row, int char_per_col, const std::string& font_texture_path )
    {
        int width, height, channels;
        std::vector<uint8_t> data = ImageLoader::load( &width, &height, &channels, 4, font_texture_path );

        if ( data.empty() )
        {
            kege::Log::error << "Failed to load font texture: " << font_texture_path;
            return {};
        }

        std::vector< Glyph > glyphs = FontCreator::createGlyphs
        ({
            width,
            height,
            char_per_row,
            char_per_col,
            data.data()
        });

        ref::Image image = graphics->createImage
        ({
            .type = ImageType::Type2D,
            .extent = {uint32_t( width ),uint32_t( height ),1},
            .array_layers = 1,
            .mip_levels = 1,
            .format = Format::rgba_u8_norm,
            .samples = SampleCount::Count1,
            .usage = ImageUsage::TransferDst | ImageUsage::Sampled | ImageUsage::Color,
            .memory_usage = MemoryUsage::GpuOnly,
            .debug_name = "font-image",
            .data = data.data()
        });

        ref::Sampler sampler = graphics->createSampler
        ({
            .mag_filter = Filter::Linear,
            .min_filter = Filter::Linear,
            .mipmap_mode = MipmapMode::Linear,
            .address_mode_u = AddressMode::ClampToEdge,
            .address_mode_v = AddressMode::ClampToEdge,
            .address_mode_w = AddressMode::ClampToEdge
        });

        return new kege::Font( glyphs, { .image = image, .sampler = sampler, .layout = kege::ImageLayout::ShaderRead });
    }

}
