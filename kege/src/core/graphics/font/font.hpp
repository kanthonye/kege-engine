//
//  font.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/3/25.
//

#ifndef font_hpp
#define font_hpp

#include "../render/core/graphics.hpp"

namespace kege{

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

namespace kege{

    class Font : public kege::RefCounter
    {
    public:

        bool save( const std::string& filename, const std::string& image_name );
        kege::vec2 computeExtent( int font_size, const char* text )const;

        float getTextWidth( int font_size, const char* str )const;
        float getCharWidth( int font_size, int c )const;

        kege::ImageBindInfo getImageBindInfo()const;
        const std::vector< Glyph >& glyphs()const;

        operator bool()const;

        Font( const std::vector< kege::Glyph >& glyphs, const kege::ImageBindInfo& image_binf_info );
        ~ Font();
        Font();

    protected:

        std::vector< kege::Glyph > _glyphs;
        kege::ImageBindInfo _image_binf_info;
        std::string _filename;
    };

}

namespace kege::ref{
    typedef kege::Ref< kege::Font > Font;
}
#endif /* font_hpp */
