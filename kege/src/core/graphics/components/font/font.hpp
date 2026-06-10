//
//  font.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/3/25.
//

#ifndef font_hpp
#define font_hpp

#include "../../core/device/core/device.hpp"

namespace kege{

    struct FontMetrics
    {
        float ascent;   // distance from baseline to highest pixel
        float descent;  // distance from baseline to lowest pixel (positive)
        float baseline;
    };
    struct Glyph
    {
        // Texture atlas (UVs or normalized rect)
        float x;
        float y;
        float width;
        float height;

        // Size relative to the font cell
        float scaled_width;
        float scaled_height;

        // Layout metrics
        float bearing_x;   // pen → left of glyph
        float bearing_y;   // baseline → top of glyph
        float advance;     // pen advance
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
        const FontMetrics& matrics()const;
        operator bool()const;

        Font( const std::vector< kege::Glyph >& glyphs, FontMetrics matrics, const kege::ImageBindInfo& image_binf_info );
        ~ Font();
        Font();

    protected:

        std::vector< kege::Glyph > _glyphs;
        kege::ImageBindInfo _image_binf_info;
        std::string _filename;
        FontMetrics _matrics;
    };

}

namespace kege::ref{
    typedef kege::Ref< kege::Font > Font;
}
#endif /* font_hpp */
