//
//  ui-numeric3.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#ifndef ui_numeric3_hpp
#define ui_numeric3_hpp

#include "ui-numeric.hpp"

namespace kege::ui{

    struct Numeric3
    {
        void update( ui::Layout& layout, vec3& value, const char* text );

        ui::EID container;
        ui::EID content;
        ui::EID label;
        Numeric x;
        Numeric y;
        Numeric z;
    };
}

#endif /* ui_numeric3_hpp */
