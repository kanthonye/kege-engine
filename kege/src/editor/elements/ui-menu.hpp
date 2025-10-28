//
//  ui-menu.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/23/25.
//

#ifndef ui_menu_hpp
#define ui_menu_hpp

#include "ui-button.hpp"
#include "droplist.hpp"

namespace kege::ui{

    struct Menu
    {
        bool open( ui::Layout& layout, const char* text );
        void beginContent( ui::Layout& layout );
        void endContent( ui::Layout& layout );

        ui::EID container;
        ui::EID content;
        ui::EID field;

        bool state;
    };

}

#endif /* ui_menu_hpp */
