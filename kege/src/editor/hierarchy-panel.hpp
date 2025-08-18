//
//  hierarchy-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef hierarchy_panel_hpp
#define hierarchy_panel_hpp

#include "../core/engine/engine.hpp"
#include "../core/gui/ui-input.hpp"
#include "../core/gui/ui-viewer.hpp"
#include "../core/gui/ui-layout.hpp"
#include "../core/gui/ui-font-creator.hpp"

namespace kege{

    class HierarchyPanel
    {
    public:

        HierarchyPanel& init( Engine* engine, ui::Layout& layout );
        void put( ui::Layout& layout );
        HierarchyPanel();

    public:

        Engine* _engine;
        ui::EID _main;
    };
}

#endif /* hierarchy_panel_hpp */
