//
//  droplist.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/21/25.
//

#ifndef droplist_hpp
#define droplist_hpp

#include "ui-tree-node.hpp"

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

        ui::Elem container;
        ui::Elem content;
        ui::Elem field;
        ui::Elem label;
        ui::Elem icon;

        DropState state;
    };
}
#endif /* droplist_hpp */
