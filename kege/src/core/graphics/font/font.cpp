//
//  font.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/3/25.
//

#include "font.hpp"

namespace kege{

    bool Font::save( const std::string& filename, const std::string& image_name )
    {
        FILE* file = fopen( filename.c_str(), "r" );
        if ( file == nullptr )
        {
            return false;
        }

        fprintf( file, "image %s\n", image_name.data() );
        for (int i=0; i<_glyphs.size(); ++i)
        {
            fprintf( file, "x %f\n", _glyphs[i].x );
            fprintf( file, "y %f\n", _glyphs[i].x );
            fprintf( file, "width %f\n", _glyphs[i].width );
            fprintf( file, "height %f\n", _glyphs[i].height );
            fprintf( file, "scaled_width %f\n", _glyphs[i].scaled_width );
            fprintf( file, "scaled_height %f\n", _glyphs[i].scaled_height );
            fprintf( file, "bearing_x %f\n", _glyphs[i].bearing_x );
            fprintf( file, "bearing_y %f\n", _glyphs[i].bearing_y );
            fprintf( file, "advance %f\n", _glyphs[i].advance );
        }

        fclose( file );
        return true;
    }

    kege::ImageBindInfo Font::getImageBindInfo()const
    {
        return _image_binf_info;
    }

    const std::vector< Glyph >& Font::glyphs()const
    {
        return _glyphs;
    }

    float Font::getTextWidth( int font_size, const char* str )const
    {
        float length = 0;
        for ( const char* s = str; *s != 0; ++s )
        {
            length += getCharWidth( font_size, *s );
        }
        return length;
    }

    float Font::getCharWidth( int font_size, int c )const
    {
        return font_size * _glyphs[ c ].advance;
    }

    Font::operator bool()const
    {
        return !_glyphs.empty();
    }

    Font::Font( const std::vector< kege::Glyph >& glyphs, const kege::ImageBindInfo& image_binf_info )
    :   _image_binf_info( image_binf_info )
    ,   _glyphs( glyphs )
    {}
    
    Font::~ Font()
    {
        _glyphs.clear();
    }

    Font::Font()
    {
    }

}
