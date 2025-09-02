//
//  ui-numeric4.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#ifndef ui_numeric4_hpp
#define ui_numeric4_hpp

#include "ui-numeric.hpp"

namespace kege::ui{

    struct Numeric4
    {
        bool update( ui::Layout& layout, float& x, float& y, float& z, float& w, const char* text );

        ui::EID container;
        ui::EID content;
        ui::EID label;
        Numeric nx;
        Numeric ny;
        Numeric nz;
        Numeric nw;
    };
}

#endif /* ui_numeric4_hpp */
