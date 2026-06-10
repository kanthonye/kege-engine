//
//  ui-font-creator.cpp
//  physics
//
//  Created by Kenneth Esdaile on 7/29/25.
//

#include "font-creator.hpp"

namespace kege{

    struct TempGlyph
    {
        int min_y;
        int max_y;
    };

    void FontCreator::createGlyphs( const FontCreator::Info& info )
    {
        int threshold = 20;
        int char_width  = info.width / info.columns;
        int char_height = info.height / info.rows;

        info.glyphs.resize( 128 );
        std::vector<TempGlyph> temp(128);

        int global_min_y = char_height;
        int global_max_y = 0;
        //int baseline;

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
                     * what we want to do here is calculate the width from where the sine distance
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
                info.glyphs[i] = Glyph
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
                global_min_y = std::min(global_min_y, min_y);
                global_max_y = std::max(global_max_y, max_y);
                temp[i].min_y = min_y;
                temp[i].max_y = max_y;

                Glyph g;
                g.x         = float(start_x + min_x);
                g.y         = float(start_y + min_y);
                g.width     = float(max_x - min_x + 1);
                g.height    = float(max_y - min_y + 1);
                g.bearing_x = float(min_x);
                g.bearing_y = float(min_y);

                // Normalize UV
                info.glyphs[i].x              = g.x / float( info.width );
                info.glyphs[i].y              = g.y / float( info.height );
                info.glyphs[i].width          = g.width / float( info.width );
                info.glyphs[i].height         = g.height / float( info.height );

                // Normalize glyph size
                info.glyphs[i].scaled_width   = g.width / float(char_width);
                info.glyphs[i].scaled_height  = g.height / float(char_height);

                // Calculate Bearing X and Bearing Y
                info.glyphs[i].bearing_x = float(min_w - min_x) / float(char_width); // Distance from left edge of bounding box to left edge of glyph

                /**
                 * Calculate Advance (width of the glyph plus some spacing). You can increase or
                 * reduce the percentage value to adjust the spacing between letters.
                 */
                info.glyphs[i].advance = (g.width * 0.48) / float(char_width); // Add 2 pixels for spacing

            }
        }

        info.matrics = {};
        info.matrics.ascent = char_height - global_min_y;
        info.matrics.baseline = char_height - info.matrics.ascent;  // == global_min_y
        info.matrics.descent  = global_max_y - info.matrics.baseline;

        for (int i = 0; i < 128; ++i)
        {
            if (info.glyphs[i].width == 0.0f)
                continue;

            info.glyphs[i].bearing_y = (info.matrics.ascent - temp[i].min_y) / float(char_height);
        }

        info.glyphs[32].advance = info.glyphs['T'].advance;
    }

    ref::Font FontCreator::create( GraphicsDevice* graphics, int char_per_row, int char_per_col, const std::string& font_texture_path )
    {
        int width, height, channels;
        std::vector<uint8_t> data = ImageLoader::load( &width, &height, &channels, 4, font_texture_path );

        if ( data.empty() )
        {
            kege::Log::error << "FontCreator::create: Failed to load font texture: " << font_texture_path << Log::nl;
            return {};
        }

        std::vector< Glyph > glyphs;
        FontMetrics matrics;
        FontCreator::createGlyphs
        ({
            width,
            height,
            char_per_row,
            char_per_col,
            data.data(),
            glyphs,
            matrics
        });

        ref::Image image = graphics->createImage
        ({
            .type = ImageType::Type2D,
            .format = Format::rgba_u8_norm,
            .extent = {uint32_t( width ),uint32_t( height ),1},
            .mip_levels = 1,
            .array_layers = 1,
            .samples = SampleCount::Count1,
            .usage = ImageUsage::TransferDst | ImageUsage::Sampled | ImageUsage::Color,
            .memory_usage = MemoryUsage::GpuOnly,
            .data = data.data(),
            .name = "font-image",
        });

        ref::Sampler sampler = graphics->createSampler
        ({
            .min_filter = Filter::Linear,
            .mag_filter = Filter::Linear,
            .mipmap_mode = MipmapMode::Linear,
            .address_mode_u = AddressMode::ClampToEdge,
            .address_mode_v = AddressMode::ClampToEdge,
            .address_mode_w = AddressMode::ClampToEdge
        });

        return new kege::Font( glyphs, matrics, { .image = image, .sampler = sampler, .layout = kege::ImageLayout::ShaderRead });
    }

}
