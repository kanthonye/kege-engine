//
//  ui-font-loader.hpp
//  physics
//
//  Created by Kenneth Esdaile on 7/29/25.
//

#ifndef ui_font_loader_hpp
#define ui_font_loader_hpp

#include "font.hpp"
#include "ui-primatives.hpp"

namespace kege::ui{

    class FontLoader
    {
    public:

        static Ref< ui::Font > load( Graphics* graphics, const std::string& filename );
    };

}

#endif /* ui_font_loader_hpp */
