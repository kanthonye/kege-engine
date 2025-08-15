//
//  font.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/3/25.
//

#ifndef font_hpp
#define font_hpp

#include <vector>
#include "../../memory/ref.hpp"
#include "../../math/algebra/vectors.hpp"
#include "../../graphics/core/graphics.hpp"

namespace kege::ui{

    struct Glyph
    {
        // Position in the texture atlas
        float x, y, width, height;

        // Quad scale and y offset
        float scaled_width;
        float scaled_height;
        float bearing_x;
        float bearing_y;
        float offset_y;
        float advance;
    };

}

namespace kege::ui{

    class Font : public kege::RefCounter
    {
    public:

        bool save( const std::string& filename, const std::string& image_name );

        float getTextWidth( int font_size, const char* str )const;
        float getCharWidth( int font_size, int c )const;

        const std::vector< Glyph >& glyphs()const;

        kege::SamplerHandle getSampler()const;
        kege::ImageHandle getImage()const;

        operator bool()const;

        Font( const std::vector< ui::Glyph >& glyphs, kege::ImageHandle image, kege::SamplerHandle sampler );
        ~ Font();
        Font();

    protected:

        std::vector< ui::Glyph > _glyphs;
        kege::SamplerHandle _sampler;
        kege::ImageHandle _image;
        std::string _filename;
    };

}

namespace kege
{
    typedef Ref< ui::Font > Font;
}

#endif /* font_hpp */
