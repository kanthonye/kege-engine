//
//  ui-tab.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/11/25.
//

#ifndef ui_tab_hpp
#define ui_tab_hpp

#include "ui-tree-node.hpp"

namespace kege::ui{

    struct Tabs
    {
        void recreate( ui::Layout* layout, const std::vector< std::string >& tabs );
        bool update( ui::Layout* layout, const std::vector< std::string >& tabs );
        Tabs();
        std::vector< UID > tab_elements;
        UID container;
        int selection;
    };

}
#endif /* ui_tab_hpp */
