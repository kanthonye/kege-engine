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
        void update( ui::Layout& layout, vec4& value, const char* text );

        ui::EID container;
        ui::EID content;
        ui::EID label;
        Numeric x;
        Numeric y;
        Numeric z;
        Numeric w;
    };
}

#endif /* ui_numeric4_hpp */
