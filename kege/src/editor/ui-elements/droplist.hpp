//
//  droplist.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/21/25.
//

#ifndef droplist_hpp
#define droplist_hpp

#include "../../core/engine/engine.hpp"
#include "../../core/gui/ui-input.hpp"
#include "../../core/gui/ui-viewer.hpp"
#include "../../core/gui/ui-layout.hpp"
#include "../../core/gui/ui-font-creator.hpp"

namespace kege{

    struct DropState
    {
        bool open[2] = {false, false};
    };

    struct Droplist
    {
        bool begin( ui::Layout& layout, bool has_child, int spacer, const char* text );
        void beginContent( ui::Layout& layout );
        void endContent( ui::Layout& layout );
        void end( ui::Layout& layout );

        ui::EID container;
        ui::EID content;
        ui::EID field;
        ui::EID label;
        ui::EID icon;
        ui::Style spacer;

        DropState state;
    };

}
#endif /* droplist_hpp */
