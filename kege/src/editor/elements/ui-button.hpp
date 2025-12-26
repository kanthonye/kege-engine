//
//  ui-button.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/23/25.
//

#ifndef ui_button_hpp
#define ui_button_hpp

#include "ui-tree-node.hpp"

namespace kege::ui{

    struct Button
    {
        bool clicked( ui::Layout& layout, const char* text );
        UID field;
    };

}

#endif /* ui_button_hpp */
