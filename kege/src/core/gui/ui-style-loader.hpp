//
//  ui-style-loader.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/27/25.
//

#ifndef ui_style_loader_hpp
#define ui_style_loader_hpp

#include "ui-core.hpp"
#include "../utils/json-parser.hpp"

namespace kege::ui{

    class StyleLoader
    {
    public:

        static std::vector< std::pair< std::string, Style > > load( const std::string& filename );
    };
}

#endif /* ui_style_loader_hpp */
