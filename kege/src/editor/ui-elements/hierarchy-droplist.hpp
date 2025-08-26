//
//  hierarchy-droplist.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#ifndef hierarchy_droplist_hpp
#define hierarchy_droplist_hpp

#include "droplist.hpp"

namespace kege::ui{

    struct HierarchyDroplist
    {
        bool open( ui::Layout& layout, bool has_child, int spacer, const char* text );
        void beginContent( ui::Layout& layout );
        void endContent( ui::Layout& layout );

        ui::EID container;
        ui::EID content;
        ui::EID field;
        ui::EID label;
        ui::EID icon;
        ui::EID spacer;

        ui::Style spacer_style;
        DropState state;
    };

}
#endif /* hierarchy_droplist_hpp */
