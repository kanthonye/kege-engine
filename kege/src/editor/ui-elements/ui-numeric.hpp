//
//  ui-numeric.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#ifndef ui_numeric_hpp
#define ui_numeric_hpp

#include "../../core/engine/engine.hpp"
#include "../../core/gui/ui-input.hpp"
#include "../../core/gui/ui-layout.hpp"

namespace kege::ui{

    struct Numeric
    {
        void update( ui::Layout& layout, float& value, const char* text );

        ui::EID container;
        ui::EID label;
        ui::EID value;

        std::string data;
        int32_t cursor;
        bool has_focused;
    };
}

#endif /* ui_numeric_hpp */
