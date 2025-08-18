//
//  ui-font-creator.hpp
//  physics
//
//  Created by Kenneth Esdaile on 7/29/25.
//

#ifndef ui_font_creator_hpp
#define ui_font_creator_hpp

#include "font.hpp"
#include "../graphics/loaders/image-loader.hpp"

namespace kege::ui{

    class FontCreator
    {
    public:

        struct Info
        {
            int width;
            int height;
            int rows;
            int columns;
            unsigned char* image_data;
        };

        static Ref< ui::Font > create( Graphics* graphics, int char_per_row, int char_per_col, const std::string& font_texture_path );
        static std::vector< Glyph > createGlyphs( const FontCreator::Info& info );
    };

}

#endif /* ui_font_creator_hpp */
