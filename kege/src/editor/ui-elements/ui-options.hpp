//
//  ui-options.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/23/25.
//

#ifndef ui_options_hpp
#define ui_options_hpp

#include "ui-button.hpp"
#include "droplist.hpp"

namespace kege::ui{

    struct Options
    {
        bool operator()( ui::Layout& layout, const char* text, const std::vector<const char*>& list );
        void display( ui::Layout& layout );

        std::vector< ui::EID > options;
        ui::EID container;
        ui::EID content;
        ui::EID field;

        uint32_t index[2];
        bool state;
    };

}

#endif /* ui_options_hpp */
