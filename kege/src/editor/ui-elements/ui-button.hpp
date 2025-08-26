//
//  ui-button.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/23/25.
//

#ifndef ui_button_hpp
#define ui_button_hpp

#include "../../core/engine/engine.hpp"
#include "../../core/gui/ui-input.hpp"
#include "../../core/gui/ui-viewer.hpp"
#include "../../core/gui/ui-layout.hpp"
#include "../../core/gui/ui-font-creator.hpp"

namespace kege::ui{

    struct Button
    {
        bool clicked( ui::Layout& layout, const char* text );
        ui::EID field;
    };

}

#endif /* ui_button_hpp */
